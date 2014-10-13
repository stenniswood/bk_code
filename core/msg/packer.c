#include "bk_system_defs.h"
#include <wiringPi.h>
#include "can_eid.h"
#include "CAN_base.h"
//#include "can_instance.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "packer.h"


/* This is a generic configure message.  More specific individual board messages
   will wrap this function.  ie.  
  		tilt_configure( "send raw data; update rate=50ms; ");
		bigmotor_configure( "no respond to tilt; send position; send current");
*/
void pack_configure( struct sCAN* mMsg, byte mDestInstance, byte mIndex, byte mMask, byte mValue )
{
	mMsg->id = create_CAN_eid( ID_SYSTEM_CONFIGURE, mDestInstance );
	mMsg->header.DLC = 3;
	mMsg->header.rtr = 0;
	mMsg->data[0] = mIndex;
	mMsg->data[1] = mMask;
	mMsg->data[2] = mValue;
}

/*	old way was to send all 4 masks & values!
void pack_configure( struct sCAN* mMsg, byte mDestInstance, byte* mMasks, byte* mValues )
{
	mMsg->id = create_CAN_eid( ID_SYSTEM_CONFIGURE, mDestInstance );
	mMsg->header.DLC = 8;
	mMsg->header.rtr = 0;

	mMsg->data[0] = mMasks[0];
	mMsg->data[2] = mMasks[1];
	mMsg->data[4] = mMasks[2];	
	mMsg->data[6] = mMasks[3];

	mMsg->data[1] = mValues[0];
	mMsg->data[3] = mValues[1];
	mMsg->data[5] = mValues[2];	
	mMsg->data[7] = mValues[3];
}

	mResponseTypeRequested : (select one only)
			ASK_PRESENCE,
			ASK_REVISION,
			ASK_SERIAL_NUMBER,
			ASK_DESCRIPTION
*/
void pack_board_presence_request( struct sCAN* mMsg, byte mResponseTypeRequested )
{
	mMsg->id = create_CAN_eid_b(0, ID_BOARD_PRESENCE_REQUEST, 0 );
	parse_CAN_eid_b( &(mMsg->id), mMsg->id.id_array );	
	mMsg->data[0] = mResponseTypeRequested;
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;	
}
/*
INPUT:	mMode - SYSTEM_LED_MODE_DEVICE, SYSTEM_LED_MODE_PATTERN
				SYSTEM_LED_MODE_STROBE, SYSTEM_LED_MODE_MYINSTANCE
		mPattern - lower nibble is led pattern		
*/
void pack_system_led( struct sCAN* mMsg, byte mDestInstance, byte mMode, byte mPattern )
{
	mMsg->id = create_CAN_eid( ID_SYSTEM_LED_REQUEST, mDestInstance );
	mMsg->data[0] = (mMode & 0x0F);
	mMsg->data[1] = (mPattern & 0x0F);
	if (mMode == SYSTEM_LED_MODE_STROBE)
		 mMsg->data[1] = (mPattern >0);		// On or Off.
	mMsg->header.DLC = 2;
	mMsg->header.rtr = 0;	
}

/*
INPUT:	mMode - SYSTEM_LED_MODE_DEVICE, SYSTEM_LED_MODE_PATTERN
				SYSTEM_LED_MODE_STROBE, SYSTEM_LED_MODE_MYINSTANCE
		mPattern - lower nibble is led pattern		
*/
void pack_system_shutdown(struct sCAN* mMsg )
{
	mMsg->id = create_CAN_eid( ID_SYSTEM_SHUT_DOWN, 0xFF );
	mMsg->header.DLC = 0;
	mMsg->header.rtr = 0;
}

/* This informs one/all boards on the network to erase their EEPROM 
   instance number (and confirmed status).  And begin the process again
   (on next bootup or immediately?)

INPUT: mDestInstance	: [0..254] Specific board.  [255] for all boards
*/
void pack_instance_restart(struct sCAN* mMsg, byte mDestInstance )
{
	mMsg->id = create_CAN_eid( ID_INSTANCE_RESTART, mDestInstance );
	mMsg->header.DLC = 2;		// No Data 
	mMsg->header.rtr = 0;
	mMsg->data[0] = 0x12;
	mMsg->data[1] = 0x34;
}

/* This informs one/all boards on the network to erase their EEPROM 
   instance number (and confirmed status).  And begin the process again
   (on next bootup or immediately?)

INPUT: mDestInstance	: [0..254] Specific board.  [255] for all boards
*/
void pack_instance_reassign(struct sCAN* mMsg, byte mDestInstance, byte mNewInstance )
{
	mMsg->id = create_CAN_eid( ID_INSTANCE_ASSIGN, mDestInstance );
	mMsg->header.DLC = 1;		// No Data 
	mMsg->data[0] = mNewInstance;
	mMsg->header.rtr = 0;		
}



