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
}

PowerLevel::~PowerLevel()
{
}
PowerLevel::calc_metrics( )
{
	float box_height = (body_height / number_levels);
	float box_width  = width * (1-2*margin);

}

PowerLevel::draw_box(float mx, float my, long int mColor )
{
	Stroke_l( mColor );
	Fill_l  ( mColor );
	Rect( mx, my, box_height, box_width );	
}

PowerLevel::draw( )
{
	// We'll draw from bottom up:
	float box_bottom = bottom + (height*margin);
	float box_top    = box_bottom + box_height;
	float box_top_final = bottom + height - (height*margin);
	float box_left = left + (height*margin);
	long int color = 0xFF00FF00;		// start at green.
	int box_number = 0;					// 
	for ( float y=box_bottom; y<box_top_final; y++)
	{
		draw_box( box_left, box_bottom, color );		
		box_number++;
		if (box_number<)
		{
			
		}
	}	
}

