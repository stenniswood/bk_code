#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <vector> 
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_Interface.h"
#include "can_instance.h"
#include "can_txbuff.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "packer.h"

#include "vector_file.h"
#include "packer_motor.h"
#include "motor_vector.h"
#include "motor.hpp"
#include "Appendage.hpp"
#include "robot.hpp"
#include "config_file.h"


/**********************************************************************
Name	:	Constructor()
Purpose	:	
Inputs	:	mDuty	- [0..1.0] fraction of the motor power (PWM Duty)
Return	:	Amount of Torque 
**********************************************************************/
Motor::Motor(float mMaxRatedTorque, float mStallCurrent)
{
	Initialize();
	MaxRatedTorque = mMaxRatedTorque;
	StallCurrent   = mStallCurrent;
}
 
Motor::Motor()
{
	Initialize();
}

void Motor::Initialize()
{
	Reads = 0;			// number of CAN positioning reads done since duty was last updated.
	ReadsAllowed=3;		// number of CAN positioning reads before duty will be updated.

	Instance 		= 0;
	stop1.Angle 	= 0.0;
	stop1.PotValue  = 10;
	stop2.Angle 	= 0.0;
	stop2.PotValue  = 1014;

	StartPotValue	= 0;		// First reading
	CurrPotValue	= 0;		// Latest reading
	DestinationPotValue	= 0;	// Vector position
	DestinationReached = false;	
	
	CurrAngle		= 0;		// in Degrees 
	NextAngleDeg	= 0;		// in Degrees * 10 
	SpeedTimesTen	= 0.;		// [-100.0 , 100.0] 
	CurrentTimesTen	= 0.;		// fixed point

	ZeroOffset		= 512;		// center	
	MotorEnable		= TRUE;		// If FALSE, does not participate in the sequencing
	MotorDirection	= 1;		// 
	MotorStopped	= 0;		// not stopped!

	MaxRatedTorque	= 1.;		// inch*lbs  around (1,546 for 82666)
	StallCurrent	= 100.;		// 	
	Duty			= 0.;		// 	
}

/**********************************************************************
Name	:	calc_motor_torque()
Purpose	:	
Inputs	:	mDuty	- [0..1.0] fraction of the motor power (PWM Duty)
Return	:	Amount of Torque 
**********************************************************************/
float Motor::calc_motor_torque( float mDuty ) 
{
	return (MaxRatedTorque * mDuty);
}

// Safe way of checking.  It Looks for overshoot which is past tolerance and
// considers this reach also.
bool Motor::destination_reached( float mTolerance )
{
	if (MotorEnable == FALSE) 	   return true;
	if (DestinationReached==true)  return true;	
	struct timeval tv;
			
	if (within_tolerance( DestinationPotValue, mTolerance )) 
	{			
		// ie only the first time it's reached
		printf("  Reached within tol: %d %5.1f ", DestinationPotValue, mTolerance );			
		gettimeofday(&tv, NULL);
		completed = tv;
		DestinationReached=true;
		return true;
	}
	
	// Overshoot ? 
	bool overshot = false;
	if (StartPotValue < DestinationPotValue)
	{
		//Then an overshoot would be when CurrPotValue is over DestinationPotValue				
		overshot = (CurrPotValue > DestinationPotValue) ? (true):(false);	
	} else {
		//Then an overshoot would be when CurrPotValue is under DestinationPotValue	
		overshot = (CurrPotValue < DestinationPotValue) ? (true):(false);		
	}
	float mag = fabs(DestinationPotValue - CurrPotValue);
	if (overshot)
	{
		DestinationReached=true;
		printf(" overshoot=%d %5.2f ", overshot, mag );	
		gettimeofday(&tv, NULL);
		completed = tv;
	}
	return overshot;
}

bool Motor::within_tolerance( float mDestination, float mTolerance )
{
	//printf("withinTol: \n");
	if (MotorEnable==FALSE)
		return true;		// move ahead

	float delta = fabs(mDestination - CurrPotValue);
	//printf("withinTol: %5.2f/ %5.2f \n", delta, mTolerance );
	if (delta < mTolerance)
		return true;
	else
		return false;
}

float Motor::compute_angle( )
{
	CurrAngle = (CurrPotValue - ZeroOffset) * DegreesPerCount;
	return CurrAngle;
}

float Motor::compute_duty( float mDestination )
{
	float  delta = 0.0;
	delta = ( mDestination - CurrPotValue );
	float duty = 100.*delta/1024.;
	
	if (MotorDirection==-1)
		duty = -duty;

	SpeedTimesTen = duty;		
	return duty;
}

void print_stop_( struct sStopInfo mstop )
{
	if (mstop.Enabled)
		printf("Enabled");
	else 
		printf("Disabled");
	printf(";  Angle=%6.2f;  Pot=%x", mstop.Angle, mstop.PotValue );
}

void Motor::print_stop( int mStopNum )
{
	if (mStopNum==1)
	{  printf("Stop1: ");	print_stop_( stop1 );	}
	else if (mStopNum==2)
	{  printf("Stop2: ");	print_stop_( stop2 );	}
}

void Motor::print_positioning( )
{
	printf(" Start=%d; Curr=%d; Dest=%d; Speed=%6.2f  ", 
			StartPotValue, CurrPotValue, DestinationPotValue, SpeedTimesTen );	
}

// Computes the speed, then sends it.
void  Motor::send_speed_pid(  )
{
	if (MotorEnable==FALSE) return;

	SpeedTimesTen = compute_duty( DestinationPotValue );
	//print_positioning();
	send_speed( SpeedTimesTen );
}

void  Motor::send_speed( float mDuty )
{
	bool proceed = true;
	if (MotorStopped)
		proceed = correct_direction_out_of_stop( mDuty );

	if ((MotorEnable) && (proceed))
	{		
		// takes [-100.0 , 100.0]
		pack_move_speed( &msg1, Instance, mDuty );
		short Speed    = mDuty * 100;	// *100 = 10,000 
		//printf("Spd= %4x \n", Speed);
		//print_message( &msg1 ); 
		AddToSendList( &msg1 );
	}
}

bool Motor::correct_direction_out_of_stop( float mDuty )
{
	BOOL sign = (mDuty>0) ? 1:0;
	if (MotorStopped == 1)
		if (sign == MotorDirection)
			return true;
	else if (MotorStopped == 2)
		if (sign != MotorDirection)
			return true;
	else return true;
	return false;
}

int Motor::check_stops( )
{
	MotorStopped = 0;
	if (stop1.Enabled)
		if (CurrPotValue < stop1.PotValue)
			MotorStopped = 1;

	if (stop2.Enabled)
		if (CurrPotValue > stop2.PotValue)
			MotorStopped = 2;
	return MotorStopped;
}

void Motor::send_stop( )
{
	send_speed( 0.0 );
}

void Motor::send_moveto_angle( float mAngle )
{
	struct sCAN Msg;	
	if (MotorEnable)
	{
		if (mAngle != 9999.)
			NextAngleDeg = mAngle;

		// takes [-100.0 , 100.0]
		pack_move_to_angle( &Msg, Instance, NextAngleDeg, SpeedTimesTen );
		// 				     mSpeedHundredthPercent );
		//print_message( &msg1 );
		AddToSendList( &msg1 );
	}
}

void Motor::send_config( byte mindex, byte mValue, byte mMask )
{
	struct sCAN Msg;	
	// Located in packer.c : 
	pack_configure( &Msg, Instance, mindex, mMask, mValue );
	//print_message( &msg1 );
	AddToSendList( &msg1 );	
}

/* Need: a duty which corresponds counteracts the force of gravity.
		 This will get added to the desired duty. (may be negative or
		 positive)
		 
   LengthCenterOfMass  *  mg sin(theta)
*/
float  Motor::calc_gravity_boost(  )
{	
	float boost = K_const * sin(CurrAngle);
	return boost;

	// K will be found thru a self adjusting algorithm.
	// ie we set up some pendulumn motion and adjust K until : 
	//  get a constant period	
}

/* For the swing leg - air born.  We need some mutual
	compensation for the action/reaction.  ie hip needs
	+10% when knee moves.  The motor will react due to pots.
	but we want to add a little umph matching knee so that 
	there is no reaction.
	Alpha may be negative.
*/
float  Motor::calc_reaction_torque( Motor& mMotor, float mAlpha )
{
	float boost = mMotor.SpeedTimesTen * mAlpha;
	SpeedTimesTen += boost;
	return boost;	
}

/* 
	Handles incoming ID_MOTOR_VALUE & recomputes duty.

*/
int Motor::update_position( struct sCAN* mMsg )
{
	struct stMotorStateInfo tmp;

	can_parse_motor_value( mMsg, &(tmp.PotValue), 
							     &(tmp.CurrentTimesTen), 
						   (short*)&(tmp.SpeedTimesTen) );
	
	CurrPotValue = tmp.PotValue;
	check_stops();
	send_speed_pid();

	compute_angle();

	// Check over current here... (once current reading have been tested/verified)
			
	BOOL reached = destination_reached( 10 );
	if (reached) {
		printf( "Destn reached!" );
	}
	printf("\n");
}

