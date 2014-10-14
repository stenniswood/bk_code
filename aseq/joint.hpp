#ifndef _JOINT_HPP_
#define _JOINT_HPP_



class Joint
{
public:
	Joint( float mu_static, float mu_kinetic );
	
	float calc_normal_force		(						);
	float calculate_friction_torque(					);
	float calc_motor_torque		( float mDuty 			);
	float calc_sum_torques		( float mDuty 			);
	float find_motor_duty		( float mRequestedAccel	);

private:
	float mu_static;			// coefficient of friciton when stationary.
	float mu_kinetic;			// coefficient of friction when moving.
}



#endif
