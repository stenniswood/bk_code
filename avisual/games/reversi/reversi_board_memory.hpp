#ifndef _OTHELLO_BOARD_MEMORY_
#define _OTHELLO_BOARD_MEMORY_

#define bool bool 

const byte MAX_X   = 8;
const byte MAX_Y   = 8;

const byte NOT_PLACED   = 0;
const byte WHITE_PLAYER = 1;
const byte BLACK_PLAYER = 2;

//byte ALREADY = 4;
//byte NO_GUESS= 8;

struct GridPt {
	byte row;
	byte col;
};

class BoardMemory 
{
	friend class AI_Player;
	
public:
	BoardMemory();

	void clearSlate();
	bool checkValidPlacement( short row, short col 	);
	byte getOppositeColor	( byte mPlayer 			);
	bool flip_square		( byte row, byte col	);
	byte get_square_color	( byte row, byte col	);
	void reverseColumn		( byte row, byte col, byte switchingColor, bool mUp );
	void reverseRow			( byte row, byte col, byte switchingColor, bool mRight );
	bool scanCol			( byte row, byte col, byte mPlayer, bool mUp 		);
	bool scanRow			( byte row, byte col, byte mPlayer, bool mRight		);
	int  countColors		( byte mPlayer 						);
	bool scanDiagonal		( byte row, byte col, short row_inc, short col_inc, byte mPlayer );
	void reverseDiagonal	( byte row, byte col, short row_inc, short col_inc, byte switchingColor );
	void flipChips			( byte Row, byte Col, byte mPlayer	);
	bool isLegalMove		( byte row, byte col, byte mPlayer	);
	bool EvaluateGuess		( byte row, byte col, byte mPlayer	);
	bool isGameOver			( 									);
	bool validMovePossible	( byte mPlayer						);
	bool validMoveEitherPlayer();	
	
private:
	//byte board[][] = new byte[MAX_Y][MAX_X];
	byte board[MAX_Y][MAX_X];
};


#endif
