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
//#include "draw_app.hpp"
#include "draw_app2.hpp"
#include "CAN_base.h"
#include "draw_canvas.hpp"



//static VerticalMenu     draw_file_menu(-1,-1);
static VerticalMenu     draw_edit_menu(-1,-1);
static VerticalMenu     draw_view_menu(-1,-1);

DrawApp*  draw_app = NULL;

void init_drawing_app( )
{
	printf("init_apps()\n");
	if (draw_app==NULL)	
		draw_app = new DrawApp();
	if (draw_app)
		MainDisplay.start_app( draw_app );
}


static int draw_view_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: draw_app->show_grid	();		break;
	default: break;
	} 
}

/************************************************************************/

DrawApp::DrawApp () 
{
	Initialize(); 
}
DrawApp::DrawApp ( Rectangle* mRect ) 
{
	Initialize(); 
}
DrawApp::~DrawApp() 
{ 
}

void 	DrawApp::Initialize		(	) 
{ 
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/

	m_main_window = new Drawing2D();
	m_application_name = "Drawing App";
	
	setup_app_menu();
	setup_main_menu();
	onPlace();	

}	// create all the objects here.

int		DrawApp::onPlace		(	) 
{ 

}

void	DrawApp::setup_app_menu( )
{
	Application::setup_app_menu( );
}

void	DrawApp::setup_main_menu ( ) 
{ 
	Application::setup_main_menu();

	draw_view_menu.clear_all();
	draw_view_menu.add_simple_command( "Erase "	  );
	draw_view_menu.add_simple_command( "Color" 	  );
	draw_view_menu.add_simple_command( "Zoom In"  );
	draw_view_menu.add_simple_command( "Zoom Out" );	
	draw_view_menu.add_simple_command( "Show brush toolbar" );		
	draw_view_menu.add_simple_command( "Show shape toolbar" );
	draw_view_menu.add_callback_all_items( draw_view_menu_actions  );

	m_main_menu.add_entry_text( "Edit" );
	m_main_menu.add_sub_menu  ( "View",     &draw_view_menu );
}

void 	DrawApp::register_with_display_manager() 
{ 
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( m_main_window );
	MainDisplay.m_status.set_text("Draw App");
	MainDisplay.set_menu  	( &m_main_menu );
}	

void				DrawApp::About			(	) 
{ 
}
void				DrawApp::Preferences		(	) 
{ 
}

void	DrawApp::file_new		( )
{ 
}
void	DrawApp::file_open		( ) 
{ 
}	
void	DrawApp::file_open_recent( ) 
{ 
}
	
void	DrawApp::file_save		( ) 
{ 
}
void	DrawApp::file_save_as	( ) 
{ 
}

void DrawApp::show_grid		( )
{
}


void DrawApp::zoom_in			() 
{ 
}
void DrawApp::zoom_out			()
{ 
}
