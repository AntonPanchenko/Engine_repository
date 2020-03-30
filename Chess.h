#include "Utils.h"
#include "Synchronization.h"

#ifndef CHESS_H_INCLUDED
#define CHESS_H_INCLUDED

enum ChessErrors
{
	CH_ERR_OK,
	CH_ERROR_0 = CHESS_ERROR_BASE,
	CH_ERR_CELL_OCCUPIED,
	CH_ERR_CELL_HAS_NO_PIECE,
	CH_ERR_PIECE_CANNOT_GO_LIKE_THAT,
	CH_ERR_CANNOT_GO_OUT_OF_BOARD,
	CH_ERR_CANNOT_GO_ALREADY_THERE,
	CH_ERR_INVALID_CHESS_ADDRESS,
	CH_ERR_PIECE_NOT_ON_BOARD,
	CH_ERR_NO_PIECE_SPECIFIED,
	CH_ERR_WRONG_TURN, 
	CH_ERR_NOWHERE_TO_ADVANCE,
	CH_ERR_NO_CHESS_ADDRESS_STRING,
	CH_ERR_INVALID_CHESS_ADDRESS_STRING,
	CH_ERR_INVALID_CHESS_TYPE,
	CH_ERR_BUFFER_TOO_SMALL,
	CH_ERR_INVALID_PARAMETERS,
	CH_ERR_UNDEFINED_ERROR = -1
};

/*count of cells on both dimensions start with 1, not 0.*/

enum {
	CH_CONST_MIN_NUMBER = 1,
	CH_CONST_MIN_LETTER = 1,
	CH_CONST_MAX_NUMBER = 8,
	CH_CONST_MAX_LETTER = 8,
	CH_CONST_CELLS_COUNT = 64,
	CH_CONST_ROW_WIDTH = 8,
	CH_CONST_TOTAL_PIECES_COUNT = 16,	//for one color
	CH_CONST_INVALID_LETTER = 9,	//these are to init ChessAddress as empty (with default constructor)
	CH_CONST_INVALID_NUMBER = 9,
	CH_CONST_MAX_ADDRESS_STRING_LENGTH = 10		//this is reserved in order to implement advanced address format for
												//chess boards larger then 8x8
};

/*letters go along horizontal side of the board, numbers along the vertical side*/

/*TODO at this point there is 8 x 8 chess board. review code to allow larger chess board and new (extended)
format of chess addresses such as H12-V16 and so on where H and V stands for horizontal and vertical and followed
by decimal column and row indexes. (also see cells count and row width in the enum above)*/

enum {
	CH_LETTER_A = 1,
	CH_LETTER_B = 2,
	CH_LETTER_C = 3,
	CH_LETTER_D = 4,
	CH_LETTER_E = 5,
	CH_LETTER_F = 6,
	CH_LETTER_G = 7,
	CH_LETTER_H = 8
};

enum
{
	CH_PIECE_VALUE_PAWN = 1,
	CH_PIECE_VALUE_ROOK = 3,
	CH_PIECE_VALUE_KNIGHT = 2,
	CH_PIECE_VALUE_BISHOP = 3,
	CH_PIECE_VALUE_QUEEN = CH_PIECE_VALUE_ROOK + CH_PIECE_VALUE_BISHOP,	//because it moves like both of them
	CH_PIECE_VALUE_KING = 100
};

/*classes below are no reentrant*/

/*letters refer to columns, numbers to rows (letters go along horizontal side of the board, 
numbers along the vertical side)*/

/*chess address letters and numbers begin with 1*/

class ChessAddress
{
public:
	ChessAddress():
		m_letter(CH_CONST_INVALID_LETTER), m_number(CH_CONST_INVALID_NUMBER)
	{}
	ChessAddress(unsigned char letter, unsigned char number) :
		m_letter(letter), m_number(number)
		{
			ASSERT(m_letter > 0);
			ASSERT(m_number > 0);
			ASSERT(m_letter <= CH_CONST_MAX_LETTER);
			ASSERT(m_number <= CH_CONST_MAX_NUMBER);
		}
	ChessAddress(const wchar_t* string)
		{
			unsigned int error = FromString(string);
			if (error != CH_ERR_OK)
			{
				throw Exception(CH_ERR_INVALID_CHESS_ADDRESS,
					L"Cannot create chess address, invalid chess address string",
					EXC_HERE);
			}
		}
	ChessAddress(const char* string)
		{
			unsigned int error = FromString(string);
			if (error != CH_ERR_OK)
			{
				throw Exception(CH_ERR_INVALID_CHESS_ADDRESS,
					L"Cannot create chess address, invalid chess address string",
					EXC_HERE);
			}
		}
	inline bool operator == (const ChessAddress& another) const
	{
		if ((m_letter == another.m_letter) && (m_number == another.m_number))
		{
			return true;
		}
		return false;
	}
	inline bool operator != (const ChessAddress& another) const
	{
		if ((m_letter != another.m_letter) || (m_number != another.m_number))
		{
			return true;
		}
		return false;
	}
	inline ChessAddress& operator = (const ChessAddress& another)
	{
		m_letter = another.m_letter;
		m_number = another.m_number;
		return *this;
	}
	inline unsigned char GetLetter() const
	{
		return m_letter; 
	}
	inline void SetLetter(unsigned char letter)
	{
		//ASSERT(letter <= CH_CONST_MAX_LETTER);
		m_letter = letter;
	}
	inline bool IncrementLetter()
	{
		if (m_letter < CH_CONST_MAX_LETTER)
		{
			m_letter++;
			return true;
		} else {
			return false;
		}
	}
	inline bool DecrementLetter()
	{
		if (m_letter > 0)
		{
			m_letter--;
			return true;
		} else {
			return false;
		}
	}
	inline bool IncrementNumber()
	{
		if (m_number < CH_CONST_MAX_NUMBER)
		{
			m_number++;
			return true;
		} else {
			return false;
		}
	}
	inline bool DecrementNumber()
	{
		if (m_number > 0)
		{
			m_number--;
			return true;
		} else {
			return false;
		}
	}
	inline unsigned char GetNumber() const
	{
		return m_number;
	}
	inline void SetNumber(unsigned char number)
	{
		//ASSERT(number <= CH_CONST_MAX_NUMBER);
		m_number = number;
	}
	inline bool IsValid() const
	{
		if ((m_letter > CH_CONST_MAX_LETTER) ||
			(m_letter < CH_CONST_MIN_LETTER) ||
			(m_number > CH_CONST_MAX_NUMBER) ||
			(m_number < CH_CONST_MIN_NUMBER))
		{
			return false;
		}
		return true;
	}
	int /*length of string*/ ToString(wchar_t* str, unsigned int length) const;
	int /*length of string*/ ToString(char* str, unsigned int length) const;
	unsigned int /*error code*/ FromString(const wchar_t* string);
	unsigned int /*error code*/ FromString(const char* string);
protected:
	unsigned char m_letter;
	unsigned char m_number;
};

enum CellContentType
{
	EMPTY,
	BLACK_PAWN,
	BLACK_ROOK,
	BLACK_KNIGHT,
	BLACK_BISHOP,
	BLACK_QUEEN,
	BLACK_KING,
	WHITE_PAWN,
	WHITE_ROOK,
	WHITE_KNIGHT,
	WHITE_BISHOP,
	WHITE_QUEEN,
	WHITE_KING,
	UNDEFINED
};

const wchar_t* GetCellContentTypeString(unsigned int cct);

class MoveRecord
{
public:
	MoveRecord(const CellContentType& cct, const ChessAddress& from, const ChessAddress& to) :
		m_piece_type(cct),
		m_from(from),
		m_to(to)
	{
		if(m_from == m_to)
		{
			ASSERT(true);
		}
	}
	MoveRecord():
		m_piece_type(UNDEFINED)
	{}
	inline CellContentType& PieceType()
		 { return m_piece_type;	}
	inline ChessAddress& From()
		{ return m_from; }
	inline ChessAddress& To()
		{ return m_to; }
	inline bool IsValid() const
		{ return ((m_piece_type != UNDEFINED) && (m_from.IsValid()) && (m_to.IsValid())); }
#ifdef _DEBUG
	unsigned int ToString(wchar_t* out_str)
	{
		unsigned int length = 0;
		wchar_t* ptr = out_str;
		const wchar_t *piece_type = GetCellContentTypeString(m_piece_type);
		length = wsprintf(ptr, L"move record: %s, from: ", piece_type);
		ptr += length;
		length = m_from.ToString(ptr, 2);
		ptr += length;
		length = wsprintf(ptr, L", to: ");
		ptr += length;
		length = m_to.ToString(ptr, 2);
		ptr += length;
		length = wsprintf(ptr, L"\n");
		ptr += length;
		return length;
	}
#endif //_DEBUG
protected:
	CellContentType m_piece_type;
	ChessAddress m_from;
	ChessAddress m_to;
};

//instances of this class are used to load disposition (for instance loaded from savefile).
struct Disposition
{
public:
	Disposition()
		{
			for (unsigned int index = 0; index < CH_CONST_CELLS_COUNT; ++index)
			{
				m_board[index] = EMPTY;
			}
		}
	Disposition(const Disposition& another)
		{
			for (unsigned int index = 0; index < CH_CONST_CELLS_COUNT; ++index)
			{
				m_board[index] = another.m_board[index];
			}
		}
	//note that addresses are 1-based, e.g. 1, 2, 3, not 0, 1, 2... the same is with letters, a==1, b==2, ...
	//made_a_move parameter has a meaning for pawns only. for any other pieces it can be passed here but will have no effect.
	inline unsigned int GetPiece(const ChessAddress& address, bool* out_made_a_move = NULL) const
	{
		//const unsigned int* ptr = m_board + ((address.GetNumber() - 1) * CH_CONST_MAX_NUMBER) + address.GetLetter() - 1;
		const unsigned int* ptr = GetPiecePtr(address);
		return (*ptr & m_piece_mask);
	}
	inline bool GetMadeAMove(const ChessAddress& address) const
	{
		const unsigned int* ptr = GetPiecePtr(address);
		return (*ptr & m_made_a_move_mask);
	}
	inline void SetPiece(const ChessAddress& address, unsigned int piece, bool made_a_move = false)
	{
		unsigned int* ptr = GetPiecePtr(address);
		*ptr = piece;
		if (made_a_move)
		{
			*ptr &= m_made_a_move_mask;
		}
	}
	inline void SetMadeAMove(const ChessAddress& address, bool made_a_move)
	{
		unsigned int* ptr = GetPiecePtr(address);
		if (made_a_move)
		{
			*ptr &= m_made_a_move_mask;
		} else {
			*ptr &= m_piece_mask;
		}
	}
	inline bool GetIsBlackSideLower() const
	{
		return m_is_black_lower; 
	}
	void SetIsBlackLower(bool value)
	{
		m_is_black_lower = value;
	}
	//black - true, white - false
	inline bool GetTurn() const
	{
		return m_turn;
	}
	inline void SetTurn(bool turn)
	{
		m_turn = turn;
	}
protected:
	inline unsigned int* GetPiecePtr(const ChessAddress& address) const
	{
		return (unsigned int*)(m_board + ((address.GetNumber() - 1) * CH_CONST_MAX_NUMBER) + address.GetLetter() - 1);
	}
	const unsigned int m_made_a_move_mask = 1 << BIT_SIZEOF_INT;
	const unsigned int m_piece_mask = ~m_made_a_move_mask;
	unsigned int m_board[CH_CONST_CELLS_COUNT];
	bool m_is_black_lower;
	bool m_turn;
};

class ChessPiece;
class Chess;

class ChessCell
{
	friend class Chess;
protected:
	ChessCell():
		/*m_address(address),*/ m_piece(NULL), m_is_black(false)
	{}
	void Init(const ChessAddress& address, bool is_black)
	{
		m_address = address; m_is_black = is_black;
	}
public:
	/*ChessCell(const ChessAddress& address, bool is_black) :
		m_address(address), m_piece(NULL), m_is_black(is_black)
	{}*/

	ChessPiece* GetPiece() const
	{
		return m_piece; 
	}

	unsigned int RemovePiece(ChessPiece* piece)
	{
		ASSERT(piece != NULL);
		if (m_piece != piece)
		{
			return CH_ERR_CELL_HAS_NO_PIECE;
		}
		m_piece = NULL;
		return CH_ERR_OK;
	}

	unsigned int InsertPiece(ChessPiece* piece)
	{
		ASSERT(piece != NULL);
		if (m_piece != NULL)
		{
			return CH_ERR_CELL_OCCUPIED;
		}
		m_piece = piece;
		return CH_ERR_OK;
	}

	const ChessAddress& GetAddress() const
	{
		return m_address;
	}
protected:
	ChessAddress m_address;
	ChessPiece* m_piece;
	bool m_is_black;
};

class ChessPiece
{
public:
	class GetNextAddressContext
	{
	public:
		/*
		Open directions are those where no enemies still detected.
		Endpoint directions are those where enemies detected so piece can go only before or at the enemy (and kill it),
		bot not beyond.
		*/
		GetNextAddressContext() :
			m_open_directions(-1),	//-1 is 0xFFFFFFFF or 0xFFFFFFFFFFFFFFFF depending on architecture.
			m_endpoint_directions(0)
		{}
		//here direction is those direction in GetNextAddress methods of chess piece classes.
		inline bool GetDirectionOpen(unsigned int direction) const
		{
			unsigned int one(1);
			unsigned int bitmask = (one << direction);
			unsigned int dir_bit = m_open_directions & bitmask;
			return dir_bit != 0;//((m_open_directions & bitmask/*(1 >> direction)*/) != 0);
		}
		inline void SetDirectionOpen(unsigned int direction, bool is_open)
		{
			if (is_open)
			{
				m_open_directions |= (1 << direction);
			}
			else {
				m_open_directions &= ~(1 << direction);
			}
		}
		inline bool GetEndpointDirection(unsigned int direction) const
		{
			return ((m_endpoint_directions & (1 << direction)) != 0);
		}
		inline void SetEndpointDirection(unsigned int direction, bool is_open)
		{
			if (is_open)
			{
				m_endpoint_directions |= (1 << direction);
			}
			else {
				m_endpoint_directions &= ~(1 << direction);
			}
		}
	protected:
		unsigned int m_open_directions;
		unsigned int m_endpoint_directions;
	};

	/*It is assumed that next position iterators move througn nearest cells counter-clockwise
	6  5  4
	7  A  3 -----A is a chess piece.
	8  1  2
	9  10 11-------and so on.
	but only through the cells available for the next step.
	*/

	friend class NextPositionIterator;
	class NextPositionIterator
	{
	public:
		NextPositionIterator(const ChessPiece* piece,
							 const ChessCell* initial_position,
							 const GetNextAddressContext& src_context) :
			m_piece(piece),
			m_initial_position(initial_position),
			m_current_position(initial_position),
			m_context(src_context)
			{
				ASSERT(m_piece != NULL);
				//ASSERT(m_initial_position != NULL);
			}
		//this method moves the iterator to the next position if there is at least one available
		//position. if there is nowhere to move, it returns CH_ERROR_NOWHERE_TO_ADVANCE. 
		//(i think this is more convenient then operator ++)
		//if the chess piece is in the position where it is nowhere to move, it returns iterator with 
		//NULL current position.
		virtual unsigned int /*error code*/ Advance();
		const ChessCell* Dereference () const
			{ return m_current_position; }
		bool IsNull() const
			{ return m_current_position == NULL; }
		ChessAddress GetCurrentAddress()
		{
			if (m_current_position != NULL)
			{
				return m_current_position->GetAddress();
			} else {
				return ChessAddress();	//invalid by default
			}
		}
	protected:
		const ChessPiece* m_piece;
		const ChessCell* m_initial_position;
		const ChessCell* m_current_position;
		GetNextAddressContext m_context;
	};

	ChessPiece(Chess* chess, const ChessAddress& addr, bool is_black);
	ChessAddress GetIteratorStartAddress(GetNextAddressContext& out_context) const;
	//If address passed here is invalid address, return address must be the first address where the piece can move to.
	//If there is nowhere to move, function returns invalid address.
	//GetNextAddressContext is intened to track directions which GetNextAddress will skip. dDrections here expressed
	//in terms they are expressed in GetNextAddress implementations in descendant classes.
	virtual ChessAddress GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const = 0;

	ChessCell* GetCurrentCell() const
	{
		return m_current_cell; 
	}

	bool GetIsBlack() const
	{
		return m_is_black; 
	}

	unsigned int /*error*/ GetCurrentAddress(ChessAddress& addr) const
	{
		ChessCell* cell = GetCurrentCell();
		if (cell != NULL)
		{
			addr = cell->GetAddress();
			return CH_ERR_OK;
		}
		return CH_ERR_PIECE_NOT_ON_BOARD;
	}

	//tmp in order to find out why heap currupts
	//void* operator new(size_t size);
	//
	int ToString(wchar_t* str, unsigned int str_length) const;

	//this method calls to Chess::Capture. also it sets 'this' object to the same cell instead of captured object.
	unsigned int /*error*/ Capture(ChessPiece* another_piece);

	//in this method each chess piece class will implement it's behaviour (how it can be
	//moved and how cannot)
	//sometimes piece cannot reach the destination. so always check where_it_came
	virtual unsigned int /*error code*/ MoveTo(const ChessAddress& destination, ChessAddress& where_it_came) = 0;
	//for convenience purpose only, the iterator this function returns points to the nearest cell
	//with lower letter || number value. if the piece stands in such position that there is nowhere to go from
	//it NULL.
	//caller is responsible for iterator deletion
	//it is assumed that NextPositionIterator instance always points to some chess cell
	//available to move there. if the chess piece stands in the position from where there is nowhere to move,
	//it returns NULL.
	//since chess pieces return pointers to iterator I think it is more convenient to provide Dereference()
	//function rather then operator -> ();.
	virtual NextPositionIterator* GetNextPositionIterator() const = 0;
	enum
	{
		CAN_GO_CAN = 0,
		CAN_GO_KILL_ENEMY = 1,
		CAN_GO_CANNOT = 2
	};
	//this function is redefined in Pawn because Pawn moves in one way and captures in another way.
	virtual unsigned int /*CAN_GO_* constant*/ CanGoThere(const ChessAddress& address) const;
	virtual unsigned int GetValue() const = 0;
	virtual bool operator == (CellContentType cct) const = 0;
	inline bool operator != (CellContentType cct) const
		{ return (operator == (cct) == false); }
	virtual CellContentType GetCellContentType() const = 0;
protected:
	Chess* m_chess;
	ChessCell* m_current_cell;	//can be NULL if the piece is out of board
	bool m_is_black;
};

class Pawn : public ChessPiece		//Pawn - peshka
{
public:
	class NextPositionIterator : public ChessPiece::NextPositionIterator
	{
	public:
		NextPositionIterator(const ChessPiece* piece, const ChessCell* initial_position, const GetNextAddressContext& c):
			ChessPiece::NextPositionIterator(piece, initial_position, c)
		{}
	};

	Pawn(Chess* chess, const ChessAddress& addr, bool is_black, bool already_made_a_move = false) :
		ChessPiece(chess, addr, is_black),
		m_already_made_a_move(already_made_a_move)
	{}
	virtual ChessAddress GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const;
	unsigned int MoveTo(const ChessAddress& dest, ChessAddress& where_it_came);
	virtual ChessPiece::NextPositionIterator* GetNextPositionIterator() const;
	virtual unsigned int CanGoThere(const ChessAddress& address) const;
	unsigned int GetValue() const
		{ return CH_PIECE_VALUE_PAWN; }
	bool operator == (CellContentType cct) const;
	CellContentType GetCellContentType() const;
	bool GetAlreadyMadeAMove() const
		{ return m_already_made_a_move; }
protected:
	bool m_already_made_a_move;
};

class Rook : public ChessPiece		//Rook - ladya
{
public:
	class NextPositionIterator : public ChessPiece::NextPositionIterator
	{
	public:
		NextPositionIterator(const ChessPiece* piece, const ChessCell* initial_position, const GetNextAddressContext& c) :
			ChessPiece::NextPositionIterator(piece, initial_position, c)
		{}
	};

	Rook(Chess* chess, const ChessAddress& addr, bool is_black):
		ChessPiece(chess, addr, is_black)
	{}
	virtual ChessAddress GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const;
	unsigned int MoveTo(const ChessAddress& dest, ChessAddress& where_it_came);
	virtual ChessPiece::NextPositionIterator* GetNextPositionIterator() const;
	unsigned int GetValue() const
		{ return CH_PIECE_VALUE_ROOK; }
	bool operator == (CellContentType cct) const;
	CellContentType GetCellContentType() const;
};

class Knight : public ChessPiece	//Knight - vnezapno kon'
{
public:
	class NextPositionIterator : public ChessPiece::NextPositionIterator
	{
	public:
		NextPositionIterator(const ChessPiece* piece, const ChessCell* initial_position, const GetNextAddressContext& c) :
			ChessPiece::NextPositionIterator(piece, initial_position, c)
		{}
	};

	Knight(Chess* chess, const ChessAddress& addr, bool is_black):
		ChessPiece(chess, addr, is_black)
	{}
	virtual ChessAddress GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const;
	unsigned int MoveTo(const ChessAddress& dest, ChessAddress& where_it_came);
	virtual ChessPiece::NextPositionIterator* GetNextPositionIterator() const;
	unsigned int GetValue() const
		{ return CH_PIECE_VALUE_KNIGHT;	}
	bool operator == (CellContentType cct) const;
	virtual CellContentType GetCellContentType() const;
};

class Bishop : public ChessPiece	//Bishop - slon
{
public:
	class NextPositionIterator : public ChessPiece::NextPositionIterator
	{
	public:
		NextPositionIterator(const ChessPiece* piece, const ChessCell* initial_position, const GetNextAddressContext& c) :
			ChessPiece::NextPositionIterator(piece, initial_position, c)
		{}
	};

	Bishop(Chess* chess, const ChessAddress& addr, bool is_black):
		ChessPiece(chess, addr, is_black)
	{}
	virtual ChessAddress GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const;
	unsigned int MoveTo(const ChessAddress& dest, ChessAddress& where_it_came);
	virtual ChessPiece::NextPositionIterator* GetNextPositionIterator() const;
	unsigned int GetValue() const
		{ return CH_PIECE_VALUE_BISHOP;	}
	bool operator == (CellContentType cct) const;
	virtual CellContentType GetCellContentType() const;
};

class Queen : public ChessPiece
{
public:
	class NextPositionIterator : public ChessPiece::NextPositionIterator
	{
	public:
		NextPositionIterator(const ChessPiece* piece, const ChessCell* initial_position, const GetNextAddressContext& c) :
			ChessPiece::NextPositionIterator(piece, initial_position, c)
		{}
	};

	Queen(Chess* chess, const ChessAddress& addr, bool is_black):
		ChessPiece(chess, addr, is_black)
	{}
	virtual ChessAddress GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const;
	unsigned int MoveTo(const ChessAddress& dest, ChessAddress& where_it_came);
	virtual ChessPiece::NextPositionIterator* GetNextPositionIterator() const;
	unsigned int GetValue() const
		{ return CH_PIECE_VALUE_QUEEN;	}
	bool operator == (CellContentType cct) const;
	virtual CellContentType GetCellContentType() const;
};

class King : public ChessPiece
{
public:
	class NextPositionIterator : public ChessPiece::NextPositionIterator
	{
	public:
		NextPositionIterator(const ChessPiece* piece, const ChessCell* initial_position, const GetNextAddressContext& c) :
			ChessPiece::NextPositionIterator(piece, initial_position, c)
		{}
	};

	King(Chess* chess, const ChessAddress& addr, bool is_black):
		ChessPiece(chess, addr, is_black)
	{}
	virtual ChessAddress GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const;
	unsigned int MoveTo(const ChessAddress& dest, ChessAddress& where_it_came);
	virtual ChessPiece::NextPositionIterator* GetNextPositionIterator() const;
	unsigned int GetValue() const
		{ return CH_PIECE_VALUE_KING; }
	bool operator == (CellContentType cct) const;
	virtual CellContentType GetCellContentType() const;
};

class AbstractChessNotificationHandler
{
public:
	virtual void OnResetGame(Chess* chess) = 0;
	virtual void OnMove(Chess* chess, ChessPiece* piece, const ChessAddress& prev_pos, const ChessAddress& new_pos) = 0;
	virtual void OnCapture(Chess* chess,
		ChessPiece* winner,
		ChessPiece* loser) = 0;
};

class Chess
{
public:
	/*Chess is to be constructed in 2 steps: 1 - Chess(); 2 - ResetGame()
	ResetGame() sets pieces to initial position*/
	Chess(BasicReadWriteLock* lock = NULL);
	virtual ~Chess();
	unsigned int /*error code*/ ResetGame(bool black_side = false);
	//this is instead of copy constructor because, see above, chess is constructed in 2 steps.
	unsigned int /*error code*/ CopyGame(const Chess& chess);
	/*if any member of disposition structure is invalid (call IsValid() method of ChessAddress th check that),
	then the corresponding piece does not exist on board.*/
	unsigned int /*error code*/ LoadDisposition(const Disposition& disposition);
	unsigned int /*error code*/ SaveDisposition(Disposition& out_disposition) const;
	unsigned int /*error code*/ MovePiece(ChessPiece* piece, const ChessAddress& where, ChessAddress& where_it_came);
	//can return NULL is addr is invalid.
	ChessCell* GetCell(const ChessAddress& addr) const;
	inline ChessPiece* GetPiece(const ChessAddress& addr) const
	{
		ChessCell* cell = GetCell(addr);
		ChessPiece* ret_val = NULL;
		if (cell != NULL)
		{
			ret_val = cell->GetPiece();
		}
		return ret_val;
	}
	inline unsigned int SetPiece(const ChessAddress& address, ChessPiece* piece)
	{
		unsigned int ret_val(CH_ERR_UNDEFINED_ERROR);
		ASSERT(piece != NULL);
		ChessCell* cell = GetCell(address);
		if (cell != NULL)
		{
			ret_val = cell->InsertPiece(piece);
		}
		return ret_val;
	}
	inline bool GetIsBlackSideLower() const
	{
		return m_black_side;
	}
	unsigned int /*error code*/ CapturePiece(ChessPiece* piece);
	enum
	{
		NOTIFICATION_HANDLERS_COUNT = 32,
		INVALID_HANDLER_INDEX = -1
	};
	inline unsigned int /*handler index*/ SetNotificationHandler(AbstractChessNotificationHandler* handler)
	{
		//m_notification_handler = handler;
		unsigned int ret_val = INVALID_HANDLER_INDEX;
		for (unsigned int index = 0; index < NOTIFICATION_HANDLERS_COUNT; ++index)
		{
			if (m_notification_handlers[index] == NULL)
			{
				m_notification_handlers[index] = handler;
				ret_val = index;
				break;
			}
		}
		return ret_val;
	}
	inline void RemoveNotificationHandler(unsigned int index)
	{
		if (index < NOTIFICATION_HANDLERS_COUNT)
		{
			m_notification_handlers[index] = NULL;
		}
		for (unsigned int i = index + 1; index < NOTIFICATION_HANDLERS_COUNT; ++index)
		{
			m_notification_handlers[i - 1] = m_notification_handlers[i];
		}
	}
	unsigned int GetNotificationHandlersCount() const
	{
		for (unsigned int index = 0; index < NOTIFICATION_HANDLERS_COUNT; ++index)
		{
			if (m_notification_handlers[index] == NULL)
			{
				return index;
			}
		}
		return NOTIFICATION_HANDLERS_COUNT;
	}
	inline AbstractChessNotificationHandler* GetNotificationHandler(unsigned int index) const
	{
		if (index <= NOTIFICATION_HANDLERS_COUNT)
		{
			return m_notification_handlers[index];
		}
		return NULL;
	}
	inline bool operator == (const Chess& rval) const
		{ return Compare(rval); }
	inline bool operator != (const Chess& rval) const
		{ return !Compare(rval); }
	inline bool GetIsBlackTurn() const
		{ return m_black_turn; }
	//this is methods to be called from chess pieces on events. notification handlers are called from here.
	void OnMove(ChessPiece* piece, const ChessAddress& prev, const ChessAddress& next);
	void OnCapture(ChessPiece* winner, ChessPiece* loser);
	void OnResetGame();
private:
	void DeleteAllPieces();	//both from board and from captured arrays
	bool Compare(const Chess& rval) const;
	
	ChessCell m_cells[CH_CONST_CELLS_COUNT];
	AbstractChessNotificationHandler* m_notification_handlers[NOTIFICATION_HANDLERS_COUNT];
	bool m_black_side;	//true if black side is at lower rows, false if at higher rows.
	bool m_black_turn;	//true if it's time for black to make a move
	ChessPiece* m_captured_black[CH_CONST_TOTAL_PIECES_COUNT];
	ChessPiece* m_captured_white[CH_CONST_TOTAL_PIECES_COUNT];
	BasicReadWriteLock* m_rw_lock;
};

/*class ChessPlayer
{
public:
	ChessPlayer(Chess* chess, AbstractChessNotificationHandler& nh);
	/*this method can make a move synchronously or asynchronously depending on implementation.
	in cases where it takes time to make a next move (waiting for a peer from the network or performing a
	complicated algorithm) it will return and a move will be made later asynchronously.
	virtual unsigned int /*error code MakeAMove(Chess* chess) = 0;
protected:
	Chess* m_chess;
	AbstractChessNotificationHandler* m_notification_handler;
};*/

#endif //CHESS_H_INCLUDED