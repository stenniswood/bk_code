/* 
In this file.  Let's stick just to the hip for now. It's as if the knee and foot were 
not attached.

Simple Pendulum motion:

Simple Linear Motion (no gravity)
Has an initial velocity & a distance to go.
We know the max deceleration.
Specify percentage of max decel.

Max_Decel = 1 yard per second per second

Typical Problem:
	stop in 5 yards.
	stop at 50% max.
	Vel = 100 yards / 30 seconds = 100/30 = 3.3333 yards/sec

Given Vel :
Time_to_stop = Vel / (Max_Decel * stop_rate) = 3.333 / (0.5) = 6.666 seconds
distance = at^2 /2 = (0.5) * (6.666)*(6.666) / 2 = 11.1 yards

Several Cases:
	a) Distance to_stop > distance requested
	b) Distance to stop < distance requested (maintain constant Vel until)
	c) Distance == Distance requested.
Solved
==============================================================
Extended problem:
	Starting at known Pos & Vel.
	Ending at a known Pos & Vel.  
In other words, same as before but the final velcotiy is not necessarily 
 zero.  For each timeslice, the acceleration will be constant.

a = Vel_Final - Vel_Initial

 Final Vel = InitVel + a * t
Where t is known time slice period.
a is computed based on change of velocity vectors.

======================================
Given: a sequence of vector postions.
Calculate : sequence of velocity vectors.  The delta between positions.

The whole reason for these calculations are because the inertias may be 
big compared to the torques.

Is there a short cut?
	Don't worry about accel/decel
	Just specify the angles and make duty based on PID
	Yes but bad motion quality.  ignores gravity so swings are weak going up
		and overly fast going down hill.
	Biggest downgrades due to gravity, static/kinetic friction

Driving is torque-time based
Sum of forces : 
Net Torque 				 = Motor Torque + Gravity Torque + Frictional (static or kinetic)
(Inertia * Acceleration) = (MaxMotorTorque * (Duty-PWM(maintainance_speed)/100) + 
			(mass*gravity*sin(hip angle)) + (mu * normal (depends on stance leg or swing leg))

Solve for Duty:
Duty = Maintain Speed + Desired Acceleration + Gravity + Frictional (static or kinetic)

Motor Power = (K * vel) + (Inertia * Acceleration) + (mass*gravity*sin(hip angle))
	   
*/

/* Simplifications:
	When computing the hip angle torques, just use a straight knee for 
	inertia.  Not true but will get us close.  so the knee and ankle are always
	at 0 degrees.
*/
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_Interface.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "packer_motor.h"
#include "motor_vector.h"
#include "config_file.h"
#include "angular_vectors.h"



float calc_gravity_force_parallel_hip()
{
	// swing leg gets mass of everything below the hip joint.
	float gravity_force  = HipInfo.Mass * g * cos(HipInfo.Angle);	 // parallel force of gravity
	return gravity_force;
}

float calc_gravity_force_perpendicular_hip()
{
	// swing leg gets mass of everything below the hip joint.
	float gravity_force  = HipInfo.Mass * g * sin(HipInfo.Angle);	 // perpendicular force of gravity
	return gravity_force;
}

float calc_normal_force()
{
	float normal_force = 0.0;
	// due to weight of robot, or leg depending on 
	if (HipInfo.Stance_leg)
	{
		// Actual weight of the robot exerted on this hip.
		// Not sure how to do exactly because if there's 2 stance legs,
		// then weight is cut in half.  And if it's a bouncy walk, more
		// weight when it first hits down and less later.
		// If in an elevator, of course this would be problem too.
		// This can be derived from the accelerometer sensor in the ab module.
		normal_force  = weight_abdomin * g;   //calc_weight_of_abdomin_on_leg();
	}
	else 	// swing leg 
	{
		normal_force = calc_gravity_force_parallel_hip();
	}
	return normal_force;
}

/* This is the Torque.  
Our Mu constants will take into account the radius and 
the angle is perpendicular to motion so sin(angle) = 1
*/
float calculate_friction_torque()
{
	float normal_force = calc_normal_force();
	if (HipInfo.AngularSpeed==0)
		friction_force = HipInfo.mu_static * normal_force;
	else
		friction_force = HipInfo.mu_kinetic * normal_force;
	return normal_force;
}

float calc_gravity_torque( float mAngle )
{
	float gravity_force = 0;
	float gravity_torque = 0;
	float center
	if (HipInfo.Stance_leg)
	{
		// weight of everything above the hip joint
		// center of gravity above the hip
		// sum of torques above hip (assume 0 for now)
		
		// assumption is the lower leg will not move

		// This is the force on the hip joint for friction calculation.
		float torque 		= abdomin_mass * g * sin(mAngle);
		return torque;
	}
	else
	{
		// Here we don't care about abdominal weight.  It is holding us up.
		// Rather we care about the leg weight (since it is suspended in air).
		float gravity_force = calc_gravity_force_perpendicular();
		// Cross product = A B sin theta
		gravity_torque 		= gravity_force * HipInfo.distance_to_center_of_mass * sin(HipInfo.Angle);
		return gravity_torque;
	}
	return 0;
}

float calc_motor_torque( float mDuty )
{
	return (MaxMotorTorque * mDuty)
}

/* When we know the motor duty, we can calculate the total torque. */
// SumTorques = gt + ft + MotorTorque	
// Inertia * Accel = SumTorques = gt + ft + MotorTorque
float calc_sum_torques( float mDuty )
{
	float gt = 	calc_gravity_torque();
	float ft = calc_friction_torque();
	float mt = calc_motor_torque(mDuty);

	// SumTorques = gt + ft + MotorTorque
	float sum = gt + ft + mt;
		
	return sum;
}

/* When we know the motor duty, we can calculate the acceleration. */
float calc_net_accel(float mDuty)
{
	 return (calc_sum_torques(mDuty) / HipInfo.Inertia);
}

/* More often we wish to find the motor duty to accomplish a certain goal (net acceleration) */
// inertia * accel = Sum of Torques
// inertia * accel - (SumTorques-MotorTorque) = MotorTorque = MotorDuty * MaxMotorTorque
// inertia * accel - (SumTorques-MotorTorque) = MotorTorque
// RequestedTorque - (SumTorques-MotorTorque) = MotorTorque
// This accleration is angular so the unit will be :
//				(degrees / second * second)
//		or 		(radians / second * second)
float find_motor_duty(float mRequestedAccel)
{
	// Requested Torque
	float RequestedTorque = (mRequestedAccel * HipInfo.Inertia);

	// We use any arbitrary motor duty (because it is subtracted, it does not effect the sum below:
	float sum_minusMotor = calc_sum_torques( 0.5 ) - calc_motor_torque( 0.5 );

	// 
	float MotorTorque = (RequestedTorque - sum_minusMotor);
	float MotorDuty = (MotorTorque / MaxMotorTorque);
	return MotorDuty;
}


/***********************************************************
 *** Now let's include initial & final velocity.  And
 *** timeslice period.
 **********************************************************/

/* We want to get to a destination in a certain amount of time.
   Distance = velocity * time

   At start of time slice, we'll have a certain initial velocity.
   At the end of timeslice, we'll have a final velocity.
   Initial Velocity (at Time0) AND Final Velocity (at Time1)

   Acceleration = (delta Velocity / delta Time);
   We approximate with constant velocity during that timeslice 
   (because the motor torque will be constant & timeslices are small 
   so gravity change over a small angle will be slight).

   Usually we know our initial velocity and position.  And know the 
   distance.  We can predict ahead based on Init Velocity.

	Init Velocity * Timeslice_Period = Distance

	We compare to the requested Distance:
	delta = (Requested Distance - Distance)

	Better to predict based on average velocity.  
	Final Velocity = Initial Velocity + a * t
	
	Avg Velocity = (Final - Initial Velocity ) / 2 + Initial Velocity
	             = (a * t) / 2 + Initial Velocity

	Requested Distance = (Avg Velocity * Timeslice_Period)
					   = ((a * t) / 2 + Initial Velocity ) * Timeslice_Period
					   = (a*Timeslice_Period*Timeslice_Period) / 2 + Initial Velocity*Timeslice_Period
	solve for a:
	a = (Requested Distance - Initial Velocity*Timeslice_Period)*2 / Timeslice_Period*Timeslice_Period

	If a>MaxMotorAcceleration
		a = MaxMotorAcceleration
		and compute the distance gap (save for a rainy day - ie. next timeslice or later will eventually catch up
		to where we're supposed to be.  The robot motions won't exactly match the requested (but not so far off either)


*/

float calc_avg_velocity( float Initial_Velocity, float mAcceleration )
{
	float average = ((mAcceleration * TimeSlicePerido) / 2) + Initial_Velocity;
	return average;
}

float calc_final_velocity(float InitialVelocity, float Acceleration, float TimeSlicePeriod)
{
	// Final Velocity = Initial Velocity + a * t
	float final =  InitialVelocity + Acceleration*TimeSlicePeriod;
	return final;
}

/* 
   Find the acceleration needed to reach the destination in the timeslice Period as close
   as possible.
 */
float calc_acceleration(float RequestedDistance, float InitialVelocity, float Timeslice_Period)
{
	// a = (Requested Distance - Initial Velocity*Timeslice_Period)*2 / Timeslice_Period*Timeslice_Period
	float projected_distance = InitialVelocity*Timeslice_Period;
	float discrepancy = RequestedDistance - projected_distance;
	float accel = (discrepancy * 2) / (Timeslice_Period*Timeslice_Period);
	return accel;
}


/*
INPUTS:
	float RequestedDestination = 20;	// An angle in degrees +/- 
	HipInfo.Angle			// An angle in degrees +/- 
	HipInfo.AngularSpeed
	Timeslice_Period = 100ms
*/
void timeslice_hip()
{
	// INPUTS
	float RequestedDestination = 20;	// An angle in degrees +/- 
	float Timeslice_Period     = 50;	// 50=2o hertz; 40=25hertz

	// CODE:
	float distance = (RequestedDestination - HipInfo.Angle);
	float Velocity = HipInfo.AngularSpeed;	// convert to rps or dps? (3.14/180);
	float acceleration = calc_acceleration( distance, Velocity, Timeslice_Period );

	// Check for exceeding the Max Acceleration:
	float MaxAcceleration = MaxMotorTorque / HipInfo.Inertia;
	if (acceleration > MaxAcceleration)
		Acceleration = MaxAcceleration;
	
	float find_motor_duty = find_motor_duty(acceleration) * 1000;	// tenths of percent
	
	//pack_motor_duty( &msg1, find_motor_duty);
	// AddToSendList( &msg1 );
}

/* 	This completes it for
	     1 motor on a single pendulumn

	Further analysis will be incorporating the lower leg which is attached
*/

