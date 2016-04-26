#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_base.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "packer.h"
#include "packer_motor.h"

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
	byte Value = (mRate | mReports);
	pack_configure( mMsg, mDestInstance, 2, 0xFF, Value );
}

void pack_configure_motor( struct sCAN* mMsg, byte mDestInstance, byte mIndex, byte mFlags )
{
	byte Mask  = 0xFF;
	byte Value = 0x00;
	pack_configure( mMsg, mDestInstance, mIndex, Mask, Value );	
}

/*************************** PARSE POSITION **************************/
/* Return :		Instance number 									 */
byte can_parse_motor_angle( struct sCAN* mMsg, float* mAngle, word* mCurrent, short* mSpeedTenthDegSec )
{
	//System_Send_Status=0;
	long angle_hundreds_deg = mMsg->data[0] + (mMsg->data[1]<<8) +
		+ (mMsg->data[2]<<16) + (mMsg->data[3]<<24);
	if (mAngle)
		*mAngle = angle_hundreds_deg / 100;
	if (mCurrent)	
		*mCurrent    = (mMsg->data[4]<<8) + mMsg->data[5];
	if (mSpeedTenthDegSec)
		*mSpeedTenthDegSec = (mMsg->data[6]<<8) + mMsg->data[7];	
	return mMsg->id.group.instance;
}
/* Return :		Instance number 									 */
byte can_parse_motor_value( struct sCAN* mMsg, word* mAngle, word* mCurrent, short* mSpeedTenthDegSec )
{
	//System_Send_Status=0;
	*mAngle = (mMsg->data[0]<<8) + (mMsg->data[1]);
	
	word LeftCurrentAvg;
	word RightCurrentAvg;
	word Speed;
	
	LeftCurrentAvg  = (mMsg->data[2]<<8) + mMsg->data[3];
	RightCurrentAvg = (mMsg->data[4]<<8) + mMsg->data[5];
	Speed 			= (mMsg->data[6]<<8) + mMsg->data[7];
	return mMsg->id.group.instance;
}
byte can_parse_motor_speed( struct sCAN* mMsg, float* mAngle, word* mCurrent, short* mSpeedTenthDegSec )
{
	//System_Send_Status=0;
	word angle_count  = (mMsg->data[0]<<8) + (mMsg->data[1]);
	
	word LeftCurrentAvg;
	word RightCurrentAvg;
	word Speed;
	
	LeftCurrentAvg  = (mMsg->data[2]<<8) + mMsg->data[3];
	RightCurrentAvg = (mMsg->data[4]<<8) + mMsg->data[5];
	Speed 			= (mMsg->data[6]<<8) + mMsg->data[7];
	return mMsg->id.group.instance;
}

/*********************************** COMMANDS **************************/
/* String may be upto 8 characters only! */
void pack_move_to_angle( struct sCAN* mMsg, byte mDestInstance, 
						 float mAngleDeg, word mSpeedHundredthPercent )
{
	mMsg->id = create_CAN_eid( ID_MOVE_TO_ANGLE, mDestInstance );
	mMsg->header.DLC = 6;
	mMsg->header.rtr = 0;

	// PREPARE ANGLE:
	long int AngleHundredths = mAngleDeg * 100;
	word hi_Angle = hi_word(AngleHundredths);
	word lo_Angle = lo_word(AngleHundredths);	
	mMsg->data[0] = hi(hi_Angle);
	mMsg->data[1] = lo(hi_Angle);
	mMsg->data[2] = hi(lo_Angle);
	mMsg->data[3] = lo(lo_Angle);

	// PREPARE SPEED:
	mMsg->data[4] = hi(mSpeedHundredthPercent);		// or tenth percent?
	mMsg->data[5] = lo(mSpeedHundredthPercent);
}

/* String may be upto 8 characters only! */
void pack_move_speed( struct sCAN* mMsg, byte mDestInstance, 
					  float mSpeedFraction )
{
	mMsg->id = create_CAN_eid( ID_MOVE_SPEED, mDestInstance );
	mMsg->header.DLC = 2;
	mMsg->header.rtr = 0;

	// User enters a number [-100.0 , 100.0]
	short Speed    = mSpeedFraction * 100;	// *100 = 10,000
	mMsg->data[0] = hi(Speed);		// 65 535
	mMsg->data[1] = lo(Speed);		// 10 000
}

/* String may be upto 8 characters only! */
void pack_stop( struct sCAN* mMsg, byte mDestInstance  )					 
{
	mMsg->id = create_CAN_eid( ID_STOP_MOTOR, mDestInstance );
	mMsg->header.DLC = 3;
	mMsg->header.rtr = 0;

	// datum [0] & [1] do not matter on a stop.  see coast.
	mMsg->data[2] = HARD_STOP_MOTOR;
}

void pack_coast( struct sCAN* mMsg, byte mDestInstance, 
				 float mSpeedPercent )
{
	mMsg->id = create_CAN_eid( ID_STOP_MOTOR, mDestInstance );
	mMsg->header.DLC = 3;
	mMsg->header.rtr = 0;

	// User enters a number [-100.0 , 100.0]
	float fraction = (mSpeedPercent / 100.);
	short Speed    = fraction*10000.;	// *100 = 10,000
	mMsg->data[0] = hi(Speed);		// 65 535
	mMsg->data[1] = lo(Speed);		// 10 000
	mMsg->data[2] = 1;				// non-zero & !HARD_STOP_MOTOR	
}

/* For BigMotorEn boards which are set to 2 Motor configuration. */
void pack_move_dual_speed( struct sCAN* mMsg,   byte mDestInstance, 
	 					   float mSpeedPercent, float mSpeedPercent2 )
{
	mMsg->id = create_CAN_eid( ID_MOVE_DUAL_SPEED, mDestInstance );
	mMsg->header.DLC = 4;
	mMsg->header.rtr = 0;

	// User enters a number [-100.0 , 100.0]
	short Speed    = mSpeedPercent * 100;	// *100 = 10,000
	mMsg->data[0] = hi(Speed);		// 65 535
	mMsg->data[1] = lo(Speed);		// 10 000
	Speed    = mSpeedPercent2 * 100;	// *100 = 10,000
	mMsg->data[2] = hi(Speed);		// 65 535
	mMsg->data[3] = lo(Speed);		// 10 000
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
	mMsg->header.DLC = 7;
	mMsg->header.rtr = 0;

	long int AngleHundredths = mAngleDeg * 100;
	word hi_Angle = hi_word(AngleHundredths);
	word lo_Angle = lo_word(AngleHundredths);

	mMsg->data[0] = mStopNumber;		// 0 or 1;  or 0x10 for read
	mMsg->data[1] = hi(hi_Angle);
	mMsg->data[2] = lo(hi_Angle);
	mMsg->data[3] = hi(lo_Angle);
	mMsg->data[4] = lo(lo_Angle);
	mMsg->data[5] = hi(mValue);
	mMsg->data[6] = lo(mValue);
}

void pack_read_motor_stop( struct sCAN* mMsg, byte mDestInstance, 
						   byte mStopNumber )
{
	mMsg->id = create_CAN_eid( ID_MARK_MOTOR_STOP, mDestInstance );
	mMsg->header.DLC = 1;
	mMsg->header.rtr = 0;
	mMsg->data[0] = 0x10 | mStopNumber;		// 0 or 1;  or 0x10 for read
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

