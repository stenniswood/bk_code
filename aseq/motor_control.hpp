#ifndef _MOTOR_CONTROL_HPP_
#define _MOTOR_CONTROL_HPP_

#include <vector>
#include "motor.hpp"

using namespace std;

/* This class is derived from Motor and contains the functions/algorithm for 
controling the motor's speed and power

NO LOAD:
Speed of the 82666 Motor with no load is:  0.683 sec/rev
==> 1.45 revs/sec.
==> 88   rpm
==> with 512 counts per rev encoder, it is:  15 counts / 20ms timeslice.

90 degree change = 0.25rev / 1.45 = 0.172 sec  noload max motor capability.
However we want to target something less than that for now.  Because
experimenting and don't want parts flying off!
90 in 0.5 sec should be reasonable max speed.


Now at 50% duty, the speed is about 14 counts/20ms timeslice!
In other words under no load, don't bother with accel/decel at all!
*/

class ControlSpeedCalculator 
{
public:
	ControlSpeedCalculator();
	void	compute_speeds();
	void	print_speeds  ();
		
	int		m_start_value;	
	int		m_destination_value;
	float	m_time_delta;
	float	m_average_speed;
	float	m_deceleration;			// counts / (s*s)
	
	vector<float>	m_speed;
};

class MotorControl : public Motor
{
public:
	MotorControl();
	MotorControl(float mMaxRatedTorque, float mStallCurrent);

	void	Initialize				( 						);
	bool	init_calculator			(						);
	virtual bool read_config_data	( Preferences& mprefs, int mIndex );

	bool	is_breaking_region		( word mCount			);
	void	run_algorithm			( 						);
	
	void  	set_destination			( int mDestinationCount, float mRequestedSpeed );
	bool 	destination_reached 	( float mTolerance 		);	
	float 	compute_motor_torque	( float mDuty 			);

	float 	compute_duty		  	( 					 	);		// PID
	float	compute_gravity_boost 	( 						);
	float	compute_braking_speed 	( 				 		);
	float	compute_accel_speed 	( 						);
	float	get_control_speed		( 						);	

	float	compute_stopping_distance( float mSpeed,  float mDeceleration );
	float  	compute_reaction_torque  ( Motor& mMotor, float mAlpha        );
	
	void  	send_speed_pid			(  						);
	virtual int  	handle_CAN_message	    ( struct sCAN* mMsg 	);	// handles incoming msg

	// CHANGE FREQUENTLY (ie realtime) : 
	word	StartCount;		 	 // Reading when the send_speed() was called.
	word	DestinationCount; 	 // Reading when the send_speed() was called.	
	word 	BeginBrakingCount;	 // Trigger for breaking (pid control)
	bool	DestinationReached;	 // 

	float	NextAngleDeg;			// Destination when in Angle mode. in Degrees * 10 

	//float	RequestedSpeed;			// Counts per second

	// PID CONTROL VARIABLES:
	float	position_error_sum;		// Integral
	float	position_error_prev;	// 
	float	position_error_deriv;	// Derivative

	float	speed_error_sum;		// Integral
	float	speed_error_prev;		// 
	float	speed_error_deriv;		// Derivative

	void	reset_pid  ();
	void	init_params();
	float	K_const;				// multiply by sin theta gives the torque.
	float	Kp_position_alpha;
	float	Kp_speed_alpha;
	float	Ki_position_alpha;
	float	Ki_speed_alpha;
	float	Kd_position_alpha;
	float	Kd_speed_alpha;
	
	struct timeval submitted;
	struct timeval completed;
	ControlSpeedCalculator calc;	// 

	// Semi Constant Variables (change very infrequently)
	float	deceleration_rate_cpss;	// Depends on the load.  How to determine?! algorithm to sense?

private:
};


#endif
