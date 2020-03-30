#ifndef CHESS_TREE_H_INCLUDED
#define CHESS_TREE_H_INCLUDED

#include "Chess.h"
#include "Utils.h"
#include "Collections.h"

enum
{
	CH_TREE_BASE = CHESS_TREE_ERROR_BASE,
	CH_ERR_CANNOT_BUILD_TREE_ALREADY_EXISTS,
	/*CH_TREE_CANNOT_ADD_ENTRY,
	CH_TREE_CANNOT_REMOVE_ENTRY,
	CH_TREE_CANNOT_ADD_ENTRY_ROOT_ALREADY_EXISTS*/
};

/*class VectorAllocator : public BasicVector::Allocator
{
public:
VectorAllocator();
virtual ~VectorAllocator();
virtual char* AllocateDataArray(unsigned int entry_size, unsigned int count) = 0;
virtual void FreeDataArray(char* data_array) = 0;
};*/

struct Damage
{
	Damage() :
		m_black_damage(0),
		m_white_damage(0)
	{}
	unsigned int m_black_damage;
	unsigned int m_white_damage;
};

class ChessTreeEntry
{
public:
	ChessTreeEntry(Chess& board)
		{
			unsigned int error = m_board.CopyGame(board);
			if (error != CH_ERR_OK)
			{
				throw Exception(error, L"Cannot copy game into ChessTreeEntry", EXC_HERE);
			}
		}
	ChessTreeEntry(const ChessTreeEntry& src) :
		m_move_record(src.m_move_record)
		{
			unsigned int error = m_board.CopyGame(src.m_board);
			if (error != CH_ERR_OK)
			{
				throw Exception(error, L"Cannot copy game into ChessTreeEntry", EXC_HERE);
			}
		}
	MoveRecord& GetMoveRecordRef()
		{ return m_move_record;	}
	Chess& GetBoardRef()
		{ return m_board; }
	/*Damage& GetDamageRef() no need of damage here at this point because damage recalculation on UpdateTree takes time.
		//better recalculate damage relative to root on search for next move, it's up to caller, CalculateDamage method 
		//is in ChessTree.
		{ return m_damage; }*/
protected:
	MoveRecord m_move_record;
	Chess m_board;
	//Damage m_damage;
};

class ChessTree: public Tree<ChessTreeEntry>//<Chess>
{
public:
	typedef Tree<ChessTreeEntry> BaseClass;
	typedef BaseClass::Entry Entry;

	ChessTree(unsigned int height);
	virtual ~ChessTree();
	unsigned int /*error code*/ BuildTree(Chess& board);
	unsigned int /*error code*/ UpdateTree(Chess& board);
	unsigned int /*error code*/ DestroyTree();
#ifdef _DEBUG
	void PrintTree();
	void PrintEntry(Entry* entry, unsigned int level, wchar_t* buffer, unsigned int length);
#endif //_DEBUG
	bool IsEmpty() const;
	//MoveRecord FindTheBestMove(bool for_black);
	static Chess& GetBoard(BasicTree::Entry* entry)
	{
		ASSERT(entry != NULL);
		ChessTree::Entry* ct_entry = dynamic_cast<ChessTree::Entry*>(entry);
		ASSERT(ct_entry != NULL);
		ChessTreeEntry& cte = (ChessTreeEntry&)(*ct_entry);
		Chess& ret_val = cte.GetBoardRef();
		return ret_val;
		//return (Chess&)(*dynamic_cast<ChessTree::Entry*>(entry));
	}
	static MoveRecord& GetMoveRecord(BasicTree::Entry* entry)
	{
		ASSERT(entry != NULL);
		ChessTree::Entry* ct_entry = dynamic_cast<ChessTree::Entry*>(entry);
		ASSERT(ct_entry != NULL);
		ChessTreeEntry& cte = (ChessTreeEntry&)(*ct_entry);
		MoveRecord& ret_val = cte.GetMoveRecordRef();
		return ret_val;
	}
	//this method returns the nearest move record (root's direct child) required to reach the direction.
	MoveRecord GetNearestMoveRecord(BasicTree::Entry* direction_entry);
	/*bool LockForRead(); //all this stuff now is in basic tree
	bool LockForWrite();
	void Unlock();*/
	bool IsInTimeConsumingOperation() const
	{
		return m_is_in_time_consuming_operation;
	}
	bool CancelTimeConsumingOperation()
	{ 
		if (m_is_in_time_consuming_operation)
		{
			m_cancel_time_consuming_operation = true;
		}
	}
protected:
	/*leftmost here means first child of its parent (--iterator made with iterator pointing at this entry makes iterator invalid)
	if some other entry is passed here, then children of previous entries will not be built.
	if entry passed here already has a children, this method returns error.
	note that this method does not unbuild the tree if some error happens. call DestroyTree in this case.*/
	unsigned int /*error code*/ BuildNextLevel(Entry* leftmost_entry, bool color /*true == black*/);
	/*here parent must not have a children, otherwise error.*/
	unsigned int /*error code*/ BuildChildren(Entry* parent, bool color /*true == black*/);
	void DestroyBranch(Entry* branch_root);
	void DestroyTopLevel(Entry* branch_root);
	unsigned int m_height;
	//this variable is to be set back to false right after time consuming operation is finished.
	bool m_cancel_time_consuming_operation;
	bool m_is_in_time_consuming_operation;
};

#endif //CHESS_TREE_H_INCLUDED