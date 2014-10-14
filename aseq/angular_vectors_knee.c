/* Simple Pendulum motion:
==============================================================
Given	  : a sequence of vector postions.
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
(Inertia * Acceleration) = (MaxMotorTorque * (Duty-PWM(maintainance_speed)/100) + (mass*gravity*sin(hip angle)) + (mu * normal (depends on stance leg or swing leg))
Solve for Duty:
Duty = Maintain Speed + Desired Acceleration + Gravity + Frictional (static or kinetic)

Motor Power = (K * vel) + (Inertia * Acceleration) + (mass*gravity*sin(hip angle))
	   
*/

/* Simplifications:
	When computing the hip angle torques, just use a straight knee for 
	inertia.  Not true but will get us close.  so the knee and ankle are always
	at 0 degrees.
   -------------------------------------------------
   
*/	
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "bk_system_defs.h"
			
#include "angular_vectors.h"


float calc_gravity_force_parallel_knee()
{
	// swing leg gets mass of everything below the hip joint.
	float gravity_force  = KneeInfo.Mass * g * cos(KneeInfo.Angle);	 // perpendicular force of gravity
	return gravity_force;
}

float calc_gravity_force_perpendicular_knee()
{
	// swing leg gets mass of everything below the hip joint.
	float gravity_force  = KneeInfo.Mass * g * sin(KneeInfo.Angle);	 // perpendicular force of gravity
	return gravity_force;
}

// 
float calculate_friction()
{
	/* First we calculate the frictional force.
	   Then we need to turn that into a torque.
	   So where is the friction?  
			It's the wheel in the motor with greese on it.			 // torque radius = wormgear radius.
			It's also the turning of the motor spiral against the worm gear.
			and then any friction against the metal/plastic cladding.
			All of these should factor into the mu constants!
			So that the frictional force is the frictional torque.
	*/
	if (KneeInfo.Stance_leg)
	{
		// Actual weight of the robot exerted on this hip.
		// Not sure how to do exactly because if there's 2 stance legs,
		// then weight is cut in half.  And if it's a bouncy walk, more
		// weight when it first hits down and less later.
		// If in an elevator, of course this would be problem too.
		float normal_force  = g* (weight_abdomin + HipInfo.Mass);  //calc_weight_of_abdomin_on_leg();
		if (KneeInfo.AngularSpeed==0)
			friction_force  = normal_force * KneeInfo.mu_static;
		else
			friction_force = normal_force * KneeInfo.mu_static;
		return friction_force;
	}
	else 	// swing leg
	{
		// add in weight of lower leg
		float normal_force_lower  = calc_gravity_force_parallel_knee();
		// add in weight of foot
		float normal_force_foot  = calc_gravity_force_parallel_foot();
		float total_force =   normal_force_upper + normal_force_lower + normal_force_foot;
		float friction_force = KneeInfo.mu_static * total_force;
		if (KneeInfo.AngularSpeed==0)
			friction_force = KneeInfo.mu_static * total_force;
		else
			friction_force = KneeInfo.mu_kinetic * total_force;
		return friction_force;
	}
	return normal_force;
}

float calc_gravity_torque( float mAngle )
{
	float gravity_force = 0;
	float gravity_torque = 0;
	float center
	if (KneeInfo.Stance_leg)
	{
		// weight of everything above the hip joint.
		// center of gravity above the hip.
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
		// Cross product = A B sin Theta
		gravity_torque 		= gravity_force * KneeInfo.distance_to_center_of_mass * sin(KneeInfo.Angle); 
		return gravity_torque;
	}
	return 0;
}

float calc_normal_force()
{
	float normal = 0.0;
	// due to weight of robot, or leg depending on 
	if (Stance_leg)
	{
		// weight of everything above the hip joint.
	}
	else
	{
		// weight of everything below the hip joint.
	}
	return normal;
}

float calc_static_frictional_torque()
{
}

float calc_kinetic_frictional_torque()
{
}

