#ifndef _PIVOT_HPP_
#define _PIVOT_HPP_


/* 
	Pivot is a combination of a Joint, Segment, & Motor.
	
	Both are needed to evaluate Tao = Inertia * Accel		
*/
class Pivot
{
public:	
	Pivot						( Joint* mJoint, Segment* mSegment, Motor* mMotor 		);
	float calc_net_accel		( float mDuty											);
	float calc_final_velocity	( float InitialVelocity, float Acceleration, float TimeSlicePeriod);	
	float calc_acceleration		( float RequestedDistance, float InitialVelocity, float Timeslice_Period);
	float calc_avg_velocity		( float Initial_Velocity, float mAcceleration 			);

	void  timeslice				(														);

private:
	Joint*		joint;
	Segment*	segment;	
	Motor*		motor;

}


#endif
