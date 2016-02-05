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
#include "video_app.hpp"
#include "video.h"


static VerticalMenu     Video_edit_menu(-1,-1);
static VerticalMenu     Video_view_menu(-1,-1);
static VerticalMenu     Video_settings_menu(-1,-1);

VideoApp*  video_app = NULL;

static int video_edit_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: ((VideoApp*)mApp)->cut();		break;
	case 1: ((VideoApp*)mApp)->copy();		break;
	case 2: ((VideoApp*)mApp)->paste();		break;	
	case 3: ((VideoApp*)mApp)->snap_shot();	break;
	default: break;
	} 
}

static int video_view_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: ((VideoApp*)mApp)->camera_list();		break;
	case 1: ((VideoApp*)mApp)->local_camera();		break;
	case 2: ((VideoApp*)mApp)->remote_camera();		break;		
	case 3: //((VideoApp*)mApp)->movie();	
			video_decode_test( sample_file_name );
				break;
	case 4: ((VideoApp*)mApp)->Incoming();			break;	
	case 5: ((VideoApp*)mApp)->YouTube();			break;
	default: break;
	}  
}
static int video_settings_menu_actions( void* menuPtr, int mMenuIndex, Application* mApp )
{
	switch(mMenuIndex) 
	{
	case 0: ((VideoApp*)mApp)->view_size();			break;
	case 1: ((VideoApp*)mApp)->video_quality();		break;
	case 2: ((VideoApp*)mApp)->camera_settings();	break;		
	default: break;
	} 
}

void init_Video_app( )
{
	printf("init_Video_app()\n");
	if (video_app==NULL)	
		video_app = new VideoApp();
	if (video_app)
		video_app->register_with_display_manager();
}


/************************************************************************/

VideoApp::VideoApp () 
{
	Initialize(); 
}
VideoApp::VideoApp ( Rectangle* mRect ) 
{
	Initialize(); 
}
VideoApp::~VideoApp() 
{ 
}

void 	VideoApp::Initialize		(	) 
{
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/
	m_main_window 	   = new Control();
	m_welcome_status   = "Video & Camera";
	m_application_name = "Video App";


}	// create all the objects here.


void	VideoApp::setup_app_menu( )
{
	Application::setup_app_menu( );
}

void	VideoApp::setup_main_menu  	( ) 
{ 
	Application::setup_main_menu();

	// EDIT MENU:
	Video_edit_menu.clear_all();	
	Video_edit_menu.add_simple_command( "Cut"  );	
	Video_edit_menu.add_simple_command( "Copy"  );
	Video_edit_menu.add_simple_command( "Paste"  );	
	Video_edit_menu.add_simple_command( "Take Snapshot"  );	
	//Video_edit_menu.add_simple_command( "Save Stream"  );		// camera or incoming.
	Video_edit_menu.add_callback_all_items( video_edit_menu_actions  );

	// VIEW MENU:
	Video_view_menu.clear_all();	
	Video_view_menu.add_simple_command( "Available Cameras"  );	
	Video_view_menu.add_simple_command( "Local Camera"  );
	Video_view_menu.add_simple_command( "Remote Camera"  );	
	Video_view_menu.add_simple_command( "Movie"   );
	Video_view_menu.add_simple_command( "Incoming"  );
	Video_view_menu.add_simple_command( "You Tube"  );	
	Video_view_menu.add_callback_all_items( video_view_menu_actions );

	Video_settings_menu.clear_all();	
	Video_settings_menu.add_simple_command( "Size"  );	
	Video_settings_menu.add_simple_command( "Video Quality"  );
	Video_settings_menu.add_simple_command( "Camera Settings"  );
	Video_settings_menu.add_callback_all_items( video_settings_menu_actions );
		
	// H MENU:
	m_main_menu.add_sub_menu  ( "Edit", 	&Video_edit_menu );
	m_main_menu.add_sub_menu  ( "View", 	&Video_view_menu );
	m_main_menu.add_sub_menu  ( "Settings", &Video_settings_menu );	
}

void VideoApp::setup_sidebar(	)
{
	MainDisplay.m_side.unload_controls();
	
	IconView* tmp = new IconView();
	tmp->set_file( "/home/pi/bk_code/avisual/resources/play.jpg" );
	tmp->set_width_height( 48., 48.);

	tmp = new IconView();
	tmp->set_file( "/home/pi/bk_code/avisual/resources/stop.jpg" );
	tmp->set_width_height( 48., 48.);
	
	tmp->load_resources( );
	MainDisplay.m_side.add_control( tmp, "Refresh" );
}

void 	VideoApp::register_with_display_manager() 
{ 
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( m_main_window );
	MainDisplay.m_status.set_text("Video App");
	MainDisplay.set_menu  	( &m_main_menu );
}	

void		VideoApp::About			(	) 
{ Application::About();
}
void		VideoApp::Preferences	(	)
{ Application::Preferences();
}

void	VideoApp::cut				()
 { }
void	VideoApp::copy				() 
{ }
void	VideoApp::paste				() 
{ }
void	VideoApp::snap_shot			() 
{ }

// View Menu commands - (here for now)
void	VideoApp::camera_list		() 
{ }
void	VideoApp::local_camera		() 
{ 
	//m_main_window = m_msg_view;
		// This will be better as a tab control page probably.
	register_with_display_manager();			

}
void	VideoApp::remote_camera		() 
{
	//m_main_window = m_msg_view;
		// This will be better as a tab control page probably.
	register_with_display_manager();			
 
}
void	VideoApp::movie				() 
{ }
void	VideoApp::Incoming			() 
{ }	
void	VideoApp::YouTube			() 
{ }				

// Settings Menu commands - (here for now)
void	VideoApp::view_size			() 
{ }		
void	VideoApp::video_quality		() 
{ }	
void	VideoApp::camera_settings	() 
{ }



void	VideoApp::file_new		( )
{ 
}
void	VideoApp::file_open		( ) 
{ 
}	
void	VideoApp::file_open_recent( ) 
{ 
}
	
void	VideoApp::file_save		( ) 
{ 
}
void	VideoApp::file_save_as	( ) 
{ 
}
