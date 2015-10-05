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


RobotApp* robot_app=NULL;


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

	RobotPanel* tmp = new RobotPanel();
	m_main_window = (Control*) tmp;

	m_application_name = "Robot";
	
	setup_app_menu();		// About, Preferences, quit, 
	setup_main_menu    ();		// 
	onPlace();	
}	// create all the objects here.

int	RobotApp::onPlace			(	)
{ 
	Application::onPlace();
}

VerticalMenu view_menu;
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
	m_main_menu.add_sub_menu    ( "View", &view_menu );
}
void RobotApp::register_with_display_manager()
{ 
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

