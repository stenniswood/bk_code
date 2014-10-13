/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles can messages for board identification.

The complete 29 bit message consists of:	
Bits	Name			Description
-----	----			---------------------------
5 		Block			(for remapping messages)
16 		MESSAGE_ID
8		INSTANCE_ID 	because there may be more than 1 button board put into the network
--------------------------------------------
INCOMING MESSAGES:
	ID_INSTANCE_RESERVE_REQUEST
	ID_INSTANCE_QUERY

OUTGOING MESSAGES:
	ID_INSTANCE_RESERVE_REQUEST
	ID_INSTANCE_QUERY

--------------------------------------------
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdlib.h>
#include <inttypes.h>
#include "bk_system_defs.h"
#include "can_eid.h"
#include "can_id_list.h"
#include "CAN_Interface.h"
#include "can_instance.h"
#include "eeprom.h"

/* Strategy:
		We begin counting from 0 (or down from 255)
		1) Pick a random instance on startup 
		1.5) Send an ID_INSTANCE_RESERVE_REQUEST  Do this as a one-shot only.
		2) If TXOK goes, then save MyInstance into EEPROM
		3) On Error, set a timeout period and wait
		4) If we receive an ID_INSTANCE_RESERVE_REQUEST with less than our attempted id, 
				ignore it.
		5) If we receive an ID_INSTANCE_RESERVE_REQUEST >= MyInstance, then bump to incoming +1
		6) If x amount of time expires without receiving, then resend the MyInstance		
		Repeat

	Once the Instance ID is found for each board, this must be saved into EEPROM 
	so that future sessions can refer to the knee motor with the same id.
	Otherwise we'll have randomized boards and motor movements belonging to knee will go
	to the hip, etc.			
	
	ID Established.  Future Power ups will setup a MOB with auto-reply mode and an ID of 
	ID_INSTANCE_QUERY.  Each module will have an auto-reply MOB setup to be triggered.

	Then send an ID_INSTANCE_QUERY message.  	
	If there is a reply, then we need to re-initiate the assignment.  Or report a DTC trouble code
	
	Each time we add a new module to the network, we'd have to re-assign the hip, knee, and
	ankle motors.  YIKES!
	
	How about sending an instance query to 0x00, and within the reply, the "Confirmed" bit
	will be sent.  Any module which is not confirmed then can scan thru the ID_INSTANCE_QUERY
	until no more replies, and assume this identifier.	
*/

// NEEDS TO BE SAVED INTO EEPROM!
byte 	MyInstance = 0x00;
byte 	LastReceivedInstance = 0xFF;
byte 	Confirmed  = 0x00;				// MyInstance was approved on previous boot.	0x80 is dirty bit
byte 	init_complete = FALSE;
word 	rand_instance;
word 	rand_delay;					// 

/************************************************
adc_init()  - Used for Random Number generation
Need to work out sharing with "adc.c" module.  
Some platforms use adc.c and others don't
*************************************************/
/* Set up the ADC.  Needed for the current sense and pot	*/
// REFSn bits in ADMUX  (external AREF voltage not connected!)	
#include "adc.h"


void random_number_adc_init()
{
}


word pick_random_number()
{
	word result = rand();
	return result;
}


void tx_instance_callback()
{
	Confirmed = DIRTY;		// save to EEPROM on next timeslice; then => CLAIMED
}

/* Let's get to first base.  Obtain an instance number.
	!Confirmed, Obtains, Confirmed status.
*/
void can_instance_init()
{
	//srand(CANTIML);
	read_confirmed_status();
	Confirmed = NOT_CLAIMED;
	if (Confirmed==NOT_CLAIMED)
	{
		// To obtain an instance, we need a receive mob:
		// SETUP MOB3 for ID_INSTANCE_CLAIM (no Instance - checks done in software):
		can_setup_receive_mob		   ( INSTANCE_TX_MOB, 0x0000, 0x0000, 8 );
		can_remove_instance_from_filter( INSTANCE_TX_MOB 					);
		can_add_id_to_filter		   ( INSTANCE_TX_MOB, ID_INSTANCE_CLAIM, ID_INSTANCE_CLAIM );
		//set_tx_callback( tx_instance_callback );
		//ObtainInstanceNumber();	
		word tmp = 	pick_random_number();		// random time delay
		rand_instance = rand_delay = (tmp & 0xFF);
	} else {		
		read_instance_number();
/*		Add this later after above is working.
		can_prep_instance_query( &msg2);
		can_send_msg( 0, &msg2 );
		if (reply){
			// extract the largest id from the reply 
			// and do an instance claim on it.
			// if it has a reply (2 nodes bad in network), try it's reply largest value.
			// repeat until done.
		}	*/
	}
	//show_result_toggle( rand_instance );
	init_complete = TRUE;
}
byte timeout_10ms_mult = 0;

void can_instance_timeslice()
{
	byte restore=0;
	if (Confirmed==CLAIMED)	  return;		// nothing to do
	if (init_complete==FALSE) return;		//
	if (rand_delay-- > 0)	  return;		// wait until our appointed time to claim
	rand_delay = 1;							// so that it comes back in here next timeslice
	//show_result_toggle(rand_instance);

	if (Confirmed==NOT_CLAIMED)
	{
		Confirmed = CLAIM_PENDING;
		can_prep_instance_request( &msg2, rand_instance );
		can_send_msg_no_wait	 ( 0, &msg2 			);	// no wait here!
		timeout_10ms_mult = 200;	// 2 seconds
	}
	if (Confirmed==CLAIM_PENDING)
	{
		// Wait for either TXOK or RX ID_INSTANCE_CLAIM (aborts the TX)
		// OR a TIMEOUT

// select CAN module
// Put Dirty into the interrupt service routine!!

//		if (TXOK_FLAG)
			// We finished sending, so we claimed it.
			Confirmed = DIRTY;		// save on next timeslice
			// see "can_board_msg.c"  can_board_msg_responder(),
			//   if an incoming ID_INSTANCE_CLAIM comes in, it will
			//   abort transmission & update Confirmed.
			// It was aborted and MyInstance was bumped.
			// do nothing just wait for next timeslice.
			// Confirmed=NOT_CLAIMED;
		timeout_10ms_mult--;
		if (timeout_10ms_mult == 0)
			Confirmed = NOT_USING_INSTANCES;
// reenable interrupts.
	}

	if (Confirmed == DIRTY)			// Dirty bit set?
	{	
		Confirmed = CLAIMED;		// don't save next time, just skip the Claiming
		save_instance_number();		// yes, save
		save_confirmed_status();
		//show_byte( MyInstance, 0);
		
		// SEND 1 MORE FOR DEBUG PURPOSES:
		//can_prep_instance_request( &msg2, MyInstance );
		//can_send_msg			 ( 0, &msg2    );
	}
	if (Confirmed == CLEAR_REQUEST)	
	{
		Confirmed = NOT_CLAIMED;
		save_confirmed_status();
	}
}

				
/***************************************************************
The ID_RESERVE_INSTANCE_REQUEST msg uses a random number for its 
"instance" since the instance numbers have not been established 
for the boards yet - the can priority mechanism would have no preference
if they each used sequential instance ids.  ie we would have multiple
boards claiming "i want to request instance 0".  With a random number
each board has a priority which can be easily resolved.  Once the 
requests are granted, from then on priority is not a problem if it's sequential
The first data member of the msg contains the id we wish to reserve.

***************************************************************/
void can_prep_instance_request( struct sCAN* mMsg, byte tmpInstance )
{
	// instance used to avoid conflicts on the ID_INSTANCE_RESERVE_REQUEST
    mMsg->id 	  	 = create_CAN_eid( ID_INSTANCE_CLAIM, tmpInstance );
	mMsg->data[0] 	 = MyInstance;		// Requested value
	mMsg->data[1] 	 = Confirmed;
    mMsg->header.DLC = 2;
    mMsg->header.rtr = 0;
}

/* We believe we have an instance claimed.  Make sure it's unique on the network 
	No-reply
*/
void can_prep_instance_query( struct sCAN* mMsg )
{
	// instance used to avoid conflicts on the ID_INSTANCE_RESERVE_REQUEST
    mMsg->id 	  = create_CAN_eid( ID_INSTANCE_QUERY, MyInstance );
	mMsg->data[0] = MyInstance;		// Requested value
    mMsg->header.DLC    = 1;
    mMsg->header.rtr    = 0;
}

/************************************************************
 We received a request.
 ***********************************************************/
void can_process_instance_request( struct sCAN* mMsg )
{
	byte instance = mMsg->data[0];
	if (MyInstance > instance)	return;  // ignore since someone else is behind the game.

	if (instance >= MyInstance)
	{  
		MyInstance = instance+1;
	}
}

void save_instance_number()
{
    eeprom_write_byte( (unsigned char*)INSTANCE_EEPROM_ADDRESS,   MyInstance );
}
void read_instance_number()
{
    MyInstance = eeprom_read_byte( (unsigned char*)INSTANCE_EEPROM_ADDRESS );
}

void save_confirmed_status()
{
    eeprom_write_byte( (unsigned char*)INSTANCE_EEPROM_ADDRESS+1, Confirmed  );
}
void read_confirmed_status()
{
    Confirmed  = eeprom_read_byte( (unsigned char*)INSTANCE_EEPROM_ADDRESS+1 );
}

void ObtainInstanceNumber()
{
	/* Either the one shot - returns TransmittedSuccesfully, or TXE  */
}

