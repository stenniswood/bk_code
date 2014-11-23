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
#include "wave_view.hpp"
#include "frequency_view.hpp"


#define margin_percent 0.07

FrequencyView::FrequencyView(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}

FrequencyView::FrequencyView( int Width, int Height )
{
	left=0; bottom=0; 
	width  = Width;
	height = Height;
	Initialize();
}

FrequencyView::FrequencyView()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
}

void FrequencyView::Initialize()
{
	background_color = 0xFF000000;

	number_of_bars = 20;
	margin		= 0.05;		// 5%
	max 		= 32767;
	gain		= 8.0;
}

FrequencyView::~FrequencyView()
{
}

void set_number_bars( int mMargin )
{
	if (mMargin == -1)
	{
		float eq_width = width*(1.-margin*2.);
		number_of_bars = eq_width / 5.;		
	} else 
		number_of_bars = mMargin;
}

void FrequencyView::calc_metrics( )
{
	float eq_width = width*(1.-margin*2.);

	bar_x_spacing  = eq_width / number_of_bars;
	bar_width 	   = bar_x_spacing * 0.8;	

	float eq_height= height*(1.-margin*2.);
	box_spacing_y  = eq_height / number_of_levels;
	box_height	   = box_spacing_y * 0.8;	
}

void FrequencyView::set_data( short* mData, int mLength )
{
	data = mData;
	data_length = mLength;
}

long int ColorSequence[] = {
	0xFF00FF00,
	0xFFFFFF00,
	0xFFFFFF2F,
	0xFFFF0000		// Use this for the cap "marker" box color
};
float thresholds[] 	  = {	0.5, 0.75	};
float threshold_index[2];
float box_level;

/***********************************************************************
	This draws 1 square - picking a color based on it's value and the 
	color sequence.
INPUT:
	mBoxNumber	- is the number index.
***********************************************************************/
int FrequencyView::draw_box( float mx, float my, int mBoxNumber )
{
	// Pick out a color:
	color = ColorSequence[0];
	if (mBoxNumber > threshold_index[0])	color = ColorSequence[1];
	if (mBoxNumber > threshold_index[1])	color = ColorSequence[2];

	StrokeWidth( 1.0 );
	Stroke_l( color  );
	Fill_l  ( color  );
	Rect( mx, my, box_width, box_height );
}

void FrequencyView::draw_bar( float mleftPixel, short mValue )
{
	// HOW MANY BOXES?
	int   boxes_visible = round( (float)mValue / box_level );

	// DRAW VISIBLE BOXES:
	float y = bottom + height*margin;
	for (int i=0; i<boxes_visible; i++)
	{
		draw_box( mleftPixel, y, box_width, box_height );
		y += box_spacing_y;
	}
}

int FrequencyView::draw( )
{
	Control::draw();

	// Do some preliminary calculations:
	threshold_index[0] = thresholds[0] * number_of_levels;
	threshold_index[1] = thresholds[1] * number_of_levels;
	box_level 	= max  / number_of_levels;		// range of value covered by 1 box.
	
	float x=left;
	for (int i=0; i<number_of_bars; i++)
	{
		// draw 1 sample per pixel:
		draw_bar( x, data[i] );
		x+=bar_x_spacing;
	}
}


