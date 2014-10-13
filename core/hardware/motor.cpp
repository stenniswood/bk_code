// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "bk_system_defs.h"
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_txbuff.hpp"
#include "packer.h"
//#include "system_msgs_callback.h"
//#include "board_list.hpp"
#include "can_id_list.h"
#include "vector_math.h"
#include "cmd_process.h"
#include "parser_tilt.h"
#include "buttons.h"
#include "fuse_ag.h"
#include "fuse_accel.h"
#include "fuse_magnet.h"
#include "fuse_gyro.h"
#include "fuse_log.h"
#include "dataset.hpp"
#include "leveler.hpp"
#include "display.h"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "histogram.hpp"
#include "stick_leg.hpp"
#include "window_layouts.hpp"
#include "callbacks.hpp"
#include "motor.hpp"


// This file scope variable used for all Motor messaging transmits:
// struct sCAN MMsg;

/**********************************************************
  This class is an interface to the CAN protocol for 
  a BigMotor board and the walking sequencer, calibrator,
  configurator, graphical displays, etc.	
 **********************************************************/
Motor::Motor()
{
	init();
}
Motor::Motor(byte mInstance)
{
	init();
	Instance = mInstance;
}

/*Motor::Motor(byte mInstance, int Left, int Right, int Top, int Bottom )
: Control( Left, Right, Top, Bottom )
{
	init();
	Instance = mInstance;
}*/

void Motor::init()
{
	Instance = 0;
	
	Value  =0;
	Angle  =0.0;
	Current=0.0;
	Speed  =0.0;
	
	Stop1_Value= 0;
	Stop2_Value= 1023;		
	Stop1_Angle= 0;
	Stop2_Angle= 300;
	
	MMsg.header.rtr = 0;
}

void Motor::MarkStop( byte StopNum, float mAngle, word mValue ) 
{
	if (StopNum == 1)
	{
		Stop1_Value = mValue;		// Current (last read) value
		Stop1_Angle = mAngle;		// 
	}
	else if (StopNum == 2)
	{
		Stop2_Value = mValue;		// Current (last read) value	
		Stop2_Angle = mAngle;
	}
}

void Motor::setStopAngle( byte StopNum, float mAngle )  
{
	if (StopNum == 1)
		Stop1_Angle = mAngle;
	else if (StopNum == 2)
		Stop2_Angle = mAngle;
	Value = getAngleAt( mAngle );
}

void Motor::setStopValue( byte StopNum, word mValue )  
{
	if (StopNum == 1)
		Stop1_Value = mValue;		
	else if (StopNum == 2)
		Stop2_Value = mValue;
	printf("New %d Value = %d\n", StopNum, mValue );
	Angle = getAngleAt( mValue );
}

float Motor::getStopAngle( byte StopNum )
{
	if (StopNum==1)
		return Stop1_Angle;
	else if (StopNum==2)
		return Stop2_Angle;
}
word Motor::getStopValue( byte StopNum )
{
	if (StopNum==1)
		return Stop1_Value;
	else if (StopNum==2)
		return Stop2_Value;
}

// put into a .ini file.
int	Motor::SavePreferences( Preferences* pref )
{
	/* Should also save config bytes info:  it is. 
			Basefrequency, 
			Motor Model, */
	char str[255];
	sprintf(str, "Instance=%d\nS1_angle=%6.2f; S1_value=%d; S2_Angle=%6.2f; S2_value=%d;\n", \
				Instance, Stop1_Angle, Stop1_Value, Stop2_Angle, Stop2_Value );	
	pref->write( str );

	sprintf(str, "Config1=%x; Config2=%x; Config3=%x; Config4=%x;\n", 
			config_bytes[0], config_bytes[1], config_bytes[2], config_bytes[3]);
	pref->write( str );	
}

int	Motor::LoadPreferences( Preferences* pref )
{
	char str[255];
	int  n;
	
	pref->readln_nb( str );
	Instance    = pref->getIntValue  ( str );
	printf(" %d ", Instance);
	
	pref->readln_nb( str );
	char** info = pref->split(str, ';', &n);		// Should be 5 entries	

	// Stop1:
	Stop1_Angle = pref->getFloatValue( info[0] );
	Stop1_Value = pref->getIntValue  ( info[1] );
	// Stop2:
	Stop2_Angle = pref->getFloatValue( info[2] );
	Stop2_Value = pref->getIntValue  ( info[3] );
	//printf("Read: Instance=%d\nStop1=<%d,%6.1f>\t", Instance, Stop1_Value, Stop1_Angle);
	//printf("Stop2=<%d,%6.1f>\n", Stop2_Value, Stop2_Angle);

	// CONFIG
	pref->readln_nb( str );
	info = pref->split(str, ';', &n);		// Should be 5 entries	
	config_bytes[0] = pref->getIntValue( info[0] );
	config_bytes[1] = pref->getIntValue( info[1] );
	config_bytes[2] = pref->getIntValue( info[2] );
	config_bytes[3] = pref->getIntValue( info[3] );
	//printf("Read: conf_byte 1..4= [%d, %d, %d, %d]\n", config_bytes[0], config_bytes[1],
	//		config_bytes[2], config_bytes[3] );
}

/* Linearly Interpolates */
float Motor::getAngleAt( word mValue )
{
	// Find current Zero offset:
	float AngleRange = fabs(Stop2_Angle-Stop1_Angle);
	int   ValueRange = max(Stop1_Value, Stop2_Value) - min(Stop1_Value, Stop2_Value);
	float deltaValue = mValue - Stop1_Value;

	// Delta A is to AngleRange as delta V is to ValueRange:
	float deltaAngle = (deltaValue / ValueRange)*AngleRange;	
	return (deltaAngle + Stop1_Angle);
}

word Motor::getValueAt( float mAngle )
{
	// Find current Zero offset:
	float AngleRange = fabs(Stop2_Angle-Stop1_Angle);
	int   ValueRange = max(Stop1_Value, Stop2_Value) - min(Stop1_Value, Stop2_Value);
	float deltaAngle = mAngle - Stop1_Angle;

	// Delta A is to AngleRange as delta V is to ValueRange:
	float deltaValue = (deltaAngle / AngleRange)*ValueRange;
	int   offset     = round(deltaValue + Stop1_Value);
	return offset;
}

/* Assuming the 2 stops have been set already.  We want to shift
the angles to put zero at the Value passed in. */
word Motor::getZeroOffset( word mValueAtZeroDegrees )
{
	return getValueAt( 0.0 );
}

/* Assuming the 2 stops have been set already.  We want to shift
the angles to put zero at the Value passed in. */
void Motor::adjustZeroOffset( word mValueAtZeroDegrees )
{
	// Find current Zero offset:
	int shift = getZeroOffset( mValueAtZeroDegrees ) - mValueAtZeroDegrees;
	Stop1_Angle -= shift;
	Stop2_Angle -= shift;
}

/* 
return:  TRUE if the message was handled
		 FALSE => instance did not match this motor.
*/
int Motor::process_CAN_msg( struct sCAN* mMsg )
{
	if (mMsg->id.group.instance != Instance)  return 0;	// Only deal with this motor's message!
	int result,retval;
	switch(mMsg->id.group.id)
	{
	case ID_MOTOR_ANGLE:
			result = can_parse_motor_angle( mMsg );
			// This should be read out of the motor objects and then set into the GUI at display time.
			retval = TRUE;
			break;
	case ID_MOTOR_VALUE:
			// Want to get the Value and put into our class member for it.
			// Also calcs the Angle based on stops.
			result = can_parse_motor_value( mMsg );
			//printf("rxd ID_MOTOR_VALUE: instance=%d Value=%d\n", Instance, Value);
			retval = TRUE;
			break;
	case ID_MOTOR_SPEED		:
				return can_parse_motor_speed( mMsg );
				retval = TRUE;
				break;
	case ID_CALIBRATED_STOP	:
				// Sometimes we want to verify this against what's in the object memory
				can_compare_motor_stop( mMsg );
				// Sometimes we want to read this and store in the object memory
				// can_parse_motor_stop( mMsg );
				retval = TRUE;
				break;
	case ID_MOTOR_ACCEL		:
				retval = TRUE;
				break;
	case ID_MOTOR_STATUS	:
				retval = TRUE;	
				break;
	case ID_SYSTEM_CONFIGURE_REPORT : 
				config_bytes[0] = mMsg->data[0];
				config_bytes[1] = mMsg->data[1];
				config_bytes[2] = mMsg->data[2];
				config_bytes[3] = mMsg->data[3];				
				break;
	default:
				break;
	}
	
	return retval;	
}

BOOL Motor::ValidRange  (  )
{
	// within control limits.
	BOOL valid_region = ((Value > Stop1_Value) && (Value < Stop2_Value));
	return valid_region;
}
	
byte Motor::inStopRegion( )
{
	if ( ValidRange() )	return 0;
	if ( Value < Stop1_Value )	return 1;
	if ( Value > Stop2_Value )	return 2;
	return 0;
}

///////////////////////////////////////////////////////////////////
//   CAN MESSAGING FUNCTIONS:
///////////////////////////////////////////////////////////////////
void Motor::pack_move_to_angle( float mAngleDeg, word mSpeedHundredthPercent )
{
	MMsg.id = create_CAN_eid( ID_MOVE_TO_ANGLE, Instance );
	MMsg.header.DLC = 6;
	MMsg.header.rtr = 0;

	// PREPARE ANGLE:
	long int AngleHundredths = mAngleDeg * 100;
	word hi_Angle = hi_word(AngleHundredths);
	word lo_Angle = lo_word(AngleHundredths);
	MMsg.data[0] = hi(hi_Angle);
	MMsg.data[1] = lo(hi_Angle);
	MMsg.data[2] = hi(lo_Angle);
	MMsg.data[3] = lo(lo_Angle);

	// PREPARE SPEED:
	MMsg.data[4] = hi(mSpeedHundredthPercent);		// or tenth percent?
	MMsg.data[5] = lo(mSpeedHundredthPercent);
}

void Motor::pack_move_speed( float mSpeedPercent )
{
	MMsg.id = create_CAN_eid( ID_MOVE_SPEED, Instance );
	MMsg.header.DLC = 2;
	MMsg.header.rtr = 0;

	// User enters a number [-100.0 , 100.0]
	// BigMotorEn board looks for Percent*100 however 
	//(for additional resolution)
	short Speed  = round(mSpeedPercent * 100.0);	// *100 = 10,000
	MMsg.data[0] = hi(Speed);		// 65 535
	MMsg.data[1] = lo(Speed);		// 10 000
}

void Motor::pack_move_dual_speed	  (  float mSpeedPercent, float mSpeedPercent2	)
{
	MMsg.id = create_CAN_eid( ID_MOVE_DUAL_SPEED, Instance );
	MMsg.header.DLC = 4;
	MMsg.header.rtr = 0;

	// User enters a number [-100.0 , 100.0]
	short Speed    = mSpeedPercent * 100;	// *100 = 10,000
	MMsg.data[0] = hi(Speed);		// 65 535
	MMsg.data[1] = lo(Speed);		// 10 000
	Speed    = mSpeedPercent2 * 100;	// *100 = 10,000
	MMsg.data[2] = hi(Speed);		// 65 535
	MMsg.data[3] = lo(Speed);		// 10 000
}

/* Send 1 Stop Info (will be programmed to EEPROM on device)
Input mStopNumber : 0x11 Read Stop #1
					0x12 Read Stop #2
					0x01 Program Stop #1
					0x02 Program Stop #2
*/
void Motor::pack_mark_motor_stop(  byte mStopNumber  )
{
	MMsg.id = create_CAN_eid( ID_MARK_MOTOR_STOP, Instance );
	MMsg.header.DLC = 7;
	MMsg.header.rtr = 0;

	long int AngleHundredths;
	word tValue;

	if ((mStopNumber&0x10)==0x10)
	{ tValue = 0; AngleHundredths=0; }		// Read request
	else if (mStopNumber==1)
	{  tValue=Stop1_Value;  AngleHundredths=round(Stop1_Angle*100.); }
	else
	{  tValue=Stop2_Value;  AngleHundredths=round(Stop2_Angle*100.); }
	
	word hi_Angle = hi_word(AngleHundredths);
	word lo_Angle = lo_word(AngleHundredths);

	MMsg.data[0] = mStopNumber;		// 0 or 1;  or 0x10 for read
	MMsg.data[1] = hi(hi_Angle);
	MMsg.data[2] = lo(hi_Angle);
	MMsg.data[3] = hi(lo_Angle);
	MMsg.data[4] = lo(lo_Angle);
	MMsg.data[5] = hi(tValue);
	MMsg.data[6] = lo(tValue);
}

void Motor::pack_set_max_acceleration(  float mMaxAcceleration 	)
{
	MMsg.id = create_CAN_eid( ID_SET_MAX_MOTOR_ACCEL, Instance );
	MMsg.header.DLC = 2;
	MMsg.header.rtr = 0;	

	word MaxAccel = (mMaxAcceleration*100);
	MMsg.data[0] = lo(MaxAccel);
	MMsg.data[1] = lo(MaxAccel);	
}
void Motor::pack_read_motor_stop	  (  byte mStopNumber 		)
{
	MMsg.id = create_CAN_eid( ID_MARK_MOTOR_STOP, Instance );
	MMsg.header.DLC = 1;
	MMsg.header.rtr = 0;
	MMsg.data[0] = 0x10 | mStopNumber;		// 0 or 1;  or 0x10 for read
}

void Motor::send_message()
{
	AddToSendList( &MMsg );
}
	
// CONFIGURE:
void Motor::configure_flag( byte mFlag, BOOL mOn 		)
{
	MMsg.data[0] |= mFlag;		// Mask
	if (mOn)
		MMsg.data[1] |= mFlag;		//Value
	else 
		MMsg.data[1] &= ~mFlag;
}

void Motor::configure_set_update_rate( byte mRate, byte mReports )
{
	byte Value = (mRate | mReports);
	pack_configure( 2, 0xFF, Value );
}

void Motor::set_base_frequency( byte mRate )
{
	// BASE_100HZ, BASE_300HZ, BASE_1000HZ, BASE_5000HZ
	pack_configure( 1, 0xC0, mRate );
	send_message();
} 	

void Motor::read_configure( )
{
//	pack_configure( 0x10, 0x00, 0x00 );
	send_message();
}
	
/* Note: You must request a read_configure() at least once prior 
  		 to calling this function.  You may need additional reads
  		 after changing configuration, or suspect that config change
  		 by an external program.	*/
byte Motor::get_base_frequency()
{
	return ((config_bytes[0] & 0xC0)>>6);
}

void Motor::send_all_configs( )
{
	pack_configure(1, 0xFF, config_bytes[0]);	send_message();
	pack_configure(2, 0xFF, config_bytes[1]);	send_message();
	pack_configure(3, 0xFF, config_bytes[2]);	send_message();
	pack_configure(4, 0xFF, config_bytes[3]);	send_message();	
}

/*
 Keep thinking "if I do something to the object" so it should become active immediately.
 don't want the set/send mechanism.  
	mindex:  [1..4]  
*/
void Motor::set_config_byte( byte mindex, byte value )
{
	config_bytes[mindex-1] = value;
 	pack_configure(mindex, 0xFF, config_bytes[mindex-1]);
	print_message( &MMsg );
 	send_message();
}
	
void Motor::pack_configure( byte config_index, byte Mask, byte Value 	)
{
	MMsg.id = create_CAN_eid( ID_SYSTEM_CONFIGURE, Instance );
	MMsg.header.DLC = 3;
	MMsg.header.rtr = 0;
	MMsg.data[0] = config_index;
	MMsg.data[1] = Mask ;
	MMsg.data[2] = Value;	
}

/* Parsers & Compares to what's already in the object
	only compares 1 stop!
Return:		0x10 - Stop1 Matches
			0x11 - Stop1_Angle differs
			0x12 - Stop1_Value differs
			0x13 - Both Stop1_Angle & Stop1_Value differ

			0x20 - Stop2 Matches
			0x21 - Stop2_Angle differs
			0x22 - Stop2_Value differs		
			0x23 - Both Stop2_Angle & Stop2_Value differ	*/
byte Motor::can_compare_motor_stop( struct sCAN* mMsg )
{
	byte retval = 0;
	if (mMsg->id.group.instance != Instance) return 0;	
	int stopNumber = mMsg->data[0];	// 0 or 1;  or 0x10 for read
	long int Angle;
	Angle =  (mMsg->data[1]<<24) |(mMsg->data[2]<<16) |
		 	 (mMsg->data[3]<<8 ) | mMsg->data[4];
	Value =  (mMsg->data[5]<<8 ) | mMsg->data[6];
	
	if (stopNumber==1)
	{
		printf("Angle=%x=%d; Value=%d;  Stop1_Angle=%6.1f; Stop1_Value=%d\n", 
				Angle, Angle, Value, Stop1_Angle, Stop1_Value );
		retval |= 0x10;
		if (Stop1_Angle != (Angle/100.0))	retval |= 0x01;	 // check the 10.0
		if (Stop1_Value != Value)			retval |= 0x02;  // 
	}
	else if (stopNumber==2)
	{
		printf("Angle=%x=%d; Value=%d;  Stop1_Angle=%6.1f; Stop1_Value=%d\n", 
				Angle, Angle, Value,   Stop2_Angle, Stop2_Value );
		retval |= 0x20;
		if (Stop2_Angle != (Angle/100.0))	retval |= 0x01;	 // check the 10.0
		if (Stop2_Value != Value)			retval |= 0x02;  // 
	}
	printf("Comparison of %x Stops= %x\n", stopNumber, retval);
	return retval;
}

byte Motor::can_parse_motor_stop( struct sCAN* mMsg )
{
	if (mMsg->id.group.instance != Instance) return 0;	
	int stopNumber = MMsg.data[0];	// 0 or 1;  or 0x10 for read
	long int Angle;
	Angle =  (MMsg.data[1]<<24)+(MMsg.data[2]<<16)
		 	+(MMsg.data[3]<<8) + MMsg.data[4];
	Value =  (MMsg.data[5]<<8) + MMsg.data[6];

	if (stopNumber==1)
	{
		Stop1_Angle = Angle / 10.0;	// check the 10.0
		Stop1_Value = Value;
	}
	else if (stopNumber==2)
	{
		Stop2_Angle = Angle / 10.0;	// check the 10.0
		Stop2_Value = Value;	
	}
	return 1;
}

// RECEIVE CAN MESSAGE:
byte Motor::can_parse_motor_angle(  struct sCAN* mMsg )
{
	if (mMsg->id.group.instance != Instance) return 0;	
	long angle_hundreds_deg = mMsg->data[0] + (mMsg->data[1]<<8) +
								+ (mMsg->data[2]<<16) + (mMsg->data[3]<<24);
	Angle	= angle_hundreds_deg / 100;
	Current	= (mMsg->data[4]<<8) + mMsg->data[5];
	Speed	= (mMsg->data[6]<<8) + mMsg->data[7];		// TenthDegSec
	return 1;
}

byte Motor::can_parse_motor_value( struct sCAN* mMsg )
{
	if (mMsg->id.group.instance != Instance) return 0;
	Value    = (mMsg->data[0]<<8) + (mMsg->data[1]);
	Current  = (mMsg->data[2]<<8) + mMsg->data[3];
	Speed 	 = (mMsg->data[6]<<8) + mMsg->data[7];	// mSpeedTenthDegSec
	Angle = getAngleAt(Value);	
	return 1;
}

byte Motor::can_parse_motor_speed( struct sCAN* mMsg )
{
	if (mMsg->id.group.instance != Instance) return 0;	
	word angle_count  = (mMsg->data[0]<<8) + (mMsg->data[1]);
	Current  = (mMsg->data[2]<<8) + mMsg->data[3];		// this was done improperly long time ago.  
	Current  = (mMsg->data[4]<<8) + mMsg->data[5];		// the HBridge actually only gives 1 current
	Speed 	 = (mMsg->data[6]<<8) + mMsg->data[7];	// tenth deg / sec
	return 1;
}

/* NOTE:  The functions below do a pack and then send.  When using a motor object,
		  we don't want to have to think about packaging and sending.  Just want to 
		  tell the object to move, stop, or reconfigure, etc.  The send_message() 
		  copies and adds the message to list for the transmit queue.  So we
		  should even be safe calling these from an interrupt service routine.  They'll
		  be queued and sent as soon as time permits.
*/
void Motor::move_to_angle	 	 (  float mAngleDeg, 	  word mSpeedTenthPercent 		)
{
	pack_move_to_angle( mAngleDeg, mSpeedTenthPercent );
	send_message();
}
void Motor::move_speed	 	  	 (  float mSpeedPercent 								)
{
	pack_move_speed( mSpeedPercent );
	//print_message( &MMsg );
	send_message();	
}
void Motor::move_dual_speed	  	 (  float mSpeedPercent, float mSpeedPercent2 			)
{
	pack_move_dual_speed (mSpeedPercent, mSpeedPercent2 );
	send_message();
}

BOOL Motor::verifyStops (  )
{
	printf("VerifyStops:  send_stops()  1\n");
	pack_mark_motor_stop( 0x10 );
	send_message();
	//pack_mark_motor_stop( 0x12 );
	//send_message();
}
void Motor::send_stops( )
{
	printf("Motor:  send_stops()  1\n");
	pack_mark_motor_stop( 1 );
	send_message();
	printf("Motor:  send_stops()  2\n");
	pack_mark_motor_stop( 2 );
	send_message();
}
void Motor::mark_motor_stop	  	 (  byte mStopNumber, 	  float mAngleDeg, word mValue 	)
{
	if (mStopNumber==1)
	{ Stop1_Value=mStopNumber;  Stop1_Angle=mAngleDeg; }
	else
	{ Stop2_Value=mStopNumber;  Stop2_Angle=mAngleDeg; }
	
	pack_mark_motor_stop ( mStopNumber );
	send_message();
}
void Motor::set_max_acceleration (  float mMaxAcceleration 								)
{
	pack_set_max_acceleration ( mMaxAcceleration );
	send_message();
}
void Motor::read_motor_stop	  	 (  byte mStopNumber 									)
{
	pack_read_motor_stop( mStopNumber );
	send_message();	
}
