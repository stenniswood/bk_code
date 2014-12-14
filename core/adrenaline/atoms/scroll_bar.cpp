/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>

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

ScrollBar::ScrollBar( int Width, int Height, bool mIsVertical )
:Control(Width, Height)
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
	m_MaxValue	  = 100;
	m_MinValue	  = 0;	
	m_AmountVisibleValue = 10;
	m_position 	  = 0;
	m_start_pixel   = bottom;
	m_bar_pixel_length = height;
	bar_color		 = 0x7FFF7F7F;
}

void ScrollBar::print_scroll_info( )
{
	printf("ScrollInfo: max=%d; min=%d; lines_visible=%d; m_position=%d\n", 
			m_MaxValue, m_MinValue, m_AmountVisibleValue, m_position);
}


/*void ScrollBar::set_m_position( int Left, int Right, int Top, int Bottom )
{
	Control::set_m_position(Left, Right, Top, Bottom);	
} */
	
// 
void ScrollBar::calc_vertical_bar_pixels( )
{
	// Values <==> Pixels :

	// Values first : 
	float   value_range    = (m_MaxValue-m_MinValue);
	float fraction_visible = (float)m_AmountVisibleValue / value_range;

	// m_position is the start value 

	// Now pixels : 
	m_bar_pixel_length = (int)(fraction_visible * (float)height);

	// Assume Vertical bar first:
	// Range / m_position = height / m_start_pixel;
	// cross multiply
	//float max_start_value = (m_MaxValue - m_AmountVisibleValue);
	//float fraction = (float)m_position / max_start_value ;
	//float max_m_start_pixel = height - m_bar_pixel_length)
	
	int top_pixel = (bottom+height) - ((float)m_position / value_range) * (float)height;
	//printf("top_pix=%d; range=%4.1f; height=%d\n", top_pixel, range, height);

	m_start_pixel = (top_pixel - m_bar_pixel_length);
	//printf("top=%d; bpheight=%d; m_start_pixel=%d\n",     top_pixel, m_bar_pixel_length, m_start_pixel );
	//printf("Bar: pix_height=%d bottom=%d bot_pix=%d\n", m_bar_pixel_length, bottom, m_start_pixel );
}

void ScrollBar::calc_horizontal_bar_pixels( )
{
	// Values <==> Pixels :

	// Values first : 
	float   range  = (m_MaxValue-m_MinValue);
	float fraction = (float)m_AmountVisibleValue / range;	
	// m_position is the start value 

	// Now pixels : 
	m_bar_pixel_length = (int)(fraction * (float)width);

	// Assume top down text first:
	// Range / m_position = height / m_start_pixel;
	// cross multiply
	int left_pixel = (left) + ((float)m_position / range) * (float)width;

	m_start_pixel = left_pixel;
	//printf("Bar: pix_height=%d bottom=%d bot_pix=%d\n", m_bar_pixel_length, bottom, m_start_pixel );
}

int ScrollBar::draw (	)
{	
	StrokeWidth(1);

	// Draw Border:
	Stroke_l( border_color );
	Fill_l  ( background_color );
	Rect    ( left, bottom, width, height);

	// Draw the bar:
	Stroke_l( bar_color );
	Fill_l  ( bar_color );
	if (isVertical)
	{
		calc_vertical_bar_pixels();
		Rect( left+1, m_start_pixel, width-2, m_bar_pixel_length );
	} else { 
		calc_horizontal_bar_pixels(); 
		Rect( m_start_pixel, bottom, m_bar_pixel_length, height ); 
	} 
}

void ScrollBar::set_max_value		( long int  mMax )
{
	m_MaxValue = mMax;		
}
void ScrollBar::set_min_value		( long int  mMin )
{
	m_MinValue = mMin;
}

long int ScrollBar::get_position	(  )
{
	return m_position;
}
void ScrollBar::scroll_by			( long int  mPixelDelta )
{
	
	//mPixelDelta
}
void ScrollBar::scroll_to			( long int  mValue )
{
	m_position = mValue;
}

void ScrollBar::set_amount_visible	( long int  mValue )
{		
	m_AmountVisibleValue = mValue;
}

int	ScrollBar::HitTestArea(int x, int y)
{
	if (m_is_dragging)
		return HIT_BAR;		// even if it moves sideways outside the rectangle!
		
	if ((x>left) && (x<(left+width)))
	{
		if ((y>m_start_pixel) && (y<(m_start_pixel+m_bar_pixel_length)))
		{	
			// clicked on the bar
			return HIT_BAR;
		}
		return HIT_SPACE;
	}
	return 0;	// no hit
}

// linear interpolate between:  bottom,height,y, and min,m_MaxValue
// Assume top down for now.  so min at top and Max at bottom.
long int ScrollBar::interpolate(float bar_top)
{
	// linear interpolate between:  bottom,height,y, and min,m_MaxValue
	// Assume top down for now.  so min at top and Max at bottom.
	int value_range = (m_MaxValue-m_AmountVisibleValue - m_MinValue);	// most value 
	int pixel_range = (height - m_bar_pixel_length);  // most pixels from top to top_of_bar
	
	// now where is the bar top?
	float pixel_height = (height-bar_top);		// pixel distance from top.
	
	// pix height is to pixel range as val is to value_range:
	// (pixel_height / pixel_range) = value / value_range	
	float value = (pixel_height / pixel_range * value_range);
	return value;
}

// need to put mouse_is_down into Control:: and all derived classes!
int	ScrollBar::onClick	 (int x, int y, bool mouse_is_down)
{
	if ((m_is_dragging) )		// release
	{
		// Want to place the same portion of the bar as when we picked it up; in the
		// new location.
		// Subtract off where we :
		float top_pixel = y + m_top_to_y_delta;		// adjust to top of bar
		long int new_value = interpolate(y);
		scroll_to(new_value);
		
		if (mouse_is_down==false)
			m_is_dragging = false;
	}
	int area = HitTestArea(x,y);
	if (area == HIT_BAR)
	{
		if (mouse_is_down)	
		{
			// store the number of pixels to top of the bar.
			m_top_to_y_delta = (m_start_pixel+m_bar_pixel_length)-y;
			m_is_dragging = true;			
			printf("ScrollBar::onClick()  Mouse Hit BAR!\n");
		}
	} else if (area == HIT_SPACE)	
	{
		if (mouse_is_down)	
		{
			printf("ScrollBar::onClick()  Mouse Hit SPACE!\n");		
			// Do a page down or up.
			if (y < m_start_pixel)
			{
				page_down();				
				// Page Down
			} else {
				page_up();
				// Page Up
			}			
		}		
	}	
}


void ScrollBar::page_down(  )	
{
	m_position += m_AmountVisibleValue/2.; 

	float highest_value = m_MaxValue-m_AmountVisibleValue;	
	if (m_position > highest_value)
		m_position = highest_value;
}

void ScrollBar::page_up	 (  )	
{
	m_position -= m_AmountVisibleValue/2.;
	if (m_position < m_MinValue)
		m_position = m_MinValue;
}

