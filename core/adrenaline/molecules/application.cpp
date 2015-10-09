#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <vector>

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
#include "adrenaline_windows.h"
#include "application.hpp"

using namespace std;

#define Debug 1


/* Standard handlers for About, Preferences, and Quit() */
static int app_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	if (Debug) printf("app_menu_actions() app=%x\n", mApp);
	if (mApp)
		switch(mMenuIndex) 
		{
			case 0: mApp->About();			break;
			case 1: mApp->Preferences();	break;
			case 2: mApp->Quit();			break;
			default: break;
		} 
}

static int app_file_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	if (mApp)
	switch(mMenuIndex) 
	{
	case 0: mApp->file_new	 	();		break;
	case 1: mApp->file_open	 	();		break;
	case 2: mApp->file_open_recent();	break;
	case 3: mApp->file_save	 	();		break;
	case 4: mApp->file_save_as	();		break;
	default: break;
	} 
}
 

Application::Application () 
{ 
	Initialize();
	//if (Debug) printf("Application::Application()done\n");	
	// Note: Any derived class from this will get it's constructor body
	// 		 executed after this function.
}
Application::Application ( Rectangle* mRect ) 
{
	Initialize();
	//if (Debug) printf("Application::Application(mrect)done\n");		
	// Note: Any derived class from this will get it's constructor body
	// 		 executed after this function.
}
Application::~Application() 
{ 
	
}

void 	Application::Initialize(	) 
{ 
	// Create Menu:
	//init_hMenu();  Override in each class.
	if (Debug) printf("Application::Initialize() \n");
	m_main_window = NULL;
	if (m_application_name.size()==0)
	{
		m_welcome_status   = "Generic Application";
		m_application_name = "Generic Application";
	}

	if (Debug) printf("Application::Initialize() done\n");

/*	setup_app_menu();		// About, Preferences, quit, 
	setup_main_menu    ();		// 
	onPlace();	*/
}

void	Application::setup_sidebar	(	)  // derived class adds these here
{
	// SideBar 
	Control* tmp = new IconView();
	m_sidebar_controls.push_back( tmp );	
	MainDisplay.m_side.load_controls( &m_sidebar_controls );
}

void	Application::setup_main_menu	(	)  // derived class adds these here
{
	m_main_menu.m_application = this;
	m_main_menu.clear_all();
	m_main_menu.add_sub_menu( m_application_name.c_str(), &m_app_menu  );

	m_file_menu.create_std_file_menu  ( );
	m_file_menu.add_callback_all_items( app_file_menu_actions );
	m_main_menu.add_sub_menu( "File", &m_file_menu  );
}

// Name of App, About, Preferences, Quit
void	Application::setup_app_menu(	)
{
	if (Debug) printf("Application::setup_app_menu()\n");
	m_app_menu.clear_all();
	m_app_menu.add_simple_command( "About"		);
	m_app_menu.add_simple_command( "Preferences");
	m_app_menu.add_simple_command( "Quit"    	);
	m_app_menu.add_callback_all_items( app_menu_actions );
	if (Debug) printf("Application::setup_app_menu() done\n");
}

int		Application::calc_metrics() 
{ 
}

// First Time application is run - load resources etc.
// chance to load resources, call functions which use fonts
int	Application::onCreate	(  ) 
{
	setup_app_menu();
	setup_main_menu();
	onPlace();
}

void Application::register_with_display_manager()
{
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( m_main_window 	 );
	MainDisplay.set_menu  ( &m_main_menu  	 	 );
	printf("register_with_display_manager- mid \n");
	
	// Establish the sidebar controls:
	// Create Sidebar items:
//	MainDisplay.m_side.load_controls( &m_sidebar_controls );	
	MainDisplay.m_status.set_text( m_welcome_status.c_str() );	
	onPlace();
	printf("register_with_display_manager- done \n");	
}

int		Application::onPlace( ) 
{
	if (Debug) printf("\tApplication::onPlace( ) \n");
	
	// Put the MainWindow in the requested client area.
	Rectangle* client_rect = MainDisplay.get_useable_rect();
	if (Debug) printf("\n\nMainDisplay.get_useable_rect() %x %x done\n", m_main_window, client_rect );	
	client_rect->print_positions();		
	if ( (m_main_window) && (client_rect) )
	{
		m_main_window->set_position( client_rect );
		if (Debug) printf("MainWindow. set position done\n");
		m_main_window->print_positions();

		//m_main_window->set_width_height( client_rect->get_width(), client_rect->get_height() );
		//m_main_window->move_to			( client_rect->get_left(), client_rect->get_bottom() );		
	}	
}

/* Return:  1=> all done.
			0=> need more time. */
int	Application::background_time_slice(	)
{
	return 1;
}

int		Application::onActivate	() 
{ 
}		// Future reactivation - set menus, sidebars, status, etc.

// Standard Menu Handlers:	
void	Application::file_new		() 
{ 
}
void	Application::file_open		() 
{ 
}	
void	Application::file_open_recent() 
{ 
}	
void	Application::file_save		() 
{ 
}
void	Application::file_save_as	() 
{ 
}

int		Application::About			(	)
{
	TextView* about = new TextView();
	MainDisplay.add_object( about );
}
int		Application::Preferences(	)
{
}

int	Application::Quit	() 
{
	if (Debug) printf("Application::Quit()\n");
	MainDisplay.remove_all_objects(	);
	MainDisplay.set_menu		( NULL 	);
	MainDisplay.m_side.unload_controls( );
	MainDisplay.m_status.set_text( "Closed app" );	
	if (Debug) printf("Application::Quit() done \n");
}

int		Application::onClick(int x, int y, bool mouse_is_down) 
{ 
}
int   	Application::draw		 	(	) 
{ 
}
