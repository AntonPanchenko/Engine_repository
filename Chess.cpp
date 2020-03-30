#include "Chess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils.h"
//
#include <Windows.h>
#include <typeinfo>

//this macro is to be used as InvokeNotificationHandlers(object, method, (parameters))
//InvokeNotificationHandlers(this, OnMove, (chess, piece, prev_pos, next_pos))
#define InvokeNotificationHandlers(object, event, params)	\
for (unsigned int index = 0; index < Chess::NOTIFICATION_HANDLERS_COUNT; ++index)	\
		{	\
		AbstractChessNotificationHandler* cnh = object->GetNotificationHandler(index); \
		if (cnh != NULL)	\
			{	\
			cnh->event params;	\
			}	\
		}

int ChessAddress::ToString(wchar_t* str, unsigned int length) const
{
	wchar_t* letter = L"-";
	switch (m_letter)
	{
	case 1: letter = L"A"; break;
	case 2: letter = L"B"; break;
	case 3: letter = L"C"; break;
	case 4: letter = L"D"; break;
	case 5: letter = L"E"; break;
	case 6: letter = L"F"; break;
	case 7: letter = L"G"; break;
	case 8: letter = L"H"; break;
	default: break;
	}
	return wsprintf(str, L"letter: %s, number: %d", letter, m_number);
}
//

int /*length of string*/ ChessAddress::ToString(char* str, unsigned int length) const
{
	if (length < 2)
	{
		return CH_ERR_BUFFER_TOO_SMALL;
	}
	char* letter = "-";
	switch (m_letter)
	{
	case CH_LETTER_A: str[0] = 'A'; break;
	case CH_LETTER_B: str[0] = 'B'; break;
	case CH_LETTER_C: str[0] = 'C'; break;
	case CH_LETTER_D: str[0] = 'D'; break;
	case CH_LETTER_E: str[0] = 'E'; break;
	case CH_LETTER_F: str[0] = 'F'; break;
	case CH_LETTER_G: str[0] = 'G'; break;
	case CH_LETTER_H: str[0] = 'H'; break;
	default: break;
	}
	switch (m_number)
	{
	case 1: str[1] = '1'; break;
	case 2: str[1] = '2'; break;
	case 3: str[1] = '3'; break;
	case 4: str[1] = '4'; break;
	case 5: str[1] = '5'; break;
	case 6: str[1] = '6'; break;
	case 7: str[1] = '7'; break;
	case 8: str[1] = '8'; break;
	default: break;
	}
	str[2] = 0;
	return 2;
	//return sprintf(str, "letter: %s, number: %d", letter, m_number);
}

unsigned int /*error code*/ ChessAddress::FromString(const wchar_t* string)
{
	if (string == NULL)
	{
		return CH_ERR_NO_CHESS_ADDRESS_STRING;
	}
	unsigned int letter(CH_CONST_INVALID_LETTER);
	switch (string[0])
	{
	case L'A': letter = CH_LETTER_A; break;
	case L'B': letter = CH_LETTER_B; break;
	case L'C': letter = CH_LETTER_C; break;
	case L'D': letter = CH_LETTER_D; break;
	case L'E': letter = CH_LETTER_E; break;
	case L'F': letter = CH_LETTER_F; break;
	case L'G': letter = CH_LETTER_G; break;
	case L'H': letter = CH_LETTER_H; break;
	default: return CH_ERR_INVALID_CHESS_ADDRESS_STRING;
	}
	unsigned int number(CH_CONST_INVALID_NUMBER);
	switch (string[1])
	{
	case L'1': number = 1; break;
	case L'2': number = 2; break;
	case L'3': number = 3; break;
	case L'4': number = 4; break;
	case L'5': number = 5; break;
	case L'6': number = 6; break;
	case L'7': number = 7; break;
	case L'8': number = 8; break;
	default: return CH_ERR_INVALID_CHESS_ADDRESS_STRING;
	}
	m_letter = letter;
	m_number = number;
	return CH_ERR_OK;
}

unsigned int /*error code*/ ChessAddress::FromString(const char* string)
{
	if (string == NULL)
	{
		return CH_ERR_NO_CHESS_ADDRESS_STRING;
	}
	unsigned int letter(CH_CONST_INVALID_LETTER);
	switch (string[0])
	{
	case 'A': letter = CH_LETTER_A; break;
	case 'B': letter = CH_LETTER_B; break;
	case 'C': letter = CH_LETTER_C; break;
	case 'D': letter = CH_LETTER_D; break;
	case 'E': letter = CH_LETTER_E; break;
	case 'F': letter = CH_LETTER_F; break;
	case 'G': letter = CH_LETTER_G; break;
	case 'H': letter = CH_LETTER_H; break;
	default: return CH_ERR_INVALID_CHESS_ADDRESS_STRING;
	}
	unsigned int number(CH_CONST_INVALID_NUMBER);
	switch (string[1])
	{
	case '1': number = 1; break;
	case '2': number = 2; break;
	case '3': number = 3; break;
	case '4': number = 4; break;
	case '5': number = 5; break;
	case '6': number = 6; break;
	case '7': number = 7; break;
	case '8': number = 8; break;
	default: return CH_ERR_INVALID_CHESS_ADDRESS_STRING;
	}
	m_letter = letter;
	m_number = number;
	return CH_ERR_OK;
}

const wchar_t* GetCellContentTypeString(unsigned int cct)
{
	switch (cct)
	{
	case EMPTY: return L"Empty";
	case BLACK_PAWN: return L"Black pawn";
	case BLACK_ROOK: return L"Black Rook";
	case BLACK_KNIGHT: return L"Black Knight";
	case BLACK_BISHOP: return L"Black Bishop";
	case BLACK_QUEEN: return L"Black Queen";
	case BLACK_KING: return L"Black King";
	case WHITE_PAWN: return L"White Pawn";
	case WHITE_ROOK: return L"White Rook";
	case WHITE_KNIGHT: return L"White Knight";
	case WHITE_BISHOP: return L"White Bishop";
	case WHITE_QUEEN: return L"White Queen";
	case WHITE_KING: return L"White King";
	case UNDEFINED: 
	default: return L"Undefined";
	}
}

unsigned int ChessPiece::NextPositionIterator::Advance()
{
	ASSERT(m_piece->m_chess != NULL);
	unsigned int ret_val = CH_ERR_OK;
	//OutputDebugMsg(L"Advance begin---------\n");
	ChessAddress current_address = m_current_position->GetAddress();
	bool exit_flag = false;
	while (exit_flag == false)
	{
		/*
		wchar_t debug_msg[0xFF];
		wchar_t* ptr = debug_msg + swprintf(debug_msg, L"NextPosIt::Advance: current_address==");
		ptr += current_address.ToString(ptr, sizeof(debug_msg) - 1);
		*ptr = L'\n';
		OutputDebugMsg(debug_msg);
		*/
		ChessAddress next_address = m_piece->GetNextAddress(current_address, m_context);
		if (next_address.IsValid() == false)
		{
			ret_val = CH_ERR_NOWHERE_TO_ADVANCE;
			exit_flag = true;
			break;
		}
		ChessPiece* piece = m_piece->m_chess->GetPiece(next_address);
		if (piece == NULL)	//empty cell, ok to move there
		{
			exit_flag = true;
			m_current_position = m_piece->m_chess->GetCell(next_address);
			ret_val = CH_ERR_OK;
			break;
		}
		if (piece->GetIsBlack() != m_piece->GetIsBlack())
		{	//enemy piece, ok to move there and kill it
			exit_flag = true;
			m_current_position = m_piece->m_chess->GetCell(next_address);
			ret_val = CH_ERR_OK;
			break;
		}
		current_address = next_address;
	}
	//OutputDebugMsg(L"Advance end---------\n");
	return ret_val;
}

ChessPiece::ChessPiece(Chess* chess, const ChessAddress& addr, bool is_black) :
	m_chess(chess), m_current_cell(NULL), m_is_black(is_black)
{
	ASSERT(m_chess != NULL);
	m_current_cell = m_chess->GetCell(addr);
	ASSERT(m_current_cell != NULL);
	ASSERT(m_current_cell->GetPiece() == NULL);
}

ChessAddress ChessPiece::GetIteratorStartAddress(GetNextAddressContext& out_context) const
{
	ChessAddress address;
	bool exit_flag = false;
	while (exit_flag == false)
	{
		address = GetNextAddress(address, out_context);
		if (address.IsValid())
		{
			ChessPiece* piece = m_chess->GetPiece(address);
			if (piece == NULL)
			{
				exit_flag = true;
			}
			else {
				if (piece->GetIsBlack() != GetIsBlack())
				{
					exit_flag = true;
				}
			}
		}
		else {
			exit_flag = true;
		}
	}
	return address;
}

unsigned int /*error*/ ChessPiece::Capture(ChessPiece* another_piece)
{
	ASSERT(another_piece != NULL);
	unsigned int error = m_chess->CapturePiece(another_piece);
	if (error == CH_ERR_OK)
	{
		m_chess->OnCapture(this, another_piece);
	}
	return error;
}

unsigned int ChessPiece::CanGoThere(const ChessAddress& address) const
{
	if (address.IsValid() == false)
	{
		return CAN_GO_CANNOT;
	}
	ChessPiece* piece = m_chess->GetPiece(address);
	if (piece != NULL)
	{
		if (piece->GetIsBlack() != GetIsBlack())
		{
			return CAN_GO_KILL_ENEMY;
		} else {
			return CAN_GO_CANNOT;
		}
	} else {
		return CAN_GO_CAN;
	}
	return CAN_GO_CANNOT;
}

/*void* ChessPiece::operator new(size_t size)
{
	static HANDLE heap(NULL);
	if (heap == NULL)
	{
		heap = HeapCreate(0, 0, 0);
	}
	void* ret_val = HeapAlloc(heap GetProcessHeap() , HEAP_ZERO_MEMORY, size);
	if (ret_val == NULL)
	{
		int error = GetLastError();
		wchar_t errmsg[50];
		wsprintf(errmsg, L"heap error: %L", error);
		OutputDebugString(errmsg);
	}
	return ret_val;
}*/

int ChessPiece::ToString(wchar_t* str, unsigned int str_length) const
{
	const wchar_t* piece_type = NULL;
	const wchar_t* color = NULL;
	if (typeid(*this) == typeid(Pawn))
	{
		piece_type = L"Pawn";
	}
	else if (typeid(*this) == (typeid(Rook)))
	{
		piece_type = L"Rook";
	}
	else if (typeid(*this) == typeid(Knight))
	{
		piece_type = L"Knight";
	}
	else if (typeid(*this) == typeid(Bishop))
	{
		piece_type = L"Bishop";
	}
	else if (typeid(*this) == typeid(Queen))
	{
		piece_type = L"Queen";
	}
	else if (typeid(*this) == typeid(King))
	{
		piece_type = L"King";
	}
	if (m_is_black)
	{
		color = L"Black";
	} else 
	{
		color = L"White";
	}
	ASSERT(piece_type != NULL);
	ASSERT(color != NULL);
	int piece_type_length = wcslen(piece_type);
	int color_length = wcslen(color);
	int current_offset = 0;
	int total_length = color_length + 1 + piece_type_length + 1;
	if (str_length < total_length)
	{
		return 0;
	}
	wcscpy(str, color);
	current_offset += color_length;
	str[current_offset] = L'-';
	++current_offset;
	wcscpy(str + current_offset, piece_type);
	current_offset += piece_type_length;
	//str[current_offset] = 0;
	//++current_offset;
	return current_offset;
}

unsigned int Pawn::MoveTo(const ChessAddress& dest, ChessAddress& where_it_came)
{
	unsigned int ret_val = CH_ERR_OK;
	if(dest.IsValid() == false)
	{
		return CH_ERR_INVALID_CHESS_ADDRESS;
	}
	ChessCell* current_cell = GetCurrentCell();
	ASSERT(current_cell != NULL);
	const ChessAddress curr_address = current_cell->GetAddress();
	ChessCell* another_cell = m_chess->GetCell(dest);
	ChessPiece* another_piece = NULL;
	if (dest.GetLetter() == curr_address.GetLetter())
	{
		  if (m_chess->GetIsBlackSideLower())
		{
			if (GetIsBlack() == true)
			{
				if (dest.GetNumber() == curr_address.GetNumber() - 1)
				{
					another_piece = m_chess->GetPiece(dest);
					if (another_piece != NULL)
					{
						another_piece = NULL;
						return CH_ERR_CELL_OCCUPIED;
					}
				}
				else if ((dest.GetNumber() == curr_address.GetNumber() - 2) && 
					(dest.GetNumber() == 5) &&
					(m_already_made_a_move == false))
				{
					another_piece = m_chess->GetPiece(dest);
					if (another_piece != NULL)
					{
						return CH_ERR_CELL_OCCUPIED;
					}
					//else ok, go on
				} else {
					return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
				}
			} else {
				if (dest.GetNumber() == curr_address.GetNumber() + 1)
				{
					another_piece = m_chess->GetPiece(dest);
					if (another_piece != NULL)
					{
						another_piece = NULL;
						return CH_ERR_CELL_OCCUPIED;
					}
				} else if ((dest.GetNumber() == curr_address.GetNumber() + 2) && 
					(dest.GetNumber() == 4) &&
					(m_already_made_a_move == false))
				{
					another_piece = m_chess->GetPiece(dest);
					if (another_piece != NULL)
					{
						another_piece = NULL;
						return CH_ERR_CELL_OCCUPIED;
					}
					//else ok, go on
				} else {
					return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
				}
			}
		  }
		  else {
			  if (GetIsBlack() == false)
			  {
				  if (dest.GetNumber() == curr_address.GetNumber() - 1)
				  {
					  another_piece = m_chess->GetPiece(dest);
					  if (another_piece != NULL)
					  {
						  another_piece = NULL;
						  return CH_ERR_CELL_OCCUPIED;
					  }
				  }
				  else if ((dest.GetNumber() == curr_address.GetNumber() - 2) &&
					  (dest.GetNumber() == 5) &&
					  (m_already_made_a_move == false))
				  {
					  another_piece = m_chess->GetPiece(dest);
					  if (another_piece != NULL)
					  {
						  return CH_ERR_CELL_OCCUPIED;
					  }
					  //else ok, go on
				  }
				  else {
					  return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
				  }
			  }
			  else {
				  if (dest.GetNumber() == curr_address.GetNumber() + 1)
				  {
					  another_piece = m_chess->GetPiece(dest);
					  if (another_piece != NULL)
					  {
						  another_piece = NULL;
						  return CH_ERR_CELL_OCCUPIED;
					  }
				  }
				  else if ((dest.GetNumber() == curr_address.GetNumber() + 2) &&
					  (dest.GetNumber() == 4) &&
					  (m_already_made_a_move == false))
				  {
					  another_piece = m_chess->GetPiece(dest);
					  if (another_piece != NULL)
					  {
						  another_piece = NULL;
						  return CH_ERR_CELL_OCCUPIED;
					  }
					  //else ok, go on
				  }
				  else {
					  return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
				  }
			  }
		}
		//move the piece
		ret_val = current_cell->RemovePiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = NULL;
		ret_val = another_cell->InsertPiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = another_cell;
		where_it_came = dest;
		m_already_made_a_move = true;
	} else {
		//capture
		if (((dest.GetLetter() == curr_address.GetLetter() + 1) ||
			(dest.GetLetter() == curr_address.GetLetter() - 1)) &&
			(dest.GetNumber() == curr_address.GetNumber() + 1) ||
			(dest.GetNumber() == curr_address.GetNumber() - 1))
		{	//diagonal move, kill the enemy piece
			another_piece = m_chess->GetPiece(dest);
			if (another_piece != NULL)
			{
				if (another_piece->GetIsBlack() != GetIsBlack())
				{
					ret_val = Capture(another_piece);
				} else {
					return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
				}
			} else {
				return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
			}
			ret_val = current_cell->RemovePiece(this);
			if (ret_val != CH_ERR_OK)
			{
				return ret_val;
			}
			m_current_cell = NULL;
			ret_val = another_cell->InsertPiece(this);
			if (ret_val != CH_ERR_OK)
			{
				return ret_val;
			}
			m_current_cell = another_cell;
			where_it_came = dest;
			m_already_made_a_move = true;
		}
		else {
			return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
		}
	}
	GetCurrentAddress(where_it_came);
	m_chess->OnMove(this, curr_address, where_it_came);
	return ret_val;
}

ChessAddress Pawn::GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const
{
	bool is_black = GetIsBlack();
	//ASSERT(m_current_position != NULL);
	ChessAddress piece_address;
	ChessAddress next_address;
	enum
	{
		INCR_NUMBER,
		DECR_NUMBER,
		INCR_LETTER_INCR_NUMBER,
		INCR_LETTER_DECR_NUMBER,
		DECR_LETTER_INCR_NUMBER,
		DECR_LETTER_DECR_NUMBER,
		INCR_NUMBER_2,	/*these both are for the first step whicn can be 2 cells long*/
		DECR_NUMBER_2,
		UNSPECIFIED
	};
	unsigned int moveto(UNSPECIFIED);
	unsigned int error = GetCurrentAddress(piece_address);
	if (error != CH_ERR_OK)
	{
		throw Exception(error, L"Cannot get piece address", EXC_HERE);
	}
	ChessAddress current_address = address;// m_current_position->GetAddress();
	Chess* chess = m_chess;
	ASSERT(chess != NULL);
	bool moving_upward = false;
	/*if ((chess->GetIsBlackSideLower()) && (is_black))
	{
		moving_upward = true;
	}*/
	if (chess->GetIsBlackSideLower())
	{
		if (is_black)
		{
			moving_upward = true;
		} else {
			moving_upward = false;
		}
	} else {
		if (is_black)
		{
			moving_upward = false;
		} else {
			moving_upward = true;
		}
	}
	//upward here means up on the board as it is displayed, e.g. towards lower numbers.
	//return the first available address
	if (address.IsValid() == false)
	{
		if (moving_upward == false)
		{
			if (m_already_made_a_move == false)
			{
				moveto = INCR_NUMBER_2;
			}
			else {
				moveto = INCR_NUMBER;
			}
		} else {
			if (m_already_made_a_move == false)
			{
				moveto = DECR_NUMBER_2;
			}
			else {
				moveto = DECR_NUMBER;
			}
		}
	}
	else if ((address.GetLetter() == piece_address.GetLetter()) &&
		(address.GetNumber() == piece_address.GetNumber() + 2) &&
		(moving_upward == false))
	{
		moveto = INCR_NUMBER;
	}
	else if ((address.GetLetter() == piece_address.GetLetter()) &&
		(address.GetNumber() == piece_address.GetNumber() - 2) &&
		(moving_upward == true))
	{
		moveto = DECR_NUMBER;
	}
	else if (current_address.GetLetter() == piece_address.GetLetter())
	{	//this is the initial position as it is set in GetNextPositionIterator()
		moveto = INCR_LETTER_INCR_NUMBER;
	}
	else if ((current_address.GetLetter() == (piece_address.GetLetter() + 1)) &&
		(current_address.GetNumber() == (piece_address.GetNumber() + 1)))
	{
		moveto = INCR_LETTER_DECR_NUMBER;
	}
	else if ((current_address.GetLetter() == (piece_address.GetLetter() + 1)) &&
		(current_address.GetNumber() == (piece_address.GetNumber() - 1)))
	{
		moveto = DECR_LETTER_DECR_NUMBER;
	}
	else if ((current_address.GetLetter() == (piece_address.GetLetter() - 1)) &&
		(current_address.GetNumber() == (piece_address.GetNumber() - 1)))
	{
		moveto = DECR_LETTER_INCR_NUMBER;
	}
	else 
	{
		return ChessAddress();
	}
	switch (moveto)
	{
	case INCR_NUMBER_2:
		if (m_already_made_a_move == false)
		{
			next_address = piece_address;
			next_address.SetNumber(piece_address.GetNumber() + 2);
			if (next_address.IsValid() == true)
			{
				if (CanGoThere(next_address) == CAN_GO_CAN)
				{
					ChessAddress on_the_way = piece_address;
					on_the_way.SetNumber(piece_address.GetNumber() + 1);
					if (CanGoThere(on_the_way) == CAN_GO_CAN)
					{
						return next_address;
					}
				}
			}
		}
	case INCR_NUMBER:
		next_address = piece_address;
		if (next_address.IncrementNumber() == true)
		{
			ChessPiece* piece = chess->GetPiece(next_address);
			if (CanGoThere(next_address) == CAN_GO_CAN)
			{
				return next_address;
			}
		}
		goto DIAGONAL_NEIGHBOUR_CELLS;
	case DECR_NUMBER_2:
		if (m_already_made_a_move == false)
		{
			next_address = piece_address;
			next_address.SetNumber(next_address.GetNumber() - 2);
			if (next_address.IsValid() == true)
			{
				if (CanGoThere(next_address) == CAN_GO_CAN)
				{
					ChessAddress on_the_way = piece_address;
					on_the_way.SetNumber(piece_address.GetNumber() - 1);
					if (CanGoThere(on_the_way) == CAN_GO_CAN)
					{
						return next_address;
					}
				}
			}
		}
	case DECR_NUMBER:
		next_address = piece_address;
		if (next_address.DecrementNumber() == true)
		{
			if (CanGoThere(next_address) == CAN_GO_CAN)
			{
				return next_address;
			}
		}
	case INCR_LETTER_INCR_NUMBER:
		DIAGONAL_NEIGHBOUR_CELLS:
		next_address = piece_address;
		if (next_address.IncrementLetter() == true)
		{
			if (next_address.IncrementNumber() == true)
			{
				if (CanGoThere(next_address) == CAN_GO_KILL_ENEMY)
				{
					return next_address;
				}
			}
		}
	case INCR_LETTER_DECR_NUMBER:
		next_address = piece_address;
		if (next_address.IncrementLetter() == true)
		{
			if (next_address.DecrementNumber() == true)
			{
				if (CanGoThere(next_address) == CAN_GO_KILL_ENEMY)
				{
					return next_address;
				}
			}
		}
	case DECR_LETTER_DECR_NUMBER:
		next_address = piece_address;
		if (next_address.DecrementLetter() == true)
		{
			if (next_address.DecrementNumber() == true)
			{
				if (CanGoThere(next_address) == CAN_GO_KILL_ENEMY)
				{
					return next_address;
				}
			}
		}
	case DECR_LETTER_INCR_NUMBER:
		next_address = piece_address;
		if (next_address.DecrementLetter() == true)
		{
			if (next_address.IncrementNumber() == true)
			{
				if (CanGoThere(next_address) == CAN_GO_KILL_ENEMY)
				{
					return next_address;
				}
			}
		}
	case UNSPECIFIED:
	default:
		return ChessAddress(); //invalid as created by default constructor.
	}
}

ChessPiece::NextPositionIterator* Pawn::GetNextPositionIterator() const
{
	ChessPiece::NextPositionIterator* ret_val = NULL;
	//return iterator pointing to the next forward cell
	ASSERT(m_current_cell != NULL);
	ChessAddress next_address = m_current_cell->GetAddress();
	bool direction_up = false;
	//note that upwards are lower numbers on board.
	if (m_chess->GetIsBlackSideLower())
	{
		if (GetIsBlack())
		{
			direction_up = true;
		} else {
			direction_up = false;
		}
	} else {
		if (GetIsBlack())
		{
			direction_up = false;
		} else {
			direction_up = true;
		}
	}
	if (direction_up)
	{
		if (m_already_made_a_move == false)
		{
			next_address.DecrementNumber();
		}
		next_address.DecrementNumber();
	}
	else {
		if (m_already_made_a_move == false)
		{
			next_address.IncrementNumber();
		}
		next_address.IncrementNumber();
	}
	ChessPiece::GetNextAddressContext context;
	int distance = next_address.GetNumber() - m_current_cell->GetAddress().GetNumber();
	unsigned int abs_distance = abs(distance);
	switch (abs_distance)
	{
	case 1: break;
	case 2:
		//this is about skipping initial 2-cell move (if there something stands between).
		if (distance > 0)
		{
			ChessAddress next_cell = m_current_cell->GetAddress();
			bool ok = next_cell.IncrementNumber();
			ASSERT(ok != false);
			if (CanGoThere(next_cell) != CAN_GO_CAN)
			{
				next_address = GetNextAddress(next_address, context);
			}
		} else {
			ChessAddress next_cell = m_current_cell->GetAddress();
			bool ok = next_cell.DecrementNumber();
			ASSERT(ok != false);
			if (CanGoThere(next_cell) != CAN_GO_CAN)
			{
				next_address = GetNextAddress(next_address, context);
			}
		}
		break;
	default:
		ASSERT(false);
	}
	bool exit_flag = false;
	while (exit_flag == false)
	{
		switch (CanGoThere(next_address))
		{
		case CAN_GO_CAN:
			exit_flag = true;
			continue;
		case CAN_GO_KILL_ENEMY:
			exit_flag = true;
			continue;
		case CAN_GO_CANNOT:;
		}
		next_address = GetNextAddress(next_address, context);
		if (next_address.IsValid() == false)
		{
			exit_flag = true;
		}
	}
	if (next_address.IsValid())
	{
		ChessCell* init_pos = m_chess->GetCell(next_address);
		ret_val = new Pawn::NextPositionIterator(this, init_pos, context);
	}
	return ret_val;
}

unsigned int Pawn::CanGoThere(const ChessAddress& dest_address) const
{
	wchar_t dbgstr[0xFF];
	memset(dbgstr, 0, sizeof(dbgstr));
	if (dest_address.IsValid() == false)
	{
		return false;
	}
	ChessAddress current_address = m_current_cell->GetAddress();
	ChessPiece* another_piece = m_chess->GetPiece(dest_address);
	if (dest_address.GetLetter() != current_address.GetLetter())
	{
		if (another_piece != NULL)
		{
			if (another_piece->GetIsBlack() != GetIsBlack())
			{
				return CAN_GO_KILL_ENEMY;
			} else {
				return CAN_GO_CANNOT;
			}
		}
	}
	else {
		if (another_piece != NULL)
		{
			return CAN_GO_CANNOT;
		} else {
			return CAN_GO_CAN;
		}
	}
	return false;
}

bool Pawn::operator == (CellContentType cct) const
{
	if (GetIsBlack())
	{
		if (cct == BLACK_PAWN)
		{
			return true;
		}
	}
	else {
		if (cct == WHITE_PAWN)
		{
			return true;
		}
	}
	return false;
}

CellContentType Pawn::GetCellContentType() const
{
	if(GetIsBlack())
	{
		return BLACK_PAWN;
	} else {
		return WHITE_PAWN;
	}
}

unsigned int Rook::MoveTo(const ChessAddress& dest, ChessAddress& where_it_came)
{
	unsigned int ret_val = CH_ERR_OK;
	if (dest.IsValid() == false)
	{
		return CH_ERR_INVALID_CHESS_ADDRESS;
	}
	ChessCell* current_cell = GetCurrentCell();
	ASSERT(current_cell != NULL);
	const ChessAddress curr_address = current_cell->GetAddress();
	//1. is dest address on the same row or line as current?
	bool horizontal_move = false;
	bool vertical_move = false;
	if (dest.GetLetter() == curr_address.GetLetter())
	{
		vertical_move = true;
	}
	else if (dest.GetNumber() == curr_address.GetNumber())
	{
		horizontal_move = true;
	}
	ChessPiece* someone_on_the_way = NULL;
	bool go_on = true;
	if (vertical_move == true)
	{
		//2. is there someone on the way?
		if (dest.GetNumber() > curr_address.GetNumber())
		{
			//way up
			ChessAddress addr = curr_address;
			while (go_on == true)
			{
				if ((addr.GetNumber() < CH_CONST_MAX_NUMBER) && (addr.GetNumber() < dest.GetNumber()))
				{
					bool ok = addr.IncrementNumber();
					ASSERT(ok == true);
				} else {
					go_on = false;
					break;
				}
				ChessCell* cell = m_chess->GetCell(addr);
				ASSERT(cell != NULL);
				someone_on_the_way = cell->GetPiece();
				if (someone_on_the_way != NULL)
				{
					go_on = false;
					break;
				}
			}
		}
		else if (dest.GetNumber() < curr_address.GetNumber())
		{ 
			//way down
			ChessAddress addr = curr_address;
			while (go_on == true)
			{
				if ((addr.GetNumber() > 0) && (addr.GetNumber() > dest.GetNumber()))
				{
					bool ok = addr.DecrementNumber();
					ASSERT(ok == true);
				}
				else {
					go_on = false;
					break;
				}
				ChessCell* cell = m_chess->GetCell(addr);
				ASSERT(cell != NULL);
				someone_on_the_way = cell->GetPiece();
				if (someone_on_the_way != NULL)
				{
					go_on = false;
					break;
				}
			}
		}
		else {
			return CH_ERR_CANNOT_GO_ALREADY_THERE;
		}
	} else if (horizontal_move == true)
	{
		//2. is there someone on the way?
		ChessAddress addr = curr_address;
		if (dest.GetLetter() > curr_address.GetLetter())
		{
			while (go_on == true)
			{
				if ((addr.GetLetter() < CH_CONST_MAX_LETTER) && (addr.GetLetter() < dest.GetLetter()))
				{
					bool ok = addr.IncrementLetter();
					ASSERT(ok == true);
				} else {
					go_on = false;
					break;
				}
				ChessCell* cell = m_chess->GetCell(addr);
				ASSERT(cell != NULL);
				someone_on_the_way = cell->GetPiece();
				if (someone_on_the_way != NULL)
				{
					go_on = false;
					break;
				}
			}
		} else {
			while (go_on == true)
			{
				if (((addr.GetLetter()) > 0) && (addr.GetLetter() > dest.GetLetter()))
				{
					bool ok = addr.DecrementLetter();
					ASSERT(ok == true);
				}
				else {
					go_on = false;
					break;
				}
				ChessCell* cell = m_chess->GetCell(addr);
				ASSERT(cell != NULL);
				someone_on_the_way = cell->GetPiece();
				if (someone_on_the_way != NULL)
				{
					go_on = false;
					break;
				}
			}
		}
	} else {
		return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
	}
	if (someone_on_the_way != NULL)
	{
		if (someone_on_the_way->GetIsBlack() != GetIsBlack())
		{
			ChessCell* target_cell = someone_on_the_way->GetCurrentCell();
			ASSERT(target_cell != NULL);
			where_it_came = target_cell->GetAddress();
			ret_val = Capture(someone_on_the_way);
			if (ret_val == CH_ERR_OK)
			{
				m_chess->OnCapture(this, someone_on_the_way);
				ret_val = current_cell->RemovePiece(this);
				if (ret_val != CH_ERR_OK)
				{
					return ret_val;
				}
				ChessCell* prev_current = m_current_cell;
				m_current_cell = NULL;
				ret_val = target_cell->InsertPiece(this);
				if (ret_val != CH_ERR_OK)
				{
					return ret_val;
				}
				m_current_cell = target_cell;
				GetCurrentAddress(where_it_came);
				m_chess->OnMove(this, prev_current->GetAddress(), target_cell->GetAddress());
			}
		}
		else {
			return CH_ERR_CELL_OCCUPIED;
		}
	} else {
		ChessCell* next_cell = m_chess->GetCell(dest);
		ASSERT(next_cell != NULL);
		ChessCell* prev_current = m_current_cell;	//this variable is needed just to pass it to notification handler
		ret_val = current_cell->RemovePiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = NULL;
		ret_val = next_cell->InsertPiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = next_cell;
		GetCurrentAddress(where_it_came);
		m_chess->OnMove(this, prev_current->GetAddress(), dest);
		where_it_came = dest;
		ret_val = CH_ERR_OK;
	}
	return ret_val;
}

ChessAddress Rook::GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const
{
	ChessAddress piece_address;
	unsigned int error = GetCurrentAddress(piece_address);
	if(error != CH_ERR_OK)
	{
		throw Exception(error, TEXT("Piece not on board"), EXC_HERE);
	}
	int letter_distance(0);
	int number_distance(0);
	if (address.IsValid() == false)
	{	//get the first address
		letter_distance = 0;
		number_distance = 0;
	}
	else {
		letter_distance = address.GetLetter() - piece_address.GetLetter();
		number_distance = address.GetNumber() - piece_address.GetNumber();
	}
	unsigned int abs_letter_distance = abs(letter_distance);
	unsigned int distance = abs_letter_distance;
	if (distance == 0)
	{
		distance = abs(number_distance);
	}
	ChessAddress ret_val;
	enum
	{
		ROOK_UNSPECIFIED = 0,
		ROOK_NUMBER_DOWN,
		ROOK_LETTER_UP,
		ROOK_NUMBER_UP,
		ROOK_LETTER_DOWN,
	};
	enum
	{
		ALL_OPEN = 0,
		STOP_DIR_DOWN = 1,
		STOP_DIR_UP = 2,
		STOP_DIR_LEFT = 4,
		STOP_DIR_RIGHT = 8
	};
	unsigned int stop_dir(ALL_OPEN);
	unsigned int moveto(ROOK_UNSPECIFIED);
	if ((letter_distance == 0) && (number_distance == 0))
	{	//this is initial case
		moveto = ROOK_NUMBER_UP;
		++distance;
	}
	if ((letter_distance > 0) && (number_distance == 0))
	{
		moveto = ROOK_NUMBER_DOWN;
	}
	else if ((letter_distance == 0) && (number_distance > 0))
	{
		moveto = ROOK_LETTER_UP;
	}
	else if ((letter_distance < 0) && (number_distance == 0))
	{
		moveto = ROOK_NUMBER_UP;
		++distance;
	}
	else if ((letter_distance == 0) && (number_distance < 0))
	{
		moveto = ROOK_LETTER_DOWN;
	}
	unsigned int max_letter_distance = max(piece_address.GetLetter() - 1, CH_CONST_MAX_LETTER - piece_address.GetLetter());
	unsigned int max_number_distance = max(piece_address.GetNumber() - 1, CH_CONST_MAX_NUMBER - piece_address.GetNumber());
	bool increase_distance(false);
	bool exit_flag = false;
	while (exit_flag == false)
	{
		int new_value(0);
		switch (moveto)
		{
			//cases here must be strictly in that order as iterator moves through cells with the same distance. (counter-clockwise)
		case ROOK_NUMBER_UP:
			if (context.GetEndpointDirection(ROOK_NUMBER_UP))
			{
				context.SetDirectionOpen(ROOK_NUMBER_UP, false);
			}
			if (context.GetDirectionOpen(ROOK_NUMBER_UP))
			{
				new_value = piece_address.GetNumber() + distance;
				if (new_value <= CH_CONST_MAX_NUMBER)
				{
					ret_val.SetLetter(piece_address.GetLetter());
					ret_val.SetNumber(new_value);
					if (ret_val.IsValid())
					{
						switch (CanGoThere(ret_val))
						{
						case CAN_GO_CAN:
							context.SetDirectionOpen(ROOK_NUMBER_UP, true);
							return ret_val;
						case CAN_GO_KILL_ENEMY:
							//mark this direction so not to come back to it.
							context.SetEndpointDirection(ROOK_NUMBER_UP, true);
							return ret_val;
						case CAN_GO_CANNOT:
							context.SetDirectionOpen(ROOK_NUMBER_UP, false);
							break;
						default:
							ASSERT(false);
						}
						return ret_val;
					}
				}
			}
		case ROOK_LETTER_UP:
			if (context.GetEndpointDirection(ROOK_LETTER_UP))
			{
				context.SetDirectionOpen(ROOK_LETTER_UP, false);
			}
			if (context.GetDirectionOpen(ROOK_LETTER_UP))
			{
				new_value = piece_address.GetLetter() + distance;
				if (new_value <= CH_CONST_MAX_LETTER)
				{
					ret_val.SetLetter(new_value);
					ret_val.SetNumber(piece_address.GetNumber());
					if (ret_val.IsValid())
					{
						switch (CanGoThere(ret_val))
						{
						case CAN_GO_CAN:
							context.SetDirectionOpen(ROOK_LETTER_UP, true);
							return ret_val;
						case CAN_GO_KILL_ENEMY:
							//mark this direction so not to come back to it.
							context.SetEndpointDirection(ROOK_LETTER_UP, true);
							return ret_val;
						case CAN_GO_CANNOT:
							context.SetDirectionOpen(ROOK_LETTER_UP, false);
							break;
						default:
							ASSERT(false);
						}
						//return ret_val;
					}
				}
			}
		case ROOK_NUMBER_DOWN:
			if (context.GetEndpointDirection(ROOK_NUMBER_DOWN))
			{
				context.SetDirectionOpen(ROOK_NUMBER_DOWN, false);
			}
			if (context.GetDirectionOpen(ROOK_NUMBER_DOWN))
			{
				if (distance == 0) //+1 is because this is the first direction.
								   //control comes here when distance == 0.
				{
					++distance;
				}
				new_value = piece_address.GetNumber() - distance; 
				if (new_value >= CH_CONST_MIN_NUMBER)
				{
					ret_val.SetLetter(piece_address.GetLetter());
					ret_val.SetNumber(new_value);
					if (ret_val.IsValid())
					{
						switch (CanGoThere(ret_val))
						{
						case CAN_GO_CAN:
							context.SetDirectionOpen(ROOK_NUMBER_DOWN, true);
							return ret_val;
						case CAN_GO_KILL_ENEMY:
							//mark this direction so not to come back to it.
							context.SetEndpointDirection(ROOK_NUMBER_DOWN, true);
							return ret_val;
						case CAN_GO_CANNOT:
							context.SetDirectionOpen(ROOK_NUMBER_DOWN, false);
							break;
						default:
							ASSERT(false);
						}
					}
				}
			}
		case ROOK_LETTER_DOWN:
			if (context.GetEndpointDirection(ROOK_LETTER_DOWN))
			{
				context.SetDirectionOpen(ROOK_LETTER_DOWN, false);
			}
			if (context.GetDirectionOpen(ROOK_LETTER_DOWN))
			{
				new_value = piece_address.GetLetter() - distance;
				if (new_value >= CH_CONST_MIN_LETTER)
				{
					ret_val.SetLetter(new_value);
					ret_val.SetNumber(piece_address.GetNumber());
					if (ret_val.IsValid())
					{
						switch (CanGoThere(ret_val))
						{
						case CAN_GO_CAN:
							context.SetDirectionOpen(ROOK_LETTER_DOWN, true);
							exit_flag = true;
							break;
						case CAN_GO_KILL_ENEMY:
							//mark this direction so not to come back to it.
							context.SetEndpointDirection(ROOK_LETTER_DOWN, true);
							exit_flag = true;
							break;
						case CAN_GO_CANNOT:
							context.SetDirectionOpen(ROOK_LETTER_DOWN, false);
							increase_distance = true;
							break;
						default:
							ASSERT(false);
						}
						//return ret_val;
					} else {
						increase_distance = true;
					}
				}
				else {
					increase_distance = true;
				}
			}
			else {
				increase_distance = true;
			}
		case ROOK_UNSPECIFIED:;
		}
		if (increase_distance)
		{
			++distance;
			if ((distance > max_letter_distance) && (distance > max_number_distance))
			{
				exit_flag = true;
			}
			increase_distance = false;
		}
		moveto = ROOK_NUMBER_UP;
	}
	return ret_val; //blank, not valid
}

ChessPiece::NextPositionIterator* Rook::GetNextPositionIterator() const
{
	ChessPiece::NextPositionIterator* ret_val = NULL;
	ChessAddress address;
	ChessAddress current_address = m_current_cell->GetAddress();
	bool exit_flag = false;
	bool found_cell = false;
	GetNextAddressContext context;
	while (exit_flag == false)
	{
		address = GetNextAddress(address, context);
		if (address.IsValid() == false)
		{
			exit_flag = true;
			continue;
		}
		switch (CanGoThere(address))
		{
		case CAN_GO_CAN:
			found_cell = true;
			exit_flag = true;
			break;
		case CAN_GO_KILL_ENEMY:
			found_cell = true;
			exit_flag = true;
			break;
		case CAN_GO_CANNOT:
			;
		}
	}
	if (found_cell)
	{
		ChessCell* cell = m_chess->GetCell(address);
		ASSERT(cell != NULL);
		ret_val = new Rook::NextPositionIterator(this, cell, context);
	}
	return ret_val;
}

bool Rook::operator==(CellContentType cct) const
{
	if (GetIsBlack())
	{
		if (cct == BLACK_ROOK)
		{
			return true;
		}
	}
	else {
		if (cct == WHITE_ROOK)
		{
			return true;
		}
	}
	return false;
}

CellContentType Rook::GetCellContentType() const
{
	if(GetIsBlack())
	{
		return BLACK_ROOK;
	} else {
		return WHITE_ROOK;
	}
}

unsigned int Knight::MoveTo(const ChessAddress& dest, ChessAddress& where_it_came)
{
	unsigned int ret_val = CH_ERR_OK;
	if (dest.IsValid() == false)
	{
		return CH_ERR_INVALID_CHESS_ADDRESS;
	}
	ChessCell* curr_cell = GetCurrentCell();
	ASSERT(curr_cell != NULL);
	ChessAddress curr_address = curr_cell->GetAddress();
	ChessAddress next_address;
	if (dest.GetLetter() < curr_address.GetLetter())
	{	//to the left
		if (dest.GetNumber() > curr_address.GetNumber())
		{	//up
			if ((dest.GetLetter() == curr_address.GetLetter() - 1) &&
				(dest.GetNumber() == curr_address.GetNumber() + 2))
			{	//left 1 up 2
				next_address.SetLetter(dest.GetLetter());
				next_address.SetNumber(dest.GetNumber());
			}
			else if ((dest.GetLetter() == curr_address.GetLetter() - 2) &&
				(dest.GetNumber() == curr_address.GetNumber() + 1))
			{	//left 2 up 1
				next_address.SetLetter(dest.GetLetter());
				next_address.SetNumber(dest.GetNumber());
			}
			else {
				return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
			}
		}
		else if (dest.GetNumber() < curr_address.GetNumber())
		{	//down
			if ((dest.GetLetter() == curr_address.GetLetter() - 1) &&
				(dest.GetNumber() == curr_address.GetNumber() - 2))
			{	//left 1 down 2
				next_address.SetLetter(dest.GetLetter());
				next_address.SetNumber(dest.GetNumber());
			}
			else if ((dest.GetLetter() == curr_address.GetLetter() - 2) &&
				(dest.GetNumber() == curr_address.GetNumber() - 1))
			{	//left 2 down 1
				next_address.SetLetter(dest.GetLetter());
				next_address.SetNumber(dest.GetNumber());
			}
			else {
				return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
			}
		}
		else {	//stands still
			return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
		}
	}
	else if (dest.GetLetter() > curr_address.GetLetter())
	{	//to the right
		if (dest.GetNumber() > curr_address.GetNumber())
		{	//up
			if ((dest.GetLetter() == curr_address.GetLetter() + 1) &&
				(dest.GetNumber() == curr_address.GetNumber() + 2))
			{	//right 1 up 2
				next_address.SetLetter(dest.GetLetter());
				next_address.SetNumber(dest.GetNumber());
			}
			else if ((dest.GetLetter() == curr_address.GetLetter() + 2) &&
				(dest.GetNumber() == curr_address.GetNumber() + 1))
			{	//right 2 up 1
				next_address.SetLetter(dest.GetLetter());
				next_address.SetNumber(dest.GetNumber());
			} else {
				return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
			}
		}
		else if (dest.GetNumber() < curr_address.GetNumber())
		{	//down
			if ((dest.GetLetter() == curr_address.GetLetter() + 1) &&
				(dest.GetNumber() == curr_address.GetNumber() - 2))
			{ //right 1 down 2
				next_address.SetLetter(dest.GetLetter());
				next_address.SetNumber(dest.GetNumber());
			}
			else if ((dest.GetLetter() == curr_address.GetLetter() + 2) &&
				(dest.GetNumber() == curr_address.GetNumber() - 1))
			{ //right 2 down 1
				next_address.SetLetter(dest.GetLetter());
				next_address.SetNumber(dest.GetNumber());
			} else {
				return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
			}
		}
		else {	//stands still
			return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
		}
	}
	else {
		return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
	}
	unsigned int error = CH_ERR_OK;
	//if there is enemy piece on next_address, capture it
	ChessCell* next_cell = m_chess->GetCell(next_address);
	ASSERT(next_cell != NULL);
	ChessPiece* another_piece = next_cell->GetPiece();
	if (another_piece != NULL)
	{
		if (another_piece->GetIsBlack() != GetIsBlack())
		{
			error = Capture(another_piece);
			if (error != CH_ERR_OK)
			{
				return error;
			}
			m_chess->OnCapture(this, another_piece);
		} else {
			return CH_ERR_CELL_OCCUPIED;
		}
	}
	//now actually move the knight
	ASSERT(m_current_cell != NULL);
	error = m_current_cell->RemovePiece(this);
	if (error != CH_ERR_OK)
	{
		return error;
	}
	m_current_cell = NULL;
	where_it_came = next_address;
	error = next_cell->InsertPiece(this);
	if (error != CH_ERR_OK)
	{
		return error;
	}
	m_current_cell = next_cell;
	GetCurrentAddress(where_it_came);
	m_chess->OnMove(this, curr_address, where_it_came);
	return error;
}

ChessPiece::NextPositionIterator* Knight::GetNextPositionIterator() const
{
	//find nearest available address
	GetNextAddressContext context;
	ChessAddress current_address = GetIteratorStartAddress(context);
	ASSERT(m_chess != NULL);
	if(current_address.IsValid())
	{
		ChessCell* cell = m_chess->GetCell(current_address);
		ASSERT(cell != NULL);
		return new NextPositionIterator(this, cell, context);
	} else {
		return NULL;
	}
}

ChessAddress Knight::GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const
{
	ChessAddress current_address;
	ChessAddress ret_val; //created as 0.0, invalid.
	unsigned int error = GetCurrentAddress(current_address);
	if (error != CH_ERR_OK)
	{
		throw Exception(CH_ERR_PIECE_NOT_ON_BOARD, TEXT("Have no address, not on board"), EXC_HERE);
	}
	enum
	{
		KNIGHT_MOVE_POS_0,
		KNIGHT_MOVE_POS_1,
		KNIGHT_MOVE_POS_2,
		KNIGHT_MOVE_POS_3,
		KNIGHT_MOVE_POS_4,
		KNIGHT_MOVE_POS_5,
		KNIGHT_MOVE_POS_6,
		KNIGHT_MOVE_POS_7,
		KNIGHT_MOVETO_UNSPECIFIED
	};
	unsigned int moveto(KNIGHT_MOVETO_UNSPECIFIED);
	if (address.IsValid() == false)
	{
		moveto = KNIGHT_MOVE_POS_0;
	}
	else if ((address.GetLetter() == current_address.GetLetter() - 1) &&
		(address.GetNumber() == current_address.GetNumber() - 2))
	{
		moveto = KNIGHT_MOVE_POS_1;
	}
	else if ((address.GetLetter() == current_address.GetLetter() - 2) &&
		(address.GetNumber() == current_address.GetNumber() - 1))
	{
		moveto = KNIGHT_MOVE_POS_2;
	}
	else if ((address.GetLetter() == current_address.GetLetter() - 2) &&
		(address.GetNumber() == current_address.GetNumber() + 1))
	{
		moveto = KNIGHT_MOVE_POS_3;
	}
	else if ((address.GetLetter() == current_address.GetLetter() - 1) &&
		(address.GetNumber() == current_address.GetNumber() + 2))
	{
		moveto = KNIGHT_MOVE_POS_4;
	}
	else if ((address.GetLetter() == current_address.GetLetter() + 1) &&
		(address.GetNumber() == current_address.GetNumber() + 2))
	{
		moveto = KNIGHT_MOVE_POS_5;
	}
	else if ((address.GetLetter() == current_address.GetLetter() + 2) &&
		(address.GetNumber() == current_address.GetNumber() + 1))
	{
		moveto = KNIGHT_MOVE_POS_6;
	}
	else if ((address.GetLetter() == current_address.GetLetter() + 2) &&
		(address.GetNumber() == current_address.GetNumber() - 1))
	{
		moveto = KNIGHT_MOVE_POS_7;
	} else {
		return ret_val;
	}
	/*--------> + letters
	..|.........POS 0......POS 7.......
	..|...POS 1...................POS 6
	..|............Knight..............
	..|...POS 2...................POS 5
	--V---------POS 3......POS 4.......
	..+ numbers
	*/
	switch (moveto)
	{
	case KNIGHT_MOVE_POS_0:
		ret_val.SetLetter(current_address.GetLetter() - 1);
		ret_val.SetNumber(current_address.GetNumber() - 2);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:
				;
			}
		}
	case KNIGHT_MOVE_POS_1:
		ret_val.SetLetter(current_address.GetLetter() - 2);
		ret_val.SetNumber(current_address.GetNumber() - 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:
				;
			}
		}
	case KNIGHT_MOVE_POS_2:
		ret_val.SetLetter(current_address.GetLetter() - 2);
		ret_val.SetNumber(current_address.GetNumber() + 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:
				;
			}
		}
	case KNIGHT_MOVE_POS_3:
		ret_val.SetLetter(current_address.GetLetter() - 1);
		ret_val.SetNumber(current_address.GetNumber() + 2);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:
				;
			}
		}
	case KNIGHT_MOVE_POS_4:
		ret_val.SetLetter(current_address.GetLetter() + 1);
		ret_val.SetNumber(current_address.GetNumber() + 2);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:
				;
			}
		}
	case KNIGHT_MOVE_POS_5:
		ret_val.SetLetter(current_address.GetLetter() + 2);
		ret_val.SetNumber(current_address.GetNumber() + 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:
				;
			}
		}
	case KNIGHT_MOVE_POS_6:
		ret_val.SetLetter(current_address.GetLetter() + 2);
		ret_val.SetNumber(current_address.GetNumber() - 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:
				;
			}
		}
	case KNIGHT_MOVE_POS_7:
		ret_val.SetLetter(current_address.GetLetter() + 1);
		ret_val.SetNumber(current_address.GetNumber() - 2);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:
				;
			}
		}
	case KNIGHT_MOVETO_UNSPECIFIED:
	default:;
		//default: ret_val remains initialized as invalid address. OK.
	}
	return ret_val;
}

bool Knight::operator == (CellContentType cct) const
{
	if (GetIsBlack())
	{
		if (cct == BLACK_KNIGHT)
		{
			return true;
		}
	}
	else {
		if (cct == WHITE_KNIGHT)
		{
			return true;
		}
	}
	return false;
}

CellContentType Knight::GetCellContentType() const
{
	if(GetIsBlack())
	{
		return BLACK_KNIGHT;
	} else {
		return WHITE_KNIGHT;
	}
}

unsigned int Bishop::MoveTo(const ChessAddress& dest, ChessAddress& where_it_came)
{
	//validate dest address
	unsigned int ret_val = CH_ERR_OK;
	if (dest.IsValid() == false)
	{
		return CH_ERR_INVALID_CHESS_ADDRESS;
	}
	ChessCell* current_cell = GetCurrentCell();
	ASSERT(current_cell != NULL);
	ChessAddress current_address = current_cell->GetAddress();
	bool do_move_left = false;
	bool do_move_up = false;
	int letter_difference = dest.GetLetter() - current_address.GetLetter();
	if (letter_difference < 0)
	{
		letter_difference = -letter_difference;
		do_move_left = true;
	}
	else {
		do_move_left = false;
	}
	int number_difference = dest.GetNumber() - current_address.GetNumber();
	if (number_difference < 0)
	{
		number_difference = -number_difference;
		do_move_up = false;
	}
	else {
		do_move_up = true;
	}
	//criteria to move bishp according to the chess rules
	if (letter_difference != number_difference)
	{
		return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
	}
	//is there someone on the way?
	bool exit_flag = true;
	ChessAddress tmp_addr = current_address;
	ChessAddress prev_address = tmp_addr;
	ChessPiece* enemy_piece = NULL;
	ChessPiece* friend_piece = NULL;
	while (exit_flag == true)
	{
		prev_address = tmp_addr;
		bool ok = false;
		if ((tmp_addr.GetLetter() == dest.GetLetter()) &&
			(tmp_addr.GetNumber() == dest.GetNumber()))
		{
			exit_flag = true;
			break;
		}
		if (do_move_left == true)
		{
			ok = tmp_addr.DecrementLetter();
			if (ok == false)
			{
				exit_flag = false;
				break;
			}
		}
		else {
			ok = tmp_addr.IncrementLetter();
			if (ok == false)
			{
				exit_flag = false;
				break;
			}
		}
		if (do_move_up == true)
		{
			ok = tmp_addr.IncrementNumber();
			if (ok == false)
			{
				exit_flag = false;
				break;
			}
		} else {
			ok = tmp_addr.DecrementNumber();
			if (ok == false)
			{
				exit_flag = false;
				break;
			}
		}
		ChessCell* cell = m_chess->GetCell(tmp_addr);
		ASSERT(cell != NULL);
		ChessPiece* piece = cell->GetPiece();
		if (piece != NULL)
		{
			//we found one on the way
			//now tmp_addr points to the cell where some figure is.
			if (piece->GetIsBlack() == GetIsBlack())
			{
				friend_piece = piece;
			}
			else {
				enemy_piece = piece;
			}
			exit_flag = true;
			break;
		}
	}
	//AbstractChessNotificationHandler* cnh = m_chess->GetNotificationHandler();
	//here we've got tmp_addr pointing to where the piece will actually come
	//(original destination or in place of captured enemy piece or near friend piece)
	if (enemy_piece != NULL)
	{
		ASSERT(friend_piece == NULL);
		ret_val = Capture(enemy_piece);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_chess->OnCapture(this, enemy_piece);
		ChessCell* cell = enemy_piece->GetCurrentCell();
		ASSERT(cell != NULL);
		ChessCell* tmp_cell = m_current_cell;
		ret_val = m_current_cell->RemovePiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = NULL;
		ret_val = cell->InsertPiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = cell;
		//where_it_came = tmp_addr;      
	}
	else if (friend_piece != NULL)
	{
		return CH_ERR_CELL_OCCUPIED;
	}
	else {
		//nothing is there, move there
		ASSERT(enemy_piece == NULL);
		ASSERT(friend_piece == NULL);
		ChessCell* next_cell = m_chess->GetCell(tmp_addr);
		ASSERT(next_cell != NULL);
		ret_val = m_current_cell->RemovePiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = NULL;
		ret_val = next_cell->InsertPiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = next_cell;
		//where_it_came = tmp_addr;
	}
	GetCurrentAddress(where_it_came);
	m_chess->OnMove(this, prev_address, tmp_addr);
	return ret_val;
}

ChessPiece::NextPositionIterator* Bishop::GetNextPositionIterator() const
{
	GetNextAddressContext context;
	ChessAddress address = GetIteratorStartAddress(context);
	if (address.IsValid() == false)
	{
		address = ChessAddress();
	}
	ChessCell* cell = m_chess->GetCell(address); //may be null
	if (cell != NULL)
	{
		return new NextPositionIterator(this, cell, context);
	}
	else {
		return NULL;
	}
}

ChessAddress Bishop::GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const
{
	ChessAddress current_address;
	ChessAddress ret_val;
	unsigned int error = GetCurrentAddress(current_address);
	if (error != CH_ERR_OK)
	{
		throw Exception(error, L"Cannot get Bishop's current address", EXC_HERE);
	}

	/* up and down here as well as left and right are interms of board as it is presented to the user.
	-|--------->letters-----------------          iterator move <--- next distance
	-|----LEFTUP---------------RIGHTUP-             <-------0     |
	-|------LEFTUP------RIGHTUP--------             |            |
	-|-------------bishop-------------- 			|  bishop   |
	-|-------LEFTDOWN-----RIGHTDOWN----             |          |
	-v----LEFTDOWN-----------RIGHTDOWN-             V-------->|
	numbers
	*/
	
	enum
	{
		BISHOP_MOVE_LEFTDOWN,
		BISHOP_MOVE_LEFTUP,
		BISHOP_MOVE_RIGHTUP,
		BISHOP_MOVE_RIGHTDOWN,
		BISHOP_MOVE_UNSPECIFIED
	};

	if (address.IsValid() == false)
	{
		ret_val.SetLetter(current_address.GetLetter() + 1);
		ret_val.SetNumber(current_address.GetNumber() - 1);
		if (ret_val.IsValid() == true)
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
				return ret_val;
			case CAN_GO_KILL_ENEMY:
				context.SetEndpointDirection(BISHOP_MOVE_RIGHTUP, false);
				return ret_val;
			case CAN_GO_CANNOT:
				context.SetDirectionOpen(BISHOP_MOVE_RIGHTUP, false);
			}
		}
		ret_val.SetLetter(current_address.GetLetter() - 1);
		ret_val.SetNumber(current_address.GetNumber() - 1);
		if (ret_val.IsValid() == true)
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
				return ret_val;
			case CAN_GO_KILL_ENEMY:
				context.SetEndpointDirection(BISHOP_MOVE_LEFTUP, false);
				return ret_val;
			case CAN_GO_CANNOT:
				context.SetDirectionOpen(BISHOP_MOVE_LEFTUP, false);
			}
		}
		ret_val.SetLetter(current_address.GetLetter() - 1);
		ret_val.SetNumber(current_address.GetNumber() + 1);
		if (ret_val.IsValid() == true)
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
				return ret_val;
			case CAN_GO_KILL_ENEMY:
				context.SetEndpointDirection(BISHOP_MOVE_LEFTDOWN, false);
				return ret_val;
			case CAN_GO_CANNOT:
				context.SetDirectionOpen(BISHOP_MOVE_LEFTDOWN, false);
			}
		}
		ret_val.SetLetter(current_address.GetLetter() + 1);
		ret_val.SetNumber(current_address.GetNumber() + 1);
		if (ret_val.IsValid() == true)
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
				return ret_val;
			case CAN_GO_KILL_ENEMY:
				context.SetEndpointDirection(BISHOP_MOVE_RIGHTDOWN, false);
				return ret_val;
			case CAN_GO_CANNOT:
				context.SetDirectionOpen(BISHOP_MOVE_RIGHTDOWN, false);
				ret_val = ChessAddress();//invalid
			}
		}
		return ret_val;
	}

	int distance_letters = address.GetLetter() - current_address.GetLetter();
	int distance_numbers = address.GetNumber() - current_address.GetNumber();
	ASSERT(abs(distance_letters) == abs(distance_numbers));
	int distance = abs(distance_letters);
	unsigned int valid_count(4);
	unsigned int moveto(BISHOP_MOVE_UNSPECIFIED);
	if ((distance_letters > 0) && (distance_numbers < 0))
	{
		//currently right up
		valid_count = 4;
		moveto = BISHOP_MOVE_LEFTUP;
	} else
	if ((distance_letters < 0) && (distance_numbers < 0))
	{
		//currently left up
		valid_count = 3;
		moveto = BISHOP_MOVE_LEFTDOWN;
	} else
	if ((distance_letters < 0) && (distance_numbers > 0))
	{
		//currently left down
		valid_count = 2;
		moveto = BISHOP_MOVE_RIGHTDOWN;
	} else 
	if ((distance_letters > 0) && (distance_numbers > 0))
	{
		//currently right down
		valid_count = 1;
		moveto = BISHOP_MOVE_RIGHTUP;
	}
	bool last_loop_flag = false;
	bool exit_flag = false;
	while (exit_flag == false)
	{
		switch (moveto)
		{
		case BISHOP_MOVE_RIGHTUP:
			if (context.GetEndpointDirection(BISHOP_MOVE_RIGHTUP))
			{
				context.SetDirectionOpen(BISHOP_MOVE_RIGHTUP, false);
			}
			++distance;
			if (context.GetDirectionOpen(BISHOP_MOVE_RIGHTUP))
			{
				ret_val.SetLetter(current_address.GetLetter() + distance);
				ret_val.SetNumber(current_address.GetNumber() - distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(BISHOP_MOVE_RIGHTUP, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(BISHOP_MOVE_RIGHTUP, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
			//currently must be left up
		case BISHOP_MOVE_LEFTUP:
			if (context.GetEndpointDirection(BISHOP_MOVE_LEFTUP))
			{
				context.SetDirectionOpen(BISHOP_MOVE_LEFTUP, false);
			}
			if (context.GetDirectionOpen(BISHOP_MOVE_LEFTUP))
			{
				ret_val.SetLetter(current_address.GetLetter() - distance);
				ret_val.SetNumber(current_address.GetNumber() - distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(BISHOP_MOVE_LEFTUP, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(BISHOP_MOVE_LEFTUP, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
			//currently must be left up
		case BISHOP_MOVE_LEFTDOWN:
			if (context.GetEndpointDirection(BISHOP_MOVE_LEFTDOWN))
			{
				context.SetDirectionOpen(BISHOP_MOVE_LEFTDOWN, false);
			}
			if (context.GetDirectionOpen(BISHOP_MOVE_LEFTDOWN))
			{
				ret_val.SetLetter(current_address.GetLetter() - distance);
				ret_val.SetNumber(current_address.GetNumber() + distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(BISHOP_MOVE_LEFTDOWN, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(BISHOP_MOVE_LEFTDOWN, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
			//currently must be left down
		case BISHOP_MOVE_RIGHTDOWN:
			if (context.GetEndpointDirection(BISHOP_MOVE_RIGHTDOWN))
			{
				context.SetDirectionOpen(BISHOP_MOVE_RIGHTDOWN, false);
			}
			if (context.GetDirectionOpen(BISHOP_MOVE_RIGHTDOWN))
			{
				ret_val.SetLetter(current_address.GetLetter() + distance);
				ret_val.SetNumber(current_address.GetNumber() + distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(BISHOP_MOVE_RIGHTDOWN, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(BISHOP_MOVE_RIGHTDOWN, false);
					}
				}
			}
			moveto = BISHOP_MOVE_RIGHTUP;
			if (valid_count != 0)
			{
				--valid_count;
			}
			//currently must be right down
			if (last_loop_flag)
			{
				exit_flag = true;
			}
			if (valid_count == 0)
			{
				valid_count = 4;
				last_loop_flag = true;
			}
		}
	}
	return ret_val;
}

bool Bishop::operator == (CellContentType cct) const
{
	if (GetIsBlack())
	{
		if (cct == BLACK_BISHOP)
		{
			return true;
		}
	} else {
		if (cct == WHITE_BISHOP)
		{
			return true;
		}
	}
	return false;
}

CellContentType Bishop::GetCellContentType() const
{
	if(GetIsBlack())
	{
		return BLACK_BISHOP;
	} else {
		return WHITE_BISHOP;
	}
}

unsigned int Queen::MoveTo(const ChessAddress& dest, ChessAddress& where_it_came)
{
	enum 
	{
		QUEEN_STANDS_STILL,
		QUEEN_DIR_LEFT,
		QUEEN_DIR_LEFT_UP,
		QUEEN_DIR_UP,
		QUEEN_DIR_RIGHT_UP,
		QUEEN_DIR_RIGHT,
		QUEEN_DIR_RIGHT_DOWN,
		QUEEN_DIR_DOWN,
		QUEEN_DIR_LEFT_DOWN
	};
	unsigned int ret_val = CH_ERR_OK;
	if (dest.IsValid() == false)
	{
		return CH_ERR_INVALID_CHESS_ADDRESS;
	}
	unsigned int queen_dir = QUEEN_STANDS_STILL;
	ChessAddress current_addr;
	ret_val = GetCurrentAddress(current_addr);
	if (ret_val != CH_ERR_OK)
	{
		return ret_val;
	}
	if (dest.IsValid() == false)
	{
		return CH_ERR_INVALID_CHESS_ADDRESS;
	}
	ASSERT(current_addr.IsValid());
	int letter_increment = dest.GetLetter() - current_addr.GetLetter();
	int number_increment = dest.GetNumber() - current_addr.GetNumber();
	if (letter_increment < 0)
	{//to the left
		if (number_increment < 0)
		{
			if (letter_increment != number_increment)
			{
				return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
			}
			queen_dir = QUEEN_DIR_LEFT_DOWN;
		}
		else if (number_increment > 0)
		{
			if (letter_increment != -number_increment)
			{
				return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
			}
			queen_dir = QUEEN_DIR_LEFT_UP;
		}
		else {	//horizontal
			queen_dir = QUEEN_DIR_LEFT;
		}
	}
	else if (letter_increment > 0)
	{//to the right
		if (number_increment < 0)
		{
			if (letter_increment != -number_increment)
			{
				return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
			}
			queen_dir = QUEEN_DIR_RIGHT_DOWN;
		}
		else if (number_increment > 0)
		{
			if (letter_increment != number_increment)
			{
				return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
			}
			queen_dir = QUEEN_DIR_RIGHT_UP;
		}
		else {  //horizontal
			queen_dir = QUEEN_DIR_RIGHT;
		}
	}
	else {//vertical only
		if (number_increment < 0)
		{
			queen_dir = QUEEN_DIR_DOWN;
		}
		else if (number_increment > 0)
		{
			queen_dir = QUEEN_DIR_UP;
		}
		else {
			//Queen stands still
			queen_dir = QUEEN_STANDS_STILL;
			//and this is not legal. return error.
			return CH_ERR_CANNOT_GO_ALREADY_THERE;
		}
	}
	//test each cell in this loops to detect a chess piece there
	ChessPiece* detected_piece = NULL;
	ChessAddress address_right_before_piece = current_addr;
	switch (queen_dir)
	{
	case QUEEN_DIR_LEFT:
		for (int letter = current_addr.GetLetter() - 1; letter >= dest.GetLetter(); letter--)
		{
			ChessAddress addr(letter, current_addr.GetNumber());
			detected_piece = m_chess->GetPiece(addr);
			if (detected_piece != NULL)
			{
				break;
			}
			address_right_before_piece = addr;
		}
		break;
	case QUEEN_DIR_LEFT_UP:
		for (int letter = current_addr.GetLetter() - 1, number = current_addr.GetNumber() + 1;
			((letter >= dest.GetLetter()) && (number <= dest.GetNumber()));
			letter--, number++)
		{
			ChessAddress addr(letter, number);
			detected_piece = m_chess->GetPiece(addr);
			if (detected_piece != NULL)
			{
				break;
			}
			address_right_before_piece = addr;
		}
		break;
	case QUEEN_DIR_UP:
		for (int number = current_addr.GetNumber() + 1;
			number <= dest.GetNumber();
			number++)
		{
			ChessAddress addr(current_addr.GetLetter(), number);
			detected_piece = m_chess->GetPiece(addr);
			if (detected_piece != NULL)
			{
				break;
			}
			address_right_before_piece = addr;
		}
		break;
	case QUEEN_DIR_RIGHT_UP:
		for (int letter = current_addr.GetLetter() + 1, number = current_addr.GetNumber() + 1;
			((letter <= dest.GetLetter()) && (number <= dest.GetNumber()));
			letter++, number++)
		{
			ChessAddress addr(letter, number);
			detected_piece = m_chess->GetPiece(addr);
			if (detected_piece != NULL)
			{
				break;
			}
			address_right_before_piece = addr;
		}
		break;
	case QUEEN_DIR_RIGHT:
		for (int letter = current_addr.GetLetter() + 1; letter <= dest.GetLetter(); letter++)
		{
			ChessAddress addr(letter, current_addr.GetNumber());
			detected_piece = m_chess->GetPiece(addr);
			if (detected_piece != NULL)
			{
				break;
			}
			address_right_before_piece = addr;
		}
		break;
	case QUEEN_DIR_RIGHT_DOWN:
		for (int letter = current_addr.GetLetter() + 1, number = current_addr.GetNumber() - 1;
			((letter <= dest.GetLetter()) && (number >= dest.GetNumber()));
			letter++, number--)
		{
			ChessAddress addr(letter, number);
			detected_piece = m_chess->GetPiece(addr);
			if (detected_piece != NULL)
			{
				break;
			}
			address_right_before_piece = addr;
		}
		break;
	case QUEEN_DIR_DOWN:
		for (int number = current_addr.GetNumber() - 1; number >= dest.GetNumber(); number--)
		{
			ChessAddress addr(current_addr.GetLetter(), number);
			detected_piece = m_chess->GetPiece(addr);
			if (detected_piece != NULL)
			{
				break;
			}
			address_right_before_piece = addr;
		}
		break;
	case QUEEN_DIR_LEFT_DOWN:
		for (int letter = current_addr.GetLetter() - 1, number = current_addr.GetNumber() - 1;
			((letter >= dest.GetLetter()) && (number >= dest.GetNumber()));
			letter--, number--)
		{
			ChessAddress addr(letter, number);
			detected_piece = m_chess->GetPiece(addr);
			if (detected_piece != NULL)
			{
				break;
			}
			address_right_before_piece = addr;
		}
		break;
	default:
		ASSERT(false);	//cannot be here
	}
	ChessCell* current_cell = GetCurrentCell();
	ASSERT(current_cell != NULL);
	//AbstractChessNotificationHandler* cnh = m_chess->GetNotificationHandler();
	if (detected_piece != NULL)
	{
		if (detected_piece->GetIsBlack() == GetIsBlack())
		{
			return CH_ERR_CELL_OCCUPIED;
		} else {
			//kill the enemy
			ret_val = Capture(detected_piece);
			if (ret_val != CH_ERR_OK)
			{
				return ret_val;
			}
			m_chess->OnCapture(this, detected_piece);
			ChessCell* detected_piece_cell = detected_piece->GetCurrentCell();
			ASSERT(detected_piece_cell != NULL);
			ret_val = m_current_cell->RemovePiece(this);
			if (ret_val != CH_ERR_OK)
			{
				return ret_val;
			}
			m_current_cell = NULL;
			ret_val = detected_piece_cell->InsertPiece(this);
			if (ret_val != CH_ERR_OK)
			{
				return ret_val;
			}
			m_current_cell = detected_piece_cell;
		}
		ChessCell* next_cell = detected_piece->GetCurrentCell();
		ASSERT(next_cell != NULL);
	}
	else {
		ChessCell* next_cell = m_chess->GetCell(dest);
		ASSERT(next_cell != NULL);
		ret_val = current_cell->RemovePiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = NULL;
		ret_val = next_cell->InsertPiece(this);
		if (ret_val != CH_ERR_OK)
		{
			return ret_val;
		}
		m_current_cell = next_cell;
		//where_it_came = next_cell->GetAddress();
	}
	GetCurrentAddress(where_it_came);
	m_chess->OnMove(this, current_addr, dest);
	return ret_val;
}

ChessAddress Queen::GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const
{
	/*	     how iterator moves
	 -------->letters--------------->
     |
	 |.pos10.......pos9.....dist*pos1 (pos 8)
     |.......pos2..pos1..pos0
	.|.pos12.pos3..Queen.pos7      A to pos 15
	 |.......pos4..pos5..pos6      |
	 |.pos12.......pos13.....pos14-|
     |
     |
     V
	 numbers
	*/
	enum
	{
		QUEEN_MOVETO_POS_0,
		QUEEN_MOVETO_POS_1,
		QUEEN_MOVETO_POS_2,
		QUEEN_MOVETO_POS_3,
		QUEEN_MOVETO_POS_4,
		QUEEN_MOVETO_POS_5,
		QUEEN_MOVETO_POS_6,
		QUEEN_MOVETO_POS_7,
		QUEEN_MOVETO_POS_8,
		QUEEN_MOVETO_POS_INVALID
	};
	ChessAddress piece_address;
	ChessAddress ret_val;
	unsigned int error = GetCurrentAddress(piece_address);
	if (error != CH_ERR_OK)
	{
		throw Exception(error, L"Queen is not on board", EXC_HERE);
	}
	if (address.IsValid() == false)
	{	//get the initial address
		ret_val.SetLetter(piece_address.GetLetter() + 1);
		ret_val.SetNumber(piece_address.GetNumber() - 1);
		switch (CanGoThere(ret_val))
		{
		case CAN_GO_CAN:
			return ret_val;
		case CAN_GO_KILL_ENEMY:
			context.SetEndpointDirection(QUEEN_MOVETO_POS_0, true);
			return ret_val;
		case CAN_GO_CANNOT:
			context.SetDirectionOpen(QUEEN_MOVETO_POS_0, false);
		}
		ret_val.SetLetter(piece_address.GetLetter());
		ret_val.SetNumber(piece_address.GetNumber() - 1);
		switch (CanGoThere(ret_val))
		{
		case CAN_GO_CAN:
			return ret_val;
		case CAN_GO_KILL_ENEMY:
			context.SetEndpointDirection(QUEEN_MOVETO_POS_1, true);
			return ret_val;
		case CAN_GO_CANNOT:
			context.SetDirectionOpen(QUEEN_MOVETO_POS_1, false);
		}
		ret_val.SetLetter(piece_address.GetLetter() - 1);
		ret_val.SetNumber(piece_address.GetNumber() - 1);
		switch (CanGoThere(ret_val))
		{
		case CAN_GO_CAN:
			return ret_val;
		case CAN_GO_KILL_ENEMY:
			context.SetEndpointDirection(QUEEN_MOVETO_POS_2, true);
			return ret_val;
		case CAN_GO_CANNOT:
			context.SetDirectionOpen(QUEEN_MOVETO_POS_2, false);
		}
		ret_val.SetLetter(piece_address.GetLetter() - 1);
		ret_val.SetNumber(piece_address.GetNumber());
		switch (CanGoThere(ret_val))
		{
		case CAN_GO_CAN:
			return ret_val;
		case CAN_GO_KILL_ENEMY:
			context.SetEndpointDirection(QUEEN_MOVETO_POS_3, true);
			return ret_val;
		case CAN_GO_CANNOT:
			context.SetDirectionOpen(QUEEN_MOVETO_POS_3, false);
		}
		ret_val.SetLetter(piece_address.GetLetter() - 1);
		ret_val.SetNumber(piece_address.GetNumber() + 1);
		switch (CanGoThere(ret_val))
		{
		case CAN_GO_CAN:
			return ret_val;
		case CAN_GO_KILL_ENEMY:
			context.SetEndpointDirection(QUEEN_MOVETO_POS_4, true);
			return ret_val;
		case CAN_GO_CANNOT:
			context.SetDirectionOpen(QUEEN_MOVETO_POS_4, false);
		}
		ret_val.SetLetter(piece_address.GetLetter());
		ret_val.SetNumber(piece_address.GetNumber() + 1);
		switch (CanGoThere(ret_val))
		{
		case CAN_GO_CAN:
			return ret_val;
		case CAN_GO_KILL_ENEMY:
			context.SetEndpointDirection(QUEEN_MOVETO_POS_5, true);
			return ret_val;
		case CAN_GO_CANNOT:
			context.SetDirectionOpen(QUEEN_MOVETO_POS_5, false);
		}
		ret_val.SetLetter(piece_address.GetLetter() + 1);
		ret_val.SetNumber(piece_address.GetNumber() + 1);
		switch (CanGoThere(ret_val))
		{
		case CAN_GO_CAN:
			return ret_val;
		case CAN_GO_KILL_ENEMY:
			context.SetEndpointDirection(QUEEN_MOVETO_POS_6, true);
			return ret_val;
		case CAN_GO_CANNOT:
			context.SetDirectionOpen(QUEEN_MOVETO_POS_6, false);
		}
		ret_val.SetLetter(piece_address.GetLetter() + 1);
		ret_val.SetNumber(piece_address.GetNumber());
		switch (CanGoThere(ret_val))
		{
		case CAN_GO_CAN:
			return ret_val;
		case CAN_GO_KILL_ENEMY:
			context.SetEndpointDirection(QUEEN_MOVETO_POS_7, true);
			return ret_val;
		case CAN_GO_CANNOT:
			context.SetDirectionOpen(QUEEN_MOVETO_POS_7, false);
		}
		return ret_val;
	}
	ASSERT(address.IsValid());
	int distance_letters = address.GetLetter() - piece_address.GetLetter();
	int distance_numbers = address.GetNumber() - piece_address.GetNumber();
	unsigned int distance = abs(distance_letters);
	if (distance == 0)
	{
		distance = abs(distance_numbers);
	}
	ASSERT(distance != NULL);   //this is an error when iterator points to the same cell where it's piece stands.
	unsigned int moveto(QUEEN_MOVETO_POS_INVALID);
	unsigned int valid_count(0);
	bool last_loop_flag(false);
	if (address.IsValid() == false)
	{
		moveto = QUEEN_MOVETO_POS_0;
		valid_count = 8;
	}
	else if ((distance_letters > 0) &&
		(distance_numbers < 0))
	{
		moveto = QUEEN_MOVETO_POS_1;
		valid_count = 7;
	}
	else if ((distance_letters == 0) &&
		(distance_numbers < 0))
	{
		moveto = QUEEN_MOVETO_POS_2;
		valid_count = 6;
	}
	else if ((distance_letters < 0) &&
		(distance_numbers < 0))
	{
		moveto = QUEEN_MOVETO_POS_3;
		valid_count = 5;
	}
	else if ((distance_letters < 0) &&
		(distance_numbers == 0))
	{
		moveto = QUEEN_MOVETO_POS_4;
		valid_count = 4;
	}
	else if ((distance_letters < 0) &&
		(distance_numbers > 0))
	{
		moveto = QUEEN_MOVETO_POS_5;
		valid_count = 3;
	}
	else if ((distance_letters == 0) &&
		(distance_numbers > 0))
	{
		moveto = QUEEN_MOVETO_POS_6;
		valid_count = 2;
	}
	else if ((distance_letters > 0) &&
		(distance_numbers > 0))
	{
		moveto = QUEEN_MOVETO_POS_7;
		valid_count = 1;
	}
	else if ((distance_letters > 0) &&
		(distance_numbers == 0))
	{
		moveto = QUEEN_MOVETO_POS_0;
		valid_count = 8;	//again pos 0, again valid count == 8
	}
	bool exit_flag = false;
	while (exit_flag == false)
	{
		switch (moveto)
		{
		case QUEEN_MOVETO_POS_0:
			if (context.GetEndpointDirection(QUEEN_MOVETO_POS_0))
			{
				context.SetDirectionOpen(QUEEN_MOVETO_POS_0, false);
			}
			++distance;
			if (context.GetDirectionOpen(QUEEN_MOVETO_POS_0))
			{
				//++distance;
				ret_val.SetLetter(piece_address.GetLetter() + distance);
				ret_val.SetNumber(piece_address.GetNumber() - distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(QUEEN_MOVETO_POS_0, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(QUEEN_MOVETO_POS_0, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
		case QUEEN_MOVETO_POS_1:
			if (context.GetEndpointDirection(QUEEN_MOVETO_POS_1))
			{
				context.SetDirectionOpen(QUEEN_MOVETO_POS_1, false);
			}
			if (context.GetDirectionOpen(QUEEN_MOVETO_POS_1))
			{
				ret_val.SetLetter(piece_address.GetLetter());
				ret_val.SetNumber(piece_address.GetNumber() - distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(QUEEN_MOVETO_POS_1, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(QUEEN_MOVETO_POS_1, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
		case QUEEN_MOVETO_POS_2:
			if (context.GetEndpointDirection(QUEEN_MOVETO_POS_2))
			{
				context.SetDirectionOpen(QUEEN_MOVETO_POS_2, false);
			}
			if (context.GetDirectionOpen(QUEEN_MOVETO_POS_2))
			{
				ret_val.SetLetter(piece_address.GetLetter() - distance);
				ret_val.SetNumber(piece_address.GetNumber() - distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(QUEEN_MOVETO_POS_2, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(QUEEN_MOVETO_POS_2, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
		case QUEEN_MOVETO_POS_3:
			if (context.GetEndpointDirection(QUEEN_MOVETO_POS_3))
			{
				context.SetDirectionOpen(QUEEN_MOVETO_POS_3, false);
			}
			if (context.GetDirectionOpen(QUEEN_MOVETO_POS_3))
			{
				ret_val.SetLetter(piece_address.GetLetter() - distance);
				ret_val.SetNumber(piece_address.GetNumber());
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(QUEEN_MOVETO_POS_3, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(QUEEN_MOVETO_POS_3, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
		case QUEEN_MOVETO_POS_4:
			if (context.GetEndpointDirection(QUEEN_MOVETO_POS_4))
			{
				context.SetDirectionOpen(QUEEN_MOVETO_POS_4, false);
			}
			if (context.GetDirectionOpen(QUEEN_MOVETO_POS_4))
			{
				ret_val.SetLetter(piece_address.GetLetter() - distance);
				ret_val.SetNumber(piece_address.GetNumber() + distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(QUEEN_MOVETO_POS_4, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(QUEEN_MOVETO_POS_4, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
		case QUEEN_MOVETO_POS_5:
			if (context.GetEndpointDirection(QUEEN_MOVETO_POS_5))
			{
				context.SetDirectionOpen(QUEEN_MOVETO_POS_5, false);
			}
			if (context.GetDirectionOpen(QUEEN_MOVETO_POS_5))
			{
				ret_val.SetLetter(piece_address.GetLetter());
				ret_val.SetNumber(piece_address.GetNumber() + distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(QUEEN_MOVETO_POS_5, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(QUEEN_MOVETO_POS_5, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
		case QUEEN_MOVETO_POS_6:
			if (context.GetEndpointDirection(QUEEN_MOVETO_POS_6))
			{
				context.SetDirectionOpen(QUEEN_MOVETO_POS_6, false);
			}
			if (context.GetDirectionOpen(QUEEN_MOVETO_POS_6))
			{
				ret_val.SetLetter(piece_address.GetLetter() + distance);
				ret_val.SetNumber(piece_address.GetNumber() + distance);
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(QUEEN_MOVETO_POS_6, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(QUEEN_MOVETO_POS_6, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
		case QUEEN_MOVETO_POS_7:
			if (context.GetEndpointDirection(QUEEN_MOVETO_POS_7))
			{
				context.SetDirectionOpen(QUEEN_MOVETO_POS_7, false);
			}
			if (context.GetDirectionOpen(QUEEN_MOVETO_POS_7))
			{
				ret_val.SetLetter(piece_address.GetLetter() + distance);
				ret_val.SetNumber(piece_address.GetNumber());
				if (ret_val.IsValid())
				{
					switch (CanGoThere(ret_val))
					{
					case CAN_GO_CAN:
						return ret_val;
					case CAN_GO_KILL_ENEMY:
						context.SetEndpointDirection(QUEEN_MOVETO_POS_7, true);
						return ret_val;
					case CAN_GO_CANNOT:
						context.SetDirectionOpen(QUEEN_MOVETO_POS_7, false);
					}
				}
			}
			if (valid_count != 0)
			{
				--valid_count;
			}
		/*case QUEEN_MOVETO_POS_8:
			if (context.GetEndpointDirection(QUEEN_MOVETO_POS_7))
			{
				context.SetDirectionOpen(QUEEN_MOVETO_POS_7, false);
			}
			if (context.GetDirectionOpen(QUEEN_MOVETO_POS_7))
			{
			}*/
		}
		if (last_loop_flag)
		{
			exit_flag = true;
		}
		if (valid_count == 0)
		{
			last_loop_flag = true;
			valid_count = 8;
			moveto = QUEEN_MOVETO_POS_0;
		}
	}
	return ret_val;
}

ChessPiece::NextPositionIterator* Queen::GetNextPositionIterator() const
{
	GetNextAddressContext context;
	ChessAddress address = GetIteratorStartAddress(context);
	if (address.IsValid() == false)
	{
		address = ChessAddress(); //invalid
	}
	ChessCell* cell = m_chess->GetCell(address);
	if (cell != NULL)
	{
		return new NextPositionIterator(this, cell, context);
	} else {
		return NULL;
	}
}

bool Queen::operator == (CellContentType cct) const
{
	if (GetIsBlack())
	{
		if (cct == BLACK_QUEEN)
		{
			return true;
		}
	} else {
		if (cct == WHITE_QUEEN)
		{
			return true;
		}
	}
	return false;
}

CellContentType Queen::GetCellContentType() const
{
	if(GetIsBlack())
	{
		return BLACK_QUEEN;
	} else {
		return WHITE_QUEEN;
	}
}

unsigned int King::MoveTo(const ChessAddress& dest, ChessAddress& where_it_came)
{
	if (dest.IsValid() == false)
	{
		return CH_ERR_INVALID_CHESS_ADDRESS;
	}
	//check if dest is valid according to chess rules
	ChessAddress curr_addr;
	unsigned int error = GetCurrentAddress(curr_addr);
	if (error != CH_ERR_OK)
	{
		return error;
	}
	int letter_increment = dest.GetLetter() - curr_addr.GetLetter();
	int number_increment = dest.GetNumber() - curr_addr.GetNumber();
	if (letter_increment < 0)
	{
		letter_increment = -letter_increment;
	}
	if (number_increment < 0)
	{
		number_increment = -number_increment;
	}
	if ((letter_increment > 1) || (number_increment > 1))
	{
		return CH_ERR_PIECE_CANNOT_GO_LIKE_THAT;
	}
	//is there someone?
	ChessPiece* piece = m_chess->GetPiece(dest);
	if (piece != NULL)
	{
		if (piece->GetIsBlack() == GetIsBlack())
		{	//friend, cannot go there
			return CH_ERR_CELL_OCCUPIED;
		} else {
			//enemy, kill it
			error = Capture(piece);
			if (error != CH_ERR_OK)
			{
				return error;
			}
			m_chess->OnCapture(this, piece);
		}
	}
	//empty, ok to move there
	ChessCell* next_cell = m_chess->GetCell(dest);
	ASSERT(next_cell != NULL);
	ChessCell* current_cell = GetCurrentCell();
	ASSERT(current_cell != NULL);
	error = current_cell->RemovePiece(this);
	if (error != CH_ERR_OK)
	{
		return error;
	}
	m_current_cell = NULL;
	error = next_cell->InsertPiece(this);
	if (error != CH_ERR_OK)
	{
		return error;
	}
	m_current_cell = next_cell;
	GetCurrentAddress(where_it_came);
	m_chess->OnMove(this, curr_addr, dest);
	return CH_ERR_OK;
}

ChessPiece::NextPositionIterator* King::GetNextPositionIterator() const
{
	GetNextAddressContext context;
	ChessAddress address = GetIteratorStartAddress(context);
	if (address.IsValid() == false)
	{
		address = ChessAddress(); //invalid
	}
	ChessCell* cell = m_chess->GetCell(address);
	if (cell != NULL)
	{
		return new NextPositionIterator(this, cell, context);
	} else {
		return NULL;
	}
}

ChessAddress King::GetNextAddress(const ChessAddress& address, GetNextAddressContext& context) const
{
	//king does not have directions, it moves only to nearest cells.
	ChessAddress current_address;
	unsigned int error = GetCurrentAddress(current_address);
	if (error != CH_ERR_OK)
	{
		throw Exception(error, L"King is not on board\n", EXC_HERE);
	}
	ASSERT(current_address.IsValid() == true);
	ChessAddress ret_val;
	enum
	{
		KING_MOVETO_LEFT_UP,
		KING_MOVETO_UP,
		KING_MOVETO_RIGHT_UP,
		KING_MOVETO_RIGHT,
		KING_MOVETO_RIGHT_DOWN,
		KING_MOVETO_DOWN,
		KING_MOVETO_LEFT_DOWN,
		KING_MOVETO_LEFT,
		KING_MOVETO_UNDEFINED
	};
	unsigned int moveto(KING_MOVETO_UNDEFINED);
	if (address.IsValid() == false)
	{
		moveto = KING_MOVETO_RIGHT_UP;
	}
	else if ((address.GetLetter() == current_address.GetLetter() + 1) &&
		(address.GetNumber() == current_address.GetNumber() - 1))
	{
		moveto = KING_MOVETO_UP;
	}
	else if ((address.GetLetter() == current_address.GetLetter()) &&
		(address.GetNumber() == current_address.GetNumber() - 1))
	{
		moveto = KING_MOVETO_LEFT_UP;
	}
	else if ((address.GetLetter() == current_address.GetLetter() - 1) &&
		(address.GetNumber() == current_address.GetNumber() - 1))
	{
		moveto = KING_MOVETO_LEFT;
	}
	else if ((address.GetLetter() == current_address.GetLetter() - 1) &&
		(address.GetNumber() == current_address.GetNumber()))
	{
		moveto = KING_MOVETO_LEFT_DOWN;
	}
	else if ((address.GetLetter() == current_address.GetLetter() - 1) &&
		(address.GetNumber() == current_address.GetNumber() + 1))
	{
		moveto = KING_MOVETO_DOWN;
	}
	else if ((address.GetLetter() == current_address.GetLetter()) &&
		(address.GetNumber() == current_address.GetNumber() + 1))
	{
		moveto = KING_MOVETO_RIGHT_DOWN;
	}
	else if ((address.GetLetter() == current_address.GetLetter() + 1) &&
		(address.GetNumber() == current_address.GetNumber() + 1))
	{
		moveto = KING_MOVETO_RIGHT;
	}
	else {
		return ret_val;
	}
	switch (moveto)
	{
	case KING_MOVETO_RIGHT_UP:
		ret_val.SetLetter(current_address.GetLetter() + 1);
		ret_val.SetNumber(current_address.GetNumber() - 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:;
			}
		}
	case KING_MOVETO_UP:
		ret_val.SetLetter(current_address.GetLetter());
		ret_val.SetNumber(current_address.GetNumber() - 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:;
			}
		}
	case KING_MOVETO_LEFT_UP:
		ret_val.SetLetter(current_address.GetLetter() - 1);
		ret_val.SetNumber(current_address.GetNumber() - 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:;
			}
		}
	case KING_MOVETO_LEFT:
		ret_val.SetLetter(current_address.GetLetter() - 1);
		ret_val.SetNumber(current_address.GetNumber());
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:;
			}
		}
	case KING_MOVETO_LEFT_DOWN:
		ret_val.SetLetter(current_address.GetLetter() - 1);
		ret_val.SetNumber(current_address.GetNumber() + 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:;
			}
		}
	case KING_MOVETO_DOWN:
		ret_val.SetLetter(current_address.GetLetter());
		ret_val.SetNumber(current_address.GetNumber() + 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:;
			}
		}
	case KING_MOVETO_RIGHT_DOWN:
		ret_val.SetLetter(current_address.GetLetter() + 1);
		ret_val.SetNumber(current_address.GetNumber() + 1);
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:;
			}
		}
	case KING_MOVETO_RIGHT:
		ret_val.SetLetter(current_address.GetLetter() + 1);
		ret_val.SetNumber(current_address.GetNumber());
		if (ret_val.IsValid())
		{
			switch (CanGoThere(ret_val))
			{
			case CAN_GO_CAN:
			case CAN_GO_KILL_ENEMY:
				return ret_val;
			case CAN_GO_CANNOT:;
			}
		}
	case KING_MOVETO_UNDEFINED:
	default:
		ret_val = ChessAddress(); //invalid
		return ret_val;
	}
}

bool King::operator == (CellContentType cct) const
{
	if (GetIsBlack())
	{
		if (cct == BLACK_KING)
		{
			return true;
		}
	} else {
		if (cct == WHITE_KING)
		{
			return true;
		}
	}
	return false;
}

CellContentType King::GetCellContentType() const
{
	if(GetIsBlack())
	{
		return BLACK_KING;
	} else {
		return WHITE_KING;
	}
}

Chess::Chess(BasicReadWriteLock* lock) :
	m_black_side(true),
	m_black_turn(false),
	m_rw_lock(lock)
{
	if (m_rw_lock != NULL)
	{
		if (m_rw_lock->LockForWrite() == false)
		{
			throw Exception(SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_READ,
				L"Cannot unlock chess on construction",
				EXC_HERE);
		}
	}
	memset(m_notification_handlers, 0, sizeof(m_notification_handlers));
	//set chess cells
	memset(m_cells, 0, sizeof(m_cells));
	bool prev_was_black = false;
	for (unsigned int index = 0; index < CH_CONST_CELLS_COUNT; ++index)
	{
		unsigned int number = index / CH_CONST_ROW_WIDTH;
		unsigned int letter = index - (number * CH_CONST_ROW_WIDTH);
		/*wchar_t msg[100];
		int l = swprintf(msg, L"index: %d, letter: %d, number: %d, chess_address: ", index, letter, number);*/
		ChessAddress addr(letter + 1, number + 1);
		/*l += addr.ToString(msg + l, 100 - l);
		msg[l] = L'\n';
		OutputDebugMsg(msg);*/
		ChessCell* cell = m_cells + index;
		if (prev_was_black == true)
		{
			cell->Init(addr, false);
			prev_was_black = false;
		} else {
			cell->Init(addr, true);
			prev_was_black = true;
		}
	}
	memset(m_captured_black, 0, sizeof(m_captured_black));
	memset(m_captured_white, 0, sizeof(m_captured_white));
	//ResetGame();
	if (m_rw_lock != NULL)
	{
		m_rw_lock->Unlock();
	}
}

Chess::~Chess()
{
	DeleteAllPieces();
}

unsigned int Chess::ResetGame(bool black_side)
{
	if (m_rw_lock != NULL)
	{
		if (m_rw_lock->LockForWrite() == false)
		{
			throw Exception(SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_WRITE,
				L"Cannot unlock chess board to reset game",
				EXC_HERE);
		}
	}
	DeleteAllPieces();
	m_black_side = black_side;
	m_black_turn = false;
	//set pieces in initial position
	unsigned int error_code = CH_ERR_OK;
	try
	{
		if (m_black_side)	//at lower rows
		{
			//create blacks
			ChessAddress addr(CH_LETTER_A, 8);
			ChessCell* current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			ChessPiece* piece = new Rook(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_B);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Knight(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_C);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Bishop(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_D);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new King(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_E);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Queen(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_F);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Bishop(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_G);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Knight(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_H);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Rook(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_A);
			addr.SetNumber(7);
			do
			{
				piece = new Pawn(this, addr, true);
				ASSERT(piece != NULL);
				current_cell = GetCell(addr);
				ASSERT(current_cell != NULL);
				error_code = current_cell->InsertPiece(piece);
			} while (addr.IncrementLetter() == true); 
			//create white
			addr.SetLetter(CH_LETTER_A);
			addr.SetNumber(1);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Rook(this, addr, false);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_B);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Knight(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_C);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Bishop(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_D);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new King(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_E);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Queen(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_F);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Bishop(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_G);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Knight(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_H);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Rook(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetNumber(2);
			addr.SetLetter(1);
			do 
			{
				piece = new Pawn(this, addr, false);
				ASSERT(piece != NULL);
				current_cell = GetCell(addr);
				ASSERT(current_cell != NULL);
				error_code = current_cell->InsertPiece(piece);
			} while (addr.IncrementLetter() == true);
		}
		else {
			//create white
			ChessAddress addr(CH_LETTER_A, 8);
			ChessCell* current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			ChessPiece* piece = new Rook(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_B);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Knight(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_C);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Bishop(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_D);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new King(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_E);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Queen(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_F);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Bishop(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_G);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Knight(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_H);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Rook(this, addr, false);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_A);
			addr.SetNumber(7);
			do
			{
				piece = new Pawn(this, addr, false);
				ASSERT(piece != NULL);
				current_cell = GetCell(addr);
				ASSERT(current_cell != NULL);
				error_code = current_cell->InsertPiece(piece);
				ASSERT(error_code == CH_ERR_OK);
			} while (addr.IncrementLetter() == true);
			//create black
			addr.SetLetter(CH_LETTER_A);
			addr.SetNumber(1);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Rook(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_B);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Knight(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_C);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Bishop(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_D);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new King(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_E);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Queen(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_F);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Bishop(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_G);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Knight(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetLetter(CH_LETTER_H);
			current_cell = GetCell(addr);
			ASSERT(current_cell != NULL);
			piece = new Rook(this, addr, true);
			ASSERT(piece != NULL);
			error_code = current_cell->InsertPiece(piece);
			ASSERT(error_code == CH_ERR_OK);
			addr.SetNumber(2);
			addr.SetLetter(CH_LETTER_A);
			do
			{
				piece = new Pawn(this, addr, true);
				ASSERT(piece != NULL);
				current_cell = GetCell(addr);
				ASSERT(current_cell != NULL);
				error_code = current_cell->InsertPiece(piece);
				ASSERT(error_code == CH_ERR_OK);
			} while (addr.IncrementLetter() == true);
		}
	} catch(Exception exc)
	{
		ShowMessageDialog(exc.GetErrorCode(), exc.GetErrorMessage());
		return exc.GetErrorCode();
	}
	if (m_rw_lock != NULL)
	{
		m_rw_lock->Unlock();
	}
	OnResetGame();
	return CH_ERR_OK;
}

unsigned int /*error code*/ Chess::CopyGame(const Chess& source)
{
	bool unlock_source = false;
	bool unlock_dest = false;
	unsigned int ret_val = CH_ERR_OK;
	if (m_rw_lock != NULL)
	{
		if (m_rw_lock->LockForWrite())
		{
			unlock_dest = true;
		} else {
			ret_val = SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_WRITE;
		}
	}
	if (source.m_rw_lock != NULL)
	{
		if (source.m_rw_lock->LockForRead())
		{
			unlock_source = true;
		} else {
			ret_val = SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_READ;
		}
	}
	ChessAddress address(CH_CONST_MIN_LETTER, CH_CONST_MIN_NUMBER);
	bool exit_flag = false;
	if (ret_val != CH_ERR_OK)
	{
		exit_flag = true;
	}
	while (exit_flag == false)
	{
		ChessPiece* piece = source.GetPiece(address);
		ChessPiece* new_piece = NULL;
		if (piece != NULL)
		{
			ChessCell* cell = GetCell(address);
			ASSERT(cell != NULL);
			if (typeid(*piece) == typeid(Pawn))
			{
				new_piece = new Pawn(this, address, piece->GetIsBlack(), dynamic_cast<Pawn*>(piece)->GetAlreadyMadeAMove());
			}
			else if (typeid(*piece) == typeid(Rook))
			{
				new_piece = new Rook(this, address, piece->GetIsBlack());
			}
			else if (typeid(*piece) == typeid(Knight))
			{
				new_piece = new Knight(this, address, piece->GetIsBlack());
			}
			else if (typeid(*piece) == typeid(Bishop))
			{
				new_piece = new Bishop(this, address, piece->GetIsBlack());
			}
			else if (typeid(*piece) == typeid(Queen))
			{
				new_piece = new Queen(this, address, piece->GetIsBlack());
			}
			else if (typeid(*piece) == typeid(King))
			{
				new_piece = new King(this, address, piece->GetIsBlack());
			}
			else {
				ASSERT(false);	//shouldn't come here, no other types of pieces.
			}
			unsigned int error = cell->InsertPiece(new_piece);
			ASSERT(error == CH_ERR_OK);
			m_black_side = source.m_black_side;
			m_black_turn = source.GetIsBlackTurn();
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
	if (unlock_source)
	{
		ASSERT(source.m_rw_lock != NULL);
		source.m_rw_lock->Unlock();
	}
	if (unlock_dest)
	{
		ASSERT(m_rw_lock != NULL);
		m_rw_lock->Unlock();
	}
	return ret_val;
}

unsigned int /*error code*/ Chess::LoadDisposition(const Disposition& disposition)
{
	if (m_rw_lock != NULL)
	{
		if (m_rw_lock->LockForWrite() == false)
		{
			throw Exception(SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_WRITE,
				L"Cannot lock a board for write",
				EXC_HERE);
		}
	}
	DeleteAllPieces();
	ChessPiece* piece = NULL;
	ChessCell* cell = NULL;
	unsigned int error = CH_ERR_UNDEFINED_ERROR;
	unsigned int ret_val = CH_ERR_UNDEFINED_ERROR;

	#define LOAD_DISPOSITION_NEW_PIECE(address, type, is_black)	\
	if (address.IsValid())										\
	{															\
		ChessPiece* piece = new type(this, address, is_black);	\
		ASSERT(piece != NULL);									\
		ChessCell* cell = GetCell(address);						\
		ASSERT(cell != NULL);									\
		unsigned int error = cell->InsertPiece(piece);			\
		if (error != CH_ERR_OK)									\
		{														\
			return error;										\
		}														\
	} else {													\
		return CH_ERR_INVALID_CHESS_ADDRESS;					\
	}

	m_black_side = disposition.GetIsBlackSideLower();
	m_black_turn = disposition.GetTurn();
	bool exit_flag = false;
	ChessAddress address(CH_CONST_MIN_LETTER, CH_CONST_MIN_NUMBER);
	while (exit_flag == false)
	{
		unsigned int type = disposition.GetPiece(address);
		switch (type)
		{
		case EMPTY: break;
		case BLACK_PAWN: LOAD_DISPOSITION_NEW_PIECE(address, Pawn, true); break;
		case BLACK_ROOK: LOAD_DISPOSITION_NEW_PIECE(address, Rook, true); break;
		case BLACK_KNIGHT: LOAD_DISPOSITION_NEW_PIECE(address, Knight, true); break;
		case BLACK_BISHOP: LOAD_DISPOSITION_NEW_PIECE(address, Bishop, true); break;
		case BLACK_QUEEN: LOAD_DISPOSITION_NEW_PIECE(address, Queen, true); break;
		case BLACK_KING: LOAD_DISPOSITION_NEW_PIECE(address, King, true); break;
		case WHITE_PAWN: LOAD_DISPOSITION_NEW_PIECE(address, Pawn, false); break;
		case WHITE_ROOK: LOAD_DISPOSITION_NEW_PIECE(address, Rook, false); break;
		case WHITE_KNIGHT: LOAD_DISPOSITION_NEW_PIECE(address, Knight, false); break;
		case WHITE_BISHOP: LOAD_DISPOSITION_NEW_PIECE(address, Bishop, false); break;
		case WHITE_QUEEN: LOAD_DISPOSITION_NEW_PIECE(address, Queen, false); break;
		case WHITE_KING: LOAD_DISPOSITION_NEW_PIECE(address, King, false); break;
		default:
			ShowMessageDialog(L"Cannot load disposition, wrong piece type");
			return CH_ERR_INVALID_CHESS_TYPE;
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
	if (m_rw_lock != NULL)
	{
		m_rw_lock->Unlock();
	}
	return CH_ERR_OK;
}

unsigned int /*error code*/ Chess::SaveDisposition(Disposition& out_disposition) const
{
	if (m_rw_lock != NULL)
	{
		if (m_rw_lock->LockForRead() == false)
		{
			throw Exception(SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_READ,
				L"Cannot lock a board for read",
				EXC_HERE);
		}
	}
	ChessAddress address(CH_CONST_MIN_LETTER, CH_CONST_MIN_NUMBER);
	unsigned int error = CH_ERR_UNDEFINED_ERROR;
	out_disposition.SetIsBlackLower(m_black_side);
	out_disposition.SetTurn(m_black_turn);
	bool exit_flag(false);
	while (exit_flag == false)
	{
		ChessPiece* piece = GetPiece(address);
		if (piece != NULL)
		{
			if (typeid(*piece) == typeid(Pawn))
			{
				if (piece->GetIsBlack())
				{
					out_disposition.SetPiece(address, BLACK_PAWN);
				}
				else {
					out_disposition.SetPiece(address, WHITE_PAWN);
				}
			}
			else if (typeid(*piece) == typeid(Rook))
			{
				if (piece->GetIsBlack())
				{
					out_disposition.SetPiece(address, BLACK_ROOK);
				}
				else {
					out_disposition.SetPiece(address, WHITE_ROOK);
				}
			}
			else if (typeid(*piece) == typeid(Knight))
			{
				if (piece->GetIsBlack())
				{
					out_disposition.SetPiece(address, BLACK_KNIGHT);
				}
				else {
					out_disposition.SetPiece(address, WHITE_KNIGHT);
				}
			}
			else if (typeid(*piece) == typeid(Bishop))
			{
				if (piece->GetIsBlack())
				{
					out_disposition.SetPiece(address, BLACK_BISHOP);
				}
				else {
					out_disposition.SetPiece(address, WHITE_BISHOP);
				}
			}
			else if (typeid(*piece) == typeid(Queen))
			{
				if (piece->GetIsBlack())
				{
					out_disposition.SetPiece(address, BLACK_QUEEN);
				}
				else {
					out_disposition.SetPiece(address, WHITE_QUEEN);

				}
			}
			else if (typeid(*piece) == typeid(King))
			{
				if (piece->GetIsBlack())
				{
					out_disposition.SetPiece(address, BLACK_KING);
				}
				else {
					out_disposition.SetPiece(address, WHITE_KING);
				}
			}
		} else {
			out_disposition.SetPiece(address, EMPTY);
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
	if (m_rw_lock != NULL)
	{
		m_rw_lock->Unlock();
	}
	return CH_ERR_OK;
}

unsigned int /*error code*/ Chess::MovePiece(ChessPiece* piece, const ChessAddress& where, ChessAddress& where_it_came)
{
	if (m_rw_lock != NULL)
	{
		if (m_rw_lock->LockForWrite() == false)
		{
			throw Exception(SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_WRITE,
				L"Cannot lock a board for write",
				EXC_HERE);
		}
	}
	unsigned int ret_val = CH_ERR_UNDEFINED_ERROR;
	if (piece == NULL)
	{
		ret_val = CH_ERR_NO_PIECE_SPECIFIED;
	}
	if ((ret_val == CH_ERR_UNDEFINED_ERROR) && (where.IsValid() == false))
	{
		ret_val = CH_ERR_INVALID_CHESS_ADDRESS;
	}
	if ((ret_val == CH_ERR_UNDEFINED_ERROR) && (piece->GetIsBlack() != m_black_turn))
	{
		ret_val = CH_ERR_WRONG_TURN;
	}
	if (ret_val == CH_ERR_UNDEFINED_ERROR)
	{
		ret_val = piece->MoveTo(where, where_it_came);
	}
	if (m_rw_lock != NULL)
	{
		m_rw_lock->Unlock();
	}
	return ret_val;
}

ChessCell* Chess::GetCell(const ChessAddress& addr) const
{
	ASSERT(addr.GetNumber() > 0);
	ChessCell* ret_val = NULL;
	if (addr.IsValid())
	{
		if (m_rw_lock != NULL)
		{
			if (m_rw_lock->LockForRead() == false)
			{
				throw Exception(SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_READ,
					L"Cannot lock a board for read",
					EXC_HERE);
			}
		}
		unsigned int offset_in_array = (addr.GetNumber() - 1) * CH_CONST_ROW_WIDTH; //offset to next row's first element
		offset_in_array += (addr.GetLetter() - 1);
		ret_val = ((ChessCell*)m_cells) + offset_in_array;
		if (m_rw_lock != NULL)
		{
			m_rw_lock->Unlock();
		}
	}
	return ret_val;
}

unsigned int /*error code*/ Chess::CapturePiece(ChessPiece* piece)
{
	if (m_rw_lock != NULL)
	{
		if (m_rw_lock->LockForWrite() == false)
		{
			throw Exception(SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_WRITE,
				L"Cannot lock a board for write",
				EXC_HERE);
		}
	}
	ChessCell* cell = piece->GetCurrentCell();
	if (cell == NULL)
	{
		return CH_ERR_PIECE_NOT_ON_BOARD;
	}
	unsigned int error = CH_ERR_OK;
	//just move a piece from the board to captured array.
	if (piece->GetIsBlack() == true)
	{
		for (unsigned int index = 0; index < CH_CONST_TOTAL_PIECES_COUNT; index++)
		{
			ASSERT(m_captured_black[index] != piece);
			if (m_captured_black[index] == NULL)
			{
				m_captured_black[index] = piece;
				break;
			}
		}
		error = cell->RemovePiece(piece);
	}
	else {
		for (unsigned int index = 0; index < CH_CONST_TOTAL_PIECES_COUNT; index++)
		{
			//add debug check if that piece is already there
			#ifdef _DEBUG_
			if(m_captured_white[index] == piece)
			{
				ASSERT(true);
			}
			#endif //_DEBUG_
			if (m_captured_white[index] == NULL)
			{
				m_captured_white[index] = piece;
				break;
			}
		}
		error = cell->RemovePiece(piece);
	}
	if (m_rw_lock != NULL)
	{
		m_rw_lock->Unlock();
	}
	return error;
}

void Chess::DeleteAllPieces()
{
	if (m_rw_lock != NULL)
	{
		if (m_rw_lock->LockForWrite() == false)
		{
			throw Exception(SYNCHRONIZATION_ERROR_CANNOT_GET_LOCK_FOR_WRITE,
				L"Cannot lock a board for write",
				EXC_HERE);
		}
	}
	for (unsigned int index = 0; index < CH_CONST_CELLS_COUNT; ++index)
	{
		ChessCell* cell = m_cells + index;
		ChessPiece* piece = cell->GetPiece();
		if (piece != NULL)
		{
			unsigned int err = cell->RemovePiece(piece);
			ASSERT(err == CH_ERR_OK);
			delete piece;
		}
	}
	for (unsigned int index = 0; index < CH_CONST_TOTAL_PIECES_COUNT; index++)
	{
		ChessPiece* piece = m_captured_black[index];
		if (piece != NULL)
		{
			delete piece;
			m_captured_black[index] = NULL;
		}
		piece = m_captured_white[index];
		if (piece != NULL)
		{
			delete piece;
			m_captured_white[index] = NULL;
		}
	}
	if (m_rw_lock != NULL)
	{
		m_rw_lock->Unlock();
	}
}

void Chess::OnMove(ChessPiece* piece, const ChessAddress& prev, const ChessAddress& next)
{
	m_black_turn = !m_black_turn;
	InvokeNotificationHandlers(this, OnMove, (this, piece, prev, next));
}

void Chess::OnCapture(ChessPiece* winner, ChessPiece* loser)
{
	InvokeNotificationHandlers(this, OnCapture, (this, winner,loser));
}

void Chess::OnResetGame()
{
	InvokeNotificationHandlers(this, OnResetGame, (this));
}

bool Chess::Compare(const Chess& rval) const
{
	bool differenece_found = false;
	bool locked_this = false;
	bool locked_rval = false;
	if (m_rw_lock != NULL)
	{
		if (m_rw_lock->LockForRead())
		{
			locked_this = true;
		}
	}
	if (rval.m_rw_lock != NULL)
	{
		if (rval.m_rw_lock->LockForRead())
		{
			locked_rval = true;
		}
	}
	if (((m_rw_lock == NULL) || (locked_this)) && ((rval.m_rw_lock == NULL) || (locked_rval)))
	{
		for (unsigned int index = 0; index < CH_CONST_CELLS_COUNT; ++index)
		{
			ChessPiece* l_piece = m_cells[index].GetPiece();
			ChessPiece* r_piece = rval.m_cells[index].GetPiece();
			if((l_piece == NULL) && (r_piece == NULL))
			{
				continue;
			} else if ((l_piece != NULL) && (r_piece != NULL))
			{
				if (typeid(*l_piece) == typeid(*r_piece))
				{
					continue;
				} else {
					differenece_found = true;
					break;
				}
			} else {
				differenece_found = true;
				break;
			}
		}
	}
	if (locked_rval)
	{
		rval.m_rw_lock->Unlock();
	}
	if (locked_this)
	{
		m_rw_lock->Unlock();
	}
	return !differenece_found;
}