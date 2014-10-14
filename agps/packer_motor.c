#include "bk_system_defs.h"
#include <wiringPi.h>
#include "can_eid.h"
#include "can.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "Packer.h"

/***********************************SYSTEM CONFIGURE **************************/
/*
Possible configuration bits:
MODE_FREE_SPIN
MODE_USE_ENCODER
MODE_PID_CONTROL
MODE_TILT_RESPONDER

Reporting Configuration Bits:
MODE_SEND_POSITION_RAW
MODE_SEND_POSITION_CALC
MODE_SEND_STATUS
*/
// This adds to the current configuration in the mMsg.  
void configure_motor_flag( struct sCAN* mMsg, byte mDestInstance, byte mFlag, BOOL mOn )
{
	mMsg->data[0] |= mFlag;		// Mask
	if (mOn)
		mMsg->data[1] |= mFlag;		//Value
	else 
		mMsg->data[1] &= ~mFlag;	
}
/*
// This adds to the current configuration in the mMsg.  
mRate	 :  [MODE_SEND_UPDATES_NONE, MODE_SEND_UPDATES_10ms,
			 MODE_SEND_UPDATES_20ms, MODE_SEND_UPDATES_50ms,
			 MODE_SEND_UPDATES_100ms]
mReports :	[MODE_SEND_POSITION_RAW, MODE_SEND_POSITION_CALC, MODE_SEND_STATUS]
*/
void configure_motor_set_update_rate( struct sCAN* mMsg, byte mDestInstance, \
								 byte mRate, byte mReports )
{
	byte Masks[4]  = {0,0xF0,0,0 };
	byte Values[4] = {0,0   ,0,0 };	
	pack_configure( mMsg, Masks, Values, mDestInstance );
	mMsg->data[3] &= 0x0F;		//   clear bits
	mMsg->data[3] |= mRate;		//   set proper update bits
}

void pack_configure_motor( struct sCAN* mMsg, byte mDestInstance, byte mFlags )
{
	byte Masks[4]  = {0,0,0,0};
	byte Values[4] = {0,0,0,0};
	pack_configure( mMsg, Masks, Values, mDestInstance );
}

/*********************************** COMMANDS **************************/
/* String may be upto 8 characters only! */
void pack_move_to_angle( struct sCAN* mMsg, byte mDestInstance, 
						float mAngleDeg, float mSpeedPercent )
{
	mMsg->id = create_CAN_eid( ID_MOVE_TO_ANGLE, mDestInstance );
	mMsg->header.DLC = 4;
	mMsg->header.rtr = 0;	

	// PREPARE ANGLE:
	long int AngleHundredths = mAngleDeg * 100;
	word hi_Angle = hi_word(AngleHundredths);
	word lo_Angle = lo_word(AngleHundredths);	
	mMsg->data[0] = lo(lo_Angle);
	mMsg->data[1] = hi(lo_Angle);
	mMsg->data[2] = lo(hi_Angle);
	mMsg->data[3] = hi(hi_Angle);

	// PREPARE SPEED:
	word Speed = mSpeedPercent * 100;
	mMsg->data[4] = lo(Speed);
}

/* String may be upto 8 characters only! */
void pack_move_speed( struct sCAN* mMsg, byte mDestInstance, 
					  float mSpeedPercent )
{
	mMsg->id = create_CAN_eid( ID_MOVE_SPEED, mDestInstance );
	mMsg->header.DLC = 4;
	mMsg->header.rtr = 0;	

	word Speed = mSpeedPercent * 100;	
	mMsg->data[0] = hi(Speed);
	mMsg->data[1] = lo(Speed);
}

/*
mStopNumber	:	[0,1] Only 2 stops per motor
mAngleDeg	:	The calibrated AngleTenths
mValue		: 	The Potentiometer or encoder position to mark.
*/
void pack_mark_motor_stop( struct sCAN* mMsg, byte mDestInstance, 
						   byte mStopNumber, float mAngleDeg, word mValue )
{
	mMsg->id = create_CAN_eid( ID_MARK_MOTOR_STOP, mDestInstance );
	mMsg->header.DLC = 5;
	mMsg->header.rtr = 0;

	long int AngleHundredths = mAngleDeg * 100;
	word hi_Angle = hi_word(AngleHundredths);
	word lo_Angle = lo_word(AngleHundredths);

	mMsg->data[0] = mStopNumber;		// 0 or 1	
	mMsg->data[1] = lo(lo_Angle);
	mMsg->data[2] = hi(lo_Angle);
	mMsg->data[3] = lo(hi_Angle);
	mMsg->data[4] = hi(hi_Angle);
		
	mMsg->data[3] = lo(mValue);
	mMsg->data[4] = hi(mValue);
}

/* String may be upto 8 characters only! */
void pack_set_max_acceleration( struct sCAN* mMsg, byte mDestInstance, 
						   float mMaxAcceleration )
{
	mMsg->id = create_CAN_eid( ID_SET_MAX_MOTOR_ACCEL, mDestInstance );
	mMsg->header.DLC = 2;
	mMsg->header.rtr = 0;	

	word MaxAccel = (mMaxAcceleration*100);
	mMsg->data[0] = lo(MaxAccel);
	mMsg->data[1] = lo(MaxAccel);	
}

