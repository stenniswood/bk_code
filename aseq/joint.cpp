

#include "joint.hpp"


/**********************************************************************
Name	:	calc_normal_force()
Purpose	:	Compute the normal force on the joint (for use in friction 
			calculations.  Leg could be stance of swing - different 
			equations in each case.
Inputs	:	HipInfo.Stance_leg	- if True the leg is bearing some weight
			of the robot.  FALSE=> force comes from weight of the leg.
			HipInfo.Angle	- 
			HipInfo.Mass	- 
Return	:	Normal force of pressure on the joint.
*********************************************************************/
float Joint::calc_normal_force		(						) 
{
	float normal_force = 0.0;
	// due to weight of robot, or leg depending on 
	if (Stance_leg)
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
float Joint::calculate_friction_torque(						) 
{
	float normal_force = calc_normal_force();
	if (HipInfo.AngularSpeed==0)
		friction_force = HipInfo.mu_static * normal_force;
	else
		friction_force = HipInfo.mu_kinetic * normal_force;
	return normal_force;
}

/**********************************************************************
Name	:	calc_motor_torque()
Purpose	:	
Inputs	:	mDuty	- [0..1.0] fraction of the motor power (PWM Duty)
Return	:	Amount of Torque 
**********************************************************************/
float Joint::calc_motor_torque		( float mDuty 			) 
{
	return (MaxMotorTorque * mDuty)
}

/**********************************************************************
Name	:	calc_sum_torques()
Purpose	:	Calculates the sum of all torques on the joint. Use to
			find angular acceleration.
			Torques include: Gravity, Friction, Motor torques.
Inputs	:	HipInfo - fill all fields
Return	:	Total Torque on the joint.
*********************************************************************/
float Joint::calc_sum_torques		( float mDuty 			) 
{
	float gt = calc_gravity_torque();
	float ft = calc_friction_torque();
	float mt = calc_motor_torque(mDuty);

	// SumTorques = gt + ft + MotorTorque
	float sum = gt + ft + mt;
	return sum;
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
float Joint::find_motor_duty		( float mRequestedAccel	) 
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


