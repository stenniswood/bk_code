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



//static VerticalMenu     draw_file_menu(-1,-1);
static VerticalMenu     CAN_edit_menu(-1,-1);
static VerticalMenu     CAN_view_menu(-1,-1);
static VerticalMenu     CAN_graph_menu(-1,-1);


CANApp*  can_app = NULL;

static int can_edit_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: can_app->show_messages();		break;
	case 1: can_app->show_network();		break;
	case 2: can_app->show_sequencer();		break;		
	default: break;
	} 
}

static int can_view_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: can_app->show_messages();		break;
	case 1: can_app->show_network();		break;
	case 2: can_app->show_sequencer();		break;		
	default: break;
	} 
}
static int can_graph_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: can_app->add_watch();		break;
	case 1: can_app->line_graph();		break;
	case 2: can_app->histogram();		break;		
	case 3: can_app->timing();			break;
	default: break;
	} 
}

void init_CAN_app( )
{
	printf("init_CAN_app()\n");
	if (can_app==NULL)	
		can_app = new CANApp();
	if (can_app)
		can_app->register_with_display_manager();
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

void 	CANApp::Initialize		(	) 
{ 
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/

	m_main_window = new CANMessages();
	m_application_name = "CAN App";

	setup_app_menu();
	setup_menu    ();
	setup_sidebar ();
	onPlace();	

}	// create all the objects here.

int		CANApp::onPlace		(	) 
{ 
	Application::onPlace( );

}

void	CANApp::setup_app_menu( )
{
	Application::setup_app_menu( );
}

void	CANApp::setup_menu  	( ) 
{ 
	Application::setup_menu();

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
	CAN_view_menu.add_callback_all_items( can_view_menu_actions );
		
	// GRAPH MENU:  
	CAN_graph_menu.clear_all();	
	CAN_graph_menu.add_simple_command( "Watch CAN variable"  );
	CAN_graph_menu.add_simple_command( "Variable Line Graph"  );	
	CAN_graph_menu.add_simple_command( "Variable Histogram"  );
	CAN_graph_menu.add_simple_command( "Timing Graph"  );	
	CAN_graph_menu.add_callback_all_items( can_graph_menu_actions  );
	
	// H MENU:
	m_hMenu.add_sub_menu  ( "Edit", &CAN_edit_menu );
	m_hMenu.add_sub_menu  ( "View", &CAN_view_menu );
	m_hMenu.add_sub_menu  ( "Graph", &CAN_graph_menu );
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
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( m_main_window );
	MainDisplay.m_status.set_text("CAN App");
	MainDisplay.set_menu  	( &m_hMenu );
}	

int				CANApp::About			(	) 
{ 
}
int				CANApp::Preferences		(	) 
{ 
}

void 	CANApp::show_messages	( )	// CAN Messages view.
{
	m_main_window = m_msg_view;
		// This will be better as a tab control page probably.
	register_with_display_manager();			
}

void 	CANApp::show_network	( )	// Adrenaline Network.
{
	m_main_window = m_board_view;
		// This will be better as a tab control page probably.
	register_with_display_manager();
}

void 	CANApp::show_sequencer	( )	// Robot sequencer
{
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
