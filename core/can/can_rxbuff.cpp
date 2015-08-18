/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles the buffering of _received_ can messages.

There are 2 modes of operation:
A) History buffer (remembering previous n msgs sequentially)

B) ID Based buffer (filing msgs into mailboxes based on ID; remembers the last n distinct IDs received)
					
DATE 	:  10/16/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
//#include <avr/sfr_defs.h>
#include <stdio.h>
#include <string.h>
#include "inttypes.h"
#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_Interface.hpp"
#include "can_id_list.h"
#include "can_instance.hpp"
#include "can_rxbuff.hpp"


#define MAX_FILTER_IDS 100
tID FilterList[MAX_FILTER_IDS];
int FilterSize  	 = 0;
byte FilterMode 	 = REJECT_FILTER;
byte OperationalMode = NORMAL_HISTORY_OP_MODE;	//MAILBOX_HISTORY_OP_MODE;

struct sCAN Received[MAX_CAN_MSG_MEMORY_SIZE];
byte RxHead 		= 0;
byte RxTail 		= 0;


int	 GetRxMode()
{
	return OperationalMode;
}

void copy_can_msg( struct sCAN* mDest, struct sCAN* mSrc )
{
	memcpy( (void*)mDest, (void*)mSrc, sizeof(struct sCAN) );	
}

void AddToRxList( struct sCAN* mMsg )
{
	int in_filter = id_is_in_filter_list( mMsg );

	if ( (in_filter     && (FilterMode==PASS_FILTER)  ) ||
		((in_filter==0) && (FilterMode==REJECT_FILTER)))
	{					
		switch(OperationalMode)
		{
		case NORMAL_HISTORY_OP_MODE  :  addNormalHistory( mMsg );		break;
		case MAILBOX_HISTORY_OP_MODE :  addMailboxHistory( mMsg );		break;
		default : break;
		}
	}
}

/*	
*/
char FindMailBox( tID mID )
{
	int i;
	for (i=0; i<MAX_CAN_MSG_MEMORY_SIZE; i++)
		if ( id_match(mID, Received[i].id) )
			return i;
	return -1;
}

void addNormalHistory( struct sCAN* mMsg )
{
	//printf("CAN RX: h=%d; t=%d;\n", RxHead, RxTail );
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

BOOL isRxMessageAvailable()
{
	if (RxTail==RxHead)
		RxTail = RxHead = 0; 
	return (RxHead > RxTail);
}

struct sCAN* GetNextRxMsg()
{
//	if (RxTail==RxHead)
//		RxTail = RxHead = 0; 

	return &(Received[RxTail++]);
}

struct sCAN* GetMessagePtr( byte mIndex )
{
	return &(Received[mIndex]);
}

void  set_filter_mode( byte mMode)
{
	FilterMode = mMode;
	printf("Filter Mode = %d\n", mMode);
}

void add_id_to_filter( tID* mId )
{
	if (FilterSize < MAX_FILTER_IDS) {	
		memcpy( (void*)&(FilterList[FilterSize]), (void*)mId, sizeof(tID) );
		FilterSize++;				
	}	
}

/*
Return:
	1	- id is filtered out!
	0   - Use the id.  
*/
int	id_is_in_filter_list(struct sCAN* mMsg)
{
	int i=0; 
	for ( ; i<FilterSize; i++)
		if (mMsg->id.group.id == FilterList[i].group.id)
			return 1;
	return 0;
}

