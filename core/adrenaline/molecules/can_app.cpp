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
#include "adrenaline_windows.h"
#include "display.h"
#include "CAN_base.h"

// Demo Messages:
static void fill_data( byte* mdata, byte last)
{
	mdata[0] = 0x12;
	mdata[1] = 0x34;
	mdata[2] = 0x56;
	mdata[3] = 0x78;	
	mdata[4] = 0xAB;
	mdata[5] = 0xCD;
	mdata[6] = 0xEF;
	mdata[7] = last;
}

void CANApp::fill_phony_msgs()
{
	struct sCAN can;
	for (int i=0; i<100; i++)
	{
		can.id.group.id 	  = 0x0040+i;
		can.id.group.instance = 0x20  +i;
		can.header.DLC 		  = 8;
		fill_data( can.data, 0x10 + i );
		m_msg_view.add_message( &can );
	}
}

CANApp::CANApp() 
:Window()
{ 
}
CANApp::CANApp( int Left, int Right, int Top, int Bottom ) 
:Window( Left,Right,Top,Bottom )
{ 
}
CANApp::CANApp( int Width, int Height  ) 
:Window( Width, Height )
{ 
}
CANApp::~CANApp() 
{ 
}
void 	CANApp::Initialize(	) 
{ 
	Window::Initialize();
	printf("CAN_msg_view: object constructed and positioned.\n");	
	fill_phony_msgs();
	printf("CAN_msg_view: CAN msgs added.\n");
}

int		CANApp::calc_metrics() 
{ 
}

// chance to load resources, call functions which use fonts
int		CANApp::onCreate(  )
{
	place_views();
	fill_phony_msgs();
	
	Window::onCreate();
}

int		CANApp::place_views()
{
	printf("CANApp::place_views()\t\t");
	//print_positions();

	m_board_view.set_width_height( width/2.-100., height/2. );	
	m_board_view.move_to		 ( left, bottom+height/2.   );
	//m_board_view.print_positions ( );

	m_rescan.set_text			  ( "Rescan" );
	m_rescan.set_width_height	  ( 75, 50   );
	m_rescan.set_position_below	  ( &m_board_view, false, 0 ); 
	m_rescan.move_to 			  ( left, m_rescan.get_bottom() );

	m_msg_view.move_to  		 ( left+width/2., bottom  );
	m_msg_view.set_width_height	 ( width/2, height   );
	m_msg_view.set_text_color	 ( 0xFFFF0000 );
	//m_msg_view.print_positions();
	
	register_child( &m_rescan     );
	register_child( &m_board_view );
	register_child( &m_msg_view   );
}

int		CANApp::handle_incoming_msg	( struct sCAN* msg ) 
{
	//struct sCAN* result = m_filter_view.filter_incoming_msg( msg );
	//if (result)	
		m_msg_view.add_message( msg );
	// test if it's a trigger for another message.
	//     then send response	
}
int		CANApp::setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms ) 
{ 
}
int		CANApp::setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse ) 
{ 
}
int   	CANApp::draw		 		(				) 
{ 
	return Control::draw();
}
int		CANApp::get_hit_index		( int Mousex, int Mousey ) 
{ 
}
int		CANApp::onClick(int x, int y, bool mouse_is_down) 
{ 
	return Window::onClick(x,y,mouse_is_down);
}

