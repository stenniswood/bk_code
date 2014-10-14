/* Simple Pendulum motion:

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
(Inertia * Acceleration) = (MaxMotorTorque * (Duty-PWM(maintainance_speed)/100) + (mass*gravity*sin(hip angle)) + (mu * normal (depends on stance leg or swing leg))
Solve for Duty:
Duty = Maintain Speed + Desired Acceleration + Gravity + Frictional (static or kinetic)

Motor Power = (K * vel) + (Inertia * Acceleration) + (mass*gravity*sin(hip angle))
	   
*/

/* Simplifications:
	When computing the hip angle torques, just use a straight knee for 
	inertia.  Not true but will get us close.  so the knee and ankle are always
	at 0 degrees.
*/

#include "angular_vectors.h"

/* Humorous, knee motor, lower leg, angle motor & foot - all combined. */

/* lower leg, ankle motor, foot - all combined */
float lower_leg_center_of_mass = 12.0;


float calc_gravity_force_parallel_ab()
{
	// swing leg gets mass of everything below the hip joint.
	float gravity_force  = AbInfo.Mass * g * cos(AbInfo.Angle);	 // perpendicular force of gravity
	return gravity_force;
}

float calc_gravity_force_perpendicular_ab()
{
	// Angle of abdomin wrt gravity:
	float angle = abInfo.Angle - AbInfo.Angle;
	// swing leg gets mass of everything below the hip joint.
	float gravity_force  = AbInfo.Mass * g * sin(angle);	 // perpendicular force of gravity
	return gravity_force;
}

/* The force always opposed to the direction of movement. */
float calculate_frictional_drag()
{
	if (AbInfo.Stance_leg)
	{
		// Actual weight of the robot exerted on this hip.
		// Not sure how to do exactly because if there's 2 stance legs,
		// then weight is cut in half.  And if it's a bouncy walk, more
		// weight when it first hits down and less later.
		// If in an elevator, of course this would be problem too.
		float normal_force  = weight_abdomin * g;//calc_weight_of_abdomin_on_leg();
		if (AbInfo.AngularSpeed==0)
			friction_force  = AbInfo.mu_static * normal_force;
		else
			friction_force =  AbInfo.mu_static * normal_force;
		return friction_force;
	}
	else 	// swing leg
	{
		float normal_force_upper = calc_gravity_force_parallel_hip();
		// add in weight of lower leg
		float normal_force_lower = calc_gravity_force_parallel_knee();
		// add in weight of foot
		float normal_force_foot  = calc_gravity_force_parallel_foot();
		float total_force =   normal_force_upper + normal_force_lower + normal_force_foot;
		float friction_force;
		if (AbInfo.AngularSpeed==0)
			friction_force = AbInfo.mu_static * total_force;
		else
			friction_force = AbInfo.mu_kinetic * total_force;
		return friction_force;
	}
	return normal_force;
}

float calc_gravity_torque( float mAngle )
{
	float gravity_force = 0;
	float gravity_torque = 0;
	float center
	if (AbInfo.Stance_leg)
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
		gravity_torque 		= gravity_force * AbInfo.distance_to_center_of_mass * sin(AbInfo.Angle); 
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

