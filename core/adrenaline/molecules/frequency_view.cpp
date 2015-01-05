// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "display.h"
#include "wave.hpp"
#include "wave_view.hpp"
#include "frequency_view.hpp"


#define margin_percent 0.07
static long int ColorSequence[] = {
	0xFF00FF00,
	0xFFFFFF00,
	0xFFFF0000,
	0xFFFF0000		// Use this for the cap "marker" box color
};
float thresholds[] 	  = {	0.4, 0.7	};
short threshold_index[2];
float box_level;


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

	number_of_levels = 20;
	number_of_bars = 50;
	margin		= 0.02;		// 5%
	max 		= 32767;
	gain		= 8.0;
}

FrequencyView::~FrequencyView()
{
}

void FrequencyView::set_number_bars( int mBars )
{
	if (mBars == -1)
	{
		float eq_width = width*(1.-margin*2.);
		number_of_bars = eq_width / 5.;
	} else 
		number_of_bars = mBars;
}

void FrequencyView::calc_metrics( )
{
	float eq_width = width*(1.-margin*2.);

	bar_x_spacing  = eq_width / (number_of_bars);
	bar_width 	   = bar_x_spacing * 0.6;

	float eq_height= height*(1.-margin*2.);
	box_spacing_y  = eq_height / number_of_levels;
	box_height	   = box_spacing_y * 0.8;	
	
	// Do some preliminary calculations:
	threshold_index[0] = round(thresholds[0] * (float)number_of_levels);
	threshold_index[1] = round(thresholds[1] * (float)number_of_levels);
	box_level 	= max  / (float)number_of_levels;		// range of value covered by 1 box.
	//printf("box_level=%6.1f; thresh1=%6.1f; thresh2=%6.1f\n", box_level, threshold_index[0], threshold_index[1] );
}

void FrequencyView::set_number_levels( int mLevels  )
{
	number_of_levels = mLevels;
}
void FrequencyView::set_data( short* mData, int mLength )
{
	data = mData;
	data_length = mLength;
	number_of_bars = mLength;
}



/***********************************************************************
	This draws 1 square - picking a color based on it's value and the 
	color sequence.
INPUT:
	mBoxNumber	- is the number index.
***********************************************************************/
void FrequencyView::draw_box( float mx, float my, short mBoxNumber )
{
	// Pick out a color:
	long color = ColorSequence[0];
	if (mBoxNumber > threshold_index[0])	color = ColorSequence[1];
	if (mBoxNumber > threshold_index[1])	color = ColorSequence[2];

	// fade blue out from left to right:
	//color |= blue;

	StrokeWidth( 1.0 );
	Stroke_l( color  );
	Fill_l  ( color  );
	Rect    ( mx, my, bar_width, box_height );
}


void FrequencyView::draw_bar( float mleftPixel, short mValue )
{
	// HOW MANY BOXES?
	int   boxes_visible = round( (float)mValue / box_level );

	//printf("FreqView::LeftPix=%6.1f;  mValue=%d; boxes_visible=%d\n", mleftPixel, mValue, boxes_visible );
	
	// DRAW VISIBLE BOXES:
	float y = bottom + height*margin;
	for (int i=0; i<boxes_visible; i++)
	{
		draw_box( mleftPixel, y, i );
		y += box_spacing_y;
	}
}

int FrequencyView::draw( )
{
	Control::draw();
	calc_metrics();

	float x= left + margin*width;
	for (int bar=0; bar<number_of_bars; bar++)
	{
		//blue = round(255. * (float)(number_of_bars-bar) / (float)number_of_bars);
		//printf("blue=%x\n", blue);
		draw_bar( x, data[bar] );
		x+=bar_x_spacing;
	}
}

