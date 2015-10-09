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
#include "robot_control_panel.hpp"
#include "robot_diagnostics.hpp"
#include "robot_performance.hpp"


RobotApp* robot_app=NULL;


RobotPanel* 			robot_panel;
RobotDiagnosticsPanel* 	robot_diagnostics;
RobotPerformancePanel*	robot_performance;
//robot_vision_summary

void init_robot_app() 
{
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
}

void RobotApp::Initialize		(	)
{ 
	printf("RobotApp:: Initialize() \n");
	
	Application::Initialize();

	robot_panel 	  = new RobotPanel();
	robot_diagnostics = new RobotDiagnosticsPanel();	
	robot_performance = new RobotPerformancePanel();	
	m_main_window = (Control*) robot_panel;
	
	m_application_name = "Robot";
	
	setup_app_menu();		// About, Preferences, quit, 
	setup_main_menu();		// 
	//onPlace();	
}	// create all the objects here.

int	RobotApp::onPlace			(	)
{ 
	Application::onPlace();
}

VerticalMenu view_menu;
int robot_view_menu_callback(void* menuPtr, int mMenuIndex, Application* mApp )
{
	printf("menu_callback( %4x, %d )\n", menuPtr, mMenuIndex );
	switch( mMenuIndex )
	{
	case 0:	
			break;
	case 1:	// show 
				mApp->m_main_window = (Control*) robot_panel;
				mApp->register_with_display_manager();	
			break;	
	case 2: 
			break;
	case 3:		mApp->m_main_window = (Control*) robot_diagnostics;
				mApp->register_with_display_manager();	
			break;
	case 4:		mApp->m_main_window = (Control*) robot_performance;
				mApp->register_with_display_manager();	
			break;
	case 5:	//	mApp->m_main_window = (Control*) robot_vision_summary;
			break;
			
	default:
			break;
	}
	
}

void RobotApp::setup_main_menu  ( )
{ 
	Application::setup_main_menu();
	view_menu.add_simple_command( "Dwelling" 	  );
	view_menu.add_simple_command( "Control Panel" );	
	view_menu.add_simple_command( "Robot" 		  );
	view_menu.add_simple_command( "Diagnostics"   );
	view_menu.add_simple_command( "Performance"   );
	view_menu.add_simple_command( "History" 	  );
	view_menu.add_simple_command( "Vision History" );					
	view_menu.add_callback_all_items( robot_view_menu_callback  );	
	m_main_menu.add_sub_menu    ( "View", &view_menu );	
}
void RobotApp::register_with_display_manager()
{ 
	Application::register_with_display_manager();
}	
	
int	RobotApp::About			(	)
{ 
}
int	RobotApp::Preferences		(	)
{ 
}
int RobotApp::Quit			(	)
{ 

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

void RobotApp::show_grid		( )
{ 
}

void RobotApp::zoom_in			()
{ 
}
void RobotApp::zoom_out		()
{ 
}

