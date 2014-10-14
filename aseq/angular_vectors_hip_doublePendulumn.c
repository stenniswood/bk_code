/******************************************************************************* 
Double Pendulum motion:

There are several problems with the isolated hip (single pendulmn) equations 
presented in "angular_vectors_isolated_hip.c"

1) Need to incorporate the effects of the knee & lower leg.
2) Deceleration has to be built into the position vectors.  
	Update Processing code to do this may be wise. Deceleration
	can be done there without regard to friction, gravity, inertia, etc.

This file will solve #1 issue!
==============================================================
There is a lot of chaining going on in these equations. Want to
be able to compute each entity separately and a nice convient
way of chaining them.  Object oriented may be a better approach.

Base class Joint & Segment
And a combiner class: Pivot?  

Hip->Joint
Knee->Joint
Ankle->Joint

==============================================================
Assume a non-moving lower leg (fixed at 0deg)
What are the effects on the upper leg?
	a) Gravity Torque is larger (because extra weigt & extra long distance to center of mass)
	b) Overall inertia is bigger (so speeding up and slowing down take longer)
	c) The frictional forces in the hip will be larger due to the increased normal force.
	d) Any thing else?  Other than that it seems to be a regular single pendulumn longer
	   and heavier.
*** These need to be incorporated into "isolated_hip.c" equations.

Now, with a swinging knee...
	- Start to think about angular & linear momentum.
	- previous equations did not directly involve momentum however. so want to account
	for without using conservation of momentum if possible.
	- i know intuitively that swinging a knee forward, then stopping can result in a torque 
	on the hip raising it forward (against gravity).  
	
	okay a better way to look at it then is equal and opposite forces on the knee motor.
	As the knee give a torque to the lower leg, that torque is also felt in opposite direction
	on the upper leg.  
	   
*/

/* Simplifications:
	When computing the hip angle torques, just use a straight knee for 
	inertia.  Not true but will get us close.  so the knee and ankle are always
	at 0 degrees.
*/
#include "angular_vectors.h"

/**********************************************************************
Name	:	calc_gravity_force_parallel_hip()
Purpose	:	Calculate the force of gravity along the limb length.  
			This is used to find the Normal Force for friction calculations.
Inputs	:	HipInfo structure must be filled with Mass & Angle.
Return	:	Component of gravity in the direction of the limb.
*********************************************************************/
float calc_gravity_force_parallel_hip()
{
	// swing leg gets mass of everything below the hip joint.
	float gravity_force  = HipInfo.Mass * g * cos(HipInfo.Angle);	 // perpendicular force of gravity
	return gravity_force;
}

/**********************************************************************
Name	:	calc_gravity_force_perpendicular_hip()
Purpose	:	Calculates the component of gravity perpendicular to the limb.
			This component is what gives the torque due to gravity.
Inputs	:	HipInfo must be filled with Mass & Angle
Return	:	Force of gravity (Perpendicular to the limb)
*********************************************************************/
float calc_gravity_force_perpendicular_hip()
{
	// swing leg gets mass of everything below the hip joint.
	// To get gravity Torque, we have to compute the effective center of mass
	// let's leave it in components.  So function will remain unchanged.
	//float mass = (HipInfo.Mass + KneeInfo.Mass);
   	float gravity_force  = HipInfo.Mass * g * sin(HipInfo.Angle);	 // perpendicular force of gravity
	return gravity_force;
}

/**********************************************************************
Name	:	calc_avg_velocity()
Purpose	:	Calculate average velocity
Inputs	:	mAcceleration
Return	:	
**********************************************************************/
float calculate_friction()
{
	if (HipInfo.Stance_leg)
	{
		// Actual weight of the robot exerted on this hip.
		// Not sure how to do exactly because if there's 2 stance legs,
		// then weight is cut in half.  And if it's a bouncy walk, more
		// weight when it first hits down and less later.
		// If in an elevator, of course this would be problem too.
		float normal_force  = weight_abdomin * g;//calc_weight_of_abdomin_on_leg();
		if (HipInfo.AngularSpeed==0)
			friction_force  = HipInfo.mu_static * normal_force;
		else
			friction_force =  HipInfo.mu_static * normal_force;
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
		if (HipInfo.AngularSpeed==0)
			friction_force = HipInfo.mu_static * total_force;
		else
			friction_force = HipInfo.mu_kinetic * total_force;
		return friction_force;
	}
	return normal_force;
}

/**********************************************************************
Name	:	calc_avg_velocity()
Purpose	:	Calculate average velocity
Inputs	:	mAcceleration
Return	:	
**********************************************************************/
float calc_gravity_torque( float mAngle )
{
	float gravity_force = 0;
	float gravity_torque = 0;
	float center
	if (HipInfo.Stance_leg)
	{
		// weight of everything above the hip joint.
		// center of gravity above the hip.
		// sum of torques above hip (assume 0 for now)
		
		// assumption is the lower leg will not move
/* the dream last night:
It was mom & I making tons of money.  The two that had been least respected in our family.
and in society as a whole.
Mom bought a rolls royce.
just 2 people making millions.
	The people that had a robot with small problem.  didn't believe it was me that made it.
	they all wondered Who is the beyond kinetics people?  where are they?  Where is all this tech
	coming from.  It's like it really came from heaven.  Like the mythical stork bringing a baby.
	they just appeared.  because nobody would believe that it came directly from God to my mind
	and we know steve can't program worth beans - his code ends up all buggy.

They turned to you tube videos to get the answer.  While it was playing, I hooked up my USB to their device
and made there's work.  like a doctor with a stethescope.
	
*/
		// This is the force on the hip joint for friction calculation.
		float torque_ab  = abdomin_mass * g * sin(mAngle);
		float torque_ul = abdomin_mass * g * sin(mAngle);
		float torque_ul = abdomin_mass * g * sin(mAngle);

		return torque;
	}
	else
	{
		// Here we don't care about abdominal weight.  It is holding us up.
		// Rather we care about the leg weight (since it is suspended in air).
		float gravity_force_ul = calc_gravity_force_perpendicular_hip();
		float gravity_force_ll = calc_gravity_force_perpendicular_knee();

		// Cross product = R F sin Theta
		// Torque due to upper leg:
		gravity_torque = gravity_force_ul * HipInfo.distance_to_center_of_mass * sin(HipInfo.Angle)
			// Plus Torque due to lower leg's weight (ie. assuming lowerleg is directly along gravity) 
			+ gravity_force_ll * (HipInfo.upper_leg_length) * sin(HipInfo.Angle)
			// Plus Toruqe due to lower legs's angle wrt gravity:
			+ gravity_force_ll * (KneeInfo.distance_to_center_of_mass) * sin(HipInfo.Angle+KneeInfo.Angle);

		return gravity_torque;
	}
	return 0;
}

/**********************************************************************
Name	:	calc_avg_velocity()
Purpose	:	Calculate average velocity
Inputs	:	mAcceleration
Return	:	
**********************************************************************/
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

/**********************************************************************
Name	:	calc_avg_velocity()
Purpose	:	Calculate average velocity
Inputs	:	mAcceleration
Return	:	
**********************************************************************/
float calc_static_frictional_torque()
{
}

/**********************************************************************
Name	:	calc_avg_velocity()
Purpose	:	Calculate average velocity
Inputs	:	mAcceleration
Return	:	
**********************************************************************/
float calc_kinetic_frictional_torque()
{
}
