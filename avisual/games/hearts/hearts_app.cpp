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

/*static int draw_file_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex)
	{
	case 0: mApp->file_new	 	();		break;
	case 1: mApp->file_open	 	();		break;
	case 2: mApp->file_open_recent();	break;
	case 3: mApp->file_save	 	();		break;
	case 4: mApp->file_save_as	();		break;
	default: break;
	} 
}*/

void init_hearts( )
{
	printf("init_hearts()\n");
	if (hearts_app==NULL)	
		hearts_app = new HeartsApp();
	if (hearts_app)
		hearts_app->register_with_display_manager();		
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
	setup_menu    ();
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
	Application::onCreate();
	
}

void	HeartsApp::setup_menu  	( )
{ 
	Application::setup_menu();
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
	Application::About();
} 

int		HeartsApp::Preferences	(	)
{
	Application::Preferences(); 
} 

int		HeartsApp::Quit			(	)
{ 
	Application::Quit();
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
