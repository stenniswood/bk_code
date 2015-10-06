
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

TabularListBox	FaultList( 50, 800, 400, 200  );
Button   		Clear   ( 20, 200, 400, 300 );
Button   		ClearAll;

RobotDiagnosticsPanel::RobotDiagnosticsPanel()
{ 
	Initialize();
}

void RobotDiagnosticsPanel::Initialize(	)
{
 	setup_headings(); 	
}

void RobotDiagnosticsPanel::setup_headings()
{
	struct HeaderItemInfo hdr_info;
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

int	RobotDiagnosticsPanel::calc_metrics()
{ 
}
int	RobotDiagnosticsPanel::place_views()
{ 
}
int	RobotDiagnosticsPanel::onCreate	  (  )
{ 
	add_control( &Clear );
	add_control( &ClearAll );
	add_control( &FaultList );
}	// chance to load resources, call functions which use fonts

/*int		RobotDiagnosticsPanel::get_hit_index		( int Mousex, int Mousey )
{ 
	Window::
}*/

int RobotDiagnosticsPanel::draw		 		(				)
{ 
	Window::draw();
}
