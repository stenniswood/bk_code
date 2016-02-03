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


#define BUFFER_SIZE 6
#define Debug 1

/* Are the coordinates relative to the Window we are adding them to,
	or absolute to the display, or something else?
	The fact that I have to ask this is not too good.	
*/
Button   	Stop   ( 20, 200, 400, 300 );

ListBox  	SequenceSource( 350, 550, 400, 220 );
ListBox  	SequenceList  ( 600, 800, 400, 60  );
Button   	Add	   		  ( 450, 580, 200, 150 );
Button   	ClearList	  ( 450, 580, 140, 90  );
Button   	Go	   		  ( 325, 440, 140, 90  );

CheckBox	can_send	  ( 20, 110, 250, 200 );

CheckBox	Left_hip_rotate;
CheckBox	Left_hip;
CheckBox	Left_knee;
CheckBox	Left_ankle;

CheckBox	Right_hip_rotate;
CheckBox	Right_hip;
CheckBox	Right_knee;
CheckBox	Right_ankle;


/* ======================================================= */
//	Push Button Callbacks:
/* ======================================================= */
void stop_cb( void* mRobotControl )
{
	if (Debug) printf("Stop button pushed\n");
}

void go_cb( void* mRobotControl )
{
	RobotPanel* rp = (RobotPanel*) mRobotControl;	
	
	// package up the enables;
	static char data[BUFFER_SIZE];
	int length = rp->pack_enables(data);
	
	if (Debug) printf("Go button pushed\nEnable Buffer: ");
	for (int i=0; i<BUFFER_SIZE; i++)
		printf("%x ", data[i] );
	printf("\n");
	
}

void add_cb( void* mRobotControl )
{
	if (Debug) printf("Add button pushed\n");
	// get entry from source.
	int selected = 	SequenceSource.get_selection();
	string* move = SequenceSource.get_item( selected );

	// add entry to 
	SequenceList.add_item( move->c_str() );
}

void clear_cb( void* mRobotControl )
{
	if (Debug) printf("Clear button pushed\n");
	SequenceList.clear_items();
	
}

/* Callback for Send CAN check box! */
void send_can_messages( void* mCheckBox )
{
	if (Debug) printf("Send CAN toggled\n");

	bool clicked = ((CheckBox*)mCheckBox)->is_checked();
	char working_buffer[127];
		
	// toggle occurs after this callback, so !
	if (!clicked) {	
		// REQUEST CAN : 
		strcpy( working_buffer, "receive can" );
	} else {
		// STOP CAN : 
		strcpy( working_buffer, "stop can " );
	}


	printf("\n\nSend command: %s\n", working_buffer );
	cli_ipc_write_sentence( working_buffer );
	cli_wait_for_ack_update();	
}

/* ======================================================= */
//	Class Definition
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
 	Window::calc_metrics();
 	return 1;
}
int	RobotPanel::place_views()
{	
	// Link the Dataseries
	Stop.set_text("Stop");
	Stop.set_background_color( 0xFFAF2020 );
	Stop.set_on_click_listener( stop_cb, this );

	Go.set_text	( "Go!" );		
	Go.set_background_color( 0xFF20DF20 );
	Go.set_on_click_listener( go_cb, this );

	can_send.set_text			  ( "Send CAN", true );
	//can_send.set_background_color ( 0xFF20DF20  	 );
	can_send.set_on_click_listener( send_can_messages, &can_send );

	Add.set_text("Add ->");
	Add.set_on_click_listener( add_cb, this );

	ClearList.set_text				("Clear!");
	ClearList.set_on_click_listener ( clear_cb, this );
	
	Left_hip.set_text				("L Hip",true);		
	Left_knee.set_text				("L Knee",true);		
	Left_ankle.set_text				("L Ankle",true);	
	Left_hip.set_position_below    ( &can_send, true, 15.0 );
	Left_knee.set_position_below   ( &Left_hip  );
	Left_ankle.set_position_below  ( &Left_knee ); 

	Right_hip.set_text				("R Hip",true);		
	Right_knee.set_text				("R Knee",true);		
	Right_ankle.set_text			("R Ankle",true);	
	Right_hip.set_position_right_of ( &Left_hip  );
	Right_knee.set_position_below   ( &Right_hip ); 
	Right_ankle.set_position_below  ( &Right_knee );

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
	add_control( &can_send 		 );
	add_control( &ClearList		 );
	
	add_control( &Left_hip 		 );
	add_control( &Left_knee 	 );
	add_control( &Left_ankle 	 );	
	add_control( &Right_hip 	 );
	add_control( &Right_knee 	 );
	add_control( &Right_ankle 	 );

	print_children();
	printf("====== Robot CONTROL POSITIONS AFTER ===\n");
	return 1;
}

int	RobotPanel::onCreate	  (  )	// chance to load resources, call functions which use fonts
{
	place_views();
	return Window::onCreate();
}


// return is length
int	RobotPanel::pack_enables(char* mDataBuffer)
{
	//static char data[BUFFER_SIZE];
	for (int i=0; i<BUFFER_SIZE; i++)
		mDataBuffer[i] = 0;
		
	if (Left_hip.get_check())		mDataBuffer[0] = 1;
	if (Left_knee.get_check())		mDataBuffer[1] = 1;
	if (Left_ankle.get_check())		mDataBuffer[2] = 1;

	if (Right_hip.get_check())		mDataBuffer[3] = 1;
	if (Right_knee.get_check())		mDataBuffer[4] = 1;
	if (Right_ankle.get_check())	mDataBuffer[5] = 1;
	
	return BUFFER_SIZE;
}

int RobotPanel::draw(	)
{
	return Window::draw();	
}




