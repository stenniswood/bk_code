// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "bk_system_defs.h"
#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_txbuff.h"
#include "packer.h"
#include "board_list_oop.hpp"
#include "can_id_list.h"
#include "vector_math.h"
#include "cmd_process.h"
#include "parser_tilt.h"
#include "buttons.h"
#include "fuse_ag.h"
#include "fuse_accel.h"
#include "fuse_magnet.h"
#include "fuse_gyro.h"
#include "fuse_log.h"
#include "dataset.hpp"
#include "leveler.hpp"
#include "display.h"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "histogram.hpp"
#include "window_layouts.hpp"
#include "callbacks.hpp"
#include "listbox.hpp"
//#include "robot.hpp"
#include "button_array_gui.hpp"


BOOL ShowAccelerometerData 	= TRUE;
BOOL ShowGyroData 			= TRUE;
BOOL ShowMagnetData 		= TRUE;
BOOL ShowCANData 			= TRUE;

int DisplayNum	 = 1;

int count_accel  = 0;
int count_gyro   = 0;
int count_magnet = 0;
int count_samples= 0;
int UpdateDisplaySemaphore=0;

//BoardList 			bl;
extern ListBox  	MyBoardList;

/************************* WATCH OUT **************************
For re-entrant code.  These routines occur during the CAN_isr()
Maybe this should not be.  No it shouldn't be
There should be 2 call back types.  
	1) which is during the CAN_isr()
	   but only very quick things done there!
	2) the isr sets appropriate flag and does the call back
		in the background tasks.  This also requires a can_rxbuff.c
		because more than 1 msg can come in during the isr.  so
		we should just queue them up and process in background.
***************************************************************/
BOOL callback_board_presence( struct sCAN* mMsg )
{
	// This is all within board_list_oop.cpp now.
	// And that object is held by robot.cpp.  The CAN message routing
	// goes thru robot to board_list_oop, so this call back is no longer needed!
}

// This function delegates out to various sub callbacks.
// based upon the CAN Msg ID
BOOL callback_main( struct sCAN* mMsg )
{
	//printf("Main Callback!\n");
	//print_message(mMsg);
	BOOL retval = FALSE;
	//Onesimus.distribute_CAN_msg( mMsg );	
	if (retval==FALSE)
		retval = motor_report_callback( mMsg );		// updates Gui
	if (retval==FALSE)
		retval = button_board_callback( mMsg );
	return retval;
}

BOOL motor_report_callback( struct sCAN* mMsg )
{
	int 	result;
	int 	handled = FALSE;
	float 	angles[3];

	if ((mMsg->id.group.id==ID_MOTOR_ANGLE) ||
	    (mMsg->id.group.id==ID_MOTOR_VALUE))
	{
		// Now Want to put updated Leg info into graphical controls:
		word val1 = 0; 
		word val2 = 0; 
		word val3 = 0; 
		word val4 = 0; 
		word val5 = 0; 
		word val6 = 0; 
		//printf("cb-RightLeg Level 1-3:%d,%d,%d\n", val1, val2, val3 );
/*		ds1.shift_add( val1 );
		ds2.shift_add( val2 );
		ds3.shift_add( val3 );*/

		l1.set_level ( val1 );
		l2.set_level ( val2 );
		l3.set_level ( val3 );
		l4.set_level ( val4 );
		l5.set_level ( val5 );
		l6.set_level ( val6 );

		UpdateDisplaySemaphore = 1;
		return TRUE;
	}		
	return FALSE;	
}

void init_display();

int can_parse_button_pressed( struct sCAN* mMsg, int* Keys )
{
	int NumButtonPushed = mMsg->data[0];
	for (int i=0; i<NumButtonPushed; i++)
	  Keys[i] = mMsg->data[i+1];
	return NumButtonPushed;	
}

BOOL button_board_callback( struct sCAN* mMsg )
{
	int numPressed=0;
	int Keys[7];
	BOOL retval = FALSE;
	int  result;
	switch (mMsg->id.group.id)
	{
	case ID_BUTTON_PRESSED :		
			
		UpdateDisplaySemaphore = 1;		
		break;	
	case ID_BUTTON_ROSTER :
		//result = can_parse_button_roster( mMsg,  );
		break;
	default: break;
	}
	return FALSE;
}


BOOL callback_tilt_reading( struct sCAN* mMsg )
{
	byte 	result;
	byte 	junk_count=0;

	BOOL retval = FALSE;
	switch (mMsg->id.group.id)
	{
/*	case ID_ACCEL_XYZ : 
		count_accel++;
		parse_accel_msg	(mMsg);		
		if (ShowAccelerometerData)				
			print_raw( mMsg, &RawxyzAccel, ShowCANData );
		process_accel	(ShowAccelerometerData);
		if (ShowAccelerometerData) 	printf("\n");

		// For Histogram reporting:		
		sf1.set_ab( AccelAngularPosition.rx );
		ds_tiltx.shift_add( AccelAngularPosition.rx );
		ds_tilty.shift_add( AccelAngularPosition.ry );		
		retval= TRUE;
		break;
	case ID_GYRO_XYZ : 
		count_gyro++;
		parse_gyro_msg	(mMsg); 
		if (ShowGyroData)	
			print_raw(mMsg, &RawxyzGyro, ShowCANData);
		process_gyro	(ShowGyroData);		
		if (ShowGyroData) 							printf("\n");
		retval=TRUE;
		break;
	case ID_MAGNET_XYZ : 
		count_magnet++;
		parse_magnet_msg(mMsg);
		if (ShowMagnetData)	
			print_raw(mMsg, &RawxyzMagnet, ShowCANData);
		process_magnet	(ShowMagnetData);
		if (ShowMagnetData) 						printf("\n");
		retval= TRUE;
		break; 	
	default:
		retval= FALSE;
		break;	*/
	}

	return retval;
}


BOOL callback_instance_catcher( struct sCAN* mMsg )
{
	BOOL retval = FALSE;
	switch (mMsg->id.group.instance)
	{
	case 71 : retval = TRUE; break;
	case 72 : retval = TRUE; break;
	default: break;
	}
	return retval;
}

