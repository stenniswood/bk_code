#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "bk_system_defs.h"
#include "othello_board_memory.hpp"



	

BoardMemory::BoardMemory()
{
	clearSlate();
}

void BoardMemory::clearSlate()
{
	for (int x=0; x<MAX_X; x++)
		for (int y=0; y<MAX_Y; y++)
			board[x][y] = 0;
	
	board[3][3] = 1;
	board[4][3] = 2;
	
	board[3][4] = 2;
	board[4][4] = 1;
}

bool BoardMemory::checkValidPlacement( short row, short col )
{
	if ((row < 0 ) || (row > MAX_Y))
		return false;
	if ((col < 0 ) || (col > MAX_X))
		return false;
	return true;
}

byte BoardMemory::getOppositeColor( byte mPlayer )
{
	if (mPlayer == WHITE_PLAYER)
		return BLACK_PLAYER;
	else
		return WHITE_PLAYER;		
}
bool BoardMemory::flip_square(byte row, byte col)
{
	byte value = board[row][col];
	if (value==NOT_PLACED)
		return false;
	board[row][col] = getOppositeColor(value);
	return true;
}
/***
 * 
 * @param row
 * @param col
 * @return
 */
byte BoardMemory::get_square_color( byte row, byte col)
{
	if (!checkValidPlacement(row,col))
		return (byte) 0xFF;
	return board[row][col];
}
/****************************************************************************
 *   Flips all colors in a column until it reaches one of it's our colors.
 *   
 * @param row
 * @param col
 * @param switchingColor
 * @param mUp
 ****************************************************************************/
void BoardMemory::reverseColumn( byte row, byte col, byte switchingColor, bool mUp )
{
	short row_inc;
	if (mUp) row_inc=-1; else row_inc=+1;
	reverseDiagonal( row, col, row_inc, (short)0, switchingColor );
}
/****************************************************************************
 *   Flips all colors in a column until it reaches one of it's our colors.
 *   
 * @param row				- row of placed chip
 * @param col				- col of placed chip
 * @param switchingColor	- WHITE/BLACK, the color that will switch
 * @param mRight			- Scan from left to right.
 ****************************************************************************/
void BoardMemory::reverseRow( byte row, byte col, byte switchingColor, bool mRight )
{		
	short col_inc;
	if (mRight) col_inc=+1; else col_inc=-1;
	reverseDiagonal( row, col, (short)0, col_inc, switchingColor );
}
/******************************************************************************
 * This is used to determine if the placed location is acceptable.
 * @param row		- placed row
 * @param col		- placed col
 * @param mPlayer	- placing color
 * @param mUp		- scan up/down
 * @return
 ******************************************************************************/
bool BoardMemory::scanCol( byte row, byte col, byte mPlayer, bool mUp )
{
	int row_inc;
	if (mUp) row_inc = -1; else row_inc = +1;
	return scanDiagonal( row, col, (short)row_inc, (short)0, mPlayer);
}
/******************************************************************************
 * This is used to determine if the placed location is acceptable.
 * @param row		- placed row
 * @param col		- placed col
 * @param row_inc	- increment vertically
 * @param col_inc	- increment horizontally
 * @param mPlayer	- placing color
 * @return
 ******************************************************************************/	
bool BoardMemory::scanRow( byte row, byte col, byte mPlayer, bool mRight)
{
	int col_inc;
	if (mRight) col_inc = 1; else col_inc = -1;
	return scanDiagonal( row,col, (short)0, (short)col_inc, mPlayer);
}
/***
 * Count up how many of each piece is currently on the board.
 * @param 	mPlayer  
 * @return	Number of chips of a certain color (white/black)
 */
int BoardMemory::countColors( byte mPlayer )
{
	int count = 0;
	for (int x=0; x<MAX_X; x++)
		for (int y=0; y<MAX_Y; y++)
			if (board[y][x]==mPlayer)
				count++;
	return count;
}
/******************************************************************************
 * This is used to determine if the placed location is acceptable.
 * @param row		- placed row
 * @param col		- placed col
 * @param mPlayer	- placing color
 * @param mRight	- scan right/left
 * @return			- true if valid placement
 ******************************************************************************/
bool BoardMemory::scanDiagonal( byte row, byte col, short row_inc, short col_inc, byte mPlayer )
{
	bool first_iteration = true;
	bool opposite_found = false;		// must first find another player's color (to sandwhich)
	byte    opp  = getOppositeColor( mPlayer );

	byte    value;
	col += col_inc;  
	row += row_inc;

	while (((col>=0) && (col<MAX_X)) && ((row>=0) && (row<MAX_Y)))
	{
		value = board[row][col];
		if (value==NOT_PLACED)
			return false;
		if (value == opp)
			opposite_found = true;
		else if ((value == mPlayer) && (opposite_found))
			return true;

		col += col_inc;
		row += row_inc;
		
		// the first iteration has to have an opposite color, 
		// or else not valid (for this direction at least)	
		if ((first_iteration) && (!opposite_found))
			return false;
		first_iteration = false;
	}
	return false;
}
/***************************************************************
 * 
 * @param row
 * @param col
 * @param mPlayer
 * @param mRight
 * @return
 ****************************************************************/
void BoardMemory::reverseDiagonal( byte row, byte col, short row_inc, short col_inc, byte switchingColor )
{
	//bool opposite_found = false;		// must first find another player's color (to sandwhich)
	byte    opp  = getOppositeColor( switchingColor );
	byte    value;
	col += col_inc;  
	row += row_inc;

	while (  ((col>=0) && (col<MAX_X))  &&  ((row>=0) && (row<MAX_Y)) )		        
	{
		value = board[row][col];
		if (value == switchingColor)
			board[row][col] = opp;
		else if (value == opp)
			return;
		
		col += col_inc;  
		row += row_inc;
	}
	return;
}
/***************************************************************
 *	After the chip is placed, adjust all pieces. 
 * 		Scan left, right, up, down.
 * 
 * @param Row - The row of the placed chip
 * @param Col - The col of the placed chip
 * @param mPlayer	 - The player number WHITE / BLACK
 ****************************************************************/
void BoardMemory::flipChips( byte Row, byte Col, byte mPlayer )
{
	byte value = (byte)board[Row][Col];
	byte opposite = getOppositeColor(value);

	// LOOK UP/DOWN:
	bool longRange = scanCol(Row,Col,mPlayer,true);
	if (longRange)
		reverseColumn (Row, Col, opposite, true );

	longRange = scanCol(Row,Col,mPlayer,false);
	if (longRange)
		reverseColumn (Row, Col, opposite, false);

	// LOOK LEFT/RIGHT:
	longRange = scanRow(Row,Col,mPlayer,false);
	if (longRange)
		reverseRow(Row, Col, opposite, false );

	longRange = scanRow(Row,Col,mPlayer,true);
	if (longRange)
		reverseRow(Row, Col, opposite, true );

	
	// DIAGONALS:
	longRange = scanDiagonal(Row,Col,  (short)-1,(short)-1, mPlayer	  );
	if (longRange)
		reverseDiagonal     (Row, Col, (short)-1,(short)-1, opposite  );

	longRange = scanDiagonal(Row, Col, (short)-1, (short)+1, mPlayer  );
	if (longRange)
		reverseDiagonal		(Row, Col, (short)-1, (short)+1, opposite );
	
	longRange = scanDiagonal(Row, Col, (short)+1, (short)+1, mPlayer  );
	if (longRange)
		reverseDiagonal		(Row, Col, (short)+1, (short)+1, opposite );

	longRange = scanDiagonal(Row, Col, (short)+1, (short)-1, mPlayer  );
	if (longRange)
		reverseDiagonal		(Row, Col, (short)+1, (short)-1, opposite );
	
}

bool BoardMemory::isLegalMove( byte row, byte col, byte mPlayer )
{   
	if (board[row][col] != NOT_PLACED)
		return false;				// Square is already occupied
	
	bool longRange = false;		// to be placed, the players color must be present at the end of at least 1 row/col.
	
	// To be placed, 1 of the 4 surrounding places must be opposite color.
	if ((row+1)<MAX_Y)
		longRange = scanCol(row,col,mPlayer,false);
	if (longRange) return true;

	if (row>0)	
		longRange = scanCol(row,col,mPlayer,true );
	if (longRange) return true;
	
	if ((col+1)<MAX_X)	
		longRange = scanRow(row,col,mPlayer,true );
	if (longRange) return true;
	
	if (col>0)	
		longRange = scanRow(row,col,mPlayer,false);
	if (longRange) return true;
	
	// CHECK DIAGONALS:
	if ((row>0) && (col>0))  {
		longRange = scanDiagonal(row, col, (short)-1, (short)-1, mPlayer );		if (longRange) return true; 
	}
	if ((row>0) && (col<MAX_X))  {
		longRange = scanDiagonal(row, col, (short)-1, (short)+1, mPlayer );		if (longRange) return true;	
	}		
	if ((row<MAX_Y) && (col>0))  {
		longRange = scanDiagonal(row, col, (short)+1, (short)-1, mPlayer );		if (longRange) return true;	
	}
	if ((row<MAX_Y) && (col<MAX_X))  {
		longRange = scanDiagonal(row, col, (short)+1, (short)+1, mPlayer );		if (longRange) return true; 
	}
	return false;
}
/**
 * Evaluate Guess 
 * @param row
 * @param col
 * @return  Possible placement / not possible.
 */
bool BoardMemory::EvaluateGuess(byte row, byte col, byte mPlayer)
{
	board[row][col] = mPlayer;
	GameSequencer.player_passes = 0;

	OthelloActivity.oa.playSound( OthelloActivity.oa.SOUND_ACTIVATE );

	// Move from Initialized to GameInPlay.
	// If Game_Over state, we should not enter this function (touches not allowed).
	OthelloActivity.gs.State = GameSequencer.GAME_IN_PLAY;		
	
	flipChips( row, col, mPlayer );
	return true;
}
	
bool BoardMemory::isGameOver()
{		
	// GAME OVER IF ANY PLAYER IS WIPED OUT:
	int white_count = countColors((byte)1);
	int black_count = countColors((byte)2);
	if (white_count==0) return true;
	if (black_count==0) return true;
	
	// A FULL BOARD:
	for (byte x=0; x < MAX_X; x++)
		for (byte y=0; y<MAX_Y; y++)
		{
			if (board[x][y] == NOT_PLACED)		// if any square is not placed, then not over.
				return false;
		}
	return true;
}
/***********************************************************************************
 * Scans all available places and evaluates if a move is possible for the player.
 * Caller can decide if what to do if not possible.
 * 
 * @return	true - if valid Move is possible.	
 ***********************************************************************************/
bool BoardMemory::validMovePossible(byte mPlayer)
{
	for (byte r=0; r < MAX_X; r++)
		for (byte c=0; c<MAX_Y; c++)
		{
			if (board[r][c]==NOT_PLACED)
				if (isLegalMove( r,c, mPlayer )==true)
					return true;
		}
	return false;
}
/***********************************************************************************
 * Scans all available places and evaluates if a move is possible for the player.
 * Caller can decide if what to do if not possible.
 * 
 * @return	true - if valid Move is possible.	
 ***********************************************************************************/
bool BoardMemory::validMoveEitherPlayer()
{
	if (validMovePossible(BoardMemory.WHITE_PLAYER)==false)
		if (validMovePossible(BoardMemory.BLACK_PLAYER)==false)
			return false;
	return true;
}



