// Draws a simple line thru center of a circle Top
// show the angle of orientation for a accel/gyro board.

// Steve Tenniswood
// Feb 2, 2015

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <math.h>
#include <vector>
//#include "global.h"
#include "bk_system_defs.h"
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"

#include "control.hpp"
#include "display.h"
#include "compass.hpp"
#include "vector_math.h"
#include "fuse_accel.h"
 

#define Debug 1

char needle_filename[] = "../resources/compass_needle.jpg";
char dial_filename[]   = "../resources/compass_dial.jpg";

VGImage needle_image;
VGImage dial_image;

struct image_info 	needle_ImageInfo;
struct image_info 	dial_ImageInfo;


CompassView::CompassView( )
{  
	Initialize();
}
CompassView::CompassView( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	Initialize();
}
CompassView::CompassView( int mCenterX, int mCenterY, float mRadius )
{
	Initialize();
	set_position( mCenterX-mRadius, mCenterX+mRadius,  mCenterY+mRadius, mCenterY-mRadius );
	
	m_center_x = mCenterX;
	m_center_y = mCenterY;
	m_radius   = mRadius;
}
CompassView::~CompassView( )
{  
}

void 	CompassView::Initialize			( )
{  
	strcpy (class_name, "CompassView");
	m_left_margin = left;
	m_right_margin = left+width;
	background_color = 0xFF5f5f00;
	
	calc_metrics();
	set_heading_angle_deg(-30.0);				// North is 0 degrees.	
}

void 	CompassView::calc_metrics( )
{  
	float space = width * 0.05;
	m_left_margin  = left + space;
	m_right_margin = left + width - space;	
	
	m_center_x = left   + width/2;
	m_center_y = bottom + height/2;
	m_radius   = min(width,height)/4.;	// diameter is half the width of the window.
	// radius should be 1/4.  And the Circle() function takes a diameter not radius.	
}

/************ ROLL & PITCH ANGLE FUNCTIONS ************/
void CompassView::set_heading_angle_deg	( float mDegrees )
{
	// Plus 90 because we want North (0.0 deg) to be straight up.
	float angle_deg = mDegrees;
	m_heading = radians(angle_deg);			
}
float CompassView::get_heading_angle_deg	( )
{
	// Minus 90 because we want North (0.0 deg) to be straight up.
	return degrees(m_heading)-90.;
}
/************ ROLL & PITCH ANGLE FUNCTIONS ************/

void	CompassView::print_info( )
{
	printf("Gyro heading = %6.2f\n", get_heading_angle_deg() );			
}

void 	CompassView::set_width_height  	 ( int Width, int Height 		)
{  
	Control::set_width_height( width, height );
	calc_metrics();	
}

int		CompassView::onCreate	  (  )
{
	Control::onCreate();
	load_resources();
}

void CompassView::load_resources()
{
	needle_image = createImageFromJpeg( needle_filename, &needle_ImageInfo );
	dial_image   = createImageFromJpeg( dial_filename,   &dial_ImageInfo   );
	width = dial_ImageInfo.width;
	height = dial_ImageInfo.height;
}

void 	CompassView::draw_pointer(  )
{  
	float n_left = left+width;
	float n_bottom = bottom;	
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	vgScale(0.5, 0.5);	
	vgSetPixels( n_left, n_bottom, needle_image, 0, 0, dial_ImageInfo.width, needle_ImageInfo.height );
	//needle_ImageInfo.width, needle_ImageInfo.height );	
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);	
}

void 	CompassView::draw_dial (  )
{	
	float center_x = left   + width /2;
	float center_y = bottom + height/2;
	
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

	Translate( center_x, center_y );
	Rotate( degrees(m_heading) );
	Translate( -width/2, -height/2 );
	vgDrawImage(dial_image);

	Translate( +width/2, +height/2 );
	Rotate( -degrees(m_heading) );
	Translate( -center_x, -center_y );
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
}

void 	CompassView::draw_info(  )
{
	// DRAW TEXT:
	char title[50];
	float yaw   = degrees( m_heading );
	
	sprintf(title, "Heading:" );
	Stroke_l ( 0xFF000000 );
	Fill_l   ( 0xFFFFFFFF );	
	TextMid( m_center_x, bottom+0.12*height, title, SerifTypeface, text_size );	
	
	sprintf(title, "%5.1f", yaw );
	TextMid( m_center_x, bottom+0.12*height-text_size*1.2, title, SerifTypeface, text_size );	
}


int CompassView::handle_incoming_msg( struct sCAN* msg )
{	
}

int   	CompassView::draw   	(  )
{  
	Control::draw();
	
	// DRAW TEXT:
	char title[50];
	sprintf(title, "Compass" ); 	
	Stroke_l ( 0xFFFF0000 );
	Fill_l   ( 0xFFFFFFFF );	
	TextMid( m_center_x, bottom+0.9*height, title, SerifTypeface, text_size );

	draw_dial   ();
	//draw_pointer();

	draw_info();
//	draw_heading();
	return 1;
}	


