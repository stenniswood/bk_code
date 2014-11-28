#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

//#include <fontinfo.h>
#include <shapes.h>
#include "bk_system_defs.h"
#include "control.hpp"
#include "tk_colors.hpp"
#include "display.h"
#include "rectangle.hpp"

#include "reversi_board_memory.hpp"
//#include "reversi_ai_player.hpp"
#include "reversi_board.hpp"




bool TouchIsHorizontal = false;
Rectangle temp_ship_r;

ReversiBoardView::ReversiBoardView()
{
	bm   	= new BoardMemory();
	GuessPending	= false;
	incw			=0;
	inch			=0;

	//long	white	= 0xFFFFFFFF;
	//long	black	= 0xFF000000;
	//long	blue	= 0xFF00FFFF;	// Color cyan( 0 ,  255,  255); 

    TouchedColumn=-2;
    TouchedRow	 =-2;
    touched_x 	 = 0;
    touched_y 	 = 0;
	TouchAllowed = true;
}

void ReversiBoardView::calculatePreliminaries( )
{
	int rwidth  = width -(LEFT_MARGIN+RIGHT_MARGIN);
	int rheight = height-(TOP_MARGIN+BOTTOM_MARGIN);
	incw   	= (rwidth)/BINS;
	inch   	= (rheight)/BINS;
	right  	= LEFT_MARGIN  + BINS*incw;
	bottom 	= RIGHT_MARGIN + BINS*inch;

	int i=0;
	for (int x=LEFT_MARGIN; x<right+1; x+=incw)
		xcols[i++] = x;
	i=0;
	for (int y=TOP_MARGIN; y<bottom+1; y+=inch)
		ycols[i++] = y;
}



// Zero indexed rows and cols:
Rectangle* ReversiBoardView::calc_square_rect(int row, int col )
{
	static Rectangle* tmp   = new Rectangle();

	tmp->set_left ( xcols[col]  );
	tmp->set_right( xcols[col+1] );     // thin ones
	tmp->set_top  ( ycols[row+1] );
	tmp->set_bottom ( ycols[row] );		// 
	return tmp;
}

// Canvas canvas
void ReversiBoardView::drawPiece(int row, int col, byte mPlayer)
{	
	float cx = (xcols[col] + xcols[col+1]) / 2;			// center x 
	float cy = (ycols[row] + ycols[row+1]) / 2;			// center y
	
	float dx = (xcols[row+1] - xcols[row]);
	float dy = (ycols[col+1] - ycols[col]);
	float radius ;
	if (dx<dy)	radius = dx/2;  else radius = dy/2;
	radius *= 0.9;

	if (mPlayer==1)
		Fill_l(0xFFFFFFFF);
	else
		Fill_l(0xFF000000);

	Circle(cx, cy, radius);		
}

/*Canvas canvas*/
void ReversiBoardView::drawAllPieces()
{
	for (byte r=0; r<BINS; r++)
		for (byte c=0; c<BINS; c++)
			if ( bm->get_square_color(r,c) != NOT_PLACED )
				drawPiece(r,c, bm->get_square_color(r,c));
}

void ReversiBoardView::onDraw() {
	calculatePreliminaries( );
	drawGrid     ();
	drawAllPieces();
	if (GuessPending)
		drawGuess( TouchedRow,TouchedColumn);
}

void Rect_d( Rectangle* mRect )
{
	Rect(mRect->get_left(), mRect->get_bottom(), mRect->get_width(), mRect->get_height() );
}

void ReversiBoardView::drawGrid()
{
	Fill_l(0xFF000000);
	
	// DRAW COLUMNS:
	for (int i=0; i<VH_LINES; i++)
		Line( xcols[i], TOP_MARGIN, xcols[i], bottom );

	// DRAW ROWS:
	for (int i=0; i<VH_LINES; i++)
		Line( LEFT_MARGIN, ycols[i], right, ycols[i]);	

	//Paint border = new Paint();
	Stroke_c( black );
	StrokeWidth(4);

	Rectangle r;
	r.set_left	( xcols[2]+1   );
	r.set_right	( xcols[6]-1   );
	r.set_top	( ycols[2]+1   );
	r.set_bottom( ycols[6]-1   );
	Rect_d(&r);	
}

void ReversiBoardView::drawGuess(int row, int col )
{  
	// While waiting for network to reply.
	float cx = (xcols[col] + xcols[col+1]) / 2;			// center x 
	float cy = (ycols[row] + ycols[row+1]) / 2;			// center y
	
	float dx = (xcols[row+1] - xcols[row]);
	float dy = (ycols[col+1] - ycols[col]);
	float radius ;
	if (dx<dy)	radius = dx/2;  else radius = dy/2;
	radius *= 0.9;

	Stroke_c( blue );
	// need fill too?	i don't remember blue pieces.
	Circle(cx, cy, radius);
}	

/*void ReversiBoardView::doGuess(byte row, byte col, byte mColor)
{
	if (gs.online_Partner)
	{
		GuessPending = true;
		TouchAllowed = false;
		OthelloActivity.sc.Send_My_Guess(row, col, OthelloActivity.dlg.prefMyColor );
	} 
	else  
	{
		// SINGLE PLAYER MODE:
		bm->EvaluateGuess( row, col, mColor );
		if (GameSequencer.MyTurn) {					// White player
			gs.TakeMyTurn();
		} else {									// Black Player
			gs.FriendsTurnOver();						
		}
	}
}*/

bool ReversiBoardView::computeBinTouched(float x, float y)
{
	touched_x = round( x );
	touched_y = round( y );
	TouchedColumn = BINS+1;
	TouchedRow    = BINS+1;
	for (byte i=0; i<VH_LINES; i++)
	{
		if (touched_x < xcols[i]) { TouchedColumn = (byte) (i-1); break; }
	}
	for (byte i=0; i<VH_LINES; i++)
	{
		if (touched_y < ycols[i]) { TouchedRow = (byte) (i-1); break; 	}
	}
	if (TouchedRow<0   ) 		TouchedRow 	  = 0;
	if (TouchedColumn<0) 		TouchedColumn = 0;
	return ((TouchedColumn<(BINS+1)) && (TouchedRow<(BINS+1)));
}

/*MotionEvent event*/
bool ReversiBoardView::onTouchEvent(/*MotionEvent event*/) 
{
/*	try {
	if (TouchAllowed==false) return false;

	if (computeBinTouched( event.getX(), event.getY() )==false) 
		return true;

	int act = event.getActionMasked();
	if ((act == MotionEvent.ACTION_DOWN) || (act == MotionEvent.ACTION_MOVE))
	{
		int num_fingers = event.getPointerCount();
		float pressure = event.getPressure(1);			
		if ((num_fingers>0) && (pressure>0.1))
		{
			int x1,x2,y1,y2;
			x1 = (int) event.getX(0);
			y1 = (int) event.getY(0);
			x2 = (int) event.getX(1);
			y2 = (int) event.getY(1);
			int delta_x = Math.abs( x2-x1 );
			int delta_y = Math.abs( y2-y1 );
			if (delta_x > delta_y)
			{
					TouchIsHorizontal = true;
			} else {
					TouchIsHorizontal = false;		
			}
		}
	}
	if (act == MotionEvent.ACTION_UP)
	{	
		switch (OthelloActivity.gs.State)
		{
			case GameSequencer.INITIALIZED:
				OthelloActivity.gs.State = GameSequencer.GAME_IN_PLAY;
				break;
			case GameSequencer.GAME_IN_PLAY:
				if ((OthelloActivity.gs.online_Partner==true) && (GameSequencer.MyTurn==false))
					return false;
				
				byte mplayer  = OthelloActivity.gs.get_square_color();
				bool valid = bm->isLegalMove(TouchedRow, TouchedColumn, mplayer );
				if (valid)
				{
					doGuess(TouchedRow, TouchedColumn);
				} else
					OthelloActivity.oa.playSound( OthelloActivity.oa.SOUND_DEACTIVATE );
				break;
		}
	}
	invalidate();
	} catch (ArrayIndexOutOfBoundsException e) {
		Log.d( e.getMessage(), "SJT");
	} */
	return true;
}


