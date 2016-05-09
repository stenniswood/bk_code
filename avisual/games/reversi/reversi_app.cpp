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
#include "reversi_board.hpp"
#include "reversi_ai_player.hpp"
#include "reversi_app.hpp"


//static VerticalMenu     bj_file_menu(-1,-1);
ReversiApp* 	reversi_app = NULL;

static int draw_file_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex)
	{
	case 0: reversi_app->file_new	 	();		break;
	case 1: reversi_app->file_open	 	();		break;
	case 2: reversi_app->file_open_recent();		break;
	case 3: reversi_app->file_save	 	();		break;
	case 4: reversi_app->file_save_as	();		break;
	default: break;
	} 
}

void init_reversi( )
{
	printf("init_reversi()\n");
	
	if (reversi_app==NULL)	
		reversi_app = new ReversiApp();
	if (reversi_app)
		MainDisplay.start_app( reversi_app );
}

ReversiApp::ReversiApp ()
{
	Initialize(); 
}
 
ReversiApp::ReversiApp ( Rectangle* mRect )
{
	Initialize(); 
} 

ReversiApp::~ReversiApp()
{ 

}

void 	ReversiApp::Initialize	(	)
{
	//printf("ReversiApp::Initialize()\n");
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/

	m_application_name = "Reversi";
	m_main_window = new ReversiBoardView( );
	//printf("ReversiApp::Initialize()\n");	

//	setup_app_menu();
//	setup_menu    ();
//	onPlace();	
}

// First Time application is run - load resources etc.
int		ReversiApp::onCreate(	)	
{
	Application::onCreate();
	
}

void	ReversiApp::setup_main_menu  	( )
{ 
	Application::setup_main_menu();
//	bj_file_menu.create_std_file_menu();
//	bj_file_menu.add_callback_all_items( draw_file_menu_actions );
//	m_hMenu.add_sub_menu( "File", &bj_file_menu  );	

} 

void	ReversiApp::setup_app_menu( )
{ 
	Application::setup_app_menu();
} 

void 	ReversiApp::register_with_display_manager()
{ 
	Application::register_with_display_manager();
	
} 	

int		ReversiApp::About			(	)
{ 
	Application::About();
} 

int		ReversiApp::Preferences	(	)
{
	Application::Preferences(); 
} 

int		ReversiApp::Quit			(	)
{ 
	Application::Quit();
} 	

void	ReversiApp::file_new		( )
{ 

} 
void	ReversiApp::file_open		( )
{ 
} 	
void	ReversiApp::file_open_recent( )
{ 
} 	
void	ReversiApp::file_save		( )
{ 
} 
void	ReversiApp::file_save_as	( )
{ 
} 
