/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
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
#include "button.hpp"
#include "display.h"
#include "text_view.hpp"
#include "scroll_bar.hpp"


ScrollBar::ScrollBar(int Left, int Right, int Top, int Bottom, bool mIsVertical )
:Control(Left, Right, Top, Bottom)
{
	isVertical = mIsVertical;
	Initialize();
}

ScrollBar::ScrollBar(bool mIsVertical)
{
	isVertical = mIsVertical;
	Initialize();
}
ScrollBar::~ScrollBar()
{

}

void ScrollBar::Initialize()
{
	MaxValue	  = 100;
	MinValue	  = 0;	
	AmountVisibleValue = 10;
	position 	  = 0;
	start_pixel   = bottom;
	bar_pixel_length = height;
	bar_color		 = 0x7FFF7F7F;
}

void ScrollBar::print_scroll_info( )
{
	printf("ScrollInfo: max=%d; min=%d; lines_visible=%d; position=%d\n", 
			MaxValue, MinValue, AmountVisibleValue, position);
}

/*void ScrollBar::set_position( int Left, int Right, int Top, int Bottom )
{
	Control::set_position(Left, Right, Top, Bottom);	
} */
	
void ScrollBar::calc_vertical_bar_pixels( )
{
	// Values <==> Pixels :

	// Values first : 
	float   range    = (MaxValue-MinValue);
	float fraction = (float)AmountVisibleValue / range;

	// position is the start value 

	// Now pixels : 
	bar_pixel_length = (int)(fraction * (float)height);

	// Assume top down text first:
	// Range / position = height / start_pixel;
	// cross multiply
	int top_pixel = (bottom+height) - ((float)position / range) * (float)height;
	//printf("top_pix=%d; range=%4.1f; height=%d\n", top_pixel, range, height);

	start_pixel = (top_pixel - bar_pixel_length);
	//printf("top=%d; bpheight=%d; start_pixel=%d\n",     top_pixel, bar_pixel_length, start_pixel );
	//printf("Bar: pix_height=%d bottom=%d bot_pix=%d\n", bar_pixel_length, bottom, start_pixel );
}

void ScrollBar::calc_horizontal_bar_pixels( )
{
	// Values <==> Pixels :

	// Values first : 
	float   range  = (MaxValue-MinValue);
	float fraction = (float)AmountVisibleValue / range;	
	// position is the start value 

	// Now pixels : 
	bar_pixel_length = (int)(fraction * (float)width);

	// Assume top down text first:
	// Range / position = height / start_pixel;
	// cross multiply
	int left_pixel = (left) + ((float)position / range) * (float)width;

	start_pixel = left_pixel;
	//printf("Bar: pix_height=%d bottom=%d bot_pix=%d\n", bar_pixel_length, bottom, start_pixel );
}

int ScrollBar::draw (				)
{	
	StrokeWidth(1);
	// Draw Border:
	Stroke_l( border_color );
	Fill_l  ( background_color );
	Rect    ( left, bottom, width, height);

	// Draw the bar:
	Stroke_l( border_color  );
	Fill_l  ( bar_color  );
	if (isVertical)
	{
		calc_vertical_bar_pixels();
		Rect( left+1, start_pixel, width-2, bar_pixel_length );
	} else { 
		calc_horizontal_bar_pixels(); 
		Rect( start_pixel, bottom, bar_pixel_length, height ); 
	} 
}

void ScrollBar::set_max_value		( long int  mMax )
{
	MaxValue = mMax;
}
void ScrollBar::set_min_value		( long int  mMin )
{
	MinValue = mMin;
}

long int ScrollBar::get_position	(  )
{
	return position;
}

void ScrollBar::scroll_to			( long int  mValue )
{
	position = mValue;
}

void ScrollBar::set_amount_visible	( long int  mValue )
{
	AmountVisibleValue = mValue;
}



