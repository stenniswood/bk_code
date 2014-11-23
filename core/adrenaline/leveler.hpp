#ifndef  _LEVELER_H_
#define  _LEVELER_H_

#include "control.hpp"

class Leveler : public Control
{
public:
	Leveler	(float Max, float Min);
	// set level percent actually moves the marker.
	// want to expand this to set max, min, and level.
	// so we can show any unit, not just percents!
	
	int   set_level			(float level		 );
	void  set_level_cap		(float level		 );
	void  set_level_percent	(float percent		 );
	float get_level_pixel	(	);

	void  setPosition(int Left, int Right, int Top, int Bottom);
	void  setPosition_left_of (Leveler& mSibling, int width, int Top=-1, int Bottom=-1);
	void  setPosition_right_of(Leveler& mSibling, int width, int Top=-1, int Bottom=-1);

	//float calc_scale		(float* data, int n );

	void draw_horizontal_tics(  );
	void draw_vertical_line	(	);
	int  draw_y_label		(	);
	int  draw_x_label		(	);
	int  draw_l_title		(	);
	void draw_marker		(	);

	int draw				(	);

private:
	int 	left;
	int 	right;
	int 	top;
	int 	bottom;	
	float 	yMax, yMin;
	float	Level;		// Must be somewhere between yMin & yMax
	
	char* 	xAxisLabel;
	char* 	yAxisLabel;
	char* 	title;
};


#endif
