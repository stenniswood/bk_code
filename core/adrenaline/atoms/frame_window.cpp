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
#include "window.hpp"
#include "display_manager.hpp"
#include "frame_window.hpp"
#include "global_funcs.h"

#define Debug 0
 
FrameWindow::FrameWindow ( )
:Window()
{
	Initialize();
}

FrameWindow::FrameWindow ( int Left, int Right, int Top, int Bottom )
:Window(Left, Right, Top, Bottom)
{
	Initialize();
}
FrameWindow::~FrameWindow( )
{
}
void	FrameWindow::Initialize		 (   )
{
	strcpy (class_name, "FrameWindow");
	Window::Initialize();
	title_height = 1.5*text_size;		
	m_size_state = NORMAL;
	
	calc_metrics();
	Dprintf("\t\tFrameWindow::Initialize() done\n");
}
void FrameWindow::calc_metrics(  	)
{
	body_height = height-title_height;
	m_buttons_radius = (title_height * 0.9) / 2.;
	
	int t = bottom+height;
	int b = packer_vertical_position = bottom+body_height;

	close_button_coords[LEFT]  = left+LEFT_PADDING_CLOSE;			
	close_button_coords[RIGHT] = left+LEFT_PADDING_CLOSE+2.0*m_buttons_radius;			
	close_button_coords[TOP]   = t;			
	close_button_coords[BOTTOM]= b;

	min_button_coords[LEFT]    = close_button_coords[RIGHT]+LEFT_PADDING_CLOSE;
	min_button_coords[RIGHT]   = close_button_coords[RIGHT]+3.0*m_buttons_radius;
	min_button_coords[TOP]     = t;			
	min_button_coords[BOTTOM]  = b;
		
	max_button_coords[LEFT]  = min_button_coords[RIGHT]+LEFT_PADDING_CLOSE;
	max_button_coords[RIGHT] = min_button_coords[RIGHT]+3.0*m_buttons_radius;
	max_button_coords[TOP]   = t;			
	max_button_coords[BOTTOM]= b;
	
}	

void FrameWindow::move_to			( float NewX, float NewY )
{
	Window::move_to( NewX, NewY );
	calc_metrics();	
}

void FrameWindow::set_width_height( int NewWidth, int NewHeight )
{
	if (NewHeight<title_height) return ;		// Not allowed
	Window::set_width_height( NewWidth, NewHeight );
	calc_metrics();
}

int   	FrameWindow::draw_header( )
{
	Stroke_l( 0xAFFFFFFF );
	Fill_l  ( 0xFF9f9fAf );
	Rect    ( left, bottom+body_height, width, title_height );
	return 1;
}

/* Draws the close, minimize, and maximize buttons
ala Mac style. 
*/
int   	FrameWindow::draw_close ( bool mHovering )
{
	Stroke_l( 0xFF8F0000 );
	Fill_l  ( 0xFFFF0000 );	
	//if (mHovering)
	{
		// Draw x
		float center_x   = (close_button_coords[RIGHT]-close_button_coords[LEFT])/2.0+close_button_coords[LEFT];
		float center_y   = (title_height)/2.0+close_button_coords[BOTTOM];
		//Dprintf("Close button:  %6.1f, %6.1f, %6.1f, %6.1f \n", close_button_coords[0],
		//		close_button_coords[1], close_button_coords[2], close_button_coords[3] );
		//float angle_rad  = radians(45.0);
		//float x_component= m_buttons_radius * sin(angle_rad);
		Circle( center_x, center_y, m_buttons_radius );	
		// assume y_component is identical.
/*		float left_x  = center_x-x_component;
		float right_x = center_x+x_component;
		Line( left_x, center_y-x_component, right_x, center_y+x_component );
		Line( left_x, center_y+x_component, right_x, center_y-x_component ); */
	}
	return 1;
}

int FrameWindow::draw_min(	bool mHovering )
{
	Stroke_l( 0xFFFFFF00 );
	Fill_l  ( 0xFFFFFF00 );
	
	//if (mHovering)
	{
		float center_x = (min_button_coords[1]-min_button_coords[0])/2.0+min_button_coords[0];
		float center_y = (min_button_coords[2]-min_button_coords[3])/2.0+min_button_coords[3];
		
		//Dprintf("Min button:  %6.1f, %6.1f, %6.1f, %6.1f \n", min_button_coords[0],
		//		min_button_coords[1], min_button_coords[2], min_button_coords[3] );

		Circle( center_x, center_y, m_buttons_radius );
		Stroke_l( 0xFF8F8F00 );
		//Line(min_button_coords[0], center_y, min_button_coords[1], center_y );
	}
	return 1;
}

int FrameWindow::draw_max(	bool mHovering )
{
	Stroke_l( 0xFF00FF00 );
	Fill_l  ( 0xFF00FF00 );
	//if (mHovering)
	{
		float center_x = (max_button_coords[1]-max_button_coords[0])/2.0+max_button_coords[0];
		float center_y = (max_button_coords[2]-max_button_coords[3])/2.0+max_button_coords[3];
		Circle( center_x, center_y, m_buttons_radius );
	
		Stroke_l( 0xFF008F00 );	
		//Line( max_button_coords[0], center_y, max_button_coords[1], center_y ); 
		//Line( center_x, center_y-m_buttons_radius, center_x, center_y+m_buttons_radius );
	}
	return 1;
}

int FrameWindow::draw( )
{
	Window::draw();
	draw_header();
	draw_max   ();
	if (m_size_state != MAXIMIZED)	draw_min   ();
	draw_close ();
	return 1;
}

void FrameWindow::allocate_save_buff	()		// for current dimensions.
{
	// Allocate memory for restoring mouse covered pixels:
	int cbsize = (width*height) + 1;
	restore_image = vgCreateImage(VG_sABGR_8888, cbsize, cbsize, VG_IMAGE_QUALITY_BETTER);
}
// for current dimensions.

// saveCursor saves the pixels under the mouse cursor
void FrameWindow::save_pixels( /*int screen_width, int screen_height*/) 
{
	int sx, sy, ex, ey;

	sx = left;					   // horizontal 
	if (sx < 0) {
		sx = 0;
	}
	ex = left+width;
	//if (ex > screen_width) {		ex = screen_width;	}
	save_sx = sx;
	save_w  = ex - sx;

	sy = bottom;					   // vertical 
	if (sy < 0) {
		sy = 0;
	}
	ey = bottom+height;
	//if (ey > screen_height) {		ey = screen_height;	}
	save_sy = sy;
	save_h  = ey - sy;
	vgGetPixels( restore_image, 0, 0, save_sx, save_sy, save_w, save_h );
	saved_bytes = save_w * save_h;
}

// restoreCursor restores the pixels under the mouse cursor
void FrameWindow::restore_pixels() 
{
	if (saved_bytes != 0) {
		vgSetPixels(save_sx, save_sy, restore_image, 0, 0, save_w, save_h);
	}
}

Control* FrameWindow::HitTest(int x, int y)
{
	//int title_bar_hit = HitTestTitleBar(x,y);  what sense in doing here?
	return Window::HitTest(x,y);
}


int	FrameWindow::HitTestTitleBar(int x, int y)
{
	int hdrT = bottom+ height;
	int hdrB = bottom+ body_height;
	
	if ((y>hdrB) && (y<hdrT))	// In the title bar region?
	{	
		printf("HitTestTitleBar() \n");		
		// yes, check for close:
		if ((x>close_button_coords[LEFT]) && (x<close_button_coords[RIGHT]))
			return CLOSE_HIT;

		// check for min:
		if (m_size_state != MAXIMIZED)	
			if ((x>min_button_coords[LEFT]) && (x<min_button_coords[RIGHT]))
				return MIN_HIT;		

		// check for max:
		if ((x>max_button_coords[LEFT]) && (x<max_button_coords[RIGHT]))
			return MAX_HIT;
		
		// General Titlebar area, candidate for drag
		return DRAG_HIT;

	} else {
		// check child controls.
		// return CLIENT_AREA_HIT;
		return 0;		// miss!
	}
	return 0;
}  
void FrameWindow::save_position(   )
{
	m_restore_rect.set_left  ( left );
	m_restore_rect.set_bottom( bottom );
	m_restore_rect.set_width ( width );	
	m_restore_rect.set_height(height );
}
void FrameWindow::restore_position(   )
{
	set_width_height( m_restore_rect.get_width(),m_restore_rect.get_height() );
	move_to			( m_restore_rect.get_left(), m_restore_rect.get_bottom() );
}

int FrameWindow::onClick(int x, int y, bool mouse_is_down)
{
	Rectangle* rect=NULL;
	int title_bar_hit = HitTestTitleBar( x, y );
	printf("HitTestTitleBar() = %d\n", title_bar_hit );	
	switch(title_bar_hit)
	{
	case CLOSE_HIT: 
			
			break;
	case MIN_HIT:	
			if (m_size_state == MINIMIZED) {
				restore_position();
				m_size_state = NORMAL;				
			} else  {
				save_position();
				set_width_height( max_button_coords[RIGHT], title_height );	
				m_size_state = MINIMIZED;
			}
			Invalidate();			
			break;
	case MAX_HIT: 
			if (m_size_state == MAXIMIZED)
			{
				restore_position();			
				m_size_state = NORMAL;			
			} else {			
				if (m_size_state != MINIMIZED)
					save_position();
				rect = MainDisplay.get_useable_rect();
				set_width_height( rect->get_width(), rect->get_height() );	
				move_to(rect->get_left(), rect->get_bottom() );
				m_size_state = MAXIMIZED;
			}
			Invalidate();		
			break;
	case DRAG_HIT : 
			m_size_state = DRAGGING;	
			if (mouse_is_down)
				restore_pixels(); 		// we're moving out.  put camp ground back as we found it.
			else
				save_pixels( /*screen_width, screen_height */ );		
			Invalidate();	
			break;
	default : break;
	}
	
	// Disperse to affected child:
	return Control::onClick(x,y, mouse_is_down);	
}

