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
#include "frame_window.hpp"


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
	if (Debug) printf("\t\tFrameWindow::Initialize()\n");
	Window::Initialize();
	body_height = height-(1.5*text_size);
	
	int t = bottom+height;
	int b = packer_vertical_position = bottom+body_height;

	close_button_coords[LEFT]  = left+LEFT_PADDING_CLOSE;			
	close_button_coords[RIGHT] = left+LEFT_PADDING_CLOSE+2.0*MIN_MAX_CLOSE_CIRCLE_RADIUS;			
	close_button_coords[TOP]   = t;			
	close_button_coords[BOTTOM]= b;

	min_button_coords[LEFT]    = close_button_coords[RIGHT]+LEFT_PADDING_CLOSE;
	min_button_coords[RIGHT]   = close_button_coords[RIGHT]+3.0*MIN_MAX_CLOSE_CIRCLE_RADIUS;
	min_button_coords[TOP]     = t;			
	min_button_coords[BOTTOM]  = b;
		
	max_button_coords[LEFT]  = min_button_coords[RIGHT]+LEFT_PADDING_CLOSE;
	max_button_coords[RIGHT] = min_button_coords[RIGHT]+3.0*MIN_MAX_CLOSE_CIRCLE_RADIUS;
	max_button_coords[TOP]   = t;			
	max_button_coords[BOTTOM]= b;
}

void FrameWindow::move_to			( float NewX, float NewY )
{
	float deltaX = NewX-left;
	float deltaY = NewY-bottom;
	Window::move_to( NewX, NewY );
	
	close_button_coords[LEFT]  += deltaX;
	close_button_coords[RIGHT] += deltaX;
	close_button_coords[TOP]   += deltaY;
	close_button_coords[BOTTOM]+= deltaY;

	min_button_coords[LEFT]  += deltaX;
	min_button_coords[RIGHT] += deltaX;
	min_button_coords[TOP]   += deltaY;
	min_button_coords[BOTTOM]+= deltaY;
		
	max_button_coords[LEFT]  += deltaX;
	max_button_coords[RIGHT] += deltaX;
	max_button_coords[TOP]   += deltaY;
	max_button_coords[BOTTOM]+= deltaY;
}

void FrameWindow::set_width_height( int NewWidth, int NewHeight )
{
	if (NewHeight<(1.5*text_size)) return ;		// Not allowed
	
	Window::set_width_height( NewWidth, NewHeight );
	body_height = height-1.5*text_size;
}

int   	FrameWindow::draw_header( )
{
	Stroke_l( 0xAFFFFFFF );
	Fill_l  ( 0xFF9f9fAf );
	Rect    ( left, bottom+body_height, width, height-body_height );
}

/* Draws the close, minimize, and maximize buttons
ala Mac style. 
*/
int   	FrameWindow::draw_close ( bool mHovering )
{
	Stroke_l( 0xFF2F0000 );
	Fill_l  ( 0xFFFF0000 );	
	//if (mHovering)
	{
		// Draw x
		float center_x   = (close_button_coords[RIGHT]-close_button_coords[LEFT])/2.0+close_button_coords[LEFT];
		float center_y   = (close_button_coords[TOP]-close_button_coords[BOTTOM])/2.0+close_button_coords[BOTTOM];		
		if (Debug) printf("Close button:  %6.1f, %6.1f, %6.1f, %6.1f \n", close_button_coords[0],
				close_button_coords[1], close_button_coords[2], close_button_coords[3] );
				
		float angle_rad  = 45.0*M_PI/180.0;
		float x_component= MIN_MAX_CLOSE_CIRCLE_RADIUS * sin(angle_rad);
		Circle( center_x, center_y, MIN_MAX_CLOSE_CIRCLE_RADIUS );	
		// assume y_component is identical.
		float left_x  = center_x-x_component;
		float right_x = center_x+x_component;
		Line( left_x, center_y-x_component, right_x, center_y+x_component );
		Line( left_x, center_y+x_component, right_x, center_y-x_component );
	}
}

int FrameWindow::draw_min (	bool mHovering )
{
	Stroke_l( 0xFFFFFF00 );
	Fill_l  ( 0xFFFFFF00 );
	
	//if (mHovering)
	{
		float center_x = (min_button_coords[1]-min_button_coords[0])/2.0+min_button_coords[0];
		float center_y = (min_button_coords[2]-min_button_coords[3])/2.0+min_button_coords[3];
		if (Debug) printf("Min button:  %6.1f, %6.1f, %6.1f, %6.1f \n", min_button_coords[0],
				min_button_coords[1], min_button_coords[2], min_button_coords[3] );

		Circle( center_x, center_y, MIN_MAX_CLOSE_CIRCLE_RADIUS );
		Stroke_l( 0xFF0F0F00 );
		Line(min_button_coords[0], center_y, min_button_coords[1], center_y );
	}
}

int FrameWindow::draw_max (	bool mHovering )
{
	Stroke_l( 0xFF00FF00 );
	Fill_l  ( 0xFF00FF00 );
	//if (mHovering)
	{
		float center_x = (max_button_coords[1]-max_button_coords[0])/2.0+max_button_coords[0];
		float center_y = (max_button_coords[2]-max_button_coords[3])/2.0+max_button_coords[3];
		Circle( center_x, center_y, MIN_MAX_CLOSE_CIRCLE_RADIUS );
	
		Stroke_l( 0xFF0F0000 );	
		Line( max_button_coords[0], center_y, max_button_coords[1], center_y ); 
		Line( center_x, center_y-MIN_MAX_CLOSE_CIRCLE_RADIUS, center_x, center_y+MIN_MAX_CLOSE_CIRCLE_RADIUS );
	}
}

int FrameWindow::draw( )
{
	Window::draw();
	
	draw_header();
	draw_max   ();
	draw_min   ();
	draw_close ();	
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
	return Window::HitTest(x,y);
}


int	FrameWindow::HitTestArea(int x, int y)
{
	int hdrT = bottom+ height;
	int hdrB = bottom+ body_height;
	
	if ((y>hdrB) && (y<hdrT))
	{
		// check for close:
		if ((x>close_button_coords[LEFT]) && (x<close_button_coords[RIGHT]))
			return CLOSE_HIT;

		// check for min:
		if ((x>min_button_coords[LEFT]) && (x<min_button_coords[RIGHT]))
			return MIN_HIT;		

		// check for max:
		if ((x>max_button_coords[LEFT]) && (x<max_button_coords[RIGHT]))
			return MAX_HIT;
		
		// candidate for drag
		return DRAG_HIT;

	} else {
		// check child controls.
		return CLIENT_AREA_HIT;
		return 0;		// miss!
	}
}

int FrameWindow::onClick(int x, int y, bool mouse_is_down)
{
	int hit_object = HitTestArea( x, y );
	if (hit_object==DRAG_HIT)  // && Mouse is down
	{
		if (mouse_is_down)
			restore_pixels(); 		// we're moving out.  put camp ground back as we found it.
		else
			save_pixels( /*screen_width, screen_height */ );		
	}		
	// Disperse to affected child:
	Control::onClick(x,y, mouse_is_down);	
}

