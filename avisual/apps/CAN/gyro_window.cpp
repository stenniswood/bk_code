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
//#include <jpeglib.h>
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
	//set_width_height( 1500., 900 );	
	move_to		    ( 0, 100   );
	Initialize();
}
GyroView::GyroView( int Left, int Right, int Top, int Bottom )
: Window (Left,Right,Top,Bottom)
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

const char x_title[] = "X Axis Accel/Gyro";
const char y_title[] = "Y Axis Accel/Gyro";
const char z_title[] = "Z Axis Accel/Gyro";

void GyroView::Initialize(	)
{
	Window::Initialize();
	printf("GyroView::Initialize()\n");

	accel[0]  = new DataSet();
	gyro [0]  = new DataSet();
	magnet[0] = new DataSet();

	accel[1]  = new DataSet();
	gyro [1]  = new DataSet();
	magnet[1] = new DataSet();

	accel[2]  = new DataSet();
	gyro [2]  = new DataSet();
	magnet[2] = new DataSet();

	//hg     = new Histogram( 100, 300, 600, 400 );		// was ( 100, 300, 300, 100 );
	x_axis = new LineGraph( 100, 300, 600, 400 );
	y_axis = new LineGraph( 400, 600, 600, 400 );
	z_axis = new LineGraph( 700, 900, 600, 400 );

	// Chart #1
	x_axis->set_title 			( (char*) x_title );
	x_axis->set_xLabel			( (char*) "Time" 				);
	x_axis->set_yLabel			( (char*) "Deg/sec" 	);	
	x_axis->set_max				( 32767.0 );
	x_axis->set_min				( -32767.0  );	
	x_axis->add_data_series		( accel[0]	);
	x_axis->add_data_series		( gyro[0]	);
	x_axis->add_data_series		( magnet[0]	);	
	x_axis->calc_scale			( 			);
	x_axis->set_horizontal_lines( 5 		);
	x_axis->set_vertical_lines	( 5 		);

	// Chart #2
	y_axis->set_title 			( (char*) y_title );
	y_axis->set_xLabel			( (char*) "Time" 				);
	y_axis->set_yLabel			( (char*) "Deg/sec" 	);	
	y_axis->set_max				( 32767.0 );
	y_axis->set_min				( -32767.0  );	
	y_axis->add_data_series		( accel[1]	);
	y_axis->add_data_series		( gyro[1]	);
	y_axis->add_data_series		( magnet[1]	);	
	y_axis->calc_scale			( 			);
	y_axis->set_horizontal_lines( 5 		);
	y_axis->set_vertical_lines	( 5 		);

	z_axis->set_title 			( (char*) z_title );
	z_axis->set_xLabel			( (char*) "Time" 				);
	z_axis->set_yLabel			( (char*) "Deg/sec" 	);	
	z_axis->set_max				( 32767.0 );
	z_axis->set_min				( -32767.0  );	
	z_axis->add_data_series		( accel[2]	);
	z_axis->add_data_series		( gyro[2]	);
	z_axis->add_data_series		( magnet[2]	);	
	z_axis->calc_scale			( 			);
	z_axis->set_horizontal_lines( 5 		);
	z_axis->set_vertical_lines	( 5 		);

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
	
	x_axis->set_width_height ( 400., 400. );
	x_axis->move_to		 	 ( Left, bottom+40  );
	Left+=220;	
	
	y_axis->set_width_height ( 400., 400. );
	y_axis->set_position_right_of( x_axis, true, 20. );
	//move_to		 	 ( Left, bottom+40  );
	Left+=220;

	z_axis->set_width_height ( 400., 400. );
	//z_axis->move_to		 	 ( Left, bottom+40  ); 
	z_axis->set_position_right_of( y_axis, true, 20. );	
	return 0;
}

int	GyroView::handle_incoming_msg	( struct sCAN* msg )
{
	float gyro_sum_x = 0;
	float gyro_sum_y = 0;
	float gyro_sum_z = 0;
	
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
		Invalidate();		
	}
	if (msg->id.group.id == ID_GYRO_XYZ)
	{	
		instance = msg->id.group.instance;
		channel = msg->data[0];
		parse_gyro_msg( msg );
		gyro_sum_x += RawxyzGyro.x;
		gyro_sum_y += RawxyzGyro.y;
		gyro_sum_z += RawxyzGyro.z;
		gyro[0]->add(gyro_sum_x);
		gyro[1]->add(gyro_sum_y);
		gyro[2]->add(gyro_sum_z);				
		Invalidate();		
	}
	if (msg->id.group.id == ID_MAGNET_XYZ)
	{	
		instance = msg->id.group.instance;
		channel = msg->data[0];
		parse_magnet_msg( msg );
		magnet[0]->add(RawxyzMagnet.x);
		magnet[1]->add(RawxyzMagnet.y);
		magnet[2]->add(RawxyzMagnet.z);						
		Invalidate();		
	}
	return 0;
}

int	GyroView::setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms )
{
	return 0;
}
void GyroView::register_with_display_manager()
{
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
	return Window::onCreate();
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
	return Window::draw();
}

}

