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
#include "bk_system_defs.h"
//#include "global.h"

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "control.hpp"
#include "display.h"
#include "GyroViewI.hpp"
#include "vector_math.h"
#include "fuse_accel.h"

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
	m_airplane_mode = false;
	
	calc_metrics();
	set_roll_angle_deg   (0.0);
	set_pitch_angle_deg  (0.0);
	set_heading_angle_deg(0.0);				// North is 0 degrees.	
}
void 	GyroView::calc_metrics( )
{  
	float space = width * 0.05;
	m_left_margin  = left + space;
	m_right_margin = left + width - space;	
	
	m_center_x = left   + width/2;
	m_center_y = bottom + height/2;
	m_radius   = min(width,height)/4.;	// diameter is half the width of the window.
	// radius should be 1/4.  And the Circle() function takes a diameter not radius.	
}

void GyroView::set_airplane_mode( bool mAirplane )
{
	m_airplane_mode = mAirplane;
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

/*
Given:  b & r & m

Circle:		x^2 + y^2 = r^2
Line  :     y = m*x + b
Substitute:  x^2 + m^2*x^2 + 2*b*m*x  = r^2 - b^2 = - K1
			(1+m^2)*x^2 + 2*b*m*x + K1 = 0 
			a* x^2 + B * x + K1 = 0
	Two roots:
			x1 = (-B + sqrt( B*B - 4*a*K1)) / (2*a);
			x2 = (-B - sqrt( B*B - 4*a*K1)) / (2*a);			
*/
void GyroView::find_circle_intersection( float mSlope, float mIntercept, float& mX, float& mY, bool mPositiveRoot )
{
	float K1 = -(m_radius * m_radius - mIntercept*mIntercept);
	float a = 1 + mSlope*mSlope;
	float B = 2*mIntercept*mSlope;
	
	float root1_x = (-B + sqrt( B*B - 4*a*K1 )) / (2*a);
	float root2_x = (-B - sqrt( B*B - 4*a*K1 )) / (2*a);

	float root1_y = mSlope * root1_x + mIntercept;
	float root2_y = mSlope * root2_x + mIntercept;

	if (mPositiveRoot)
	{  mX = root1_x;  mY=root1_y; }
	else
	{  mX = root2_x;  mY=root2_y; }
}

void 	GyroView::draw_line	(  )
{  
	Stroke_l ( 0xFFFF7F00 );
	Fill_l   ( 0xFFFF7F00 );

	float y_offset = m_radius * sin(m_pitch_angle_radians);		// Intercept
	float Slope    = 1 * sin(m_roll_angle_radians);				// Slope

	float x1 = 0; //m_center_x + m_delta_x;
	float y1 = y_offset+ m_center_y + m_delta_y;
	find_circle_intersection( Slope, y_offset, x1,y1, true );
	
	float x2 = 0; //m_center_x - m_delta_x;
	float y2 = y_offset+ m_center_y - m_delta_y;
	find_circle_intersection( Slope, y_offset, x2,y2, false );

	printf("GyroLine: %4.1f,%4.1f;  %4.1f,%4.1f; \n", x1,y1, x2,y2 );
	//printf("GyroCenter: %4.1f,%4.1f \n", m_center_x, m_center_y );
	Line(m_center_x+x1, m_center_y+y1, m_center_x+x2, m_center_y+y2);	
}

void GyroView::draw_plumb_line(  )
{  
	Stroke_l ( 0xFFFF7F00 );
	Fill_l   ( 0xFFFF7F00 );
	float x1 = m_center_x;
	float y1 = m_center_y;

	float gravity_angle = atan2( -m_pitch_angle_radians, m_roll_angle_radians );
	float mag = fabs(AccelScaled.x) + fabs(AccelScaled.y);
	mag = min(1.0, mag);

	float line_length = m_radius * mag;
	
	float x2 = line_length * cos( gravity_angle );
	float y2 = line_length * sin( gravity_angle );

	x2 += m_center_x;
	y2 += m_center_y;
	Line(x1, y1, x2, y2);		
}

void 	GyroView::draw_circle (  )
{	
	Stroke_l ( 0xFFFFFFFF );
	Fill_l   ( 0xFF3f0000 );
	Circle(m_center_x, m_center_y, m_radius*2);	  	// takes a diameter!
}

void 	GyroView::draw_info(  )
{
	// DRAW TEXT:
	char title[50];
	float roll  = degrees( m_roll_angle_radians );
	float pitch = degrees( m_pitch_angle_radians);
	float yaw   = degrees( m_heading );
	
	sprintf(title, "Roll, Pitch, yaw:" );
	Stroke_l ( 0xFF000000 );
	Fill_l   ( 0xFFFFFFFF );	
	TextMid( m_center_x, bottom+0.12*height, title, SerifTypeface, text_size );	
	
	sprintf(title, "<%5.1f, %5.1f, %5.1f>", roll, pitch, yaw );
	TextMid( m_center_x, bottom+0.12*height-text_size*1.2, title, SerifTypeface, text_size );	
}

void 	GyroView::draw_heading(  )
{	
	float heading_radius = m_radius/2;
	float h_center_x = m_center_x+ m_radius+0.9*heading_radius;
	float h_center_y = m_center_y+ m_radius+0.7*heading_radius;
	
	Stroke_l ( 0xFF00FF00 );
	Fill_l   ( 0xFF00003F );
	Circle(h_center_x, h_center_y, heading_radius*2);	  	// takes a diameter!
	
	// DRAW Direction Line:
	float x1 = h_center_x + m_delta_x_heading;
	float y1 = h_center_y + m_delta_y_heading;
	Line(h_center_x, h_center_y, x1, y1);
}


int GyroView::handle_incoming_msg( struct sCAN* msg )
{
	
}

int   	GyroView::draw   	(  )
{  
	Control::draw();
	
	// DRAW BOUNDING BOX:
	Stroke_l ( background_color );
	Fill_l   ( background_color );
	Roundrect( left, bottom, width, height, 15.0, 15.0);

	// DRAW TEXT:
	char title[50];
	sprintf(title, "Gyro/Accelerometer" ); 	
	Stroke_l ( 0xFFFF0000 );
	Fill_l   ( 0xFFFFFFFF );	
	TextMid( m_center_x, bottom+0.9*height, title, SerifTypeface, text_size );

	draw_circle();	
	if (m_airplane_mode)
		draw_line();
	else
		draw_plumb_line();

	draw_info();
	draw_heading();
	return 1;
}	


