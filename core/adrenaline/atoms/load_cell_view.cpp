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
#include "load_cell_view.hpp"
//#include "bk_system_defs.h"
#include "global.h"
#include "global_funcs.h"


#define Debug 0


LoadCellView::LoadCellView( )
{
	Initialize(); 
}
LoadCellView::LoadCellView( int Left, int Right, int Top, int Bottom )
:Control( Left, Right, Top, Bottom )
{ 
	Initialize();
}
LoadCellView::~LoadCellView()
{ 
}

void	LoadCellView::set_sensor_value	( int mSensorNumber, float mPoundsofForce )
{ 
	m_sensor_values[mSensorNumber] = mPoundsofForce;
	compute_toe_pressure_fraction();
}

float	LoadCellView::get_sensor_value	( int mSensorNumber)
{
	return m_sensor_values[mSensorNumber];
}

void LoadCellView::set_name(const char* mName)
{
	strcpy(m_name, mName);
}

void 	LoadCellView::Initialize		(	)
{
	HasBorder = true;
	for (int i=0; i<4; i++)  
		m_sensor_values[i]  = 0.0;				// North is 0 degrees.

	m_sensor_width   = 50;		// the rectangle representing the sensor.
	m_sensor_height  = 75;	
	m_left_margin   = 10;
	m_right_margin  = 10;	 

	float x_inc = (width-m_sensor_width*2) / 5.;
	m_sensor_left[HEEL_L]   = left + 2*x_inc;
	m_sensor_left[HEEL_R]   = m_sensor_left[HEEL_L] + m_sensor_width + x_inc;		// Right side of foot.
	m_sensor_left[TOE_L]    = left + 2*x_inc;
	m_sensor_left[TOE_R]    = m_sensor_left[HEEL_L] + m_sensor_width + x_inc;		// Right side of foot.
		
	m_sensor_bottom[HEEL_L] = bottom+0.2*height+ text_size*1.5;
	m_sensor_bottom[HEEL_R] = bottom+0.2*height+ text_size*1.5;
	m_sensor_bottom[TOE_L] = m_sensor_bottom[HEEL_L]+m_sensor_height*1.5+10;		// toe
	m_sensor_bottom[TOE_R] = m_sensor_bottom[HEEL_L]+m_sensor_height*1.5+10;		// toe 
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
const float MAX_LOAD_CELL_READING = 18.0;		// lbs

float LoadCellView::compute_toe_pressure_fraction( )
{
	float toe;
	float heel;
	toe =  get_sensor_value	( TOE_R );
	toe += get_sensor_value	( TOE_L );	
	heel =  get_sensor_value( HEEL_R );
	heel += get_sensor_value( HEEL_L );	
	if ((toe+heel)==0)
		m_toe_fraction = 0.0;
	else 	
		m_toe_fraction = toe / (toe+heel);
	return m_toe_fraction;
}


int   	LoadCellView::draw   			(  )
{ 
	Control::draw();
	
	long box_color = 0xFF000000;
	float TotalWeight = 0.0;
	Stroke_l ( 0xFF00FF00 );
	Fill_l   ( 0xFF00003F );	
	Dprintf(" loadcell width=%6.1f; h=%6.2f;\n", m_sensor_width, m_sensor_height );
	
	for (int i=0; i<4; i++)  {
		// change color to m_sensor_values[i] mapped color.
		box_color = 0xFF000000;
		int scaled_reading = ceil( fabs(255. * m_sensor_values[i] / MAX_LOAD_CELL_READING) );
		scaled_reading = min (255, scaled_reading);
		
		if (m_sensor_values[i] < 0.0)
			box_color |= (scaled_reading);		
		else
			box_color |= (scaled_reading<<16);		
		Dprintf("Box color= %lx;  l=%6.2f; b=%6.2f\n", box_color, m_sensor_left[i], m_sensor_bottom[i] );
		Stroke_l ( 0xFF00FF00 );
		Fill_l   ( box_color );
		Roundrect(  m_sensor_left[i], m_sensor_bottom[i], 
					m_sensor_width, m_sensor_height, 10, 10 );
		TotalWeight += m_sensor_values[i];
	}
	// Maybe draw a net direction vector.
	
	// Text total weight:
	char title[50];
	sprintf(title, "Total Weight: %5.1f", TotalWeight ); 	
	Fill_l   ( 0xFFFFFFFF );	
	float center_x = left+ width/2.;
	TextMid( center_x, bottom+0.1*height, title,  SerifTypeface, text_size );
	TextMid( center_x, bottom+0.9*height, m_name, SerifTypeface, text_size );	
	
	// Print Toe Fraction:	
	sprintf(title, "%3.0f \%\n", m_toe_fraction*100 );
	Text( left+5, bottom+0.7*height, title, SerifTypeface, 10 );	
	
	bool left_foot = (strcmp(m_name, "left" )==0);	
	sprintf( title, "Inside" );
	Fill_l ( 0xFFFFFFFF );
	float x = left + text_size + 2;
	float center_y = bottom + height/2.;
	if (left_foot)
	{
		x = left + width - 2;
	}
	Translate( x, center_y );
	Rotate( 90.0 );
	TextMid( 0, 0, title, SerifTypeface, text_size );
	Rotate( -90.0 );	
	Translate( -x, -center_y );
	return 1;
}	



