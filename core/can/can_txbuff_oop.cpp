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
#include <stdio.h>
#include <string.h>
#include "inttypes.h"
#include "bk_system_defs.h" 
#include "can_eid.h"
#include "CAN_Interface.hpp"
//#include "can.h"
#include "can_id_list.h"
#include "can_instance.hpp"
#include "can_rxbuff.hpp"
#include "can_txbuff.hpp"
//#include "interrupt.h"

uint8_t CANTxMessageQueued = 0; 


byte		export_message[MAX_CAN_TXMSG_MEMORY_SIZE];	// if the msg was imported then don't re-export over wifi!
struct sCAN Transmitting[MAX_CAN_TXMSG_MEMORY_SIZE];
byte TxHead = 0;
byte TxTail = 0;
byte TransmissionInProgress = FALSE;
byte calls = 0;

BOOL AddToSendList( struct sCAN* mMsg, byte export_msg )
{
	BOOL ok = FALSE;
	CANTxMessageQueued++;

	copy_can_msg( &(Transmitting[TxHead]), mMsg );
	export_message[TxHead] = export_msg;
	TxHead++;
	if (TxHead >= MAX_CAN_TXMSG_MEMORY_SIZE)
		TxHead = 0;

	//printf("AddToSendList: TxHead=%d;  TxTail=%d\n", TxHead, TxTail );
	//print_message(mMsg);
	if (TransmissionInProgress == FALSE)
	{
		ReadyToSendAnother = TRUE;
	}
	else printf(" Tx in progress: %d/%d \n", TxHead, calls );
}

// called by ISR()
BOOL SendNext()
{
	//printf("SendNext t=%d; h=%d;\n",TxTail, TxHead);	
	if (TxTail == TxHead) {
		if (TxTail>0) printf("Head=Tail=0\n");
		TxTail=0; TxHead=0;
		return FALSE;		// no more to send
	} else {
		//printf("b\n");
		//printf(">>SendNext t=%d; h=%d;\n", TxTail, TxHead);
		TransmissionInProgress = TRUE;
		ReadyToSendAnother     = FALSE;
		CANTxMessageQueued--;
		print_message( &(Transmitting[TxTail]) );
		byte buff_num = send_message( &(Transmitting[TxTail]) );
		TxTail++;
		Request_To_Send( buff_num ); // have to finish the CANISR before calling this 2nd time!!! stuck in ISR with no return!
		// solve by sending next 
	}
	//printf("<<SendNext t=%d; h=%d;\n",TxTail, TxHead);
	return TRUE;
}

struct sCAN* GetTxMessagePtr( byte mIndex )
{
	if (mIndex==255) 
	{	
		if (TxTail == TxHead)
		{
			TxTail=0; TxHead=0;
			return NULL;		// no more to send
		}
		return &(Transmitting[TxTail]);
	}
	return &(Transmitting[mIndex]);
}




