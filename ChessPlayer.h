#ifndef CHESS_PLAYER_H_INCLUDED
#define CHESS_PLAYER_H_INCLUDED

#include "Chess.h"
#include "ChessTree.h"
#include "Multithreading.h"

enum
{
	CH_ERR_CANNOT_INIT_ALREADY_INITIALIZED = CHESS_PLAYER_ERROR_BASE + 1,
	CH_ERR_CHESS_PLAYER_NO_NEXT_MOVE,
	CH_ERR_CHESS_PLAYER_NO_ENEMY,
	CH_ERR_WORKER_MESSAGE_NOT_SUPPORTED,
	CH_ERR_NOT_IMPLEMENTED
};

class BasicChessPlayer
{
public:
	class EventHandler
	{
	public:
		virtual void OnMove(Chess& board, MoveRecord& result) = 0;
	};
	typedef List<EventHandler*> EventHandlerList;

	BasicChessPlayer(Chess* board, bool is_black) :
		m_board(board),
		m_cancel_time_consuming_operation(false),
		m_is_in_time_consuming_operation(false),
		m_event_handler_list_lock(true),
		m_event_handler_list(&m_event_handler_list_lock),
		m_enemy(NULL),
		m_is_black(is_black)
	{
		ASSERT(m_board != NULL);
	}
	virtual ~BasicChessPlayer();
	//this method may work asynchronously because decision regarding next move may take a time,
	//especially when this decision is expected from a human.
	virtual unsigned int /*error code*/ MakeAMove(Chess& board) = 0;
	void AddEventHandler(EventHandler* eh);
	void RemoveEventHandler(EventHandler* eh);
	void CancelTimeConsumingOperation()
		{ 
			if (m_is_in_time_consuming_operation)
			{
				m_cancel_time_consuming_operation = true;
			}
		}
	Chess* GetBoard() const
		{ return m_board; }
	void SetEnemy(BasicChessPlayer* enemy)
		{ m_enemy = enemy; }
	BasicChessPlayer* GetEnemy() const
		{ return m_enemy; }
	//this method is called from a workers, however it belongs to chess player because data needed to make a decision
	//belong to chess player descendants. default implementation is just empty.
	virtual unsigned int /*error code*/ InternalMakeAMove(Chess& board, MoveRecord& mr)
		{ return CH_ERR_NOT_IMPLEMENTED; }
	EventHandlerList* GetEventHandlerList()
		{ return &m_event_handler_list; }
	inline const bool GetIsBlack() const
		{ return m_is_black; }
protected:
	ReadWriteLock m_event_handler_list_lock;
	EventHandlerList m_event_handler_list;
	//this variable is to be set back to false right after the time consuming operation is finished.
	bool m_cancel_time_consuming_operation;
	bool m_is_in_time_consuming_operation;
	Chess* m_board;
	BasicChessPlayer* m_enemy;
	bool m_is_black;
};

class InitMessage: public WorkerMessage
{
public:
	InitMessage(Worker* dest, Chess* board, bool delete_after_completion = true):
		WorkerMessage(dest, delete_after_completion),
		m_board(board)
		{ ASSERT(m_board != NULL); }
	const Chess* GetBoard() const
		{ return m_board; }
protected:
	Chess* m_board;
};

class MakeAMoveMessage: public WorkerMessage
{
public:
	MakeAMoveMessage(Worker* dest, bool delete_after_completion = true):
		WorkerMessage(dest, delete_after_completion)
	{}
};

class AsyncChessPlayer: public BasicChessPlayer
{
public:
	AsyncChessPlayer(Chess* board, bool is_black);
	virtual ~AsyncChessPlayer();
	unsigned int /*error code*/ MakeAMove(Chess& board);
protected:
	class Worker : public ::Worker
	{
		friend class AsyncChessPlayer;
	public:
		Worker(AsyncChessPlayer* acp) :
			m_acp(acp)
		{}
	protected:
		virtual unsigned int /*error code*/ MakeAMove();

		AsyncChessPlayer* m_acp;
	};

	virtual Worker* GetMover() = 0;

	WorkerThread m_thread;
};

//GUI chess player does not need a thread. it just signals to human user. (user is instead of a thread.)
class GUIChessPlayer : public BasicChessPlayer	//AsyncChessPlayer
{
public:
	GUIChessPlayer(Chess* board, bool is_black) :
		/*Async*/BasicChessPlayer(board, is_black)
	{}
	virtual unsigned int /*error code*/ MakeAMove(Chess& board);
	//virtual unsigned int /*error code*/ Execute(WorkerMessage* wm);
protected:
};

class TreeBasedChessPlayer: public AsyncChessPlayer
{
public:
	TreeBasedChessPlayer(Chess* board, bool is_black, unsigned int height);
	~TreeBasedChessPlayer();
	//probably in the future tree height will be adjustable.
	const unsigned int GetTreeHeight()
	{
		return 5;//future is near
	}
protected:
	/*class BuildTreeMessage: public WorkerMessage
	{
	public:
		BuildTreeMessage(Worker* worker, bool delete_after_completion):
			WorkerMessage(worker, delete_after_completion)
		{}
	};*/

	typedef InitMessage BuildTreeMessage;

	class UpdateTreeMessage: public WorkerMessage
	{
	public:
		UpdateTreeMessage(Worker* worker, bool delete_after_completion):
			WorkerMessage(worker, delete_after_completion)
		{}
	};

	class DestroyTreeMessage: public WorkerMessage
	{
	public:
		DestroyTreeMessage(Worker* worker, bool delete_after_completion):
			WorkerMessage(worker, delete_after_completion)
		{}
	};

	class Worker : public AsyncChessPlayer::Worker
	{
		friend class TreeBasedChessPlayer;
	public:
		Worker(TreeBasedChessPlayer* bcp):
			AsyncChessPlayer::Worker(bcp),
			m_tbcp(bcp)
		{
			ASSERT(m_tbcp != NULL);
		}
		virtual unsigned int /*error code*/ Execute(WorkerMessage* wm);
		virtual unsigned int /*error code*/ MakeAMove(Chess& board);
		TreeBasedChessPlayer* m_tbcp;
	};

	//this is callback from chess board which makes player to make a move
	friend class NotificationHanlder;
	class NotificationHandler : public AbstractChessNotificationHandler
	{
	public:
		NotificationHandler(TreeBasedChessPlayer* player);
		virtual ~NotificationHandler();
		virtual void OnResetGame(Chess* chess);
		virtual void OnMove(Chess* chess, ChessPiece* piece, const ChessAddress& prev_pos, const ChessAddress& new_pos);
		virtual void OnCapture(Chess* chess,
			ChessPiece* winner,
			ChessPiece* loser);
	protected:
		TreeBasedChessPlayer* m_player;
	};

	unsigned int /*error code*/ InternalMakeAMove(Chess& board, MoveRecord& out_result);
	/*this method calculates damage realtive to ancestor_entry. damage values for exact pieces see ChessPiece::GetValue().
	if ancestor_entry == NULL, damage is calculated relative to the root.
	if ancestor entry is not in fact an ancestor to the entry, damage is calculated relative to the root.*/
	//NO GETTING BACK TO THE ROOTS HERE. JUST COMPARE TWO BOARDS!
	void CalculateDamage(ChessTree::Entry* entry, Damage& out_damage, ChessTree::Entry* ancestor_entry = NULL) const;
	MoveRecord FindTheBestMove(bool for_black);
	AsyncChessPlayer::Worker* GetMover();
	ChessTree m_tree;
	NotificationHandler m_nh;
	unsigned int m_nh_index;
	TreeBasedChessPlayer::Worker m_worker;
};
/* this is later. protocol required.
class RemoteChessPlayer : public BasicChessPlayer
{
public:
	RemoteChessPlayer();
	~RemoteChessPlayer();
	unsigned int /*error code MakeAMove(Chess& chess);
};
*/
#endif //CHESS_PLAYER_H_INCLUDED