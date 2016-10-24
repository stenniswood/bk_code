
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
#include "robot_diagnostics.hpp"

#define Debug 1

TabularListBox	FaultList( 50, 780, 400, 100  );
Button   		Clear    ( 20, 100,  80,  40  );
Button   		ClearAll;

RobotDiagnosticsPanel::RobotDiagnosticsPanel()
{ 
	Initialize();
}
RobotDiagnosticsPanel::~RobotDiagnosticsPanel()
{

}
void RobotDiagnosticsPanel::Initialize(	)
{
	Window::Initialize();	
}

void RobotDiagnosticsPanel::setup_headings()
{
	static struct HeaderItemInfo hdr_info;
	if (FaultList.is_created()==false)
	{
		hdr_info.start_x  = 10.;
		hdr_info.end_x	  = 10.;
		hdr_info.text 	  = "Fault Code";
		hdr_info.width	  = -1;
		hdr_info.alignment= HEADER_ALIGN_LEFT;	// left,center,right
		FaultList.add_column( &hdr_info );

		hdr_info.start_x  = 10.;
		hdr_info.end_x	  = 10.;
		hdr_info.text 	  = "Severity";
		hdr_info.width	  = -1;
		hdr_info.alignment= HEADER_ALIGN_LEFT;	// left,center,right
		FaultList.add_column( &hdr_info );

		hdr_info.start_x  = 10.;
		hdr_info.end_x	  = 10.;
		hdr_info.text 	  = "Module";
		hdr_info.width	  = -1;
		hdr_info.alignment= HEADER_ALIGN_LEFT;	// left,center,right
		FaultList.add_column( &hdr_info );

		hdr_info.start_x  = 10.;
		hdr_info.end_x	  = 10.;
		hdr_info.text 	  = "Description";
		hdr_info.width	  = -1;
		hdr_info.alignment= HEADER_ALIGN_LEFT;	// left,center,right
		FaultList.add_column( &hdr_info );		
	}
}

void	RobotDiagnosticsPanel::calc_metrics()
{
 	Window::calc_metrics();
}

void	RobotDiagnosticsPanel::place_views()
{ 
	printf("RobotDiagnosticsPanel::place_views (  )");
	Clear.set_text("Clear Fault",true);
	ClearAll.set_text("Clear All",true);
	ClearAll.set_position_right_of( &Clear );

	add_control_local_coords( &Clear 	);
	add_control_local_coords( &ClearAll  );
	register_child( &FaultList );
}

int	RobotDiagnosticsPanel::onCreate	  (  )
{ 
 	setup_headings(); 	
 	place_views();

	printf("RobotDiagnosticsPanel::onCreate	  (  )");
	return Control::onCreate();
	
}	// chance to load resources, call functions which use fonts

int RobotDiagnosticsPanel::draw		 		(				)
{ 
	return Window::draw();
}
