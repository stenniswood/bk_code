/**************************************************************************
Product of Beyond Kinetics, Inc
This code handles the CAN functional block of the atmega16m1.

void set_rx_callback( void (*mCallback)(struct sCAN*) );
void set_tx_callback( void (*mCallback)(struct sCAN*) );

void can_init(void);
void can_setup_receive_mob( byte mMobNumber, short mMaskLow, short mMaskHigh, byte mDLC );
void can_fill_tx_data( byte mMOb_Number, tID mId, byte* mData, byte mDLC );
void can_retrieve_id( struct sCAN* mMsg );

void can_get_msg( struct sCAN* mMsg )
void can_send_msg( byte mMOb_Number, struct sCAN* mMsg )
word can_get_timestamp()
byte can_retrieve_data( struct sCAN* mMsg )
void can_set_id( tID id )

void can_set_baud_100K( BOOL mHighTQ )
void can_set_baud_125K( BOOL mHighTQ )
void can_set_baud_200K( BOOL mHighTQ )
void can_set_baud_250K( BOOL mHighTQ )
void can_set_baud_500K( BOOL mHighTQ )
void can_set_baud_1M  ( BOOL mHighTQ )

**************************************************************************
*****************				MOB LAYOUT					**************
**************************************************************************
6 AVAILABLE MOBS:
MOB0	-  General Transmit			(MyInstance Tagged)
MOB1	-  Module Specific Receives (MyInstance Filtered)
MOB2	-  
MOB3	-  
MOB4	-  System Non-specific Receives 0x001x (Irrelevant MyInstance)
			ID_BOARD_PRESENCE_REQUEST  
MOB5	-  System Non-specific Receives 0x002x (Irrelevant MyInstance)
			ID_INSTANCE_RESERVE_REQUEST 
**************************************************************************
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
**************************************************************************/
#include <stdlib.h>
#include "bk_system_defs.h"
#include "can_eid.h"
//#include "can.h"
#include "can_board_msg.h"
#include "can_buff.h"
#include "can_instance.h"
#include "can_id_list.h"


struct sCAN 	msg1;
struct sCAN 	msg2;
struct sCAN 	LastReceivedMsg;
BOOL 			NewMessageReceivedFlag = FALSE;
BOOL			TransmittedSuccesfully = FALSE;

volatile int8_t data[8];
void (*rx_call_back)(struct sCAN*) = NULL;		// Call back function for processing specific to the device.
void (*tx_call_back)() = NULL;		// Call back function for processing specific to the device.

//#define USE_TX_INTERRUPTS 1

/*****************************************************************************
Name		: set_rx_callback()
INPUT 		: mCallback - a function ptr to receive incoming CAN msgs.
OUTPUT		: none
RETURN		: none
DESCRIPTION:  CAN ISR will call the function sent here on a Receive interrupt
			which is not handled by the can_board_msg.c system message handler.
*****************************************************************************/
void set_rx_callback( void (*mCallback)(struct sCAN*) )
{
	rx_call_back = mCallback;
}
/*****************************************************************************
Name		: set_tx_callback()
INPUT 		: mCallback - a function ptr to receive incoming CAN msgs.
OUTPUT		: none
RETURN		: none
DESCRIPTION:  CAN ISR will call the function sent here on a Transmit interrupt
			which is not handled by the can_board_msg.c system message handler
*****************************************************************************/
void set_tx_callback( void (*mCallback)() )
{
	tx_call_back = mCallback;
}

//***** Reception ISR **********************************

/*****************************************************************************
Name:        	can_init()
Input:			none
Returns:        none
Description:    This function: sets baudrate, enables Mob0 for Reception,
MOB LAYOUT:
0	Module Specific Transmission(MyInstance tagged)
1	Module Specific Receive 	(MyInstance filtered)
2	
3	MyInstance Query 			(autoreply mode)
4	MyInstance Requests			(random instance)
5	Broadcast Receive			( 				)
*****************************************************************************/
void can_init( byte mSpeed )
{
}

/*****************************************************************************
Name		: can_init_mobs()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
void can_init_mobs()
{
   // SETUP MOB0 for Module Specific Transmit (MyInstance tagged)
   
   // SETUP MOB1 for Module Specific Reception (MyInstance filtered)
   can_setup_receive_mob	 ( 1, 0x0000, 0x0000, 8 );
   can_add_instance_to_filter( 1, MyInstance 		);

   // SETUP MOB2 Unused

   // SETUP MOB3 for ID_INSTANCE_CLAIM (no Instance - checks done in software):
   can_setup_receive_mob		  ( 3, 0x0000, 0x0000, 8 );
   can_remove_instance_from_filter( 3 					 );
   can_add_id_to_filter			  ( 3, ID_INSTANCE_CLAIM, ID_INSTANCE_CLAIM );

   // SETUP MOB4 for ID_INSTANCE_QUERY(Auto-reply) :
/* can_setup_receive_mob		  ( 4, 0x0000, 0x0000, 8 );
   can_remove_instance_from_filter( 4 					 );
   can_add_id_to_filter			  ( 4, ID_INSTANCE_QUERY, ID_INSTANCE_QUERY ); */

   // SETUP MOB5 for BROADCAST RECEIVE (no Instance):
   can_setup_receive_mob		  ( 5, 0x0000, 0x0000, 8 );
   can_remove_instance_from_filter( 5 					 );
   can_add_id_to_filter			  ( 5, ID_SYSTEM_REQUEST_MASK, ID_SYSTEM_REQUEST_MASK );
}

/*****************************************************************************
Name :  can_add_instance_to_filter()
Input:  MyInstance variable from can_instance.h include.
OPERATES ON Currently Selected MOb (CANPAGE register)

* This requires that the instance matches this module's MyInstance
   before the message is accepted.  After doing this, the software check 
   is not required for that MOb.
*****************************************************************************/
void can_add_instance_to_filter( byte mMOb_Number, word mInstance )
{
}

/*****************************************************************************
Name :  can_remove_instance_from_filter()
Input:  none
OPERATES ON Currently Selected MOb (CANPAGE register)

* This MOb no longer requires the instance match.  ie It will accept any
  and all Instances. Use this for broadcast messages (intended for all receivers)
*****************************************************************************/
void can_remove_instance_from_filter( byte mMOb_Number )
{
}

/*****************************************************************************
Name		: can_set_id()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
void can_set_id( byte mMOb_Number, tID id )
{	
	// Set CAN id
	// Block (5 bits) & Upper 3 of ID
	byte idt1 = ((id.group.block & 0x1F)<<3) | ((id.group.id & 0xE000)>>13);
	byte idt2 = ((id.group.id & 0x1FE0)>>5);
	byte idt3 = ((id.group.id & 0x001F)<<3)  | ((id.group.instance & 0xE0)>>5);
	byte idt4 =  (id.group.instance<<3) | 0x00;		// RTR bits
}

/*****************************************************************************
Name :  can_add_id_to_filter()  
			Slightly different from can_set_id() - It only adds the 16bit portion
			of the id to the filter, AND it also sets the mask for this!
			Use when setting up the MOB for specific receive Msgs.
			
Input:  mMOb_Number	- MOb to add the filter to
		mID_mask	- Mask of which bits in the ID portion of the tID are important
		mID_tag		- Tag (value) of the ID bits which must be matched.
* This is done independent of the instance filtering.  
*****************************************************************************/
void can_add_id_to_filter( byte mMOb_Number, word mID_mask, word mID_tag )
{
	tID id   = create_CAN_eid( mID_tag,  0 );
	tID mask = create_CAN_eid( mID_mask, 0 );

	// Ignore Block & Instance
//	byte idt1 = (CANIDT1 & 0xF8) | ((id.group.id & 0xE000)>>13);
//	byte idt2 = ((id.group.id & 0x1FE0)>>5);
//	byte idt3 = ((id.group.id & 0x001F)<<3) | (CANIDT3 & 0x07);
	// Ignore Block & Instance
//	byte idm1 = (CANIDM1 & 0xF8) | ((mask.group.id & 0xE000)>>13);
//	byte idm2 = ((mask.group.id & 0x1FE0)>>5);
//	byte idm3 = ((mask.group.id & 0x001F)<<3) | (CANIDM3 & 0x07);
}

/*****************************************************************************
Name :  can_remove_instance_from_filter()
Input:  none
OPERATES ON Currently Selected MOb (CANPAGE register)

* This MOb no longer requires the id match.  ie It will accept any
  and all Instances. Use this for broadcast messages (intended for all receivers)
*****************************************************************************/
void can_remove_id_from_filter( byte mMOb_Number )
{
	int8_t savecanpage;
	byte idm1 = 0xF8;
	byte idm2 = 0;
	byte idm3 = 0x07;
}

/*****************************************************************************
Name:        	can_set_mask()
Input:
	mMaskLow	: CANIDM1 & 2
	mMaskHigh	: CANIDM3 & 4
Returns:        none
Description:    This function sets up a MOB for reception
*****************************************************************************/
void can_set_mask(byte mMOb_Number, short mMaskLow, short mMaskHigh )
{
	int8_t savecanpage;
}
/*****************************************************************************
Name		: can_set_tag()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
void can_set_tag(byte mMOb_Number, short mTagLow, short mTagHigh )
{
	int8_t savecanpage;
}

/*****************************************************************************
Name:        	can_setup_receive_mob()
Input:
	mMobNumber	: Message Object Block to use [0..5]
	mMaskLow	: CANIDM1 & 2
	mMaskHigh	: CANIDM3 & 4
	mDLC		: Data Length Check (0..8)
Returns:        none
Description:    This function sets up a MOB for reception
*****************************************************************************/
void can_setup_receive_mob( byte mMobNumber, short mMaskLow, short mMaskHigh, byte mDLC )
{
	int8_t savecanpage;

}

/***************************************************************
This setup is common to all code.  Not application specific 
There is 1 free MOB, however, for application specific tricks.
****************************************************************/
/*****************************************************************************
*****************				MOB LAYOUT						**************
6 AVAILABLE MOBS:
MOB0	-  General Transmit			(MyInstance Tagged)
MOB1	-  Module Specific Receives (MyInstance Filtered)
MOB2	-  
MOB3	-  
MOB4	-  System Non-specific Receives 0x001x (Irrelevant MyInstance)
			ID_BOARD_PRESENCE_REQUEST  
MOB5	-  System Non-specific Receives 0xFCxx (Irrelevant MyInstance)
			ID_INSTANCE_RESERVE_REQUEST 
******************************************************************************/

void can_disable_mob( byte mob )
{
}

/*****************************************************************************
Name		: can_retrieve_id()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
void can_retrieve_id( struct sCAN* mMsg )
{
	// Retrieve CAN id
/*	mMsg->id.group.block = (idt1 >> 3);		// BLOCK = UPPER NIBBLE+1 (5 BITS)

	word tmpW1 = ((idt1 & 0x07)<<(5+8));	// Lowest 3 make the highest 3 of id
	word tmpW2 = (idt2<<5);					// all 8 bits shifted up snug.
	word tmpW3 = (idt3 & 0xF8)>>3;		// 5 more form the least significant bits.
	mMsg->id.group.id = tmpW1 | tmpW2 | tmpW3;

	tmpW1 = ((idt3 & 0x07)<<5);
	mMsg->id.group.instance = ( tmpW1 | (idt4>>3));		// INSTANCE
	*/
}

/*****************************************************************************
Name		: can_retrieve_data()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
byte can_retrieve_data( struct sCAN* mMsg )
{
	// Retrieve CAN id
	can_retrieve_id( mMsg );

	// Read DLC out of reg
	return mMsg->header.DLC;
}

/************************************************************
	can_send_msg()
    Stuff the parameters into the CAN registers 
************************************************************/
void can_send_msg( byte mMOb_Number, struct sCAN* mMsg )
{  
	can_send_msg_no_wait( mMOb_Number, mMsg );

}

void can_send_msg_no_wait( byte mMOb_Number, struct sCAN* mMsg )
{  
}

/************************************************************
can_get_timestamp()
  Operates on the currently selected CANPAGE MOB.
  Gets timestamp out of the CAN timer registers 
************************************************************/
word can_get_timestamp()
{
  word retval = 0;
  return retval;
}

/************************************************************
can_get_msg()
  Operates on the currently selected CANPAGE MOB.
  Adds a timestamp in addition to can_retrieve_data();	
************************************************************/
void can_get_msg( struct sCAN* mMsg )
{
  mMsg->header.DLC = can_retrieve_data( mMsg );
  mMsg->time_stamp = can_get_timestamp();
}

/*****************************************************************************
Name:        	set_baud()
Input:			mSpeed - constant id see below..
Returns:        none
Description:    This function: sets baudrate, enables Mob0 for Reception
*****************************************************************************/
void set_baud(byte mSpeed)
{
	switch (mSpeed)
	{
	case CAN_1M_BAUD   :     can_set_baud_1M  ( FALSE );	break;
	case CAN_500K_BAUD :     can_set_baud_500K( FALSE );	break;
	case CAN_250K_BAUD :     can_set_baud_250K( FALSE );	break;
	case CAN_200K_BAUD :     can_set_baud_200K( FALSE );	break;
	case CAN_125K_BAUD :     can_set_baud_125K( FALSE );	break;
	case CAN_100K_BAUD :     can_set_baud_100K( FALSE );	break;
	default			   :	 can_set_baud_250K( FALSE );
	}
}

/*********************************************************************/
/** These adjust the BAUD RATE and SAMPLING settings		        **/
/*********************************************************************/
void can_set_baud_1M( BOOL mHighTQ )
{
}
void can_set_baud_500K( BOOL mHighTQ )
{
}
void can_set_baud_250K( BOOL mHighTQ )
{
}
void can_set_baud_200K( BOOL mHighTQ )
{
}
void can_set_baud_125K( BOOL mHighTQ )
{
}
void can_set_baud_100K( BOOL mHighTQ )
{
}



/* This should already be done before calling!
byte page = 0;
page |= (mMOb_Number << 4);
page |= AINC;        // Auto Increment register addresses (after a write)
CANPAGE = page; */

/* MOB0 for transmission */
/*void can_tx(void) 
{
   CANPAGE = 0x00;      		    		// Select MOb0 for transmission   
   while ((CANEN2 & (1<<ENMOB0))>0) {};		// Wait for ENMOb0=0 to be free

   CANSTMOB = 0x00;     // Clear mob status register
   CANIDT4 = 0x00;      // Set can id to 0
   CANIDT3 = 0x00;      // ""
   CANIDT2 = 0x00;      // ""
   CANIDT1 = 0x00;      // ""   
   for ( int8_t i = 0; i < 8; ++i ) {
        CANMSG = 0x55+i;  // set message data for all 8 bytes to 55 (alternating 1s and 0s
   }
	Can_clear_rtr();
   CANCDMOB = ( 1 << CONMOB0 ) | ( 1 << IDE ) | ( 8 << DLC0 );    // Enable transmission, data length=1 (CAN Standard rev 2.0B(29 bit identifiers))
   //while ( (CANSTMOB & (1<<TXOK))==0 );   // wait for TXOK flag set
   // todo: have this use interrupts
   //CANCDMOB = 0x00;   // Disable Transmission
   //CANSTMOB = 0x00;   // Clear TXOK flag
}*/
