/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <list>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "CAN_base.h"
#include "../core/misc/vector_base.h"
#include "can_id_list.h"
#include "parser_tilt.h"
#include "gyro_window.h"


GyroView::GyroView()
{
	set_width_height( 1200., 700 );	
	move_to		    ( 0, 100   );
	Initialize();

}
GyroView::GyroView( int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}
GyroView::GyroView( int Width, int Height  )
{
	left=0; bottom=0; 
	width  = Width;
	height = Height;
	Initialize();
}
GyroView::~GyroView()
{

}

void GyroView::Initialize(	)
{
	Window::Initialize();
	printf("AnalogView::Initialize()\n");

	accel[0]  = new DataSet();
	gyro [0]  = new DataSet();
	magnet[0] = new DataSet();

	accel[1]  = new DataSet();
	gyro [1]  = new DataSet();
	magnet[1] = new DataSet();

	accel[2]  = new DataSet();
	gyro [2]  = new DataSet();
	magnet[2] = new DataSet();

	hg  = new Histogram( 100, 300, 600, 400 );		// was ( 100, 300, 300, 100 );
	x_axis = new LineGraph( 100, 300, 600, 400 );
	y_axis = new LineGraph( 400, 600, 600, 400 );
	z_axis = new LineGraph( 700, 900, 600, 400 );

	add_control( hg );
	add_control( x_axis );
	add_control( y_axis );
	add_control( z_axis );		
	place_views();	
}

int GyroView::calc_metrics()
{
	return 0;
}
int GyroView::place_views()
{
	printf("GyroView::place_views() \n");
	float Left=100;
	Left+=125;
	x_axis->set_width_height ( 200., 200. );
	x_axis->move_to		 	 ( Left, bottom+40  );
	y_axis->set_width_height ( 200., 200. );
	Left+=220;	
	y_axis->move_to		 	 ( Left, bottom+40  );
	Left+=220;
	z_axis->set_width_height ( 200., 200. );
	z_axis->move_to		 	 ( Left, bottom+40  );
	return 0;
}

int	GyroView::handle_incoming_msg	( struct sCAN* msg )
{
	int instance;
	int channel;
	int value;
	if (msg->id.group.id == ID_ACCEL_XYZ)
	{	
		instance = msg->id.group.instance;
		channel = msg->data[0];
		parse_accel_msg( msg );
		accel[0]->add(RawxyzAccel.x);
		accel[1]->add(RawxyzAccel.y);
		accel[2]->add(RawxyzAccel.z);		
	}
	if (msg->id.group.id == ID_GYRO_XYZ)
	{	
		instance = msg->id.group.instance;
		channel = msg->data[0];
		parse_gyro_msg( msg );
		gyro[0]->add(RawxyzGyro.x);
		gyro[1]->add(RawxyzGyro.y);
		gyro[2]->add(RawxyzGyro.z);				
	}
	if (msg->id.group.id == ID_MAGNET_XYZ)
	{	
		instance = msg->id.group.instance;
		channel = msg->data[0];
		parse_magnet_msg( msg );
		magnet[0]->add(RawxyzMagnet.x);
		magnet[1]->add(RawxyzMagnet.y);
		magnet[2]->add(RawxyzMagnet.z);						
	}
	return 0;
}

int	GyroView::setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms )
{
	return 0;
}
void GyroView::register_with_display_manager()
{
	// 
//	MainDisplay.remove_all_objects(	);
//	MainDisplay.load_resources();	
}

int	GyroView::setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse )
{
	return 0;
}
void GyroView::fill_phony_msgs()
{

}
int	GyroView::onCreate	  (  )	// chance to load resources, call functions which use fonts
{
	return 0;
}
int	GyroView::get_hit_index		( int Mousex, int Mousey )
{
	return 0;
}
int	GyroView::onClick	( int x, int y, bool mouse_is_down)
{
	return 0;
}
int GyroView::draw(	)
{
	return 0;
}
}


