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
#include "draw_app.hpp"
#include "draw_app2.hpp"
#include "CAN_base.h"
#include "can_window.hpp"
#include "gyro_window.h"
#include "analog_board_app.hpp"
#include "CAN_memory.h"
#include "display_manager.hpp"
#include "client_memory.hpp"
#include "CAN_dispatcher.hpp"

 
static VerticalMenu     CAN_edit_menu (-1,-1);
static VerticalMenu     CAN_view_menu (-1,-1);
static VerticalMenu     CAN_graph_menu(-1,-1);

CANApp*  can_app = NULL;
char CAN_App_Status[] = "CAN App";

/***** Handler for Menu's *****/
static int can_edit_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: can_app->show_messages();		return 1;	break;
	case 1: can_app->show_network();		return 1;	break;
	case 2: can_app->show_sequencer();		return 1;	break;		
	default: break;
	} 
	return 0;	
}

static int can_view_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: can_app->show_messages();		return 1;	break;
	case 1: can_app->show_network();		return 1;	break;
	case 2: can_app->show_sequencer();		return 1;	break;		
	case 3: can_app->show_gyro();			return 1;	break;			
	case 4: can_app->show_analogs();		return 1;	break;	
	default: break;
	} 
	return 0;	
}

static int can_graph_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: can_app->add_watch();		return 1;	break;
	case 1: can_app->line_graph();		return 1;	break;
	case 2: can_app->histogram();		return 1;	break;		
	case 3: can_app->timing();			return 1;	break;
	default: break;
	} 
	return 0;	
}
/***** END OF Handler for Menu's *****/

void init_CAN_app( )
{
	printf("init_CAN_app()\n");
	if (can_app==NULL)
		can_app = new CANApp();
	MainDisplay.start_app( can_app );
}

/* This will request CAN from abkInstant.  When the PiCAN card is on a remote 
	host.
	
*/
void start_can()
{
	char working_buffer[127];
	strcpy( working_buffer, "receive can" );	
	printf("NOTE: You must have abkInstant running inorder to use this app!\n");
	printf("NOTE: You must have abkInstant running inorder to use this app!\n");
	cli_ipc_write_sentence( working_buffer );
	cli_wait_for_ack_update();	
}

/************************************************************************/
CANApp::CANApp () 
{
	Initialize(); 
}
CANApp::CANApp ( Rectangle* mRect ) 
{
	Initialize(); 
}
CANApp::~CANApp() 
{ 
}

extern int can_connected;		// visual_main.cpp 

void 	CANApp::Initialize		(	) 
{ 
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/

	// ESTABLISH CAN IPC :
	if (can_connected==FALSE)
		m_welcome_status   = "No CAN connection. Connect PiCAN board & run \'amon\'. ";
	else {
		m_rx_tail 	   = 0;
		m_rx_tail_laps = 0;

		//CAN_add_rx_callback( &distribute_can_msg );
		m_welcome_status   = "Monitor and generate CAN traffic. For vehicle or Adrenaline.";
	}
	// BEGIN APP INIT:	
	m_application_name = "CAN App";	
	Application::Initialize();

	m_gyro 	  		= new GyroOverview();
	m_analog  		= new AnalogView  ();
	m_msgs 	  		= new CANMessages ();	// defined in ../core/adrenaline/molecules/can_window.cpp
	m_msg_view    	= new CANMessageView();		
	m_board_view  	= NULL; // new NetworkView();
	m_main_window 	= m_msgs;
	printf("CANApp::Initialize()\n");

	setup_sidebar ();
	printf("CANApp::Initialize() done.\n");
}	// create all the objects here.


void	CANApp::setup_app_menu( )
{
	Application::setup_app_menu( );
}

void	CANApp::setup_main_menu ( ) 
{ 
	Application::setup_main_menu();

	// EDIT MENU:
	CAN_edit_menu.clear_all();	
	CAN_edit_menu.add_simple_command( "Add Triggered Event"  );	
	CAN_edit_menu.add_simple_command( "Add Periodic Event"  );
	CAN_edit_menu.add_callback_all_items( can_edit_menu_actions  );

	// VIEW MENU:
	CAN_view_menu.clear_all();	
	CAN_view_menu.add_simple_command( "Messages"  );
	CAN_view_menu.add_simple_command( "Network"   );
	CAN_view_menu.add_simple_command( "Sequencer"  );
	CAN_view_menu.add_simple_command( "Gyro"    );
	CAN_view_menu.add_simple_command( "Analog"  );		
	CAN_view_menu.add_callback_all_items( can_view_menu_actions );
		
	// GRAPH MENU:  
	CAN_graph_menu.clear_all();	
	CAN_graph_menu.add_simple_command( "Watch CAN variable"  );
	CAN_graph_menu.add_simple_command( "Variable Line Graph"  );	
	CAN_graph_menu.add_simple_command( "Variable Histogram"  );
	CAN_graph_menu.add_simple_command( "Timing Graph"  );	
	CAN_graph_menu.add_callback_all_items( can_graph_menu_actions  );
	
	// H MENU:
	m_main_menu.add_sub_menu  ( "Edit", &CAN_edit_menu );
	m_main_menu.add_sub_menu  ( "View", &CAN_view_menu );
	m_main_menu.add_sub_menu  ( "Graph", &CAN_graph_menu );
}

void CANApp::setup_sidebar(	)
{
	MainDisplay.m_side.unload_controls();
	
	IconView* tmp = new IconView();
	tmp->set_file( "/home/pi/bk_code/avisual/resources/refresh.jpg" );
	tmp->set_width_height( 48., 48.);
	
	tmp->load_resources( );
	MainDisplay.m_side.add_control( tmp, "Refresh" );
}


void 	CANApp::register_with_display_manager() 
{ 
	Application::register_with_display_manager();			
	printf("CANApp::register_with_display_manager() done\n");		
}

void	CANApp::About			(	) 
{ 
}
void	CANApp::Preferences		(	) 
{ 
}

void 	CANApp::show_messages	( )		// CAN Messages view.
{
	m_main_window = m_msgs; //_view;
		// This will be better as a tab control page probably.
	MainDisplay.set_main_window( m_main_window );
}

void 	CANApp::show_network	( )	// Adrenaline Network.
{
	m_main_window = m_board_view;
	// This will be better as a tab control page probably.
	MainDisplay.set_main_window( m_main_window );
}

void 	CANApp::show_sequencer	( )	// Robot sequencer
{
	//MainDisplay.MainDisplay.set_main_window( m_main_window );
}
void 	CANApp::show_gyro	( )		// Robot sequencer
{
	printf("CANApp::show_gyro() called\n");
	m_main_window = m_gyro;
	MainDisplay.set_main_window( m_main_window );
}
void 	CANApp::show_analogs ( )	// Robot sequencer
{
	printf("CANApp::show_ANALOG()  \n" );
	m_main_window  = m_analog;
	MainDisplay.set_main_window( m_main_window );
}

void 	CANApp::add_watch		(	) 
{
}	
void 	CANApp::line_graph		(	) 
{
}	
void 	CANApp::histogram		(	) 
{
}
void 	CANApp::timing			(	) 
{
}


BOOL  CANApp::distribute_can_msg( struct sCAN* mMsg )
{
	if (mMsg) 
	{
		//printf("got new message\n");
		
		// Distribute to All Views :
		if (m_analog)
			m_analog->handle_incoming_msg( mMsg );
		if (m_gyro)
			m_gyro->handle_incoming_msg  ( mMsg );  
//			if (m_msgs)
//				m_msgs->handle_incoming_msg( mMsg ); 

		if (m_board_view)
			m_board_view->handle_incoming_msg( mMsg );        
		if (m_msg_view) 
			m_msg_view->handle_incoming_msg( mMsg ); 
	}	
}

/* This method is deprecated though it works.  Because other parts of the OS
	may use CAN messages too.  Such as the Fusion algorithm.  We don't want to
	be the exclusive user of these messages.  So instead we'll attached a call
	back to the CAN_Dispatcher.cpp.
	
	The CAN Dispatcher can 
*/
int		CANApp::background_time_slice(	)
{	
	// CHECK CAN : 	
/*	struct sCAN* ptr=NULL;
	while (shm_isRxMessageAvailable( &m_rx_tail, &m_rx_tail_laps) )
	{	
		//printf("CANApp rx_tail=%d;  rx_tail_laps=%d\n", m_rx_tail, m_rx_tail_laps );
		ptr = shm_GetNextRxMsg (&m_rx_tail, &m_rx_tail_laps);
		distribute_can_msg( ptr );
	};	*/
	return 0;
}

void	CANApp::file_new		( )
{ 
}
void	CANApp::file_open		( ) 
{ 
}	
void	CANApp::file_open_recent( ) 
{ 
}
	
void	CANApp::file_save		( ) 
{ 
}
void	CANApp::file_save_as	( ) 
{ 
}


	//CAN_add_rx_callback( BOOL (*mCallback)(struct sCAN*) );		
	/*if (ipc_memory_can->isReceiving==FALSE)
	{	
	printf("no remote CAN data, requesting...\n");  
	start_can();
	}*/
