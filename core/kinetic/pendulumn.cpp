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
#include "CAN_base.h"
//#include "CAN_Interface.h"
#include "packer_lcd.h"
#include "packer_motor.h"
//#include "can_txbuff.h"
#include "packer.h"
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
#include "motor_pack.hpp"
#include "vector_file.hpp"
#include "Leg.hpp"
#include "pendulumn.hpp"


float g = 9.8;
float TimeSlicePeriod = 0.05;


Pendulum::Pendulum()
{
	leg_attachment_position = NULL;
}

/**********************************************************************
Name	:	calc_gravity_force_longitudinal_ul() - upper leg
Purpose	:	Calculate the force of gravity along the limb length - "longitudinal" 
			This is used to find the Normal Force for friction calculations.
Inputs	:	HipInfo structure must be filled with Mass & Angle.
Return	:	Component of gravity in the direction of the limb.
*********************************************************************/
float Pendulum::calc_gravity_force_longitudinal()
{
	// g is the hypotenus.
	float gAngle = get_angle_wrt_gravity() * M_PI / 180.;

	float longitudinal_gravity_force  = mass * g * cos(gAngle);	 // parallel force of gravity
	return longitudinal_gravity_force;
}

/**********************************************************************
Name	:	calc_gravity_force_perpendicular_hip()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of gravity (Perpendicular to the limb)
*********************************************************************/
float Pendulum::calc_gravity_force_transverse()
{
	float gAngle = get_angle_wrt_gravity() * M_PI / 180.;
	
	// swing leg gets mass of everything below the hip joint
	float  transverse_gravity_force  = mass * g * sin(gAngle);	 // perpendicular force of gravity
	return transverse_gravity_force;
}

/**********************************************************************
Name	:	calc_centrifugal_force()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of acceleration (radial)
*********************************************************************/
float Pendulum::calc_centrifugal_force()
{
	float c_force = mass * (angular_velocity * angular_velocity) / distance_to_center_of_mass;
	return c_force;
}

/**********************************************************************
Name	:	calc_normal_force()
Purpose	:	Compute the normal force on the joint (for use in friction 
			calculations.  Leg could be stance or swing - different 
			equations in each case.
Inputs	:	HipInfo.Stance_leg	- if True the leg is bearing some weight
			of the robot.  FALSE=> force comes from weight of the leg.
			HipInfo.Angle	- 
			HipInfo.Mass	- 
Return	:	Normal force of pressure on the joint.
*********************************************************************/
float Pendulum::calc_normal_force()
{
	float normal_force = 0.0;
	// due to weight of robot, or leg depending on 
	if (mode == STANCE_LEG)
	{
		// Actual weight of the robot exerted on this hip.
		// Not sure how to do exactly because if there's 2 stance legs,
		// then weight is cut in half.  And if it's a bouncy walk, more
		// weight when it first hits down and less later.
		// If in an elevator, of course this would be problem too.
		// This can be derived from the accelerometer sensor in the ab module.
		//normal_force  = AbInfo.Mass * g;   //calc_weight_of_abdomin_on_leg();
	}
	else 	// swing leg 
	{
		normal_force = calc_gravity_force_longitudinal() + calc_centrifugal_force();
	}
	return normal_force;
}

/**********************************************************************
Name	:	get_joint_frictional_force()
Purpose	:	Calculates the frictional force which counters the motion of the pendulumn.
			Since this is not a ball on a string, but metal on a motor join, friction_force
			exists.
Inputs	:	angular rate
Return	:	Force of acceleration (radial)
*********************************************************************/
#define MIN_SPEED 0.001
float Pendulum::get_joint_frictional_force()
{
	float retval = 0.;
	// due to fluctuations in the POT, a minimal speed is tolerated;  we give
	// static frictional force under that condition.
	BOOL stopped = (angular_velocity < MIN_SPEED) && (angular_velocity > -MIN_SPEED);
	float nforce = calc_normal_force();
	if (stopped)
		retval = mu_static * nforce;
	else
		retval = mu_rolling * nforce;
	return retval;
}

/***********************************************************************
Name	:	calculate_friction_torque()
Purpose	:	This is the torque due to friction in the bearings.
			Our Mu constants will take into account the radius and 
			The angle is perpendicular to motion so sin(angle) = 1
Inputs(HipInfo)	:	AngularSpeed
					Angle
					Stance_leg
					mu_static|mu_kinetic	- coefficient of friction			
Return	:	
***********************************************************************/
float Pendulum::calculate_friction_torque()
{
	float friction_force = get_joint_frictional_force();
	return friction_force;
}

/**********************************************************************
Name	:	calc_gravity_torque()
Purpose	:	
Inputs(HipInfo)	:
			Stance_leg
			Distance_to_center_of_mass
			Angle
Return	:	
*********************************************************************/
float Pendulum::calc_gravity_torque(  )
{
	float gravity_force = 0;
	float gravity_torque = 0;
	float center;
	if (mode==STANCE_LEG)
	{
		// weight of everything above the hip joint
		// center of gravity above the hip
		// sum of torques above hip (assume 0 for now)
		
		// assumption is the lower leg will not move

		// This is the force on the hip joint for friction calculation.
		//float torque 		= AbInfo.Mass * g * sin(mAngle);
		//return torque;
	}
	else
	{
		// Here we don't care about abdominal weight.  It is holding us up.
		// Rather we care about the leg weight (since it is suspended in air).
		float gravity_force = calc_gravity_force_transverse();
		// Cross product = A B sin theta
		gravity_torque 		= gravity_force * distance_to_center_of_mass * sin(angle);
		return gravity_torque;
	}
	return 0;
}

/**********************************************************************
Name	:	calc_motor_torque()
Purpose	:	
Inputs	:	mDuty	- [0..1.0] fraction of the motor power (PWM Duty)
Return	:	Amount of Torque 
**********************************************************************/

float Pendulum::calc_motor_torque( float mDuty )
{
//	return (MaxMotorTorque * mDuty);
	return 0;
}
	
/* When we know the motor duty, we can calculate the total torque. */
// SumTorques = gt + ft + MotorTorque	
// Inertia * Accel = SumTorques = gt + ft + MotorTorque
/**********************************************************************
Name	:	calc_sum_torques()
Purpose	:	Calculates the sum of all torques on the joint. Use to
			find angular acceleration.
			Torques include: Gravity, Friction, Motor torques.
Inputs	:	HipInfo - fill all fields
Return	:	Total Torque on the joint.
*********************************************************************/
float Pendulum::calc_sum_torques( float mDuty )
{
	float gt = calc_gravity_torque( );
	float ft = calculate_friction_torque();
	float mt = calc_motor_torque( mDuty );

	// SumTorques = gt + ft + MotorTorque
	float sum = gt + ft + mt;
	return sum;
}

/**************************************************************************
Name	:	calc_net_accel()
Purpose	:	When we know the motor duty, we can calculate the acceleration.
Inputs(HipInfo)	:
			mDuty	- Duty cycle of Motor fraction [0..1.0] 
			Inertia	- 
Return	:   Acceleration of the joint (degrees/second)/second
**************************************************************************/
float Pendulum::calc_net_accel(float mDuty)
{
	 return (calc_sum_torques(mDuty) / inertia);
}

/* More often we wish to find the motor duty to accomplish a certain goal (net acceleration) */
// inertia * accel = Sum of Torques
// inertia * accel - (SumTorques-MotorTorque) = MotorTorque = MotorDuty * MaxMotorTorque
// inertia * accel - (SumTorques-MotorTorque) = MotorTorque
// RequestedTorque - (SumTorques-MotorTorque) = MotorTorque
// This accleration is angular so the unit will be :
//				(degrees / second * second)
//		or 		(radians / second * second)
/**********************************************************************
Name	:	find_motor_duty()
Purpose	:	Find the duty cycle needed to obtain the requested acceleration
			(takes into account - Inertia, all torques, 
Inputs	:	HipInfo
Return	:	motor duty
*********************************************************************/
float Pendulum::find_motor_torque( float mRequestedSpeed  )
{
	/* Pendulum has a current position.
	   We have a requested position and time to be there by.
	   RequestedVelocity will be average speed.

		Easier to control by accelerating to the velocity without regard to position.	   
	   We are in a difference equation and operate on derivatives.  The integration 
	   will happen by the physics of the situation and therefore meet the positional
	   requirements.  RequestedVelocity was previously calculated to achieve the position.
	*/	
	float delta_speed = (mRequestedSpeed - angular_velocity);
	
	// 
	float gt = calc_gravity_torque( );
	float ft = calculate_friction_torque();
	float sum = gt+ft;
	
	// accel * inertia = torque
	//(delta_speed * inertia) = gt + ft + mt
	float mt = (delta_speed * inertia) - gt - ft;
	return mt;
}
float Pendulum::adjust_constants( float mRequestedVelocity, 
								   float mFinalPosition )								   
{
	/* Main parameters for adjusting are:
			mu_static, mu_rolling, inertia.
	   We assume measured values are fairly accurate for:
	   		mass, length.
	   This means there are always at least 2 numbers which need to be honed.
	   
	   If we over estimate inertia, 
	   		large changes in requested speed will give extra large motor power. spurts
	   		or small change in speed gives too much power. jerks.
	   If we under estimate inertia,
	   		large changes in requested speed will give lag, slow response.
	   		small changes in requested speed will give under powered or almost no response/movement.
	   		
	   If we over estimate mu_rolling,
	   		we'll expect drag, but leg keeps gliding.  or again overpowered
	   		
	   If we under estimate mu_rolling,
	   		we'll expect .  leg will stop sooner than expected, or be slower.
	   		
	*/	
	float AchievedVelocity = (mFinalPosition - angle) / TimeSlicePeriod;
	float accel_error = (AchievedVelocity-mRequestedVelocity);
	if (accel_error>0)
	{
		// would be neat to have a binary search on the parameters.  ie. start small, 
		// then estimate huge, then little bigger, then large, then medium.
		
	}
	else 
	{
	}
	
}
float Pendulum::find_motor_duty(float mRequestedAccel)
{
	// Requested Torque
	float RequestedTorque = (mRequestedAccel * inertia);

	// We use any arbitrary motor duty (because it is subtracted, it does not effect the sum below:
	float sum_minusMotor = calc_sum_torques( 0.5 ) - calc_motor_torque( 0.5 );

	// 
	float MotorTorque = (RequestedTorque - sum_minusMotor);
	float MotorDuty   = (MotorTorque / MaxMotorTorque);
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
/**********************************************************************
Name	:	calc_avg_velocity()
Purpose	:	Calculate average velocity
Inputs	:	mAcceleration
			TimeSlicePeriod
			Initial_Velocity
Return	:	
**********************************************************************/
float Pendulum::calc_avg_velocity( float Initial_Velocity, float mAcceleration )
{
	float average = ((mAcceleration * TimeSlicePeriod) / 2) + Initial_Velocity;
	return average;
}

/**********************************************************************
Name	:	calc_final_velocity()
Purpose	:	
Inputs	:
Return	:
***********************************************************************/
float Pendulum::calc_final_velocity(float InitialVelocity, float Acceleration, float TimeSlicePeriod)
{
	// Final Velocity = Initial Velocity + a * t
	float final =  InitialVelocity + Acceleration*TimeSlicePeriod;
	return final;
}

/* 
   Find the acceleration needed to reach the destination in the timeslice Period as close
   as possible.
 */
/**********************************************************************
Name	:	calc_acceleration()
Purpose	:	
Inputs	:
Return	:
*********************************************************************/
float Pendulum::calc_acceleration(float RequestedDistance, float InitialVelocity, float Timeslice_Period)
{
	// a = (Requested Distance - Initial Velocity*Timeslice_Period)*2 / Timeslice_Period*Timeslice_Period
	float projected_distance = InitialVelocity*Timeslice_Period;
	float discrepancy = RequestedDistance - projected_distance;
	float accel = (discrepancy * 2) / (Timeslice_Period*Timeslice_Period);
	return accel;
}


void Pendulum::SavePreferences( Preferences* mpref )
{
	char str[64];
	sprintf(str, "length=%6.2f\n", length);		
	mpref->write( str );

	sprintf(str, "inertia=%6.2f\n", inertia);		
	mpref->write( str );

	sprintf(str, "mass=%6.2f\n", mass);		
	mpref->write( str );

	sprintf(str, "coefficient_static=%6.2f\n", mu_static );		
	mpref->write( str );

	sprintf(str, "coefficient_rolling=%6.2f\n", mu_rolling );		
	mpref->write( str );

	sprintf(str, "CenterOfMass_distance=%6.2f\n", distance_to_center_of_mass );		
	mpref->write( str );
}

void Pendulum::LoadPreferences( Preferences* mpref )
{
	char str[64];
	// READ INERTIAS:
	mpref->readln_nb( str );
	length = mpref->getFloatValue( str );

	mpref->readln_nb( str );
	inertia = mpref->getFloatValue( str );
	
	mpref->readln_nb( str );
	mass = mpref->getFloatValue( str );	
	
	mpref->readln_nb( str );
	mu_static = mpref->getFloatValue( str );	

	mpref->readln_nb( str );
	mu_rolling = mpref->getFloatValue( str );	

	mpref->readln_nb( str );
	distance_to_center_of_mass = mpref->getFloatValue( str );
}


/*
INPUTS:
	float RequestedDestination = 20;	// An angle in degrees +/- 
	Angle			// An angle in degrees +/- 
	AngularSpeed
	Timeslice_Period = 100ms
*/
/**********************************************************************
Name	:	timeslice_hip()
Purpose	:	
Inputs	:
Return	:
*********************************************************************/
void Pendulum::timeslice_hip()
{
	// INPUTS
	float RequestedDestination = 20;	// An angle in degrees +/- 
	float Timeslice_Period     = 50;	// 50=2o hertz; 40=25hertz

	// CODE:
	float distance = (RequestedDestination - angle);
	float Velocity = angular_velocity;	// convert to rps or dps? (3.14/180);
	float acceleration = calc_acceleration( distance, Velocity, Timeslice_Period );

	// Check for exceeding the Max Acceleration:
	float MaxAcceleration = MaxMotorTorque / inertia;
	if (acceleration > MaxAcceleration)
		acceleration = MaxAcceleration;

	float motor_duty = find_motor_duty(acceleration) * 1000;	// tenths of percent
	
	//pack_motor_duty( &msg1, find_motor_duty);
	// AddToSendList( &msg1 );
}

