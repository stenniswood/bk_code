#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "control.hpp"


#include <string>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "control.hpp"
#include "display.h"

#include "motor_gui.hpp"



MotorGuiBaseControl::MotorGuiBaseControl()
{
}
MotorGuiBaseControl::~MotorGuiBaseControl()
{
}


/*******************************************************************************
	Class - LinearMotorGuiControl
*******************************************************************************/

ServoMotorGuiControl::ServoMotorGuiControl()
{
	Initialize(); 
} 
ServoMotorGuiControl::~ServoMotorGuiControl()
{
 
} 
void ServoMotorGuiControl::Initialize()
{
	m_range_of_motion	= 300.;			// in degrees
	m_zero_offset		= 0.;				// zero at center of range of motion.
	m_stop1				= -149.;
	m_stop2				= +150 ;	
	m_destination		= 0.;

	// Drawing Info:
	calc_metrics();	
}

void ServoMotorGuiControl::set_width_height ( int Width, int Height )
{  
	Control::set_width_height( Width, Height );
	calc_metrics();	
}
void ServoMotorGuiControl::move_to ( float Left,   float  Bottom )
{  
	Control::move_to( Left, Bottom );
	calc_metrics();	
}


void ServoMotorGuiControl::calc_metrics( )
{
	m_center_x = left + width/2.;
	m_center_y = bottom + height/2;
	m_circle_radius = min(width,height) / 4;
	
}

void ServoMotorGuiControl::draw_motor()
{
	// Draw circle with dial:  (use an image? like the dials on android apps.  It 
	// would add some flare)
	Stroke_l ( 0xFFFFFFFF );
	Fill_l   ( 0xFF3f0000 );
	Circle( m_center_x,m_center_y, m_circle_radius*2);

	// DRAW TEXT:
	char title[50];
	sprintf(title, "Motor: #1"  ); 	
	Fill_l   ( 0xFFFFFFFF );	
	TextMid( m_center_x, bottom+0.1*height, title, SerifTypeface, text_size );	
	
} 
void  ServoMotorGuiControl::draw_stops()
{
	Stroke_l ( 0xFFFFEF2F );
	Fill_l   ( 0xFFFf0000 );

	float ExtensionLength =  + 10;
	float OuterLength     =  + 10+20;
	float m_stop1_rads = radians( m_stop1 );
	float m_stop2_rads = radians( m_stop2 );
		
	Stroke_l ( 0xFFFF0000 );		
	float x1 = m_center_x + (m_circle_radius+ExtensionLength) * sin(m_stop1_rads);	// radially outward from circle center
	float y1 = m_center_y + (m_circle_radius+ExtensionLength) * cos(m_stop1_rads);	// radially outward from circle center;
	float x2 = m_center_x + (m_circle_radius+OuterLength) * sin(m_stop1_rads);	// radially outward from circle center
	float y2 = m_center_y + (m_circle_radius+OuterLength) * cos(m_stop1_rads);	// radially outward from circle center;	
	Line(x1,y1, x2,y2);	

	// Stop #2
	Stroke_l ( 0xFFFF0000 );	
	x1 = m_center_x + (m_circle_radius+ExtensionLength) * sin(m_stop2_rads);	// radially outward from circle center
	y1 = m_center_y + (m_circle_radius+ExtensionLength) * cos(m_stop2_rads);	// radially outward from circle center;
	x2 = m_center_x + (m_circle_radius+OuterLength) * sin(m_stop2_rads);	// radially outward from circle center
	y2 = m_center_y + (m_circle_radius+OuterLength) * cos(m_stop2_rads);	// radially outward from circle center;	
	Line(x1,y1, x2,y2);
} 	
void  ServoMotorGuiControl::draw_current_position()
{
	float ExtensionLength =  + 10;
	float OuterLength     =  + 10+20;
	float m_current_pos_rads = radians( m_current_pos );
	// Current Position:
	Stroke_l ( 0xFF1fFF00 );	
	float x1 = m_center_x + (m_circle_radius+ExtensionLength) * sin(m_current_pos_rads);	// radially outward from circle center
	float y1 = m_center_y + (m_circle_radius+ExtensionLength) * cos(m_current_pos_rads);	// radially outward from circle center;
	float x2 = m_center_x + (m_circle_radius+OuterLength) * sin(m_current_pos_rads);	// radially outward from circle center
	float y2 = m_center_y + (m_circle_radius+OuterLength) * cos(m_current_pos_rads);	// radially outward from circle center;	
	Line(x1,y1, x2,y2);	
 
} 
void  ServoMotorGuiControl::draw_destination_position()
{ 
	float ExtensionLength =  + 10;
	float OuterLength     =  + 10+20;
	float m_destination_rads = radians( m_destination );

	// Destination:
	Stroke_l ( 0xFF7fEF00 );		
	float x1 = m_center_x + (m_circle_radius+ExtensionLength) * sin(m_destination_rads);	// radially outward from circle center
	float y1 = m_center_y + (m_circle_radius+ExtensionLength) * cos(m_destination_rads);	// radially outward from circle center;
	float x2 = m_center_x + (m_circle_radius+OuterLength) * sin(m_destination_rads);	// radially outward from circle center
	float y2 = m_center_y + (m_circle_radius+OuterLength) * cos(m_destination_rads);	// radially outward from circle center;	
	Line(x1,y1, x2,y2);	
} 	
void  ServoMotorGuiControl::draw_motor_stats()
{
	// Such as speed, torque, duty, etc.
} 	

int ServoMotorGuiControl::draw()
{
	Control::draw();
	
	draw_motor();
	draw_stops();
	draw_current_position();
	draw_destination_position();
	draw_motor_stats();
	return 1;
}

/*******************************************************************************
	Class - LinearMotorGuiControl
*******************************************************************************/


LinearMotorGuiControl::LinearMotorGuiControl()
{
	Initialize(); 
} 
LinearMotorGuiControl::~LinearMotorGuiControl()
{
 
} 
void LinearMotorGuiControl::Initialize()
{

}
void LinearMotorGuiControl::set_width_height  	  ( int Width, int Height 		 )
{
	Control::set_width_height (  Width,  Height );
	calc_metrics();
}
void LinearMotorGuiControl::move_to	  		  	  ( float Left,   float  Bottom	 )
{
	Control::move_to (  Left, Bottom );
	calc_metrics();
}
void LinearMotorGuiControl::calc_metrics			( )
{
	m_start_x = 1/4 * width;
	m_end_x   = 3/4 * width;
	m_travel_bottom = 1/4 * height;

}

void LinearMotorGuiControl::draw_motor()
{
	// Draw line of travel:
	float x1 = m_start_x;
	float y1 = m_travel_bottom;
	float x2 = m_end_x;
	float y2 = m_travel_bottom;
	Line(x1,y1, x2,y2);	
	
	
	Line(x1,y1, x2,y2);	
	Line(x1,y1, x2,y2);			
} 
void  LinearMotorGuiControl::draw_stops()
{
 
} 	
void  LinearMotorGuiControl::draw_current_position()
{
 
} 
void  LinearMotorGuiControl::draw_destination_position()
{ 

} 	
void  LinearMotorGuiControl::draw_motor_stats()
{
	// Such as speed, torque, duty, etc.
} 	

int LinearMotorGuiControl::draw()
{
	Control::draw();
	
	draw_motor();
	draw_stops();
	draw_current_position();
	draw_destination_position();
	draw_motor_stats();
	return 1;	
}

