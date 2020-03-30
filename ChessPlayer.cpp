#include "ChessPlayer.h"
#include <QObject>
#include <typeinfo>

BasicChessPlayer::~BasicChessPlayer()
{
	while (m_event_handler_list.GetCount() > 0)
	{
		BasicList::Entry* entry = m_event_handler_list.PopBack();
		delete entry;
	}
}

void BasicChessPlayer::AddEventHandler(BasicChessPlayer::EventHandler* eh)
{
	if (eh == NULL)
	{
		throw Exception(CH_ERR_INVALID_PARAMETERS,
			L"No event handler passed",
			EXC_HERE);
	}
	EventHandlerList::Entry* new_entry = new EventHandlerList::Entry(eh);
	m_event_handler_list.PushBack(new_entry);
}

void BasicChessPlayer::RemoveEventHandler(BasicChessPlayer::EventHandler* eh)
{
	if (eh == NULL)
	{
		throw Exception(CH_ERR_INVALID_PARAMETERS,
			L"No event handler passed",
			EXC_HERE);
	}
	for (EventHandlerList::Iterator it = m_event_handler_list.Begin(); it.IsValid(); ++it)
	{
		EventHandlerList::Entry* entry = dynamic_cast<EventHandlerList::Entry*>(it.operator BasicList::Entry *());
		ASSERT(entry != NULL);
		if (*entry == eh)
		{
			m_event_handler_list.Remove(it);
		}
	}
}

unsigned int /*error code*/ AsyncChessPlayer::Worker::MakeAMove()
{
	ASSERT(m_acp != NULL);
	MoveRecord move_record;
	Chess* board = m_acp->m_board;
	ASSERT(board != NULL);
	unsigned int ret_val = m_acp->InternalMakeAMove(*board, move_record);
	if (ret_val == CH_ERR_OK)
	{	//notify subscribers
		for (EventHandlerList::Iterator it = m_acp->m_event_handler_list.Begin(); it.IsValid(); ++it)
		{
			EventHandler* eh = *(it.operator BasicChessPlayer::EventHandler **());
			ASSERT(eh != NULL);
			eh->OnMove(*board, move_record);
		}
	}
	return ret_val;
}

AsyncChessPlayer::AsyncChessPlayer(Chess* board, bool is_black):
	BasicChessPlayer(board, is_black),
	m_thread(PRIORITY_NORMAL)
{
	unsigned int platform_error = UNDEFINED_ERROR;
	unsigned int error = m_thread.Run(&platform_error);
	if(error != ERR_OK)
	{
		wchar_t msg[0x7f];
		swprintf(msg, L"Cound't start a thread, platform error: 08x%x", platform_error);
		throw Exception(error, msg, EXC_HERE);
	}
}

AsyncChessPlayer::~AsyncChessPlayer()
{
	//cancel next move detection
	CancelTimeConsumingOperation();
	m_thread.WaitForExit();
	//subscribers are to be removed in base class destructor
	//there must be My own list of workers in order to delete them.
}

unsigned int /*error code*/ AsyncChessPlayer::MakeAMove(Chess& board)
{
	Worker* mover = GetMover();
	ASSERT(mover != NULL);
	MakeAMoveMessage* move_message = new MakeAMoveMessage(mover, true);
	unsigned int ret_val = m_thread.PostMessageToWorker(move_message);
	return ret_val;
}

unsigned int /*error code*/ GUIChessPlayer::MakeAMove(Chess& board)
{
	//nothing so far. later update GUI in order to tell the user who's turn now is.
	return CH_ERR_OK;
}

TreeBasedChessPlayer::TreeBasedChessPlayer(Chess* board, bool is_black, unsigned int height) :
	AsyncChessPlayer(board, is_black),
	m_tree(height),
	m_nh(this),
	m_nh_index(Chess::INVALID_HANDLER_INDEX),
	m_worker(this)
{
	ASSERT(board != NULL);
	//m_thread.AddWorker(&m_worker);
	m_thread.Run();
	Worker* worker = dynamic_cast<Worker*>(GetMover());
	m_nh_index = board->SetNotificationHandler(&m_nh);
	WorkerMessage* build_message = new BuildTreeMessage(&m_worker, board, true);
	m_thread.PostMessageToWorker(build_message);
}

TreeBasedChessPlayer::~TreeBasedChessPlayer()
{
	if(m_nh_index != Chess::INVALID_HANDLER_INDEX)
	{
		m_board->RemoveNotificationHandler(m_nh_index);
	}
	WorkerMessage* destroy_message = new DestroyTreeMessage(&m_worker, true);
	m_thread.PostMessageToWorker(destroy_message);
	ExitMessage* exit_message = new ExitMessage(&m_worker, true);
	m_thread.PostMessageToWorker(exit_message);
	m_thread.WaitForExit();
}

unsigned int /*error code*/ TreeBasedChessPlayer::Worker::Execute(WorkerMessage* wm)
{
	unsigned int ret_val = CH_ERR_UNDEFINED_ERROR;
	ASSERT(wm != NULL);
	if(typeid(*wm) == typeid(BuildTreeMessage))
	{
		if(m_tbcp->m_tree.IsEmpty() == false)
		{
			throw Exception(CH_ERR_CANNOT_INIT_ALREADY_INITIALIZED,
				L"Cannot initialize chess player because it seems to be already initalized, tree is not empty",
				EXC_HERE);
		}
		Chess* board = m_tbcp->GetBoard();
		ASSERT(board != NULL);
		ret_val = m_tbcp->m_tree.BuildTree(*board);
		/*
		m_tbcp->m_tree.PrintTree();
		*/
	} else if (typeid(*wm) == typeid(UpdateTreeMessage))
	{
		Chess* board = m_tbcp->GetBoard();
		ASSERT(board != NULL);
		ret_val = m_tbcp->m_tree.UpdateTree(*board);
		/*
		m_tbcp->m_tree.PrintTree();
		*/
	} else if(typeid(*wm) == typeid(MakeAMoveMessage))
	{
		Chess* board = m_tbcp->GetBoard();
		ASSERT(board != NULL);
		if(board->GetIsBlackTurn() == m_tbcp->GetIsBlack())
		{
			ret_val = MakeAMove(*board);
		} else {
			ret_val = CH_ERR_OK;	//ok not my turn
		}
	} else if (typeid(*wm) == typeid(DestroyTreeMessage))
	{
		Chess* board = m_tbcp->GetBoard();
		ASSERT(board != NULL);
		ret_val = m_tbcp->m_tree.DestroyTree();
	} else if(typeid(*wm) == typeid(ExitMessage))
	{
		m_at_work = false;
		m_tbcp->m_thread.RemoveWorker(this);
		ret_val = CH_ERR_OK;
	}
	else 
	{
		wchar_t msg[0x7f];
		memset(msg, 0, sizeof(msg));
		wchar_t* ptr = wcscpy(msg, L"TreeBasedChessPlayer::Worker does not support this message: ");
		ptr += wcslen(ptr);
		const char* classname = typeid(*wm).name();
		mbstowcs(ptr, classname, strlen(classname));
		throw Exception(CH_ERR_WORKER_MESSAGE_NOT_SUPPORTED,
			ptr,
			EXC_HERE);
	}
	return ret_val;
}

unsigned int TreeBasedChessPlayer::Worker::MakeAMove(Chess& board)
{
	ASSERT(m_acp != NULL);
	MoveRecord mr;
	unsigned int ret_val = m_acp->InternalMakeAMove(*(m_acp->GetBoard()), mr);
	if (ret_val == CH_ERR_OK)
	{
		for (EventHandlerList::Iterator it = m_acp->GetEventHandlerList()->Begin(); it.IsValid(); ++it)
		{
			EventHandler* eh = *(it.operator BasicChessPlayer::EventHandler **());
			ASSERT(eh != NULL);
			Chess* board = m_acp->GetBoard();
			ASSERT(board != NULL);
			eh->OnMove(*board, mr);
		}
	} else if (ret_val == CH_ERR_CHESS_PLAYER_NO_NEXT_MOVE)
	{
		Exception exc(ret_val, L"Nowhere to Move, Game Over", EXC_HERE);
		PostExceptionToMainThread(&exc);
	}
	return ret_val;
}

unsigned int /*error code*/ TreeBasedChessPlayer::InternalMakeAMove(Chess& board, MoveRecord& out_result)
{
	unsigned int ret_val = CH_ERR_UNDEFINED_ERROR;
	ASSERT(m_tree.IsEmpty() == false);
	bool black_turn = board.GetIsBlackTurn();
	ret_val = m_tree.UpdateTree(board);
	if(ret_val == CH_ERR_OK)
	{
		MoveRecord best_move = FindTheBestMove(black_turn);
		if (best_move.IsValid())
		{
			ChessPiece* piece = board.GetPiece(best_move.From());
			ASSERT(piece != NULL);
			ChessAddress where_it_came;
			ret_val = board.MovePiece(piece, best_move.To(), where_it_came);
			if(ret_val == CH_ERR_OK)
			{
				ret_val = m_tree.UpdateTree(board);
			}
			out_result = best_move;
		} else {
			ret_val = CH_ERR_CHESS_PLAYER_NO_NEXT_MOVE;
		}
	}
	return ret_val;
}

TreeBasedChessPlayer::NotificationHandler::NotificationHandler(TreeBasedChessPlayer* player):
	m_player(player)
{
	ASSERT(m_player != NULL);
}

TreeBasedChessPlayer::NotificationHandler::~NotificationHandler()
{}

void TreeBasedChessPlayer::NotificationHandler::OnResetGame(Chess* chess)
{
	//move this stuff to worker too
	ASSERT(chess != NULL);
	ASSERT(m_player != NULL);
	WorkerMessage* message = new DestroyTreeMessage(&m_player->m_worker, true);
	m_player->m_thread.PostMessageToWorker(message);
	message = new BuildTreeMessage(&m_player->m_worker, chess, true);
	m_player->m_thread.PostMessageToWorker(message);
}

void TreeBasedChessPlayer::NotificationHandler::OnMove(Chess* chess, 
													   ChessPiece* piece, 
													   const ChessAddress& prev_pos, 
													   const ChessAddress& new_pos)
{
	ASSERT(chess != NULL);
	ASSERT(piece != NULL);
	ASSERT(m_player != NULL);
	//message is to be deleted on worker thread.
	WorkerMessage* message = new UpdateTreeMessage(&m_player->m_worker, true);
	m_player->m_thread.PostMessageToWorker(message);
	message = new MakeAMoveMessage(&m_player->m_worker, true);
	m_player->m_thread.PostMessageToWorker(message);
}

void TreeBasedChessPlayer::NotificationHandler::OnCapture(Chess* chess,
														  ChessPiece* winner,
														  ChessPiece* loser)
{
	//nothing so far..
}

void TreeBasedChessPlayer::CalculateDamage(ChessTree::Entry* entry, Damage& out_damage, ChessTree::Entry* ancestor_entry) const
{
	ASSERT(entry != NULL);
	ASSERT(entry != ancestor_entry);
#ifdef _DEBUG
	//make sure that ancestor_entry is really an ancestor to entry.
	if (ancestor_entry != NULL)
	{
		bool found = false;
		ChessTree::Entry* current_entry = entry;
		bool exit_flag = false;
		while (exit_flag == false)
		{
			current_entry = dynamic_cast<ChessTree::Entry*>(current_entry->GetParent());
			if (current_entry == NULL)
			{
				exit_flag = true;
			}
			else if (current_entry == ancestor_entry)
			{
				found = true;
				exit_flag = true;
			}
		}
		ASSERT(found);
	}
#endif //_DEBUG
	unsigned int ancestor_sum_value_black = 0;
	unsigned int ancestor_sum_value_white = 0;
	unsigned int entry_sum_value_black = 0;
	unsigned int entry_sum_value_white = 0;
	if(ancestor_entry == NULL)
	{
		ancestor_entry = m_tree.GetRoot();
	}
	ChessAddress address(CH_CONST_MIN_LETTER, CH_CONST_MIN_NUMBER);
	bool exit_flag = false;
	while (exit_flag == false)
	{
		Chess& board = m_tree.GetBoard(entry);
		ChessPiece* piece = board.GetPiece(address);//m_tree.GetBoard(entry).GetPiece(address);
		if (piece != NULL)
		{
			if (piece->GetIsBlack())
			{
				entry_sum_value_black += piece->GetValue();
			}
			else {
				entry_sum_value_white += piece->GetValue();
			}
		}
		Chess& ancestor_board = m_tree.GetBoard(ancestor_entry);
		ChessPiece* ancestor_piece = ancestor_board.GetPiece(address);
		if (ancestor_piece != NULL)
		{
			if (ancestor_piece->GetIsBlack())
			{
				ancestor_sum_value_black += ancestor_piece->GetValue();
			}
			else {
				ancestor_sum_value_white += ancestor_piece->GetValue();
			}
		}
		if (address.IncrementLetter() == false)
		{
			address.SetLetter(CH_CONST_MIN_LETTER);
			if (address.IncrementNumber() == false)
			{
				exit_flag = true;
			}
		}
	}
	out_damage.m_black_damage = ancestor_sum_value_black - entry_sum_value_black;
	out_damage.m_white_damage = ancestor_sum_value_white - entry_sum_value_white;
}

#ifdef _DEBUG
inline void CheckMoveRecord(MoveRecord& mr, Chess& chess)
{
	ChessPiece* piece = chess.GetPiece(mr.From());
	ASSERT(piece != NULL);
	ASSERT(piece->GetCellContentType() == mr.PieceType());
	ChessPiece* enemy = chess.GetPiece(mr.To());
	if(enemy != NULL)
	{
		if(enemy->GetIsBlack() == piece->GetIsBlack())
		{
			ASSERT(false);
		}
	}
}

inline bool CheckMoveRecordNoThrow(MoveRecord& mr, Chess& chess)
{
	wchar_t msg[0xff];
	memset(msg, 0, sizeof(msg));
	ChessPiece* piece = chess.GetPiece(mr.From());
	if(piece == NULL)
	{
		swprintf(msg, L"piece == NULL\n");
		OutputDebugMsg(msg);
		return false;
	}
	if(piece->GetCellContentType() != mr.PieceType())
	{
		swprintf(msg, L"piece content type == %d, move record piece type == %d\n", piece->GetCellContentType(), mr.PieceType());
		OutputDebugMsg(msg);
		return false;
	}
	ChessPiece* enemy = chess.GetPiece(mr.To());
	if(enemy != NULL)
	{
		if(enemy->GetIsBlack() == piece->GetIsBlack())
		{
			swprintf(msg, L"same colors\n");
			OutputDebugMsg(msg);
			return false;
		}
	}
	return true;
}

#else
inline void CheckMoveRecord(MoveRecord& mr, Chess& chess)
{}
#endif //_DEBUG

MoveRecord TreeBasedChessPlayer::FindTheBestMove(bool for_black)
{
	MoveRecord ret_val;
	if (m_tree.LockForRead() == false)
	{
		throw Exception(SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_READ,
			L"Cannot lock a chess tree for read",
			EXC_HERE);
	}
	int profit = 0;
	int maximum_profit = 0;
	//unsigned int maximum_damage = 0;
	Vector<MoveRecord> max_profit_moves(4, BasicVector::GetDefaultAllocator(), NULL);
	ChessTree::TopLevelIterator tl_it = m_tree.GetTopLevelIterator(true);
	while ((tl_it.IsValid()) && (m_cancel_time_consuming_operation == false))
	{
		ChessTree::Entry* current_entry = dynamic_cast<ChessTree::Entry*>(tl_it.GetEntry());
		ASSERT(current_entry != NULL);
		Damage damage;
		CalculateDamage(current_entry, damage);
		if (for_black)
		{
			profit = damage.m_white_damage - damage.m_black_damage;
			if (profit > maximum_profit)
			{
				max_profit_moves.Clear();
				maximum_profit = profit;
				MoveRecord nearest_move_record = m_tree.GetNearestMoveRecord(current_entry);
				CheckMoveRecord(nearest_move_record, *m_board);
				max_profit_moves.PushBack(nearest_move_record);
			} else if (profit == maximum_profit)
			{
				MoveRecord nearest_move_record = m_tree.GetNearestMoveRecord(current_entry);
				CheckMoveRecord(nearest_move_record, *m_board);
				max_profit_moves.PushBack(nearest_move_record);
			}
		} else {
			profit = damage.m_black_damage - damage.m_white_damage;
			if (profit > maximum_profit)
			{
				max_profit_moves.Clear();
				maximum_profit = profit;
				MoveRecord nearest_move_record = m_tree.GetNearestMoveRecord(current_entry);
				CheckMoveRecord(nearest_move_record, *m_board);
				max_profit_moves.PushBack(nearest_move_record);
			} else if (profit == maximum_profit)
			{
				maximum_profit = profit;
				MoveRecord nearest_move_record = m_tree.GetNearestMoveRecord(current_entry);
				CheckMoveRecord(nearest_move_record, *m_board);
				max_profit_moves.PushBack(nearest_move_record);
			}
		}
		++tl_it;
	}
	/*OutputDebugMsg(L"Max damage moves:\n");
	for(unsigned int iii = 0; iii < max_damage_moves.GetCount(); ++ iii)
	{
		MoveRecord mr = max_damage_moves[iii];
		wchar_t buffer[0xFF];
		mr.ToString(buffer);
		OutputDebugString(buffer);
	}
	OutputDebugMsg(L"END Max damage moves:\n");*/
	switch (max_profit_moves.GetCount())
	{
	case 0:	break; //return invalid move record because nowhere to move.
	case 1: ret_val = max_profit_moves[0]; break;
	default:
		{
			unsigned int index = rand() % max_profit_moves.GetCount();
			ret_val = max_profit_moves[index];
		}
	}
	//ret_val = dynamic_cast<ChessTreeEntry*>(tl_it.GetEntry())->GetMoveRecordRef();
	m_tree.Unlock();
	return ret_val;
}

AsyncChessPlayer::Worker* TreeBasedChessPlayer::GetMover()
{
	static Mutex mutex;
	mutex.Wait();
	AsyncChessPlayer::Worker* ret_val = &m_worker;
	//if worker is not in thread worker list, then add worker
	if(m_thread.IsWorkerAdded(ret_val) == false)
	{
		m_thread.AddWorker(ret_val);
	}
	mutex.Release();
	return ret_val;
}
/*
RemoteChessPlayer::RemoteChessPlayer()
{}

RemoteChessPlayer::~RemoteChessPlayer()
{}

unsigned int /*error code RemoteChessPlayer::MakeAMove(Chess& board)
{}
*/