/*******************************************************
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

 
static VerticalMenu     Finder_edit_menu(-1,-1);
static VerticalMenu     Finder_view_menu(-1,-1);
static VerticalMenu     Finder_graph_menu(-1,-1);


FinderApp*  finder_app = NULL;

static int finder_edit_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: finder_app->show_messages();		break;
	case 1: finder_app->show_network();		break;
	case 2: finder_app->show_sequencer();		break;		
	default: break;
	} 
}

static int finder_view_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: finder_app->show_messages();		break;
	case 1: finder_app->show_network();		break;
	case 2: finder_app->show_sequencer();		break;		
	case 3: finder_app->show_gyro();			break;			
	case 4: finder_app->show_analogs();		break;	
	default: break;
	} 
}

static int finder_graph_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: finder_app->add_watch();		break;
	case 1: finder_app->line_graph();		break;
	case 2: finder_app->histogram();		break;		
	case 3: finder_app->timing();			break;
	default: break;
	} 
}

void init_Finder_app( )
{
	printf("init_Finder_app()\n");
	if (finder_app==NULL)	
		finder_app = new FinderApp();
	if (finder_app)
		finder_app->register_with_display_manager();
}


/************************************************************************/

FinderApp::FinderApp () 
{
	Initialize(); 
}
FinderApp::FinderApp ( Rectangle* mRect ) 
{
	Initialize(); 
}
FinderApp::~FinderApp() 
{ 
}

void 	FinderApp::Initialize		(	) 
{ 
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/
	if (ipc_memory_can<=0)
		m_welcome_status   = "No CAN connection. Connect PiCAN board & run \'amon\'. ";
	else
		m_welcome_status   = "Monitor and generate CAN traffic. For vehicle or Adrenaline.";
	m_application_name = "CAN App";
	Application::Initialize();
			
	m_gyro 	  = new GyroView   ();
	m_analog  = new AnalogView ();
	m_msgs 	  = new CANMessages();	// defined in ../core/adrenaline/molecules/can_window.cpp
					//  contains a CANMessageView & NetworkView
	m_msg_view    = NULL; // new CANMessageView();		
	m_board_view  = NULL; // new NetworkView();
	m_main_window = m_msgs;
	printf("FinderApp::Initialize()\n");

	m_rx_tail 	   = 0;
	m_rx_tail_laps = 0;

	setup_sidebar ();
	printf("FinderApp::Initialize() done.\n");
}	// create all the objects here.

int		FinderApp::onPlace		(	) 
{ 
	Application::onPlace( );
}

void	FinderApp::setup_app_menu( )
{
	Application::setup_app_menu( );
}

void	FinderApp::setup_main_menu ( ) 
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

void FinderApp::setup_sidebar(	)
{
	MainDisplay.m_side.unload_controls();
	
	IconView* tmp = new IconView();
	tmp->set_file( "/home/pi/bk_code/avisual/resources/refresh.jpg" );
	tmp->set_width_height( 48., 48.);
	
	tmp->load_resources( );
	MainDisplay.m_side.add_control( tmp, "Refresh" );
}
char Finder_App_Status[] = "FINDER App";

void 	FinderApp::register_with_display_manager() 
{ 
	Application::register_with_display_manager();			
	printf("FinderApp::register_with_display_manager() done\n");		
}

void	FinderApp::About			(	) 
{ 
}
void	FinderApp::Preferences		(	) 
{ 
}

	void 			show_details	( );	// CAN Messages view.
	void 			show_files		( );	// Adrenaline Network.
	void 			show_icon		( );	// Robot sequencer

void 	FinderApp::show_details	( )	// CAN Messages view.
{
	m_main_window = m_msgs; //_view;
	MainDisplay.set_main_window( m_main_window );
}

void 	FinderApp::show_files	( )	// Adrenaline Network.
{
	m_main_window = m_board_view;
	MainDisplay.set_main_window( m_main_window );
}

void 	FinderApp::show_icon	( )	// Robot sequencer
{

}

void 	FinderApp::pie_graph		(	) 
{
}	
void 	FinderApp::histogram		(	) 
{
}
void 	FinderApp::timing			(	) 
{
}
int		FinderApp::background_time_slice(	)
{	
	return 0;
}

void	FinderApp::file_new		( )
{ 
}
void	FinderApp::file_open		( ) 
{ 
}	
void	FinderApp::file_open_recent( ) 
{ 
}
	
void	FinderApp::file_save		( ) 
{ 
}
void	FinderApp::file_save_as	( ) 
{ 
}
