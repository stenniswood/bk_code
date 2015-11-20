#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <vector> 
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <time.h>
#include <string>

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


float radians(float mDegrees)
{
	return (mDegrees * M_PI / 180.0 );	
}

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
	use_stops		= true;
	stop1.Angle 	= 0.0;
	stop1.PotValue  =  10;
	stop2.Angle 	= 0.0;
	stop2.PotValue  = 1014;
	
	CurrCount		= 0;		// Latest reading	
	CurrAngle		= 0;		// in Degrees 
	NextAngleDeg	= 0;		// in Degrees * 10 
	//SpeedTimesTen	= 0.;		// [-100.0 , 100.0] 
	RequestedSpeed  = 0.;		// Counts per second
	MeasuredSpeed   = 0.;		// Counts per second
	DutyPercent		= 0.;
	CurrentTimesTen	= 0.;		// fixed point

	ZeroOffset		= 512;		// center	
	ActiveOutputs	= TRUE;
	MotorEnable		= TRUE;		// If FALSE, does not participate in the sequencing
	MotorDirection	= 1;		// 
	MotorStopped	= 0;		// not stopped!

	MaxRatedTorque	= 1.;		// inch*lbs  around (1,546 for 82666)
	StallCurrent	= 100.;		// 	
	Duty			= 0.;		// 	
}

/**********************************************************************
Name	:	compute_motor_torque()
Purpose	:	
Inputs	:	mDuty	- [0..1.0] fraction of the motor power (PWM Duty)
Return	:	Amount of Torque 
**********************************************************************/
float Motor::compute_motor_torque( float mDuty ) 
{
	return (MaxRatedTorque * mDuty);
}


bool Motor::within_tolerance( float mDestination, float mTolerance )
{
	//printf("withinTol: \n");
	if (MotorEnable==FALSE)
		return true;		// move ahead

	float delta = fabs(mDestination - CurrCount);
	//printf("withinTol: %5.2f/ %5.2f \n", delta, mTolerance );
	if (delta < mTolerance)
		return true;
	else
		return false;
}

int Motor::compute_position( float mAngle )
{
	float PotValue = mAngle * CountsPerDegree + ZeroOffset;
	return PotValue;
}

float Motor::compute_angle( word mPotValue )
{
	float Angle = (mPotValue - ZeroOffset) * DegreesPerCount;
	return Angle;	
}


void print_stop_( struct sStopInfo mstop )
{
	if (mstop.Enabled)
		printf("Enabled ");
	else 
		printf("Disabled");
	printf(";  Angle=%6.2f;  Pot=%d\n", mstop.Angle, mstop.PotValue );
}

void Motor::print_stop( int mStopNum )
{
	printf("Stop1: ");	print_stop_( stop1 );	
	printf("Stop2: ");	print_stop_( stop2 );	
}

void Motor::print_state( )	// Position, speed, stops active, torque applied, etc.  1 liner.
{
	if (MotorEnable==FALSE)
		printf("Motor Disabled!\n");
	else {
		printf("Mot#=%3d; Pot=%4d; ZeroO=%4d; Angle=%7.2f degs;\tMeas_Spd=%8.3f Req_Spd=%8.3f degs/sec; Duty=%7.3f  ", 
				Instance, CurrCount, ZeroOffset, CurrAngle, MeasuredSpeed, RequestedSpeed, DutyPercent );	
		if (MotorStopped)
			printf("Stop %d\n",MotorStopped );
		else printf("\n"); 
	}
}
void Motor::print_speeds( )
{
	printf(" Speed:  Requested=%6.2f; Measured=%6.2f; Duty=%5.2f\n", 
					 RequestedSpeed, MeasuredSpeed, DutyPercent );	
}

/* PARAMS:
	mDuty - as a Percent
*/
void  Motor::send_speed( float mDutyPercent )
{
	bool proceed = true;
	if (MotorStopped) {
		proceed = correct_direction_out_of_stop( mDutyPercent );
	}

	if ((MotorEnable) && (proceed))
	{
		// takes [-100.0 , 100.0]
		pack_move_speed( &msg1, Instance, mDutyPercent );
		//short Speed    = mDuty * 100;	// *100 = 10,000 
		//printf("Spd= %4x \n", Speed);
		//print_message( &msg1 ); 
		AddToSendList( &msg1 );
	}
}

/* Need to take into account 3 things:
		The sign of the duty.
		Which stop 1 or 2.
		The motor's positive duty is increasing or decreasing counts.  depends on the physical.		
*/
bool Motor::correct_direction_out_of_stop( float mDuty )
{
	/* This code has been tested and is correct!  */
	short sign = (mDuty>0) ? 1:-1;
	//printf("sign=%d Stopped=%d, MDir=%d\n", sign, MotorStopped, MotorDirection );
	
	if (MotorStopped == 1)
	{	
		if (sign == MotorDirection)
			return true;
	} else if (MotorStopped == 2)
	{	
		// + duty and motordir is -
		if (sign != MotorDirection)
			return true;
	}
	return false;
}


int Motor::check_stops( )
{
	MotorStopped = 0;
	if (stop1.Enabled)
		if (CurrCount < stop1.PotValue)
			MotorStopped = 1;

	if (stop2.Enabled)
		if (CurrCount > stop2.PotValue)
			MotorStopped = 2;

	//printf("check_stops() = %d\n", MotorStopped);
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
		pack_move_to_angle( &Msg, Instance, NextAngleDeg, DutyPercent );
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
/*
return 	1 if handled.  0 if skipped.
*/
int Motor::handle_CAN_message( struct sCAN* mMsg 	)	// handles incoming ID_MOTOR_VALUE & recomputes duty.
{
	if (MotorEnable==FALSE)  return 0;

	if (mMsg->id.group.id == Feedback_Msg_id)
	{
		if (Feedback_Msg_id == ID_ANALOG_MEASUREMENT)
		{
			if (Feedback_index == mMsg->data[0])
			{
				PrevCount = CurrCount;
				CurrCount = (mMsg->data[1]<<8) + mMsg->data[2];
				printf("===Motor::handle_CAN_message %4x:  Count=%d\n", Feedback_index, CurrCount );
				update_position();
				return 1;
			}
		}
		else if (Feedback_Msg_id == ID_MOTOR_VALUE)
		{
			if (mMsg->id.group.instance == Instance)
			{			
				struct stMotorStateInfo tmp;
				can_parse_motor_value( mMsg, &(tmp.PotValue), 
											 &(tmp.CurrentTimesTen), 
									 (short*)&(tmp.SpeedTimesTen) );
				PrevCount = CurrCount;
				CurrCount = tmp.PotValue;
				update_position();
				print_state();
				return 1;
			}
		}
		
	} else if (mMsg->id.group.id == ID_MOTOR_STATUS)  {
		return 1;	
	} else if (mMsg->id.group.id == ID_MOTOR_SPEED)   {
		//can_parse_motor_speed( mMsg, &tmp.Angle, &tmp.CurrentTimesTen, (short*)&tmp.SpeedTimesTen );	
		return 1;	
	} else if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_ANGLE, 0)) )
	{	
		return 1;
	}
	return 0;
}

void Motor::print_average()
{
	printf("Mot#%d (%x) : Average Count= %6.3f  N=%d  %s\n", Instance, Feedback_index, 
													average_CurrCounts, Reads-1,
													Name );
}

/* 
	After incoming CAN msg has been processed, this will recompute duty and
	stops accordingly.
*/
int Motor::update_position(  )
{	
	// TIME LAPSE since last call here:
	PrevTime = CurrTime;	
	gettimeofday(&CurrTime, NULL);	
	struct timeval time_lapse;
	timersub( &CurrTime, &PrevTime, &time_lapse );
	float time_lapse_secs = time_lapse.tv_sec + ((float)time_lapse.tv_usec/1000000.);
	if ((MotorEnable) && (ActiveOutputs)) 
		printf("Time Lapse=%6.4f  ", time_lapse_secs);

	// AVERAGING MEASUREMENTS:
	if (Reads<ReadsAllowed)
	{
		SumCurrCounts += CurrCount;		// Clear the sum when you want to start averaging!
		Reads++;
	} else if (Reads==ReadsAllowed)	
	{
		average_CurrCounts = (float)SumCurrCounts /(float)Reads;		// Clear the sum when you want to start averaging!		
		Reads++;			
	} else { /* Leave it, SumCurrCounts now holds the average measurement. */ 
				Reads = ReadsAllowed+1;
	}
	
	// Update Angle:
	CurrAngle = compute_angle(CurrCount);
	PrevAngle = compute_angle(PrevCount);	

	// Compute Speed:
	MeasuredSpeed = (CurrAngle - PrevAngle) / time_lapse_secs;	
	check_stops();

	// Check over current here... (once current reading have been tested/verified)
}
	
#include "config_string_utils.h"


bool Motor::read_config_data( Preferences& mprefs, int mIndex )
{
	const char* ptr = NULL;
	char  key[80];

	sprintf(key, "actuator_%d_name", mIndex );
	ptr = mprefs.find_string( key );
	if (ptr==NULL)
		printf(" ptr=%x \n", ptr );
	else
		strcpy( Name, ptr );

	sprintf(key, "actuator_%d_instance", mIndex );
	Instance 	= mprefs.find_int	    ( key );

	sprintf(key, "actuator_%d_feedback_message", mIndex );
	ptr 	= mprefs.find_string( key );
	if (strcmp(ptr, "ID_ANALOG_MEASUREMENT")==0 )
		Feedback_Msg_id = ID_ANALOG_MEASUREMENT;
	else 	if (strcmp(ptr, "ID_MOTOR_VALUE")==0 )
		Feedback_Msg_id = ID_MOTOR_VALUE;

	sprintf(key, "actuator_%d_feedback_index", mIndex );
	Feedback_index  = mprefs.find_int	( key );		
	sprintf(key, "actuator_%d_feedback_zero_offset", mIndex );	
	ZeroOffset		= mprefs.find_int	( key );

	sprintf(key, "actuator_%d_direction", mIndex );	
	MotorDirection  = mprefs.find_int	( key );
	sprintf(key, "actuator_%d_enable", mIndex );	
	MotorEnable 	= mprefs.find_bool  ( key );		// Active outputs will get set equal to this...	
	sprintf(key, "actuator_%d_stop_1_angle", mIndex );
	stop1.Angle  	= mprefs.find_float	( key );
	sprintf(key, "actuator_%d_stop_1_value", mIndex );	
	stop1.PotValue  = mprefs.find_hex	( key );
	sprintf(key, "actuator_%d_stop_2_angle", mIndex );	
	stop2.Angle  	= mprefs.find_float	( key );
	sprintf(key, "actuator_%d_stop_2_value", mIndex );	
	stop2.PotValue  = mprefs.find_hex	( key );
}
