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
#include "draw_app.hpp"
#include "draw_app2.hpp"
#include "CAN_base.h"
#include "robot_app.hpp"
#include "robot_vision.hpp"
#include "client_memory.hpp"
#include "bk_system_defs.h"


RobotApp* robot_app=NULL;

#define Debug 1


void init_robot_app() 
{
	if (robot_app==NULL)
		robot_app = new RobotApp( );
	MainDisplay.start_app( robot_app );
}

RobotApp::RobotApp ()
{
	Initialize();
}
RobotApp::RobotApp ( Rectangle* mRect )
{
	Initialize(); 
}
RobotApp::~RobotApp()
{ 
	robot_app=NULL;
}

void RobotApp::StartSequence(	)
{
	// SEND ENABLES : 
	//int result = Req_CAN( mBuffer, mPin, mHigh );		
	//SendTelegram(mBuffer, mSize );

	// SEND ALL POSES : 
	//for ( )
	{
		//int result = pack_Pose( BYTE* mBuffer, struct stBodyPosition bp, boolean mHigh );		
		//SendTelegram( BYTE* mBuffer, int mSize );
	}
}
void RobotApp::Initialize		(	)
{ 
	printf("RobotApp:: Initialize() \n");
	m_application_name = "Robot";
	m_welcome_status   = "Robot Control Panel";
	Application::Initialize();

	robot_panel 	  = new RobotPanel();
	robot_diagnostics = new RobotDiagnosticsPanel();	
	robot_performance = new RobotPerformancePanel();
	joint_positions   = new AnalogView ();	
	robot_vision 	  = new RobotVisionPanel();	
	m_main_window 	  = (Control*) robot_panel;

	// gets done in Application::register_with_display_manager() now.
	//		MainDisplay.set_main_window(m_main_window);

	// Connect to the Vision RPI:
}

VerticalMenu view_menu;

int robot_view_menu_callback(void* menuPtr, int mMenuIndex, RobotApp* mApp )
{
	printf("menu_callback( %4x, %d )\n", menuPtr, mMenuIndex );
	switch( mMenuIndex )
	{
	case 0:	// Dwelling
			break; 
	case 1:	// Control Panel 
			MainDisplay.set_main_window( (Control*) mApp->robot_panel );
			break;	
	case 2: // Robot 
			MainDisplay.set_main_window( (Control*) mApp->joint_positions );
			break;
	case 3:	// Diagnostics
			MainDisplay.set_main_window( (Control*) mApp->robot_diagnostics );
			break;
	case 4:	// Performance
			MainDisplay.set_main_window( (Control*) mApp->robot_performance );
			break;
	case 5:	// History
			MainDisplay.set_main_window( (Control*) mApp->robot_vision );
			break;
	case 6:	// Vision
			MainDisplay.set_main_window( (Control*) mApp->robot_vision );
			break;
			
	default:	return 0;
			break;
	}
	return 1;
}

void RobotApp::setup_main_menu  ( )
{ 
	Application::setup_main_menu();
	view_menu.clear_all();
	view_menu.add_simple_command( "Dwelling" 	  );
	view_menu.add_simple_command( "Control Panel" );	
	view_menu.add_simple_command( "Robot" 		  );
	view_menu.add_simple_command( "Diagnostics"   );
	view_menu.add_simple_command( "Performance"   );
	view_menu.add_simple_command( "History" 	  );
	view_menu.add_simple_command( "Vision Analyzer" );					
	view_menu.add_callback_all_items( robot_view_menu_callback  );	
	m_main_menu.add_sub_menu    ( "View", &view_menu );	
}

void RobotApp::register_with_display_manager()
{ 
	Application::register_with_display_manager();
}	
	
void	RobotApp::About			(	)
{ 
	Application::About();
}
void	RobotApp::Preferences		(	)
{
	Application::Preferences();
}
int RobotApp::Quit			(	)
{ 
	return 1;
}	

void RobotApp::file_new		( )
{ 
}
void RobotApp::file_open		( )
{ 
}	
void RobotApp::file_open_recent( )
{ 
}	
void RobotApp::file_save		( )
{ 
}
void RobotApp::file_save_as	( )
{ 
}
void RobotApp::show_grid	( )
{ 
}

void RobotApp::zoom_in		()
{ 
}
void RobotApp::zoom_out		()
{ 
}

