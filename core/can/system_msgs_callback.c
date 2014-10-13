#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_base.h"
//#include "can_instance.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "board_list.h"
#include "system_msgs_callback.h"


BOOL callback_board_presence( struct sCAN* mMsg )
{
	//printf("Rxd callback_board_presence\n");
	int i;
	
	struct stBoardInfo* board;	
	switch (mMsg->id.group.id)
	{
	case ID_BOARD_PRESENCE_BROADCAST : 
		board = AddBoard( mMsg->id.group.instance, mMsg->data[0] );
		//print_board( board );
		//print_message( mMsg );
		return FALSE;
		break;
	case ID_BOARD_REVISION : 
		board = AddBoard( mMsg->id.group.instance, mMsg->data[0] );
		//print_message( mMsg );	
		//board = FindBoard( mMsg->id.group.instance, instance_finder );		
		board->HardwareRev 		= mMsg->data[0];
		board->SoftwareRevMajor = mMsg->data[1];
		board->SoftwareRevMinor = mMsg->data[2];
		board->Manufacturer		= (mMsg->data[3]<<8) | (mMsg->data[4]);
		return FALSE;
		break;
	case ID_BOARD_SERIAL_NUMBER : 
		board = AddBoard( mMsg->id.group.instance, mMsg->data[0] );	
		//print_message( mMsg );	
		//board = FindBoard( mMsg->id.group.instance, instance_finder );
		board->SerialNumber_lo  =  mMsg->data[3];
		board->SerialNumber_lo |= (mMsg->data[2]<<8);
		board->SerialNumber_hi |= (mMsg->data[1]);
		board->SerialNumber_hi |= (mMsg->data[0]<<8);
		return FALSE;
		break;
	case ID_BOARD_DESCRIPTION : 
		//board = FindBoard( mMsg->id.group.instance, instance_finder );		
		board = AddBoard( mMsg->id.group.instance, mMsg->data[0] );
		//print_message( mMsg );	

		for (i=0; i<mMsg->header.DLC; i++)
			printf("%c", mMsg->data[i] );
		return FALSE;
		break;
	default:
		return FALSE;
		break;
	}
}
