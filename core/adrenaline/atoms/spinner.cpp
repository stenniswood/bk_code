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
#include "listbox.hpp"
#include "display.h"
#include "spinner.hpp"

#define Debug 0

void on_up_click( void* mthis )
{
	((SpinnerControl*)mthis)->increment();	
	((SpinnerControl*)mthis)->Invalidate();
}

void on_down_click( void* mthis )
{
	((SpinnerControl*)mthis)->decrement();
	((SpinnerControl*)mthis)->Invalidate();
}

SpinnerControl::SpinnerControl()
:Control()
{ 
	Initialize(	);
}

SpinnerControl::SpinnerControl( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{ 
	Initialize(	);
}
SpinnerControl::SpinnerControl( int Width, int Height 					 )
:Control(Width, Height)
{ 
	Initialize(	);
}

SpinnerControl::~SpinnerControl()
{ 
}

void 	SpinnerControl::Initialize(	)
{ 
	strcpy (class_name, "SpinnerControl");					
	margin 		= 8;
	m_alignment = ALIGN_RIGHT;
	m_value     = 0;

	m_max_value = 1000;
	m_min_value = -10;
	if (Debug) printf("SpinnerControl::Initialize(	) completed\n");
}

int		SpinnerControl::calc_metrics()
{ 
	return 1;
}

int		SpinnerControl::onCreate()
{
	m_up.set_text( "+" );
	m_up.set_text( "-" );
	m_up.set_width_height  ( BUTTON_WIDTH, height/2. );
	m_down.set_width_height( BUTTON_WIDTH, height/2. );
	m_up.set_on_click_listener  ( on_up_click, this   );
	m_down.set_on_click_listener( on_down_click, this );
	return Control::onCreate();
}
void 	SpinnerControl::set_width_height( int Width, int Height 		 )  
{ 
	Control::set_width_height( Width, Height );
	m_up.set_width_height  ( BUTTON_WIDTH, height/2. );
	m_down.set_width_height( BUTTON_WIDTH, height/2. );
	m_up.move_to  ( left+width-BUTTON_WIDTH,  bottom+(height/2) );
	m_down.move_to( left+width-BUTTON_WIDTH,  bottom ); 
}
void  	SpinnerControl::move_to	  		( float Left, float  Bottom )
{
	Control::move_to   ( Left, Bottom );
	m_up.move_to  ( left+width-BUTTON_WIDTH,  bottom+(height/2) );
	m_down.move_to( left+width-BUTTON_WIDTH,  bottom );	 
}

int   	SpinnerControl::draw 				(	)
{ 
	float x,y;
	Control::draw();
	char str[30];
	sprintf( str, "%d", m_value );
	Stroke_l(text_color);
	Fill_l  (text_color);
	y = bottom + (height-text_size)/2.;
	switch(m_alignment)
	{
	case ALIGN_LEFT:	x = (left + margin);
				if (Debug) printf("%5.1f %5.1f text_color=%4lx %4lx : %s\n", x, y, text_color, background_color, str );
						Text( x, y,    str, SerifTypeface, text_size );	
						break;
	case ALIGN_CENTER:	
						x = (left + width/2.);
				if (Debug) printf("%5.1f %5.1f text_color=%4lx %4lx : %s\n", x, y, text_color, background_color, str );
						TextMid( x, y, str, SerifTypeface, text_size );
						break;
	case ALIGN_RIGHT:	x = (left + width - m_up.get_width() - margin);
				if (Debug) printf("%5.1f %5.1f text_color=%4lx %4lx : %s\n", x, y, text_color, background_color, str );
						TextEnd( x, y, str, SerifTypeface, text_size );
						break;
	default: 	if (Debug) printf(" default case \n" );
				x = (left + margin);
				if (Debug) printf("%5.1f %5.1f text_color=%4lx %4lx : %s\n", x, y, text_color, background_color, str );
				Text( x, y,    str, SerifTypeface, text_size );	
				break;
	}
	m_up.draw  ();
	m_down.draw();
	return 1;
}

int		SpinnerControl::get_hit_index		( int Mousex, int Mousey )
{ 
	return 0;
}

int		SpinnerControl::onClick				( int x, int y, bool mouse_is_down )
{ 
	Control* up_result   = m_up.HitTest  ( x, y );
	if (up_result)
		up_result->onClick( x,y,mouse_is_down );

	Control* down_result = m_down.HitTest( x, y );
	if (down_result)
		down_result->onClick( x,y,mouse_is_down );
	return 0;
}
