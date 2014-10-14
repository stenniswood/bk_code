#ifndef _MOTOR_HPP_
#define _MOTOR_HPP_

class Motor
{
public:
	Motor(float mMaxRatedTorque, float mStallCurrent);
	float calc_motor_torque( float mDuty );
	
private:
	float MaxRatedTorque;	// inch*lbs  around (1,546 for 82666)
	float StallCurrent;		// 	
	float Duty;				// 
	
}

#endif
