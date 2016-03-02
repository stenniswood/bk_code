// Draws a simple line thru center of a circle Top
// show the angle of orientation for a accel/gyro board.

// Steve Tenniswood
// Feb 2, 2015

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "Graphbase.hpp"
#include "control.hpp"
#include "checkbox.hpp"
#include "display.h"


GyroView::GyroView( )
{  
}
GyroView::GyroView( int Left, int Right, int Top, int Bottom )
{  
}
GyroView::GyroView( int Width, int Height )
{  
}

void	GyroView::set_angle_deg			( float mDegrees )
{  
	m_angle_radians = radians(mDegrees);
}
float	GyroView::get_angle_deg			( )
{  
	return degrees(m_angle_radians);
}

void 	GyroView::Initialize			( )
{  
	strcpy (class_name, "GyroView");
	m_angle_radians = 0.0;
	m_left_margin = left;
	m_right_margin = left+width;
}
void 	GyroView::calc_metrics			( )
{  
	float space = width * 0.05;
	m_left_margin  = left + space;
	m_right_margin = left + width - space;
	
}
void	GyroView::print_info			( )
{
	printf("Gyro angle = %6.2f\n", get_angle_deg() );
	
}
//void 			set_font				( Fontinfo* f	 ){  }
void 	GyroView::set_width_height  	 ( int Width, int Height 		)
{  
}
void  	GyroView::set_text_size		  	( float TextSize		 		)
{  
}

void 	GyroView::draw_line	(  )
{  

}
void 	GyroView::draw_circle (  )
{
  
}
int   	GyroView::draw   		(  )
{  
	draw_circle();
	draw_line();
	return 1;
}	


