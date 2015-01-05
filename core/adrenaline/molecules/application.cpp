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
#include "adrenaline_windows.h"
#include "application.hpp"


Application::Application () 
{ 
	printf("Application::Application()\n");
	Initialize();
	printf("Application::Application()done\n");	
}
Application::Application ( Rectangle* mRect ) 
{
	Initialize();
}
Application::~Application() 
{ 
	
}

void 	Application::Initialize(	) 
{ 
	// Create Menu:
	//init_hMenu();  Override in each class.
	m_main_window = NULL;
	m_welcome_status = "Generic Application";
}

void	Application::setup_sidebar	(	)  // derived class adds these here
{
	// SideBar 
	Control* tmp = new IconView();
	m_sidebar_controls.push_back( tmp );
	
}

void	Application::setup_menu		(	)  // derived class adds these here
{
}

// Name of App, About, Preferences, Quit
void	Application::setup_app_menu(	)
{
	// over ride this to setup the menu.	
}

int		Application::calc_metrics() 
{ 
}

// First Time application is run - load resources etc.
// chance to load resources, call functions which use fonts
int	Application::onCreate	(  ) 
{
	// Set the Menu: 
	MainDisplay.set_menu( &m_hMenu );

	// Establish the sidebar controls:
	// Create Sidebar items:
	MainDisplay.m_side.load_controls( &m_sidebar_controls );	
	MainDisplay.m_status.set_text( m_welcome_status.c_str() );	
}

void Application::register_with_display_manager()
{	
}

int		Application::onPlace( ) 
{
	// Put the MainWindow in the requested client area.
	Rectangle* client_rect = MainDisplay.get_useable_rect();
	printf("MainDisplay.get_useable_rect() done\n");	
	if ( (m_main_window) && (client_rect) )
	{
		m_main_window->set_width_height( client_rect->get_width(), client_rect->get_height() );
		m_main_window->move_to			( client_rect->get_left(), client_rect->get_bottom() );		
	}	
	printf("MainWindow. set position done\n");	
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
void	Application::file_quit	() 
{
	MainDisplay.remove_all_objects(	);
	MainDisplay.set_menu		( NULL 	);
	MainDisplay.m_side.unload_controls( );
	MainDisplay.m_status.set_text( "Closed app" );
	
}

int		Application::onClick(int x, int y, bool mouse_is_down=true) 
{ 
}
int   	Application::draw		 	(	) 
{ 
}
