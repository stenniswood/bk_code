#ifndef _REVERSI_BOARD_
#define _REVERSI_BOARD_

#include <string>
#include "control.hpp"
#include "reversi_board_memory.hpp"

extern int    TOP_MARGIN;
extern int    LEFT_MARGIN;
extern int    RIGHT_MARGIN;
extern int    BOTTOM_MARGIN;

const	int BINS 	=8;
const 	int VH_LINES =BINS+1;


class ReversiBoardView : public Control
{
public:
	ReversiBoardView();

	void	draw_background  (  );
	void 	drawPiece		 (int row, int col, byte mPlayer);
	void 	drawAllPieces	 (	);
	virtual int 	draw	 (	);
	
	void 	drawGrid		 (	);
	void 	drawGuess		 ( int row, int col 	);
	//void 	doGuess			 ( byte row, byte col	);  pull out to another location
	bool 	computeBinTouched( float x, float y		);
	bool 	onTouchEvent	 ( /*MotionEvent event*/);	
	void 	calculatePreliminaries( );
	int		onClick			 ( int x, int y, bool mouse_is_down);
	void	doGuess			 ( byte row, byte col, byte mColor);
	void	start_over		 ( );
	std::string 	form_score_string( );
	
	// Import Rectangle class from home_3D maybe.	
	//Rectangle* 	calc_square_rect(int row, int col );

	BoardMemory*	bm;
	bool 	GuessPending;
	
private:
	int    right;
	
	//int    bottom;
	int    incw	;
	int    inch	;

	int    	xcols[VH_LINES];
	int    	ycols[VH_LINES];

    byte  	TouchedColumn;
    byte  	TouchedRow	;
    int    	touched_x 	;
    int    	touched_y 	;
	bool 	TouchAllowed;
};

#endif
