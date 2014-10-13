#ifndef _PACKER_MOTOR_H_
#define _PACKER_MOTOR_H_

#ifdef  __cplusplus
extern "C" {
#endif

/************* CONFIGURATION *****************/
/**** CONFIG BYTE 1 - BIT DEFS ****/
#define MODE_USE_ENCODER 	0x01		// else use potentiometer.  default is pot (0x00)
#define MODE_STOP_1_STORED 	0x02		// else defaults (20%)
#define MODE_STOP_2_STORED 	0x04		// else defaults (80%)
#define MODE_FREE_SPIN	 	0x08		// ignore stops.  default (0) is to stop
#define MODE_TILT_RESPONDER 0x10		// 1=> respond to tilt Y axis readings
#define MODE_PID_CONTROL	0x20		// PID or constant speed.
#define MODE_RESERVED_2		0x40		// 1=> respond to tilt Y axis readings
#define MODE_RESERVED_3		0x80		// 1=> respond to tilt Y axis readings

/**** CONFIG BYTE 2 - BIT DEFS ****
	0	- No report
	1	- Report every 10ms
	2	- Report every 20ms
	3	- Report every 50ms
	4	- Report every 100ms	*/
#define MODE_SEND_UPDATES_NONE	0x00	// 
#define MODE_SEND_UPDATES_10ms	0x10	// 
#define MODE_SEND_UPDATES_20ms	0x20	// 
#define MODE_SEND_UPDATES_50ms	0x40	// 
#define MODE_SEND_UPDATES_100ms	0x80	// 
// Which messages will be reported:
#define MODE_SEND_POSITION_RAW	0x01 	// Measured Pot/Encoder, CurrentLeft, CurrentRight, Speed
#define MODE_SEND_POSITION_CALC 0x02 	// Calculated quantities: Angle (deg*100), Current (Amps*100)
#define MODE_SEND_STATUS		0x04 	// 
/****************************************/

void configure_motor_flag			( struct sCAN* mMsg, byte mDestInstance, byte mFlag, BOOL mOn );
void configure_motor_set_update_rate( struct sCAN* mMsg, byte mDestInstance, byte mRate, byte mReports );
void pack_configure_motor			( struct sCAN* mMsg, byte mDestInstance, byte mFlags );
								 
byte can_parse_motor_angle( struct sCAN* mMsg, float* mAngle, word* mCurrent, short* mSpeedTenthDegSec );
byte can_parse_motor_value( struct sCAN* mMsg, word* mAngle, word* mCurrent, short* mSpeedTenthDegSec );
byte can_parse_motor_speed( struct sCAN* mMsg, float* mAngle, word* mCurrent, short* mSpeedTenthDegSec );

void pack_move_to_angle( struct sCAN* mMsg, byte mDestInstance, 
						 float mAngleDeg, word mSpeedTenthPercent );

void pack_move_speed	 ( struct sCAN* mMsg, byte mDestInstance, 
						   float mSpeedPercent );

void pack_move_dual_speed( struct sCAN* mMsg,   byte mDestInstance, 
	 					   float mSpeedPercent, float mSpeedPercent2 );

void pack_mark_motor_stop( struct sCAN* mMsg, byte mDestInstance, 
						   byte mStopNumber, float mAngleDeg, word mValue );

void pack_set_max_acceleration( struct sCAN* mMsg, byte mDestInstance, 
						   float mMaxAcceleration );

void pack_read_motor_stop( struct sCAN* mMsg, byte mDestInstance, 
						   byte mStopNumber );

#ifdef  __cplusplus
}
#endif

#endif
