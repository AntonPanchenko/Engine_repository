#include "ChessTree.h"

ChessTree::ChessTree(unsigned int height):
	m_height(height),
	m_is_in_time_consuming_operation(false),
	m_cancel_time_consuming_operation(false)
{}

ChessTree::~ChessTree()
{
	DestroyTree();
}

unsigned int /*error code*/ ChessTree::BuildTree(Chess& board)
{
	if (m_root != NULL)
	{
		return CH_ERR_CANNOT_BUILD_TREE_ALREADY_EXISTS;
	}
	if (m_is_in_time_consuming_operation == false)
	{
		m_is_in_time_consuming_operation = true;
	}
	m_root = new Entry(board);
	ChessTree::Entry* current_entry = GetRoot();
	bool exit_flag = false;
	unsigned int level = 0;
	bool color = board.GetIsBlackTurn();
	while (exit_flag == false)
	{
		unsigned int error = BuildNextLevel(current_entry, color);
		if (error != CH_ERR_OK)
		{
			return error;
		}
		//goto next level's first entry
		current_entry = reinterpret_cast<ChessTree::Entry*>(current_entry->Begin().GetChild());
		ASSERT(current_entry != NULL);
		++level;
		color = !color;
		if (level >= m_height)
		{
			exit_flag = true;
		}
	}
	m_is_in_time_consuming_operation = false;
	return CH_ERR_OK;
}

unsigned int /*error code*/ ChessTree::UpdateTree(Chess& board)
{
	unsigned int ret_val = CH_ERR_UNDEFINED_ERROR;
	if (m_is_in_time_consuming_operation == false)
	{
		m_is_in_time_consuming_operation = true;
	}
	ChessTree::Entry* root = GetRoot();
	if (root != NULL)
	{
		//make sure that this is the same board. if not the same, then look for a nearest appropriate child.
		if(root->operator ChessTreeEntry &().GetBoardRef() == board)
		{
			m_is_in_time_consuming_operation = false;
			return CH_ERR_OK;
		}
		//find board in nearest level children
		//ChessTree::Entry::ChildrenIterator destroy_it = m_root->Begin();
		ChessTree::Entry* new_root = NULL;
		typedef Vector<ChessTree::Entry*> DestructionList;
		DestructionList destruction_list(m_root->GetChildrenCount(), DestructionList::GetDefaultAllocator(), NULL);
		bool exit_flag = false;
		ChessTree::Entry::ChildrenIterator ch_it = m_root->Begin();
		while(exit_flag == false)
		{
			ASSERT(ch_it.IsValid());
			BasicTree::Entry* e = ch_it.GetChild();
			ASSERT(e != NULL);
			ChessTree::Entry* entry = dynamic_cast<ChessTree::Entry*>(e);//(ch_it.GetChild());
			ASSERT(entry != NULL);			
			exit_flag = !ch_it.Advance(true);
			Chess& board_from_tree = GetBoard(e);//ch_it.GetChild());
			//if found, let it stay here and make this child a root...
			if (board_from_tree == board)
			{
				RemoveEntry(entry);
				ASSERT(entry->GetParent() == NULL);
				new_root = entry;
				//
				/*wchar_t msg[0xff];
				memset(msg, 0, sizeof(msg));
				wchar_t* ptr = msg + swprintf(msg, L"new root: ");
				entry->operator ChessTreeEntry &().GetMoveRecordRef().ToString(ptr);
				OutputDebugMsg(msg);*/
				//
				//new_root->m_parent = NULL;
			}
			else {
				destruction_list.PushBack(entry);
			}
		}
		for(DestructionList::Iterator it = destruction_list.Begin(); it.IsValid(); ++it)
		{
			ChessTree::Entry* e = *it;
			ASSERT(e != NULL);
			delete e;
		}
		delete m_root;
		m_root = NULL;
		if (new_root != NULL)
		{
			AddEntry(new_root, NULL);
			//m_root = new_root;
			// ...and build another level above (create children for each top level entry).
			TopLevelIterator tl_it = GetTopLevelIterator(true);
			bool last_top_level_entry = false;
			while ((tl_it.IsValid()) && (last_top_level_entry == false))
			{
				BasicTree::Entry* e = tl_it.GetEntry();
				last_top_level_entry = !tl_it.Advance(true);
				ASSERT(e != NULL);
				ChessTree::Entry* top_level_cte = dynamic_cast<ChessTree::Entry*>(e);
				ASSERT(top_level_cte != NULL);
				ChessTreeEntry& top_level_entry = top_level_cte->operator ChessTreeEntry &();
				//Entry* top_level_entry = dynamic_cast<Entry*>(tl_it.GetEntry());
				Chess& board_from_entry = top_level_entry.GetBoardRef();//(Chess&)(*top_level_entry);
				ChessAddress address(CH_CONST_MIN_LETTER, CH_CONST_MIN_NUMBER);
				bool exit_flag = false;
				while (exit_flag == false)
				{
					ChessPiece* piece = board_from_entry.GetPiece(address);
					if (piece != NULL)
					{
						if(piece->GetIsBlack() == board_from_entry.GetIsBlackTurn())
						{
							ChessPiece::NextPositionIterator* np_it = piece->GetNextPositionIterator();
							if (np_it != NULL)
							{
								ChessAddress curr_addr;
								piece->GetCurrentAddress(curr_addr);
								if(curr_addr == ChessAddress(1, 2))
								{
									if(np_it->GetCurrentAddress() == ChessAddress(1, 1))
									{
										ChessPiece::NextPositionIterator* tmp_npi = piece->GetNextPositionIterator();
										int iii = 0;
									}
								}
								do
								{
									//Entry* new_level_entry = new /Entry(Chess());
									ChessTreeEntry new_cte(board_from_entry);
									Entry* new_level_entry = new Entry(new_cte);
									ASSERT(new_level_entry != NULL);
									Chess& board_in_new_entry = new_level_entry->operator ChessTreeEntry &().GetBoardRef();
									//
									//make a move according to iterator
									ChessPiece* piece_in_new_level_entry = board_in_new_entry.GetPiece(address);//nle_chess.GetPiece(address);
									ASSERT(piece_in_new_level_entry != NULL);	//because I just copied it there.
									ChessAddress where_it_came;
									unsigned int err = board_in_new_entry.MovePiece(piece_in_new_level_entry, np_it->GetCurrentAddress(), where_it_came);
									ASSERT(where_it_came.IsValid());	//because it was valid in source board, so must be here.
									//and add a new board to the tree.
									MoveRecord& mr = new_level_entry->operator ChessTreeEntry &().GetMoveRecordRef() = MoveRecord(piece->GetCellContentType(),
										address,
										where_it_came);
									AddEntry(new_level_entry, top_level_cte);
								} while (np_it->Advance() == CH_ERR_OK);
								delete np_it;
							}
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
			}
			ret_val = CH_ERR_OK;
		} else {	//this is some new board which is not a descendant to current root. maybe it came from savefile.
			ChessTree::Entry* root = GetRoot();
			if(root != NULL)
			{
				DestroyBranch(root);
			}
			//build a new tree
			ret_val = BuildTree(board);
		}
	} else {	//build a new tree from scratch
		ret_val = BuildTree(board);
	}
	m_is_in_time_consuming_operation = false;
	return ret_val;
}

unsigned int /*error code*/ ChessTree::DestroyTree()
{
	if (m_root != NULL)
	{
		DestroyBranch(reinterpret_cast<ChessTree::Entry*>(m_root));
		m_root = NULL;
	}
	return CH_ERR_OK;
}

#ifdef _DEBUG
void ChessTree::PrintTree()
{
	/*const unsigned int buf_length = 1024;
	wchar_t buffer[buf_length];
	memset(buffer, 0, sizeof(buf_length));
	for(BaseClass::ChildrenIterator it = GetChildrenIterator(); it.IsValid(); ++ it)
	{
		Entry* cte = dynamic_cast<Entry*>(it.GetCurrentChild());
		ASSERT(cte != NULL);
		PrintEntry(cte, 0, buffer, buf_length);
	}*/
}

void ChessTree::PrintEntry(Entry* entry, unsigned int level, wchar_t* buffer, unsigned int length)
{
	/*const unsigned int level_indent = 1;
	unsigned int indent = level * level_indent;
	unsigned int space_left = length - indent;
	for(unsigned int i = 0; i < indent; ++ i)
	{
		buffer[i] = L' ';
	}
	memset(buffer + indent, 0, space_left);
	wchar_t* ptr = buffer + indent;
	MoveRecord& mr = GetMoveRecord(entry);
	const wchar_t* piece_type = GetCellContentTypeString(mr.PieceType());
	unsigned int len = wcslen(piece_type);
	if(space_left > length)
	{
		return;
	}
	wcsncpy(ptr, piece_type, len);
	ptr += len;
	space_left -= len;
	if(space_left > length)
	{
		return;
	}
	*ptr = L' ';
	++ ptr;
	space_left -= 1;
	if(space_left > length)
	{
		return;
	}
	len = mr.From().ToString(ptr, space_left);
	space_left -= len;
	if(space_left > length)
	{
		return;
	}
	ptr += len;
	*ptr = L' ';
	space_left -= 1;
	if(space_left > length)
	{
		return;
	}
	ptr += 1;
	len = mr.To().ToString(ptr, space_left);
	ptr += len;
	*ptr = L'\n';
	OutputDebugMsg(buffer);
	for(BaseClass::Entry::ChildrenIterator it = entry->Begin(); it.IsValid(); ++ it)
	{
		Entry* child = dynamic_cast<Entry*>(it.GetChild());
		ASSERT(child != NULL);
		PrintEntry(child, level + 1, buffer, space_left);
	}
	*/
}

#endif //_DEBUG

bool ChessTree::IsEmpty() const
{
	return (m_root == NULL);
}

/*
bool ChessTree::LockForRead()
{
}

bool ChessTree::LockForWrite()
{}

void ChessTree::Unlock()
{}
*/

MoveRecord ChessTree::GetNearestMoveRecord(BasicTree::Entry* direction_entry)
{
	MoveRecord ret_val;
	bool exit_flag = false;
	BasicTree::Entry* current_entry = direction_entry;
	BasicTree::Entry* root = GetRoot();
	ASSERT(current_entry != NULL);
	ASSERT(root != NULL);
	while(exit_flag == false)
	{
		BasicTree::Entry* parent_entry = current_entry->GetParent();
		if(parent_entry == root)
		{
			ret_val = GetMoveRecord(current_entry);
			exit_flag = true;
		}
		ASSERT(parent_entry != NULL);
		current_entry = parent_entry;
	}
	return ret_val;
}

unsigned int /*error code*/ ChessTree::BuildNextLevel(Entry* leftmost_entry, bool color)
{
	bool exit_flag = false;
	unsigned int ret_val = CH_ERR_UNDEFINED_ERROR;
	ASSERT(leftmost_entry != NULL);
	Entry* current_entry = leftmost_entry;
	while (exit_flag == false)
	{
		ret_val = BuildChildren(current_entry, color);
		current_entry = current_entry->GetNextSibling();
		if (current_entry == NULL)
		{
			ret_val = CH_ERR_OK;
			break;
		}
		if (m_cancel_time_consuming_operation)
		{
			exit_flag = true;
			m_cancel_time_consuming_operation = !m_cancel_time_consuming_operation;
		}
	}
	return ret_val;
}

unsigned int /*error code*/ ChessTree::BuildChildren(Entry* parent, bool color)
{
	unsigned int ret_val = CH_ERR_UNDEFINED_ERROR;
	ASSERT(parent != NULL);
	ChessTreeEntry& cte = parent->operator ChessTreeEntry &();
	Chess& chess = cte.GetBoardRef();
	ChessAddress address(CH_CONST_MIN_LETTER, CH_CONST_MIN_NUMBER);
	bool exit_flag = false;
	while (exit_flag == false)
	{
		//find another piece loop
		ChessPiece* piece = chess.GetPiece(address);
		if (piece != NULL)
		{
			//find next positions loop
			ChessPiece::NextPositionIterator* np_it = piece->GetNextPositionIterator();
			if (np_it != NULL)
			{
				do
				{
					Entry* child = new Entry(chess);
					Chess& child_chess = child->operator ChessTreeEntry &().GetBoardRef();
					ChessPiece* child_piece = child_chess.GetPiece(address);
					if (child_piece->GetIsBlack() == color)
					{
						ChessAddress where_it_came;
						unsigned int error = child_chess.MovePiece(child_piece, np_it->GetCurrentAddress(), where_it_came);
						if (error == CH_ERR_OK)
						{
							CellContentType cct = child_piece->GetCellContentType();
							child->operator ChessTreeEntry &().GetMoveRecordRef() = MoveRecord(cct, address, where_it_came);
							if (parent->AddChild(child) == false)
							{
								delete child;
							}
						}
						else {
							delete child;
						}
					}
				} while (np_it->Advance() == CH_ERR_OK);
			}
		}
		if (m_cancel_time_consuming_operation)
		{
			exit_flag = true;
			m_cancel_time_consuming_operation = !m_cancel_time_consuming_operation;
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
	return ret_val;
}

void ChessTree::DestroyBranch(Entry* branch_root)
{
	ASSERT(branch_root != NULL);
	RemoveEntry(branch_root);
	while (branch_root->GetChildrenCount() != 0)
	{
		DestroyTopLevel(branch_root);
	}
	delete branch_root;
}

void ChessTree::DestroyTopLevel(Entry* branch_root)
{
	ASSERT(branch_root != NULL);
	bool exit_flag = false;
	Entry* entry = branch_root;
	//go down to the first top level child
	while (exit_flag == false)
	{
		Entry* child_entry = dynamic_cast<Entry*>(entry->GetFirstChild());
		if (child_entry == NULL)
		{
			exit_flag = true;
		} else {
			entry = child_entry;
		}
	}
	ASSERT(entry != NULL);
	if(entry == branch_root)	//this means no children found
	{
		return;
	} else {	//destroy top level children
		exit_flag = false;
		while (exit_flag == false)
		{
			Entry* next_entry = entry->GetNextSibling();
			RemoveEntry(entry);
			delete entry;
			if (next_entry == NULL)
			{
				exit_flag = true;
			} else {
				entry = next_entry;
			}
		}
	}
}