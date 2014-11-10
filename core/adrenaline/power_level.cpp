// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
	number_levels = 10;
	margin = 0.05;		// 5%
	max = 100.;
	min = 0.;
} 

PowerLevel::~PowerLevel()
{
}

void PowerLevel::calc_metrics( )
{
	float box_height = (height / number_levels);
	float box_width  = width * (1-2*margin);
}

void PowerLevel::draw_box(float mx, float my, long int mColor )
{
	StrokeWidth(1.0);
	Stroke_l( mColor );
	//Fill_l  ( mFill  );
	Rect( mx, my,  box_width, box_height );	
}

int PowerLevel::draw( )
{
	Control::draw();
	
	// We'll draw from bottom up:
	float box_bottom 	= bottom + (height*margin);
	float box_top    	= bottom + (height*(1.0-margin));
	float box_spacing   = (box_top - box_bottom) / number_levels;
	box_height = 0.5*box_spacing;
	box_width  = (1.0-2.0*margin)*width; 

	//float box_top_final = bottom + height - (height*margin);	
	float box_left = left + (width*margin);
	long int color = 0xFF00FF00;		// start at green.
	int box_number = 0;		
	int total_boxes = number_levels;
	
	for ( float y=box_bottom; y<box_top; y+=box_spacing)
	{
		float box_level = max/total_boxes * box_number;
		if (level > box_level)
			Fill_l(color);
		else 
			Fill_l(0x00000000);

		draw_box( box_left, y, color );
		box_number++;
		if (box_number > 5)
		{
			color = 0xFFDFDF00;
		}
		if (box_number > 7)
		{
			color = 0xFFFF0000;
		}		
	}	
}

void PowerLevel::set_level( float mLevel )
{
	level = mLevel;
}
