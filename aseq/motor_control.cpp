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
#include "motor_control.hpp"
#include "config_file.h"




MotorControl::MotorControl()
{
	init_params();
}

MotorControl::MotorControl(float mMaxRatedTorque, float mStallCurrent)
: Motor(mMaxRatedTorque, mStallCurrent)
{
	init_params();
}



void MotorControl::reset_pid()
{
	position_error_sum  = 0;
	speed_error_sum	    = 0;
	position_error_prev = 0;
	speed_error_prev	= 0;
}

void MotorControl::init_params()
{
	Kp_position_alpha = 0.0;
	Ki_position_alpha = 0.1;
	Kd_position_alpha = -0.5;
	
	Kp_speed_alpha = 1.0;
	Ki_speed_alpha = 0.5;
	Kd_speed_alpha = 0.5;
	
	K_const			= 9.8 * 2;	// ~20 percent boost at 90 degrees
}

/* Given a speed and max deceleration rate.  How far will it travel before
	coming to a complete stop 
*/
float MotorControl::compute_stopping_distance( float mSpeed, float mDeceleration )
{
	// dist = a t * t / 2 
	float time = mSpeed / mDeceleration;
	float stopping_distance = 0.5 * mDeceleration * time * time;
	return stopping_distance;
}

/* Use this to find the speed along the deceleration line. 
	ie. after the BeginBrakingCount has been triggered and before
	the destination has been reached.
*/
float MotorControl::compute_braking_speed( )
{
	// Assuming perfect constant deceleration :
	// Compute the time :  d = 1/2 a t*t
	float distance = (DestinationCount - CurrCount);
	float operand  = distance*2 / deceleration_rate_cpss;
	float time     = sqrt( fabs(operand) );
	return deceleration_rate_cpss * time;
}

bool MotorControl::is_breaking_region( word mCount )
{
	// braking if  (BeginBrakingCount < mCount < DestinationCount)
	//					820								1000
	//					95								  25
	bool retval;
	float delta = (StartCount - DestinationCount);
	if (delta<0) {
		if (mCount>BeginBrakingCount)  return true;
		else return false;
	} else {
		if (mCount<BeginBrakingCount)  
			retval = true;
		else 
			retval = false;
	} 
	return false;
}

/* Done.
	Computes the speed assuming constant acceleration from the start point.
	and that the original speed there was zero.
*/
float MotorControl::compute_accel_speed( )
{
	// Compute the time :  d= 1/2 a * t * t
	float distance = fabs(CurrCount - StartCount);
	float operand  = distance*2 / deceleration_rate_cpss;
	float time     = sqrt( operand );
	return deceleration_rate_cpss * time;
}

float MotorControl::get_control_speed( )
{
	float spd;
	if (is_breaking_region(CurrCount))
	{
		spd = compute_braking_speed( );		
		printf("brake speed=%6.3f ", spd);
	} else
		spd = compute_accel_speed  ( );

	if (spd > RequestedSpeed)
		spd = RequestedSpeed;
	return spd;
}


/* Need: a duty which corresponds counteracts the force of gravity.
		 This will get added to the desired duty. (may be negative or
		 positive)
		 
   LengthCenterOfMass  *  mg sin(theta)
*/
float  MotorControl::compute_gravity_boost( )
{	
	float boost = K_const * sin( radians(CurrAngle) );
	return boost;

	// K will be found thru a self adjusting algorithm.
	// ie we set up some pendulumn motion and adjust K until : 
	//  get a constant period	
}


#define MAX_P_DUTY 775 

/* 
	Based on PID control :
*/
float MotorControl::compute_duty( )
{	
	float  position_error;
	float  speed_error 	 ;
	bool   first_iteration = false;
	
	float  control_speed  = get_control_speed();	
	//printf("control_speed=%6.3f\n", control_speed );

	// GET ERROR:
	position_error 		  = ( DestinationCount - CurrCount  );
	speed_error 		  = ( control_speed - MeasuredSpeed );

	// PID (P)	
	float pcomp_position_duty = Kp_position_alpha * position_error;
	float pcomp_speed_duty 	  = Kp_speed_alpha    * speed_error;

	// PID (I) 
	if (position_error_sum==0)	first_iteration = true;	
	position_error_sum  	 += position_error;
	speed_error_sum	    	 += speed_error;
	float icomp_position_duty = Ki_position_alpha * position_error_sum;
	float icomp_speed_duty    = Ki_speed_alpha    * speed_error_sum;

	// PID (D)
	position_error_deriv = position_error - position_error_prev;
	speed_error_deriv    = speed_error - speed_error_prev;	
	if (first_iteration)	position_error_deriv = 0;
	float  dcomp_position_duty = Kd_position_alpha * position_error_deriv;
	float  dcomp_speed_duty    = Kd_speed_alpha    * speed_error_deriv;

	position_error_prev = position_error;		// update for next time!
	speed_error_prev    = speed_error;

	float p_duty = pcomp_position_duty + icomp_position_duty + dcomp_position_duty;
	float s_duty = pcomp_speed_duty    + icomp_speed_duty    + dcomp_speed_duty;

	p_duty += compute_gravity_boost();
	s_duty += compute_gravity_boost();
	if (MotorDirection==-1)
	{
		p_duty = -p_duty;
		s_duty = -s_duty;
	}

	DutyPercent = 100 * p_duty / MAX_P_DUTY;
	if (DutyPercent > 100.0)	DutyPercent = 100.0;
	if (DutyPercent < -100.0)	DutyPercent = -100.0;
	return DutyPercent;
}


/*
INPUT:
mDestinationCount	:	Counts
mRequestedSpeed		:	Counts / second
		For example (90 degrees = ~300 counts) in 1.0 second 
		= 300/1.0 = 300 cps.
If Destination is beyond the stop limits, it will be truncated at the limits.
*/
void MotorControl::set_destination( int mDestinationCount, float mRequestedSpeed )
{
	if (MotorEnable==FALSE)	return;
	
	// BOUND BY STOP LIMITS:
	if (mDestinationCount < stop1.PotValue)
		mDestinationCount = stop1.PotValue;
	else if (mDestinationCount > stop2.PotValue)
		mDestinationCount = stop2.PotValue;
	//printf("After destCount=%d;\n", mDestinationCount );

	// CALCULATE WHEN TO START BRAKING : 	
	//  0.014/20ms= 0.014/0.020 = 0.7
	float distance_to_stop = compute_stopping_distance( mRequestedSpeed, 
														deceleration_rate_cpss  );
/*	if (is_destination_greater())
		BeginBrakingCount = DestinationCount - stopping_distance;
	else 
		BeginBrakingCount = DestinationCount + stopping_distance;  */

	RequestedSpeed		= mRequestedSpeed;
	DestinationReached  = false;
	StartCount       	= CurrCount;
	DestinationCount 	= mDestinationCount;



	reset_pid();
	printf("RequestedSpeed=%6.2f; decel=%5.3f; distance_to_stop=%6.3f; \n", 
				mRequestedSpeed, deceleration_rate_cpss,
				distance_to_stop );
}
//i give up on these algorithms.  i don't know how to make them work

//	BeginBrakingCount = DestinationCount - distance;
//  Speed is zero at DestinationCount
//	float time = MeasuredSpeed / acceleration;
//  float distance


/* For the swing leg - air born.  We need some mutual
	compensation for the action/reaction.  ie hip needs
	+10% when knee moves.  The motor will react due to pots.
	but we want to add a little umph matching knee so that 
	there is no reaction.
	Alpha may be negative.
*/
float  MotorControl::compute_reaction_torque( Motor& mMotor, float mAlpha )
{
	float boost = mMotor.DutyPercent * mAlpha; 
	DutyPercent += boost;
	return boost;	
}


// Safe way of checking.  It Looks for overshoot which is past tolerance and
// considers this reach also.
bool MotorControl::destination_reached( float mTolerance )
{
	if (MotorEnable == FALSE) 	   return true;
	if (DestinationReached==true)  return true;	
	struct timeval tv;
			
	if (within_tolerance( DestinationCount, mTolerance )) 
	{	
		// ie only the first time it's reached
		printf("  Reached within tol: %d %5.1f ", DestinationCount, mTolerance );			
		gettimeofday(&tv, NULL);
		completed = tv;
		DestinationReached=true;
		return true;
	}

	// Overshoot ? 
	bool overshot = false;
	if (StartCount < DestinationCount)
	{
		//Then an overshoot would be when CurrCount is over DestinationCount				
		overshot = (CurrCount > DestinationCount) ? (true):(false);	
	} else {
		//Then an overshoot would be when CurrCount is under DestinationCount	
		overshot = (CurrCount < DestinationCount) ? (true):(false);		
	}
	float mag = fabs(DestinationCount - CurrCount);
	if (overshot)
	{
		DestinationReached=true;
		printf(" overshoot=%d %5.2f ", overshot, mag );	
		gettimeofday(&tv, NULL);
		completed = tv;
	}
	return overshot;
}

int MotorControl::handle_CAN_message( struct sCAN* mMsg )	// handles incoming msg
{
	Motor::handle_CAN_message( mMsg );
	run_algorithm();
}

// Computes the torque, then sends it.
void  MotorControl::send_speed_pid(  )
{	
	if (MotorEnable==FALSE) return;

	DutyPercent = compute_duty( );
	//printf("\t\t\tDUTY=%5.1f\n", DutyPercent);
	//print_positioning();
	send_speed( DutyPercent );		// base motor class
}

void MotorControl::run_algorithm()
{
	BOOL reached = destination_reached( 10 );
	if (reached) {
		//printf( "Destn reached!" ); printf("\n");
	}

	if (ActiveOutputs)
		send_speed_pid();
}

#include "config_string_utils.h"


bool MotorControl::read_config_data( Preferences& mprefs, int mIndex  )
{
	Motor::read_config_data( mprefs, mIndex );
	char  key[80];
	
	sprintf(key, "actuator_%d_Kp_position_alpha", mIndex );
	Kp_position_alpha = mprefs.find_float( key );
	sprintf(key, "actuator_%d_Ki_position_alpha", mIndex );
	Ki_position_alpha = mprefs.find_float( key );
	sprintf(key, "actuator_%d_Kd_position_alpha", mIndex );
	Kd_position_alpha = mprefs.find_float( key );
	
	sprintf(key, "actuator_%d_Kp_speed_alpha", mIndex );
	Kp_speed_alpha = mprefs.find_float( key );
	sprintf(key, "actuator_%d_Ki_speed_alpha", mIndex );
	Ki_speed_alpha = mprefs.find_float( key );
	sprintf(key, "actuator_%d_Kd_speed_alpha", mIndex );
	Kd_speed_alpha = mprefs.find_float( key );
			
	sprintf(key, "actuator_%d_K_const", mIndex );
	K_const 	   = mprefs.find_float( key );
}

bool MotorControl::init_calculator()
{
	calc.m_start_value		= StartCount;
	calc.m_destination_value= DestinationCount;
	calc.m_time_delta 		= 1.0;
	calc.m_average_speed 	= RequestedSpeed;
	calc.m_deceleration		= deceleration_rate_cpss;
	calc.compute_speeds();
//	calc.print_speeds();
}

/************************************************************************
	ControlSpeedCalculator()  Functions:
 ************************************************************************/

ControlSpeedCalculator::ControlSpeedCalculator()
{
	m_start_value 		= 0;
	m_destination_value	= 1;
	m_time_delta		= 0.1;
	m_average_speed		= 0;
	m_deceleration		= 20;				// counts / (s*s) 
}

/* Before calling, Fill in the :
		m_start_value
		m_destination_value
		m_time_delta		
		
	This will generate a speed for every count for speed control in the pid algorithm.
	"RequestedSpeed"
*/
void ControlSpeedCalculator::compute_speeds()
{
	m_average_speed = fabs(m_destination_value - m_start_value) / m_time_delta;
	bool forward = (m_destination_value > m_start_value) ? (m_destination_value) : (m_start_value);
	
	for (int i=0; i<1023; i++)
	{
		if (i > m_destination_value) 
		{
			if (forward)
				m_speed.push_back( -m_average_speed );
			else
				m_speed.push_back( m_average_speed );			
		} else {
			if (forward)
				m_speed.push_back( m_average_speed );
			else
				m_speed.push_back( -m_average_speed );		
		}
	}	
	
	// Now Put in the Deceleration : 
	// 		Let Deceleration be counts/sec and distance be in counts.
	/* Note:  Velocity linearly decreases under constant deceleration.  However,
			  We are not plotting versus the time axis, but position.  We know position 
			  goes by the square verses time.  So it makes sense that the velocity
			  versus position goes like sqrt()			  			  
	*/

	// Update:   speed =  m_deceleration*time + speed_prev
	
	// Distance = 1/2 m_deceleration * (time * time)
	// time = sqrt(2 * Distance / m_deceleration);
	// time = sqrt(2 / m_deceleration);
	float time;
	float time0 = sqrt( 2 / m_deceleration ) ;	 // sqrt(0)
	/*float time1 = sqrt( 2 / m_deceleration ) * sqrt(1);	 // sqrt(1) * time
	float time2 = sqrt( 2 / m_deceleration ) * sqrt(2);	 // sqrt(2) * time
	float time3 = sqrt( 2 / m_deceleration ) * sqrt(3);	 // sqrt(3) * time
	float time4 = sqrt( 2 / m_deceleration ) * sqrt(4);		*/

	m_speed[m_destination_value] = 0;		// Stop at destination.
	int prev_index = m_destination_value;
	int index      = prev_index-1;

	// Prior to destination	
	while ( fabs(m_speed[prev_index]) < fabs(m_average_speed) )
	{
		time = time0 * sqrt(m_destination_value - index);
		float spd = m_deceleration*time + 0;
		if (spd < fabs(m_average_speed))	
			m_speed[index] = spd;
		index--;
		prev_index--;
	}
	
	// After destination ( ie. over-shot )
	prev_index = m_destination_value;
	index      = prev_index+1;
	while ( fabs(m_speed[prev_index]) < fabs(m_average_speed) )
	{
		time = time0 * sqrt(index - m_destination_value );
		float spd = -m_deceleration*time + 0;
		if (spd < fabs(m_average_speed))	
			m_speed[index] = spd;
		index++;
		prev_index++;		
	}
}

void ControlSpeedCalculator::print_speeds()
{
	for (int i=0; i<1023; i++)
		printf( "%d - %6.3f \n", i, m_speed[i] );
}

