// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "display.h"
#include "power_level.hpp"

#define margin_percent 0.07

PowerLevel::PowerLevel(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}

PowerLevel::PowerLevel( int Width, int Height )
{
	left=0; bottom=0; 
	width  = Width;
	height = Height;
	Initialize();
}

PowerLevel::PowerLevel()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
}

void PowerLevel::Initialize()
{
	background_color = 0xFF000000;
	number_levels = 10;
	if (height>5.)
		number_levels = round( (float)height / 5. );

	//printf("PowerLevel::Initialize() num_levels=%d\n", number_levels);
	margin		= 0.05;		// 5%
	max 		= 100.;
	min 		=   0.;
}

PowerLevel::~PowerLevel()
{
}

void PowerLevel::calc_metrics( )
{
	float box_height = (height / number_levels);
	float box_width  = width * (1-2*margin);
}

void PowerLevel::set_number_boxes( int mBoxes )	
{
	if (mBoxes == -1)
	{	
		//calc automatically based on height in pixels
		number_levels = round( (float)height / 10. );
		printf("PowerLevel  number_levels=%d height=%6.1f \n", number_levels, height );
	} else 
		number_levels = mBoxes;
}

void PowerLevel::draw_box(float mx, float my, long int mColor )
{
	StrokeWidth(1.0);
	Stroke_l( mColor );
	Rect( mx, my,  box_width, box_height );	
}

int PowerLevel::draw( )
{
	Control::draw();

	// We'll draw from bottom up:
	float box_bottom 	= bottom + (height*margin);
	float box_top    	= bottom + (height*(1.0-margin));
	float box_spacing   = (box_top - box_bottom) / (float)number_levels;
	box_height 			= 0.8*box_spacing;
	box_width  			= (1.0-2.0*margin)*width; 

	//float box_top_final = bottom + height - (height*margin);
	float box_left = left + (width*margin);
	long int color = 0xFF00FF00;		// start at green.
	int box_number = 0;
	int total_boxes= number_levels;
	float one_quarter = number_levels / 4.0;

	for ( float y=box_bottom; y<box_top; y+=box_spacing)
	{
		if (box_number > round(one_quarter*2.0)) 
			color = 0xFFFFFF00;
		if (box_number > round(one_quarter*3.0))
			color = 0xFFFF0000;

		float box_level = max/total_boxes * box_number;
		//printf( "box_level=%6.1f; level=%6.1f\n", box_level, level );
		if (level > box_level)
			Fill_l(color);
		else 
			Fill_l(0x00000000);
		draw_box( box_left, y, color );
		box_number++;
	}
}

// level compared to max value:
void PowerLevel::set_level( float mLevel )
{
	level = mLevel;
	if (level > max)
		level = max;
}

