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
			case 2: mApp->Quit();	
					MainDisplay.close_app( mApp );	
					break;
			default: return 0; break;
		} 
	return 1;
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
	default: return 0; 	break;
	} 
	return 1;
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
	m_sidebar_controls = new std::vector<Control*>();
	printf("m_sidebar_control size=%d\n", m_sidebar_controls->size() );
	if (Debug) printf("Application::Initialize() DONE!\n");	
}

void	Application::setup_sidebar	(	)  // derived class adds these here
{
	// SideBar 
	Control* tmp = new IconView();
	m_sidebar_controls->push_back( tmp );
	MainDisplay.m_side.load_controls( m_sidebar_controls );
	if (Debug) printf("Application::setup_sidebar() done\n");	
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

/* Must call setup_app_menu first!  */
void	Application::setup_main_menu(	)  // The horizontal menu. derived class adds these here
{
	if (Debug) printf("Application::setup_main_menu()\n");
	m_main_menu.m_application = this;
	m_main_menu.clear_all();
	m_main_menu.add_sub_menu( m_application_name.c_str(), &m_app_menu  );

	m_file_menu.create_std_file_menu  ( );
	m_file_menu.add_callback_all_items( app_file_menu_actions );
	m_main_menu.add_sub_menu( "File", &m_file_menu  );
	if (Debug) printf("Application::setup_main_menu() done\n");	
}

int		Application::calc_metrics() 
{ 
	return 0;
}

// First Time application is run - load resources etc.
// chance to load resources, call functions which use fonts
int	Application::onCreate	(  ) 
{
	// What about creating all the App's various windows?  
	// These are done on set_main_window()	
	printf("Application::onCreate()\n");
	setup_app_menu ();
	setup_main_menu();
	setup_sidebar  ();
	return 1;
}

void Application::register_with_display_manager()
{
	printf("Application::register_with_display_manager()\n");
	MainDisplay.remove_all_objects(	);
	MainDisplay.set_main_window( m_main_window );
	MainDisplay.set_menu  	   ( &m_main_menu  );

	// Establish the sidebar controls:
	// Create Sidebar items:
	MainDisplay.m_side.load_controls( m_sidebar_controls );
	MainDisplay.m_status.set_text( m_welcome_status.c_str() );	
	printf("Application::register_with_display_manager() - done \n");	
}

/* Note: Application does not have a "place".  It represents the executable,
 		and not any windowing positions.  So the old function Application::onPlace
 		which was here is deprecated and removed.  The main window may have
 		a onPlace for setting it's child controls, but even the get_usable_rect() from
 		the display manager will be done on set_main_widow().    */

/* Return:  1=> all done.
			0=> need more time. */
int	Application::background_time_slice(	)
{
	return 1;
}

int		Application::onActivate	() 
{ 
	return 1;
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

void	Application::About			(	)
{
	TextView* about = new TextView();
	about->set_text("Sample Generic Application");	
	MainDisplay.add_object( about );
	return ;	
}
void	Application::Preferences(	)
{
	return ;
}

int	Application::Quit	() 
{
	if (Debug) printf("Application::Quit()\n");

/*	MainDisplay.remove_all_objects(	);
	MainDisplay.set_menu		( NULL 	);
	MainDisplay.m_side.unload_controls( );
	MainDisplay.m_status.set_text( "Closed app" );	
	MainDisplay.close_app(this); */
	if (Debug) printf("Application::Quit() done \n");
	return 1;
}

int		Application::onClick(int x, int y, bool mouse_is_down) 
{ 
	return 1;
}
int   	Application::draw		 	(	) 
{ 
	return 1;
}

