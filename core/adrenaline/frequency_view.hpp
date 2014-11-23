#ifndef _FREQUENCY_VIEW_H_
#define _FREQUENCY_VIEW_H_

#include "../core/can/bk_system_defs.h"

#define DefaultPadding 15
class Window;

/* This class shows an audio waveform
   green, yellow, red color gradients.  with rectangular boxes. */

class FrequencyView : public Control
{
public:
	FrequencyView( int Left, int Right, int Top, int Bottom );
	FrequencyView( int Width, int Height 					 );
	FrequencyView();
	~FrequencyView();

	void 		Initialize();
	void		set_data	 	 ( short* mData, int mLength );
	void		set_max  	 	 ( short mMax 	)	{ if (mMax==-1) max = 32767; else max=mMax; }; 
	//void		show_number_boxes( int mBars    );
	void 		set_number_bars	 ( int mBars  );
	void 		set_number_levels( int mLevels  );		// vertically

	void 		draw_box 		 ( float mx, float my, short mValue );
	void 		draw_bar		 ( float mLeftPixel,   short mValue );
	virtual int draw			 ( );
	void 		calc_metrics	 ( );

protected:
	short*	data;
	long int data_length;

	int		number_of_levels;
	int		number_of_bars;

	float   box_spacing_y;
	float   box_height;

	float   bar_x_spacing;
	float   bar_width;
	float   bar_height_max;

	float	gain;
	float	max;				// usually 100, 0;
	float 	level;	
	float	margin;				// percentage of width
};

 
#endif
