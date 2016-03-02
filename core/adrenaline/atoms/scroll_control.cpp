// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <list>
#include <math.h> 
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "scroll_control.hpp"


#define Debug 0


ScrollControl::ScrollControl()
: Control()
{
	Initialize();
}

ScrollControl::ScrollControl( int Left, int Right, int Top, int Bottom )
: Control( Left, Right, Top, Bottom )
{
	Initialize();
}

ScrollControl::ScrollControl( int Width, int Height )
: Control( Width, Height )
{
	Initialize();
}

ScrollControl::~ScrollControl()
{
	if (vsb)	delete vsb;
	if (hsb)	delete hsb;	
}

void ScrollControl::Initialize()
{
	//Control::Initialize();
	strcpy (class_name, "ScrollControl");					
	vsb = NULL;
	hsb = NULL;
	h_first_visible_value = 0;
	h_visible_value 	  = 0;
	v_first_visible_value = 0;
	v_visible_value 	  = 0;
}
void ScrollControl::calc_metrics	 (   )
{

}

void ScrollControl::set_width_height( int Width, int Height )
{
	Control::set_width_height( Width, Height );
	if (vsb) {
		vsb->move_to( (left+width-vsb->width), bottom );	// move sideways.
		vsb->set_width_height( vsb->width, Height);
	}
	if (hsb)
		hsb->set_width_height( Width, hsb->height);
}

void ScrollControl::move_to( float mLeft, float  mBottom )
{
	Control::move_to(mLeft, mBottom);
	//printf("ScrollControl::move_to(): moving to %d,%d\n", mLeft, mBottom);
	if (vsb)
		vsb->move_to( (mLeft+width)-(vsb->width), bottom);
	if (hsb)
		hsb->move_to( mLeft, mBottom);
}

void ScrollControl::set_v_scroll_values( int max, int min, int first_visible, 
										 int amount_visible )
{
	int amount = (max-min);
	if ((vsb==NULL) && (amount_visible<amount))
		enable_v_scroll_bar(true);
	if (vsb==NULL) return;
	
	vsb->set_max_value	( max );
	vsb->set_min_value	( min );
	vsb->scroll_to  	( first_visible	 );
	vsb->set_amount_visible( amount_visible );
}
	
void ScrollControl::enable_v_scroll_bar( bool mEnable )
{
	if (mEnable)
	{
		vsb = new ScrollBar(true);
		vsb->bottom = bottom;
		vsb->height = height;
		vsb->left  = (left+width)-DEFAULT_VSCROLL_WIDTH;
		vsb->width = DEFAULT_VSCROLL_WIDTH;
		if (Debug) printf("ScrollControl::enabled VScroll - \t");		
		if (Debug) vsb->print_positions();
		
		vsb->set_bar_color		 ( 0x7FFF7F7F );
		vsb->set_background_color( 0x9F000000 );
		register_child( vsb );
 	} else {
		if (vsb) {
			unregister_child( vsb );
			delete vsb;
			vsb = NULL;
		}
		
 	}
}
void ScrollControl::enable_h_scroll_bar( bool mEnable,int max, int min, int first_visible, 
										 int amount_visible )
{
	if (mEnable)
	{
		hsb = new ScrollBar(true);
		hsb->bottom = bottom;		
		hsb->height = DEFAULT_HSCROLL_HEIGHT;
		hsb->left   = left;
		hsb->width  = width;

		hsb->set_max_value		( max );
		hsb->set_min_value		( min );
		hsb->set_amount_visible ( amount_visible);
		hsb->scroll_to			( first_visible );

		hsb->set_bar_color		( 0x7FFF7F7F );
		hsb->set_background_color( 0x9F000000 );
		register_child( hsb );
 	} else {
		if (hsb) {
			unregister_child( hsb );
			delete hsb;
			hsb = NULL;
		}
 	}
}

void ScrollControl::set_first_visible_v( int mValue )
{
	v_first_visible_value = mValue;
	if (vsb) vsb->scroll_to( v_first_visible_value ); 
}

void ScrollControl::set_first_visible_h( int mValue )
{
	h_first_visible_value = mValue;
	if (hsb) vsb->scroll_to( h_first_visible_value );
}

void ScrollControl::set_amount_visible_v( int mValue ) 
{
	v_visible_value = mValue;
	if (vsb) vsb->set_amount_visible( v_visible_value ); 
}

void ScrollControl::set_amount_visible_h( int mValue ) 
{
	h_visible_value = mValue; 
	if (hsb) hsb->set_amount_visible( h_visible_value ); 
}

int ScrollControl::draw()
{
	Control::draw();
	if (vsb) {
		vsb->draw();
		//vsb->print_positions();
		//vsb->print_scroll_info();	
	}
	if (hsb)
		hsb->draw();	
}
Control* ScrollControl::HitTest(int x, int y)
{
	Control::HitTest(x,y);
}

int	ScrollControl::onClick(int x, int y, bool mouse_is_down)
{	
	return Control::onClick(x,y,mouse_is_down);
}

