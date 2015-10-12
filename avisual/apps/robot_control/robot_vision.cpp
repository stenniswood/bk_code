
#include <shapes.h>
#include <fontinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"

#include "dataset.hpp"
#include "display.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "display_manager.hpp"
#include "client_memory.hpp"
#include "client_list_control.hpp"
#include "robot_vision.hpp"


#define Debug 1



RobotVisionPanel::RobotVisionPanel()
{
	Initialize();
}
void 	RobotVisionPanel::Initialize  ( )
{ 
	Window::Initialize();
}
void	RobotVisionPanel::calc_metrics( )
{
	Window::calc_metrics(); 
}
int		RobotVisionPanel::place_views ( )
{
	return 1; 
}

void	RobotVisionPanel::setup_headings( )
{ 
}

int		RobotVisionPanel::onCreate	(  )
{
	place_views();
	

	return Control::onCreate();
	
}	// chance to load resources, call functions which use fonts
int   	RobotVisionPanel::draw		(  )
{
	return Window::draw(); 
}


