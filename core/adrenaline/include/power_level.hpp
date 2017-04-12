#ifndef _POWER_LEVEL_H_
#define _POWER_LEVEL_H_

#include "bk_system_defs.h"

#define DefaultPadding 15
class Window;

/* This class shows an audio power indicator (1 channel).
   green, yellow, red color gradients.  with rectangular boxes. */

class PowerLevel : public Control
{
public:
	PowerLevel( int Left, int Right, int Top, int Bottom );
	PowerLevel( int Width, int Height 					 );
	PowerLevel();
	~PowerLevel();

	void 		Initialize();
	void		set_level	 ( float mLevel );
	void		set_max  	 ( float mMax 	)	{ max = mMax; }; 
	void		set_min  	 ( float mMin 	)	{ min = mMin; };
	void		set_number_boxes( int mBoxes );

	void 		draw_box 	( float mx, float my, long int mColor );
	virtual int draw		( );
	void 		calc_metrics( );
	
protected:
	int		number_levels;		// 10 default
	float	max,min;			// usually 100, 0;
	float 	level;
	
	float 	box_height;			// based on control window size
	float	box_width;			// calculates based on margins	
	float	margin;				// percentage of width
};

 
#endif
