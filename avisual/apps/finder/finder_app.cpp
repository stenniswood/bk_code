/*******************************************************
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <jpeglib.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <fontinfo.h>
#include <shapes.h>

#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include "Graphbase.hpp"
#include "adrenaline_windows.h"
#include "display_manager.hpp"
#include "finder_app.hpp"
 
 
static VerticalMenu     Finder_edit_menu(-1,-1);
static VerticalMenu     Finder_view_menu(-1,-1);
static VerticalMenu     Finder_graph_menu(-1,-1);


FinderApp*  finder_app = NULL;

static int finder_edit_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: finder_app->edit_cut();			break;
	case 1: finder_app->edit_copy();		break;
	case 2: finder_app->edit_paste();		break;		
	default: break;
	} 
}

static int finder_view_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: finder_app->show_preview();		break;
	case 1: finder_app->show_toolbar();		break;
	case 2: finder_app->show_history();		break;
	default: break;
	} 
}

static int finder_graph_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0 : break;
	default: break;
	} 
}

void init_finder_app( )
{
	printf("init_Finder_app()\n");
	if (finder_app==NULL)	
		finder_app = new FinderApp();

	MainDisplay.start_app( finder_app );
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
	finder_app = NULL;
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
	m_welcome_status   = "Adrenaline File browser. ";
	m_application_name = "Finder";

	m_main_window = new CompleteFileBrowser(1000,600);
	setup_sidebar ();
	printf("FinderApp::Initialize() done.\n");
}

void	FinderApp::setup_app_menu( )
{
	Application::setup_app_menu( );
}

void	FinderApp::setup_main_menu ( ) 
{ 
	Application::setup_main_menu();

	// EDIT MENU:
	Finder_edit_menu.clear_all();
	Finder_edit_menu.add_simple_command( "Cut file"  );	
	Finder_edit_menu.add_simple_command( "Copy file"  );
	Finder_edit_menu.add_simple_command( "Paste file"  );	
	Finder_edit_menu.add_callback_all_items( finder_edit_menu_actions  );

	// VIEW MENU:
	Finder_view_menu.clear_all();	
	Finder_view_menu.add_simple_command( "Show Preview"  );
	Finder_view_menu.add_simple_command( "Show Toolbar"  );
	Finder_view_menu.add_simple_command( "Show History"  );	
	Finder_view_menu.add_callback_all_items( finder_view_menu_actions );
		
	// GRAPH MENU:  
	Finder_graph_menu.clear_all();	
	Finder_graph_menu.add_simple_command( "Watch CAN variable"  );
	Finder_graph_menu.add_simple_command( "Variable Line Graph"  );	
	Finder_graph_menu.add_simple_command( "Variable Histogram"  );
	Finder_graph_menu.add_simple_command( "Timing Graph"  );	
	Finder_graph_menu.add_callback_all_items( finder_graph_menu_actions  );
	
	// H MENU:
	m_main_menu.add_sub_menu  ( "Edit", &Finder_edit_menu );
	m_main_menu.add_sub_menu  ( "View", &Finder_view_menu );
	m_main_menu.add_sub_menu  ( "Graph", &Finder_graph_menu );
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


void	FinderApp::About			(	) 
{ 
}
void	FinderApp::Preferences		(	) 
{ 
}

void  FinderApp::show_preview	( )	
{
}

void FinderApp::show_toolbar	( )	
{
}

void FinderApp::show_history	( )	
{
//	m_main_window = m_msgs;
//	MainDisplay.set_main_window( m_main_window );
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

void FinderApp::edit_cut		( )
{
}
void FinderApp::edit_copy		( )
{
}
void FinderApp::edit_paste		( )
{
}

