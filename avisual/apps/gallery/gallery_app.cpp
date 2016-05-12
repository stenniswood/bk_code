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
#include "gallery_app.hpp"




//static VerticalMenu     gallery_file_menu(-1,-1);
static VerticalMenu     gallery_edit_menu(-1,-1);
static VerticalMenu     gallery_view_menu(-1,-1);

GalleryApp*  gallery_app = NULL;

void init_gallery_app( )
{
	if (gallery_app==NULL)
		gallery_app = new GalleryApp();
	MainDisplay.start_app( gallery_app );
}

static int gallery_view_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: gallery_app->show_grid	();		break;
	default: return 0;						break;
	} 
	return 1;
}

/************************************************************************/

GalleryApp::GalleryApp () 
{
	Initialize(); 
}
GalleryApp::GalleryApp ( Rectangle* mRect ) 
{
	Initialize(); 
}
GalleryApp::~GalleryApp() 
{ 
	gallery_app = NULL;
}

void 	GalleryApp::Initialize		(	) 
{ 
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/
	m_application_name = "Gallery";	
	m_gallery.set_directory ( "/home/pi/bk_code/abkInstant/media/" );
	m_gallery.load_resources( );
	
	m_main_window = &m_gallery;
}

void	GalleryApp::setup_app_menu( )
{
	Application::setup_app_menu( );
}

void	GalleryApp::setup_main_menu ( ) 
{ 
	Application::setup_main_menu();

	gallery_view_menu.clear_all();
	gallery_view_menu.add_simple_command( "Rotate Left"	  );
	gallery_view_menu.add_simple_command( "Rotate Right"	  );
	gallery_view_menu.add_simple_command( "Set as desktop"  );
	gallery_view_menu.add_simple_command( "Zoom In" );	
	gallery_view_menu.add_simple_command( "Zoom Out" );		
	gallery_view_menu.add_callback_all_items( gallery_view_menu_actions  );

	m_main_menu.add_entry_text( "Edit" );
	m_main_menu.add_sub_menu  ( "View",     &gallery_view_menu );
}


void	GalleryApp::About			(	) 
{ 
	Application::About();
}
void	GalleryApp::Preferences		(	) 
{ 
}

void	GalleryApp::file_new		( )
{ 
}
void	GalleryApp::file_open		( ) 
{ 
}	
void	GalleryApp::file_open_recent( ) 
{ 
}
	
void	GalleryApp::file_save		( ) 
{ 
}
void	GalleryApp::file_save_as	( ) 
{ 
}

void 	GalleryApp::show_grid		( )
{
}

void GalleryApp::zoom_in			( )
{ 
}

void GalleryApp::zoom_out			( )
{ 
}
