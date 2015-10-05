// Steve Tenniswood
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
#include "robot_control_panel.hpp"


#define Debug 1

/* Are the coordinates relative to the Window we are adding them to,
	or absolute to the display, or something else?
	The fact that I have to ask this is not too good.	
*/
Button   	Stop   ( 20, 200, 400, 300 );
Button   	Go	   ( 20, 200, 250, 150 );
ListBox  	SequenceSource( 250, 450, 400, 220  );
ListBox  	SequenceList  ( 500, 700, 400, 60  );
Button   	Add	   		  ( 400, 480, 200, 150 );


void stop_cb( void* mRobotControl )
{
	if (Debug) printf("Stop button pushed\n");
}

void go_cb( void* mRobotControl )
{
	if (Debug) printf("Go button pushed\n");
	
}

void add_cb( void* mRobotControl )
{
	if (Debug) printf("Add button pushed\n");
		
}

/* ======================================================= */

/* ======================================================= */
RobotPanel::RobotPanel()
{
	Initialize();
}
void RobotPanel::Initialize(	)
{
	Window::Initialize();
}
int	RobotPanel::calc_metrics()
{
	
}
int	RobotPanel::place_views()
{	
	// Link the Dataseries
	Stop.set_text("Stop");
	Stop.set_on_click_listener( stop_cb, this );

	Go.set_text	( "Go!" );		
	Go.set_on_click_listener( go_cb, this );
	
	Add.set_text("Add ->");
	Add.set_on_click_listener( add_cb, this );

	SequenceSource.add_item( "left hip swing"  );	
	SequenceSource.add_item( "right hip swing" );
	SequenceSource.add_item( "left leg swing"  );	
	SequenceSource.add_item( "right leg swing" );
	SequenceSource.add_item( "Both legs" 	   );	
	// 
	SequenceSource.add_item( "boys bedroom" );
	SequenceSource.add_item( "back bedroom" );
	SequenceSource.add_item( "kitchen" 		);
	SequenceSource.add_item( "dining room"  );
	SequenceSource.add_item( "living room"  );
	SequenceSource.add_item( "family room"  );
	SequenceSource.add_item( "den room" 	);

	add_control( &Stop     		 );
	add_control( &Go     		 );
	add_control( &Add     		 );
	add_control( &SequenceSource );	
	add_control( &SequenceList   ); 

	printf("====== Robot CONTROL POSITIONS AFTER ===\n");
	Stop.print_positions();
	Go.print_positions ();
	Add.print_positions();
}
//	void			file_new();	 
int	RobotPanel::onCreate	  (  )	// chance to load resources, call functions which use fonts
{
	Window::onCreate();
	place_views();
}

int RobotPanel::draw(	)
{
	return Window::draw();	
}

int	RobotPanel::get_hit_index( int Mousex, int Mousey )
{

}



