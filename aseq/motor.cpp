#include "motor.hpp"




/**********************************************************************
Name	:	Constructor()
Purpose	:	
Inputs	:	mDuty	- [0..1.0] fraction of the motor power (PWM Duty)
Return	:	Amount of Torque 
**********************************************************************/
Motor::Motor(float mMaxRatedTorque, float mStallCurrent)
: MaxRatedTorque(mMaxRatedTorque)
StallCurrent(mStallCurrent)	
Duty(1.0)			
{
	
}


/**********************************************************************
Name	:	calc_motor_torque()
Purpose	:	
Inputs	:	mDuty	- [0..1.0] fraction of the motor power (PWM Duty)
Return	:	Amount of Torque 
**********************************************************************/
float Motor::calc_motor_torque		( float mDuty 			) 
{
	return (MaxRatedTorque * mDuty)
}


