/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles the buffering of can messages.

	OOOPPPPSS! This was already done in can_buff.c
	
There are 2 modes of operation:
A) History buffer (remembering previous n msgs sequentially)

B) ID Based buffer (filing msgs into mailboxes based on ID; remembers the last n distinct IDs received)
					
DATE 	:  10/16/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdio.h>
#include <string.h>
#include "inttypes.h"
#include "bk_system_defs.h" 
#include "can_eid.h"
#include "CAN_Interface.h"
#include "can_id_list.h"
#include "can_instance.h"
#include "can_buff.h"
#include "can_txbuff.h"
#include "interrupt.h"


struct sCAN Received[MAX_CAN_TXMSG_MEMORY_SIZE];

byte RxHead = 0;
byte RxTail = 0;
byte TransmissionInProgress = FALSE;

BOOL AddToRxList( struct sCAN* mMsg )
{	
	copy_can_msg( &(Received[RxHead]), mMsg );
	RxHead++;
	if (RxHead >= MAX_CAN_TXMSG_MEMORY_SIZE)
		RxHead = 0;

	//printf("AddToRxList: RxHead=%d;  RxTail=%d\n", RxHead, RxTail );
	return TRUE;
}

BOOL RxMessageAvailable()
{
	return (RxHead > RxTail);
}

struct sCAN* GetNext()
{
	return &(Received[RxTail++]);
}

