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
#include "can_window.hpp"
#include "filter_select.hpp"
#include "CAN_memory.h"
#include "packer.h"


void rescan_cb( void* mCANMessages )
{
	CANMessages* cm = (CANMessages*) mCANMessages;
	
	// REQUEST BOARDS PRESENT VIA CAN :
	struct sCAN msg;
	pack_board_presence_request( &msg, ASK_PRESENCE );
	shm_add_can_tx_message( &msg );
	
	cm->reset_boards_present();
	cm->Invalidate();	
}

FilterSelector* fs = NULL;

void adjust_filter_cb( void* mCANMessages )
{
	CANMessages* cm = (CANMessages*) mCANMessages;
	fs = new FilterSelector();	
	fs->set_width_height( 300, 200 );
	fs->move_to		    ( 200, 100 );
	fs->onCreate		( );
//	fs->m_update.set_on_click_listener( update_value_cb, mObj );
	fs->draw();		
//	cm->Invalidate();	
}


/* When the CAN_app is active, change the menu as below */

HorizontalMenu   can_menu(-1,-1);
VerticalMenu     can_file_menu(-1,-1);
VerticalMenu     can_view_menu(-1,-1);

int show_CAN_screens( void* menuPtr, int mMenuIndex )
{
/*	switch(mMenuIndex) 
	{
	case 0: show_messages		();		break;
	case 1: show_message_flowchart();	break;
	case 2: show_variable_graph ();		break;
	case 3: show_robot_limbs	();		break;
	case 4: show_button_boards	();		break;
	default: break;
	} */
}

/*void init_view_menu()
{
	can_view_menu.add_simple_command( "Messages" 	 );
	can_view_menu.add_simple_command( "Message Flow" );
	can_view_menu.add_simple_command( "Graph "		 );
	can_view_menu.add_simple_command( "Robot Limbs"  );
	can_view_menu.add_simple_command( "Button boards");

	can_view_menu.add_callback( 0, show_CAN_screens  );
	can_view_menu.add_callback( 1, show_CAN_screens  );
	can_view_menu.add_callback( 2, show_CAN_screens  );
	can_view_menu.add_callback( 3, show_CAN_screens  );
	can_view_menu.add_callback( 4, show_CAN_screens  );
}*/

void init_main_CAN_menu()
{
	can_file_menu.create_std_file_menu();

 	can_menu.add_sub_menu  ( "File", &can_file_menu );
	can_menu.add_entry_text( "Edit" );
	can_menu.add_sub_menu  ( "View", &can_view_menu );
}

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
/*****************************************************/

/*****************************************************/
void CANMessages::fill_phony_msgs()
{
	struct sCAN can;
	int i=0;
//	for (int i=0; i<100; i++)
	{
		can.id.group.id 	  = 0x0040+i;
		can.id.group.instance = 0x20  +i;
		can.header.DLC 		  = 8;
		fill_data( can.data, 0x10 + i );
		m_msg_view.add_message( &can );
	}
}

CANMessages::CANMessages() 
:Window()
{ 
	Initialize();
}
CANMessages::CANMessages( int Left, int Right, int Top, int Bottom ) 
:Window( Left,Right,Top,Bottom )
{
	Initialize(); 
}
CANMessages::CANMessages( int Width, int Height  ) 
:Window( Width, Height )
{ 
	Initialize();
}
CANMessages::~CANMessages() 
{ 
}

void 	CANMessages::Initialize(	) 
{ 
	Window::Initialize();
	//init_view_menu();
	//init_main_CAN_menu();
	//fill_phony_msgs();
}

int		CANMessages::calc_metrics() 
{ 
}

// chance to load resources, call functions which use fonts
int		CANMessages::onCreate(  )
{
	if (created)	return 0;
		
	place_views();
	
	Window::onCreate();
	m_msg_view.calc_column_positions_from_widths();
}


void  CANMessages::reset_boards_present()
{
	m_board_view.reset();
	m_board_view.Invalidate(); 
}

int		CANMessages::place_views()
{
	//printf("CANMessages::place_views()\t\t");
	//print_positions();

	m_board_view.set_width_height( width/2.-100., height/2. );	
	m_board_view.move_to		 ( left, bottom+100   );
	//m_board_view.print_positions ( );

	m_rescan.set_text		  ( "Rescan" );
	m_rescan.set_width_height	  ( 75, 50   );
	m_rescan.set_position_below	  ( &m_board_view, false, 0 ); 
	m_rescan.move_to 			  ( left, m_rescan.get_bottom() );
	m_rescan.set_on_click_listener	( rescan_cb, this );

	m_filter.set_text		  	  ( "Filter", true );
	m_filter.set_position_right_of( &m_rescan );
	m_filter.set_on_click_listener	( adjust_filter_cb, this );


	m_msg_view.move_to  		 ( left+width/2.-10, bottom  );
	m_msg_view.set_width_height	 ( width/2-10   , height   );
	m_msg_view.set_text_color	 ( 0xFF100000 );
	//m_msg_view.print_positions();
	
	register_child( &m_rescan     );
	register_child( &m_filter     );
	register_child( &m_board_view );
	register_child( &m_msg_view   );
}

int	CANMessages::handle_incoming_msg	( struct sCAN* msg ) 
{
	if (msg==NULL) return 0;

	//struct sCAN* result = m_filter_view.filter_incoming_msg( msg );
	//if (result)		

		m_msg_view.handle_incoming_msg( msg );	
		m_board_view.handle_incoming_msg(msg);

	return 1;
	// test if it's a trigger for another message.
	//     then send response	
}
int		CANMessages::setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms ) 
{ 
}
int		CANMessages::setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse ) 
{ 
}
int   	CANMessages::draw		 		(				) 
{ 
	return Control::draw();
}
int		CANMessages::get_hit_index		( int Mousex, int Mousey ) 
{ 
}
int		CANMessages::onClick(int x, int y, bool mouse_is_down) 
{ 
	return Window::onClick(x,y,mouse_is_down);
}

