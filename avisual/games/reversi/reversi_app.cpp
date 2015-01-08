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
#include "blackjack.hpp"
#include "blackjack_app.hpp"


//static VerticalMenu     bj_file_menu(-1,-1);
BlackJackApp* 	bj_app = NULL;

static int draw_file_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex)
	{
	case 0: bj_app->file_new	 	();		break;
	case 1: bj_app->file_open	 	();		break;
	case 2: bj_app->file_open_recent();		break;
	case 3: bj_app->file_save	 	();		break;
	case 4: bj_app->file_save_as	();		break;
	default: break;
	} 
}

void init_blackjack( )
{
	printf("init_blackjack()\n");
	
	if (bj_app==NULL)	
		bj_app = new BlackJackApp();
	if (bj_app)
		bj_app->register_with_display_manager();		
}

BlackJackApp::BlackJackApp ()
{
	Initialize(); 
}
 
BlackJackApp::BlackJackApp ( Rectangle* mRect )
{
	Initialize(); 
} 

BlackJackApp::~BlackJackApp()
{ 

}

void 	BlackJackApp::Initialize	(	)
{
	//printf("BlackJackApp::Initialize()\n");
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/

	m_application_name = "Black Jack";
	m_main_window = new BlackJack( 3 );
	//printf("BlackJackApp::Initialize()\n");	

	setup_app_menu();
	setup_menu    ();
	onPlace();	
}

int		BlackJackApp::onPlace		(	)
{ 
	Application::onPlace();		// Place the main window.
	
	((BlackJack*)m_main_window)->onPlace();
} 

// First Time application is run - load resources etc.
int		BlackJackApp::onCreate(	)	
{
	Application::onCreate();
	
}

void	BlackJackApp::setup_menu  	( )
{ 
	Application::setup_menu();
//	bj_file_menu.create_std_file_menu();
//	bj_file_menu.add_callback_all_items( draw_file_menu_actions );
//	m_hMenu.add_sub_menu( "File", &bj_file_menu  );	

} 

void	BlackJackApp::setup_app_menu( )
{ 
	Application::setup_app_menu();
} 

void 	BlackJackApp::register_with_display_manager()
{ 
	Application::register_with_display_manager();
	
} 	

int		BlackJackApp::About			(	)
{ 
	Application::About();
} 

int		BlackJackApp::Preferences	(	)
{
	Application::Preferences(); 
} 

int		BlackJackApp::Quit			(	)
{ 
	Application::Quit();
} 	

void	BlackJackApp::file_new		( )
{ 

} 
void	BlackJackApp::file_open		( )
{ 
} 	
void	BlackJackApp::file_open_recent( )
{ 
} 	
void	BlackJackApp::file_save		( )
{ 
} 
void	BlackJackApp::file_save_as	( )
{ 
} 