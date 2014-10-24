/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles can messages for board identification 
			( model/type, hardware/software version, serial number, etc)
and 
		System wide LED Indication messages
			Some could use to make visually appeasing LED display on the robot stack.

The complete 29 bit message consists of:	
Bits	Name			Description
-----	----			---------------------------
5 		Block			(for remapping messages)
16 		MESSAGE_ID
8		INSTANCE_ID 	because there may be more than 1 button board put into the network
------------------------
INCOMING MESSAGES:
	ID_BOARD_PRESENCE_REQUEST		0x0010		(0 Instance)
	ID_SYSTEM_LED_REQUEST						(MyInstance specific)

OUTGOING MESSAGES:
	ID_BOARD_PRESENCE_BROADCAST		0x0011		
	ID_BOARD_REVISION				0x0012		(MyInstance tagged)
	ID_BOARD_SERIAL_NUMBER			0x0013		(MyInstance tagged)
	ID_BOARD_DESCRIPTION			0x0014		(MyInstance tagged)
--------------------------------------------
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <string.h>
#include "bk_system_defs.h"
#include "can_id_list.h"
#include "CAN_base.h"
#include "can_txbuff.h"
#include "can_eid.h"
#include "can_board_msg.h"
#include "leds.h"


/**************** TYPE DEFS *************************************/
extern byte MyInstance;

/**************** VARIABLES *************************************/
// DESCRIPTION:
byte BoardStatus 		= 0x00;			// Need to update this from DTCLogger class.
byte MappedBlock 		= 0x00;
byte Starting_Index 	= 0;			// ?
byte System_Send_Status = 0;			// Use is OS_Dispatch() to send the requested message.
byte ShutDownState 		= NO_SHUTDOWN_PENDING;


const char BoardDescription[] ="Drives DC motor";
//////////////////////////////////////////////////////////////////////////////
////////////////////// INCOMING MESSAGES /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/************************************************************
INCOMING CAN MESSAGE Callback : 
	Call back function from the CAN ISR().  

This function is primarily used in the atmel code.  However Adrenaline network
(non-automotive RPI may wish to use too).

 Since each board has a unique 8 bit identifier (Instance number)
   The last 8 bits of the LED message identifiers below will 
   specify the board which board the LED is intended for.  Since this
   filtering will likely be in hardware, we don't need to check this
   in the CAN software callback function.
************************************************************/
BOOL can_board_msg_responder( struct sCAN* mMsg )
{
	if ( id_match( mMsg->id, create_CAN_eid			(ID_SYSTEM_LED_REQUEST, 0)) )
	{
		can_parse_led_request( mMsg );				// in leds.c
		return TRUE;
	}
	else if ( id_match( mMsg->id, create_CAN_eid	(ID_BOARD_PRESENCE_REQUEST, 0)) )
	{
		can_parse_board_presence_request( mMsg );
		return TRUE;
	}
	else if ( match( mMsg->id, create_CAN_eid		(ID_SYSTEM_CONFIGURE, MyInstance)) )
	{
		can_parse_configure_request( mMsg );		// in configuration.c
		return TRUE;
	}
	else if ( id_match( mMsg->id, create_CAN_eid	(ID_SYSTEM_SHUT_DOWN, 0)) )
	{
		ShutDownState = SHUTDOWN_PENDING;
		return TRUE;
	}
	// last address claim condition moved to can_instance.cpp	
	return FALSE;
}


/***************************************************************
 A system LED request message is intended for this board.  
 ID_SYSTEM_LED_REQUEST
 
INPUT:
	data[0] Upper nibble contains the desired action [0,1]
			0 = release system control of the led indicators (to whatever functionality the specific module uses them for)
			1 = override specific purpose to show the system requested pattern.
	data[0] Lower nibble
			contains the 4 bit pattern.  If the board only has 3 leds, then they 
			will use the lowest 3 bits of this nibble.
**************************************************************/
void can_parse_board_presence_request( ssCAN* mMsg )
{
	System_Send_Status=0;
	switch(mMsg->data[0])
	{
	case 0 : can_prep_board_presence_msg	( &msg1 );		System_Send_Status=1;	break;
	case 1 : can_prep_board_revision_msg	( &msg1 );		System_Send_Status=2;	break;
	case 2 : can_prep_serial_number_msg 	( &msg1 );		System_Send_Status=3;	break;
	case 3 : can_prep_board_description_msg	( &msg1 );		System_Send_Status=4;	break;
	default: break;
	}
}

/* Sends the queued message */
void can_board_timeslice_100ms()
{
	if (System_Send_Status) {
		AddToSendList( &msg1 );
		System_Send_Status=0;
	}
}
//////////////////////////////////////////////////////////////////////////////
////////////////////// OUT GOING MESSAGES ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/***************************************************************************
Board Presence Identifies it's board Model, Block, Instance, & Status (1 byte) 
 The blocks may be moved around to prevent conflicting CAN ids.  This "auto-
 arrangement" scheme will be worked out later.

The Model should be #defined in the "pin_definitions.h"
****************************************************************************/
void can_prep_board_presence_msg( ssCAN* mMsg )
{
	mMsg->id  = create_CAN_eid( ID_BOARD_PRESENCE_BROADCAST, MyInstance );
    mMsg->data[0] = BOARD_MODEL;
    mMsg->data[1] = MyInstance;
    mMsg->data[2] = BoardStatus;
    mMsg->data[3] = MappedBlock;
    mMsg->header.DLC	= 4;
    mMsg->header.rtr    = 0;
}

void can_prep_board_revision_msg( ssCAN* mMsg )
{
	mMsg->id  = create_CAN_eid( ID_BOARD_REVISION, MyInstance );
    mMsg->data[0] = HARDWARE_REVISION;
    mMsg->data[1] = SOFTWARE_MAJOR;
    mMsg->data[2] = SOFTWARE_MINOR;
    mMsg->data[3] = lo(MANUFACTURER);
    mMsg->data[4] = hi(MANUFACTURER);
    mMsg->header.DLC 	= 5;
    mMsg->header.rtr    = 0;
}

void can_prep_serial_number_msg( ssCAN* mMsg )
{
	mMsg->id      = create_CAN_eid( ID_BOARD_SERIAL_NUMBER, MyInstance );
    mMsg->data[0] = (SerialNumber & 0xFF000000) >> 24;	// MSB first
    mMsg->data[1] = (SerialNumber & 0x00FF0000) >> 16; 
    mMsg->data[2] = (SerialNumber & 0x0000FF00) >>  8; 
    mMsg->data[3] = (SerialNumber & 0x000000FF) >>  0;	// LSB 
    mMsg->header.DLC = 4;
    mMsg->header.rtr = 0;
}

void can_prep_board_description_msg( ssCAN* mMsg )
{
	int i;
	byte chars_sent = 0;
	mMsg->id  = create_CAN_eid( ID_BOARD_DESCRIPTION, MyInstance );
    mMsg->data[0] = Starting_Index;
    byte length = min(8, strlen(BoardDescription));
    
   	for (i=0; i<length; i++, chars_sent++)
	    mMsg->data[i+1] = *(BoardDescription+Starting_Index+i);

    mMsg->header.DLC = chars_sent+1;
    mMsg->header.rtr = 0;
}



