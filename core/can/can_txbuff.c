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
#include "CAN_Interface.h"
//#include "can.h"
#include "can_id_list.h"
#include "can_instance.h"
#include "can_buff.h"
#include "can_txbuff.h"
#include "interrupt.h"


struct sCAN Transmitting[MAX_CAN_TXMSG_MEMORY_SIZE];
byte TxHead = 0;
byte TxTail = 0;
byte TransmissionInProgress = FALSE;

byte calls = 0;
BOOL AddToSendList( struct sCAN* mMsg )
{
	BOOL ok = FALSE;
	calls++;
	
	copy_can_msg( &(Transmitting[TxHead]), mMsg );
	TxHead++;
	if (TxHead >= MAX_CAN_TXMSG_MEMORY_SIZE)
		TxHead = 0;

	//printf("AddToSendList: TxHead=%d;  TxTail=%d\n", TxHead, TxTail );
	// if (buffer_is_available)
	if (TransmissionInProgress == FALSE)
	{
		ReadyToSendAnother = TRUE;
		//if (ok==FALSE)  printf("something wrong with the tx queue!\n");
	}
//	else printf(" Tx in progress: %d/%d \n", TxTail, TxHead );
}


// called by ISR()
BOOL SendNext()
{
	//printf("SendNext t=%d; h=%d;\n",TxTail, TxHead );
	
	ReadyToSendAnother = FALSE;
	if (TxTail == TxHead) 
	{		
		TxTail=0; TxHead=0;
		return FALSE;		// no more to send
	} else { 		
		TransmissionInProgress = TRUE;
		//printf(">>SendNext t=%d; h=%d;\n", TxTail, TxHead);
		//print_message(  &(Transmitting[TxTail])  );
		
		byte buff_num = send_message( &(Transmitting[TxTail]) );
		TxTail++;
		Request_To_Send( buff_num ); 	
		// have to finish the CANISR before calling this 2nd time!!! stuck in ISR with 
		// no return!		
		// solve by sending next 
	}
	//printf("<<SendNext t=%d; h=%d;\n",TxTail, TxHead);
	return TRUE;
}

struct sCAN* GetTxMessagePtr( byte mIndex )
{
	return &(Transmitting[mIndex]);
}


void can_tx_timeslice()
{
	if ((TxHead > TxTail) && (TransmissionInProgress == FALSE))
	{
		SendNext();
	}
	
}