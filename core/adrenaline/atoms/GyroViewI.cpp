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
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "control.hpp"
#include "display.h"
#include "GyroViewI.hpp"


GyroView::GyroView( )
{  
	Initialize();
}
GyroView::GyroView( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	Initialize();
}
GyroView::GyroView( int mCenterX, int mCenterY, float mRadius )
{
	Initialize();
	set_position( mCenterX-mRadius, mCenterX+mRadius,  mCenterY+mRadius, mCenterY-mRadius );
	
	m_center_x = mCenterX;
	m_center_y = mCenterY;
	m_radius   = mRadius;
}

GyroView::~GyroView( )
{  
}

void 	GyroView::Initialize			( )
{  
	strcpy (class_name, "GyroView");
	m_roll_angle_radians = 0.0;
	m_left_margin = left;
	m_right_margin = left+width;
	background_color = 0xFF5f5f00;
	
	calc_metrics();
	set_roll_angle_deg   (0.0);
	set_pitch_angle_deg  (0.0);
	set_heading_angle_deg(0.0);				// North is 0 degrees.	
}
void 	GyroView::calc_metrics			( )
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
void	GyroView::set_roll_angle_deg			( float mDegrees )
{  
	m_roll_angle_radians = radians(mDegrees);
	m_delta_y = m_radius * sin(m_roll_angle_radians);
	m_delta_x = m_radius * cos(m_roll_angle_radians);
}
float	GyroView::get_roll_angle_deg			( )
{  
	return degrees(m_roll_angle_radians);
}

void	GyroView::set_pitch_angle_deg			( float mDegrees )
{
	m_pitch_angle_radians = radians(mDegrees);
	
//	m_delta_y = m_radius * sin(m_pitch_angle_radians);
//	m_delta_x = m_radius * cos(m_pitch_angle_radians);
}
float	GyroView::get_pitch_angle_deg			( )
{  
	return degrees(m_pitch_angle_radians);
}
void GyroView::set_heading_angle_deg	( float mDegrees )
{
	// Plus 90 because we want North (0.0 deg) to be straight up.
	float angle_deg = mDegrees + 90.;
	m_heading = radians(angle_deg);	
	
	//printf("pi radians=%6.1f degs\n", degrees(3.1415));
	//printf("180 degs=%6.1f rads\n", radians(180.));	
	m_delta_x_heading = m_radius/2. * cos(m_heading);
	m_delta_y_heading = m_radius/2. * sin(m_heading);
}
float GyroView::get_heading_angle_deg	( )
{
	// Minus 90 because we want North (0.0 deg) to be straight up.
	return degrees(m_heading)-90.;
}
/************ ROLL & PITCH ANGLE FUNCTIONS ************/

void	GyroView::print_info( )
{
	printf("Gyro roll  angle = %6.2f\n", get_roll_angle_deg() );	
	printf("Gyro pitch angle = %6.2f\n", get_pitch_angle_deg() );		
	printf("Gyro heading = %6.2f\n", get_heading_angle_deg() );			
}

//void 			set_font				( Fontinfo* f	 ){  }
void 	GyroView::set_width_height  	 ( int Width, int Height 		)
{  
	Control::set_width_height( width, height );
	calc_metrics();	
}

void 	GyroView::draw_line	(  )
{  
	Stroke_l ( 0xFFFF7F00 );
	Fill_l   ( 0xFFFF7F00 );

	float y_offset = m_radius * sin(m_pitch_angle_radians);

	float x1 = m_center_x + m_delta_x;
	float x2 = m_center_x - m_delta_x;

	float y1 = y_offset+ m_center_y + m_delta_y;
	float y2 = y_offset+ m_center_y - m_delta_y;
	
	Line(x1,y1, x2,y2);	
}

void 	GyroView::draw_circle (  )
{	
	Stroke_l ( 0xFFFFFFFF );
	Fill_l   ( 0xFF3f0000 );
	Circle(m_center_x, m_center_y, m_radius*2);	  	// takes a diameter!
	
	// DRAW TEXT:
	char title[50];
	sprintf(title, "Angle: %5.1f", 180*m_roll_angle_radians/3.1415 ); 	
//	Stroke_l ( 0xFFFFFFFF );
	Fill_l   ( 0xFFFFFFFF );	
	TextMid( m_center_x, bottom+0.1*height, title, SerifTypeface, text_size );	
}

void 	GyroView::draw_heading(  )
{	
	float heading_radius = m_radius/2;
	float h_center_x = m_center_x+m_radius+0.8*heading_radius;
	float h_center_y = m_center_y+m_radius+0.8*heading_radius;
	
	Stroke_l ( 0xFF00FF00 );
	Fill_l   ( 0xFF00003F );
	Circle(h_center_x, h_center_y, heading_radius*2);	  	// takes a diameter!
	
	// DRAW Direction Line:
	float x1 = h_center_x + m_delta_x_heading;
	float y1 = h_center_y + m_delta_y_heading;
	Line(h_center_x, h_center_y, x1, y1);
}

int   	GyroView::draw   	(  )
{  
	// DRAW BOUNDING BOX:
	Stroke_l ( background_color );
	Fill_l   ( background_color );
	Roundrect( left, bottom, width, height, 15.0, 15.0);

	draw_circle();
	draw_line();
	draw_heading();
	return 1;
}	


