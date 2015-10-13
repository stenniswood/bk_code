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
#include <jpeglib.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>

#include "adrenaline_windows.h"
#include "hearts_app.hpp"
#include "hearts_game.hpp"


//static VerticalMenu     bj_file_menu(-1,-1);
HeartsApp* 	hearts_app = NULL;


void init_hearts( )
{
	printf("init_hearts()\n");
	if (hearts_app==NULL)	
		hearts_app = new HeartsApp();
	MainDisplay.start_app( hearts_app );
}

HeartsApp::HeartsApp ()
{
	Initialize(); 
}
 
HeartsApp::HeartsApp ( Rectangle* mRect )
{
	Initialize(); 
} 

HeartsApp::~HeartsApp()
{ 

}

void 	HeartsApp::Initialize	(	)
{
	//printf("HeartsApp::Initialize()\n");
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/

	m_application_name = "Hearts";
	m_main_window = new HeartsGame( 3 );
	//printf("HeartsApp::Initialize()\n");	

	setup_app_menu();
	setup_main_menu    ();
	onPlace();	
}

int		HeartsApp::onPlace		(	)
{ 
	Application::onPlace();		// Place the main window.
	
	((HeartsGame*)m_main_window)->onPlace();
} 

// First Time application is run - load resources etc.
int		HeartsApp::onCreate(	)	
{
	return Application::onCreate();
	
}

void	HeartsApp::setup_main_menu  	( )
{ 
	Application::setup_main_menu();
//	bj_file_menu.create_std_file_menu();
//	bj_file_menu.add_callback_all_items( draw_file_menu_actions );
//	m_hMenu.add_sub_menu( "File", &bj_file_menu  );	

} 

void	HeartsApp::setup_app_menu( )
{ 
	Application::setup_app_menu();
} 

void 	HeartsApp::register_with_display_manager()
{ 
	Application::register_with_display_manager();
	
} 	

int		HeartsApp::About			(	)
{ 
	return Application::About();
} 

int		HeartsApp::Preferences	(	)
{
	return Application::Preferences(); 
} 

int		HeartsApp::Quit			(	)
{ 
	return Application::Quit();
} 	

void	HeartsApp::file_new		( )
{ 

} 
void	HeartsApp::file_open		( )
{ 
} 	
void	HeartsApp::file_open_recent( )
{ 
} 	
void	HeartsApp::file_save		( )
{ 
} 
void	HeartsApp::file_save_as	( )
{ 
} 
