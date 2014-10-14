/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles the buffering of can messages.

There are 2 modes of operation:
A) History buffer (remembering previous n msgs sequentially)

B) ID Based buffer (filing msgs into mailboxes based on ID; remembers the last n distinct IDs received)
					
DATE 	:  10/16/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
//#include <avr/sfr_defs.h>
#include <string.h>
#include "inttypes.h"
#include "bk_system_defs.h"
#include "can_eid.h"
#include "can.h"
#include "can_id_list.h"
#include "can_instance.h"
#include "can_buff.h"


byte OperationalMode = NORMAL_HISTORY_OP_MODE;
//byte OperationalMode = MAILBOX_HISTORY_OP_MODE;

struct sCAN Transmitting[MAX_CAN_MSG_MEMORY_SIZE];
struct sCAN Received[MAX_CAN_MSG_MEMORY_SIZE];

byte RxHead = 0;
byte RXTail = 0;

char FindMailBox( tID mID )
{
	int i;
	for (i=0; i<MAX_CAN_MSG_MEMORY_SIZE; i++)
		if ( id_match(mID, Received[i].id) )
			return i;
	return -1;
}

void copy_can_msg( struct sCAN* mDest, struct sCAN* mSrc )
{
	memcpy( (void*)mDest, (void*)mSrc, sizeof(struct sCAN) );	
}

void QueueMessage( struct sCAN* mMsg )
{
	switch(OperationalMode)
	{
	case NORMAL_HISTORY_OP_MODE  :  addNormalHistory( mMsg );		break;
	case MAILBOX_HISTORY_OP_MODE :  addMailboxHistory( mMsg );		break;
	default : break;
	}
}

void addNormalHistory( struct sCAN* mMsg )
{
	copy_can_msg( &(Received[RxHead]), mMsg );	
	RxHead++;
	if (RxHead>=MAX_CAN_MSG_MEMORY_SIZE)
		RxHead = 0;
}

void addMailboxHistory( struct sCAN* mMsg )
{
	char index = FindMailBox( mMsg->id );
	if (index == -1)
	{
		//Text_Out("NewMailBox");
		copy_can_msg( &(Received[RxHead]), mMsg );
		RxHead++;
		if (RxHead>=MAX_CAN_MSG_MEMORY_SIZE)
			RxHead = 0;
	}
	else {
		copy_can_msg( &(Received[index]), mMsg );	
		//	Text_Out("MailBox="); lcd_draw_byte( index );
	}
}


struct sCAN* GetMessagePtr( byte mIndex )
{
	return &(Received[mIndex]);
}




