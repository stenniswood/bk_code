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
#include "display_manager.hpp"


int    TOP_MARGIN	=10;
int    LEFT_MARGIN	=10;
int    RIGHT_MARGIN	=10;
int    BOTTOM_MARGIN=10;


bool TouchIsHorizontal = false;
Rectangle temp_ship_r;

ReversiBoardView::ReversiBoardView()
{
	bm   			= new BoardMemory();
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
	
	set_background_color( 0xFF0f960f );
	// want a gradiant.
}

void ReversiBoardView::start_over( )
{
	bm->clearSlate();
}

void ReversiBoardView::calculatePreliminaries( )
{
	int rwidth  = width -(LEFT_MARGIN+RIGHT_MARGIN);
	int rheight = height-(TOP_MARGIN+BOTTOM_MARGIN);
	incw   	= (rwidth )/BINS;
	inch   	= (rheight)/BINS;
	right  	= left+LEFT_MARGIN  + rwidth;
	//bottom 	= RIGHT_MARGIN + BINS*inch;

	int i=0;
	for (int x=left+LEFT_MARGIN; x<right+1; x+=incw)
		xcols[i++] = x;
	i=0;
	for (int y=bottom+BOTTOM_MARGIN; y<(bottom+height-TOP_MARGIN+1); y+=inch)
		ycols[i++] = y;
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


void Rect_d( Rectangle* mRect )
{
	Rect(mRect->get_left(), mRect->get_bottom(), mRect->get_width(), mRect->get_height() );
}

void ReversiBoardView::drawGrid()
{
	//Fill_l(0xFF000000);
	Stroke_l(0xFF000000);
	right = left+width-RIGHT_MARGIN;
	int top = bottom+height-TOP_MARGIN;
	// DRAW COLUMNS:
	for (int i=0; i<VH_LINES; i++)
		Line( xcols[i], top, xcols[i], bottom+BOTTOM_MARGIN );

	// DRAW ROWS:
	for (int i=0; i<VH_LINES; i++)
		Line( left+LEFT_MARGIN, ycols[i], right, ycols[i]);	

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

int MyTurn = 0;

void ReversiBoardView::doGuess(byte row, byte col, byte mColor)
{
	if (0)
	//if (gs.online_Partner)
	{
		GuessPending = true;
		TouchAllowed = false;
		//OthelloActivity.sc.Send_My_Guess(row, col, OthelloActivity.dlg.prefMyColor );
	} 
	else  
	{
		// SINGLE PLAYER MODE:
		bm->EvaluateGuess( row, col, mColor );
		//if (GameSequencer.MyTurn) {					// White player
		if (MyTurn) {					// White player
			//gs.TakeMyTurn();
		} else {									// Black Player
			//gs.FriendsTurnOver();						
		}
	}
}

void ReversiBoardView::draw_background()
{
	// TEST PATTERN:
	// [ Red, Green, Blue, Alpha]
	VGfloat stops[] = {
	0.0, 0.0, 0.2, 0.0, 1.0,
	0.5, 0.0, 0.5, 0.0,	1.0,
	1.0, 0.0, 0.2, 0.0, 1.0
	};	
	float x1 = left;		float y1 = bottom;
	float x2 = left+width;	float y2 = bottom+height+1;
	FillLinearGradient(x1, y1, x2, y2, stops, 3);
	Rect(x1, y1, x2-x1, y2-y1);
}
int ReversiBoardView::draw() 
{
	Control::draw();
	print_positions();
	
	draw_background();
	calculatePreliminaries( );
	drawGrid     ();
	drawAllPieces();
	if (GuessPending)
		drawGuess( TouchedRow,TouchedColumn );
}

/*
Output :	TouchedRow,TouchedColumn 
Return :	true => valid box touched.
*/
bool ReversiBoardView::computeBinTouched(float x, float y)
{
	touched_x = round( x );
	touched_y = round( y );
	TouchedColumn = BINS+1;
	TouchedRow    = BINS+1;
	for (byte i=0; i<VH_LINES; i++)
	{
		if (touched_x < xcols[i]) 
		{ TouchedColumn = (byte) (i-1); break; }
	}
	for (byte i=0; i<VH_LINES; i++)
	{
		if (touched_y < ycols[i]) 
		{ TouchedRow = (byte) (i-1); 	break; 	}
	}
	if (TouchedRow<0   ) 		TouchedRow 	  = 0;
	if (TouchedColumn<0) 		TouchedColumn = 0;
	return ((TouchedColumn<(BINS+1)) && (TouchedRow<(BINS+1)));
}
#define INITIALIZED  0
#define GAME_IN_PLAY 1
int ReversiGameState = GAME_IN_PLAY;

string ReversiBoardView::form_score_string()
{
	char  str[30];
	sprintf(str, "Black:%d;  White:%d;\n", bm->m_black_count, bm->m_white_count );	
	return str;
}

int ReversiBoardView::onClick(int x, int y, bool mouse_is_down)
{ 
	int retval = Control::onClick(x,y,mouse_is_down);
	static byte mplayer=0;
	if (retval)
	{	
		if (computeBinTouched( x, y )==false) 
			return 1;
		printf("TouchedSquare <r,c> = <%d,%d> ", TouchedRow, TouchedColumn );
		bool valid;
		std::string status;
		
		switch (ReversiGameState)
		{
			case INITIALIZED:
				//OthelloActivity.gs.State = GameSequencer.GAME_IN_PLAY;
				break;
			case GAME_IN_PLAY:
//				if ((OthelloActivity.gs.online_Partner==true) && (GameSequencer.MyTurn==false))
//					return false;
				valid = bm->isLegalMove(TouchedRow, TouchedColumn, mplayer );
				if (valid)
				{	
					printf("valid!");
					doGuess(TouchedRow, TouchedColumn, mplayer );
					mplayer = bm->getOppositeColor(mplayer);				
	
					bm->compute_score();
					if (mplayer==WHITE_PLAYER)
						status = "White Player's Turn          ";
					else 
						status = "Black Player's Turn          ";
					status += form_score_string();
					MainDisplay.set_status( status.c_str() );
					Invalidate();
				} 
//				else OthelloActivity.oa.playSound( OthelloActivity.oa.SOUND_DEACTIVATE );
				break;
			default: break;
		}
		printf("\n");
	}
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


// Zero indexed rows and cols:
/*Rectangle* ReversiBoardView::calc_square_rect(int row, int col )
{
	static Rectangle* tmp   = new Rectangle();

	tmp->set_left ( xcols[col]  );
	tmp->set_right( xcols[col+1] );     // thin ones
	tmp->set_top  ( ycols[row+1] );
	tmp->set_bottom ( ycols[row] );		// 
	return tmp;
}*/
