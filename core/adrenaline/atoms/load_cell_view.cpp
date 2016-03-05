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



LoadCellView::LoadCellView( )
{
	Initialize(); 
}
LoadCellView::LoadCellView( int Left, int Right, int Top, int Bottom )
{ 
	Initialize();
}
LoadCellView::~LoadCellView()
{ 
}

void	LoadCellView::set_sensor_value	( int mSensorNumber, float mPoundsofForce )
{ 
	m_sensor_values[mSensorNumber] = mPoundsofForce;
	
}
float	LoadCellView::get_sensor_value	( int mSensorNumber)
{
 
}

void 	LoadCellView::Initialize		(	)
{
	for (int i=0; i<4; i++)  {
		m_sensor_values[i]  = 0.0;				// North is 0 degrees.
		m_sensor_left[i]    = 0.0;
		m_sensor_bottom[i]  = 0.0;
	}
	
	m_sensor_width   = 20;		// the rectangle representing the sensor.
	m_sensor_height  = 30;	
	
	m_left_margin   = 10;
	m_right_margin  = 10;	 
}

void 	LoadCellView::calc_metrics		(  )
{ 
}

void	LoadCellView::print_info		(	)
{ 
}
void 	LoadCellView::set_width_height  ( int Width, int Height )
{ 
}
void 	LoadCellView::draw_a_sensor		(  )
{ 
}
int   	LoadCellView::draw   			(  )
{ 
	float TotalWeight = 0.0;
	Stroke_l ( 0xFF00FF00 );
	Fill_l   ( 0xFF00003F );	
	for (int i=0; i<4; i++)  {
		// change color to m_sensor_values[i] mapped color.
		Fill_l   ( 0xFF00003F );
		Rectangle(  m_sensor_left[i], m_sensor_bottom[i], 
					m_sensor_left[i]+m_sensor_width, m_sensor_bottom[i]+m_sensor_height );
		TotalWeight += m_sensor_values[i];
	}
	// Maybe draw a net direction vector.
	
	// Text total weight:
	char title[50];
	sprintf(title, "Total Weight: %5.1f", TotalWeight ); 	
	Fill_l   ( 0xFFFFFFFF );	
	TextMid( m_center_x, bottom+0.1*height, title, SerifTypeface, text_size );	
	
}	



