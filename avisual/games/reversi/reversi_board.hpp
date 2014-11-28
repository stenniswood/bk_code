

int    TOP_MARGIN	=5;
int    LEFT_MARGIN	=5;
int    RIGHT_MARGIN	=5;
int    BOTTOM_MARGIN=5;
const	int BINS 	=8;
const 	int VH_LINES =BINS+1;


class ReversiBoardView : public Control
{
public:
	ReversiBoardView();

	void 	drawPiece(int row, int col, byte mPlayer);
	void 	drawAllPieces	 (	);
	void 	onDraw			 (	);
	void 	drawGrid		 (	);
	void 	drawGuess		 ( int row, int col 	);
	//void 	doGuess			 ( byte row, byte col	);  pull out to another location
	bool 	computeBinTouched( float x, float y		);
	bool 	onTouchEvent	 ( /*MotionEvent event*/);	
	void 	calculatePreliminaries( );
	Rectangle* 	calc_square_rect(int row, int col );

	BoardMemory*	bm;
	bool 	GuessPending;
	
private:
	int    right;
	int    bottom;
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