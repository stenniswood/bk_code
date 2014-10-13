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
#include "listbox.hpp"
#include "display.h"
#include "window.hpp"
#include "frame_window.hpp"


#define Debug 0
 
FrameWindow::FrameWindow ( )
:Window()
{
	Initialize();
	printf("\t\tWindow::ctor()\n");
}

FrameWindow::FrameWindow ( int Left, int Right, int Top, int Bottom )
:Window(Left, Right, Top, Bottom)
{
	Initialize();
	printf("\t\tWindow::ctor()\n");
}
FrameWindow::~FrameWindow( )
{
}
void	FrameWindow::Initialize		 (   )
{
	printf("\t\tWindow::Initialize()\n");
	Window::Initialize();
	packer_vertical_position = bottom+height;
}

int   	FrameWindow::draw_header( )
{

}

/* Draws the close, minimize, and maximize buttons
ala Mac style. 
*/
int   	FrameWindow::draw_close ( bool mHovering )
{
	Stroke_l( 0xFF000000 );
	Fill_l( 0xFFFF0000 );
	
	if (mHovering)
	{
		// Draw x
		float center_x = (close_button_coords[1]-close_button_coords[0])/2.0+close_button_coords[0];
		float center_y = (close_button_coords[2]-close_button_coords[3])/2.0+close_button_coords[3];		
		float angle_rad = 45.0*M_PI/180.0;
		float x_component = MIN_MAX_CLOSE_CIRCLE_RADIUS * sin(angle_rad);
		Circle( center_x, center_y, MIN_MAX_CLOSE_CIRCLE_RADIUS );	
		// assume y_component is identical.
		float left_x = center_x-x_component;
		float right_x = center_x+x_component;
		Line( left_x, center_y-x_component, right_x, center_y+x_component );
		Line( left_x, center_y+x_component, right_x, center_y-x_component );
	}
}

int   	FrameWindow::draw_min (	bool mHovering )
{
	Stroke_l( 0xFF000000 );
	Fill_l( 0xFFFFFF00 );
	
	if (mHovering)
	{
		float center_x = (min_button_coords[1]-min_button_coords[0])/2.0+min_button_coords[0];
		float center_y = (min_button_coords[2]-min_button_coords[3])/2.0+min_button_coords[3];
		Circle( center_x, center_y, MIN_MAX_CLOSE_CIRCLE_RADIUS );
		float mid = (min_button_coords[2] - min_button_coords[3])/2.0;
		Line(min_button_coords[0], mid, min_button_coords[1], mid );
		
	}
}

int   	FrameWindow::draw_max (	bool mHovering )
{
	Stroke_l( 0xFF000000 );
	Fill_l( 0xFFFF0000 );
	if (mHovering)
	{
		float center_x = (max_button_coords[1]-max_button_coords[0])/2.0+max_button_coords[0];
		float center_y = (max_button_coords[2]-max_button_coords[3])/2.0+max_button_coords[3];
		Circle( center_x, center_y, MIN_MAX_CLOSE_CIRCLE_RADIUS );
	
		float mid = (max_button_coords[2] - max_button_coords[3])/2.0;
		Line( max_button_coords[0], mid, max_button_coords[1], mid );

		mid = (max_button_coords[0] - max_button_coords[1])/2.0;
		Line( max_button_coords[2], mid,  max_button_coords[3], mid );	  		 
	}
}

int   	FrameWindow::draw (	 )
{
	//printf("\tFrameWindow::\n");	print_positions();
	Window::draw();
	
	draw_header();

	draw_max   ();
	draw_min   ();
	draw_close ();
	
}

