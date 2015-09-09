#ifndef _MOTOR_HPP_
#define _MOTOR_HPP_

/* NO LOAD:
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

struct sStopInfo
{
	BOOL	Enabled;
	float	Angle;		// stores angle of stop in degrees
	word	PotValue;	// stores Pot value corresponding to Angle.	
};

// Standard RadioShack Pots give 300 degree range.  
// BigMotorBoard Firmware gives 1024 counts.
const float CountsPerDegree = 1024. / 300.;
const float DegreesPerCount = 300. / 1024.;

const float MAX_MOTOR_SPEED = (CountsPerDegree * 90.) / (0.5);

class Motor
{
public:
	Motor();
	Motor(float mMaxRatedTorque, float mStallCurrent);

	void	Initialize				( 						);
	bool	is_breaking_region		( word mCount			);
	bool	is_destination_greater	( );
	
	float 	compute_motor_torque	( float mDuty 			);
	float 	compute_angle			( word  PotValue 		);
	void 	compute_curr_angle		( 						);
	int 	compute_position		( float mAngle  		);

	float 	compute_duty		  	( 					 	);		// PID
	float	compute_gravity_boost 	( 						);
	float	compute_braking_speed 	( 				 		);
	float	compute_accel_speed 	( 						);
	float	get_control_speed		( 						);	

	float	compute_stopping_distance( float mSpeed,  float mDeceleration );
	float  	compute_reaction_torque  ( Motor& mMotor, float mAlpha        );

	void  	set_destination			( int mDestinationCount, float mRequestedSpeed );
	int  	check_stops				( 						);
	int  	update_position			( 						);	// handles incoming ID_MOTOR_VALUE & recomputes duty.
	int  	handle_CAN_message	    ( struct sCAN* mMsg 	);	// handles incoming msg
	bool	correct_direction_out_of_stop( float mDuty	);

	bool 	destination_reached 	( float mTolerance 		);
	bool 	within_tolerance		( float mDestination, float mTolerance );
	
	void  	send_speed_pid			(  						);
	void  	send_speed				( float mDuty 			);
	void  	send_stop				( 						);
	void  	send_moveto_angle		( float mAngle=9999. 	);
	void  	send_config				( byte mindex, byte mValue, byte mMask=0xFF );

	void	print_speeds			( 				);
	void	print_positioning		( 				);
	void	print_state				( 				);	// Position, speed, stops active, torque applied, etc.  1 liner.
	void	print_stop				( int mStopNum 	);

	// CHANGE FREQUENTLY (ie realtime) : 
	word	StartCount;		 	 // Reading when the send_speed() was called.
	word	DestinationCount; 	 // Reading when the send_speed() was called.	
	word	PrevCount;			 // Latest reading
	word	CurrCount;			 // Latest reading
	word 	BeginBrakingCount;	 // Trigger for breaking (pid control)
	bool	DestinationReached;	 // 
	int		MotorStopped;		 // Indicator of which Stop is in: 0 none, 1, 2.

	float	PrevAngle;
	float	CurrAngle;				// in Degrees 
	float	NextAngleDeg;			// Destination when in Angle mode. in Degrees * 10 

	float	DutyPercent;			// [-100.0 , 100.0] 
	float	RequestedSpeed;			// Counts per second
	float	MeasuredSpeed;			// Counts per second	

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
	
	// MOTOR CURRENT DRAW:
	word	CurrentTimesTen;		// Motor current draw (fixed point)

	//struct timespec
	struct timeval submitted;
	struct timeval completed;
	struct timeval CurrTime;
	struct timeval PrevTime;


	word 	Reads;					// number of CAN positioning reads done since duty was last updated
	word 	ReadsAllowed;			// number of CAN positioning reads before duty will be updated

	// Semi Constant Variables (change very infrequently)
	byte 	Instance;
	bool	use_stops;				// if true, then always check limits.
	struct  sStopInfo stop1;
	struct  sStopInfo stop2;
	float	deceleration_rate_cpss;	// Depends on the load.  How to determine?! algorithm to sense?

	word	ZeroOffset;				// in counts.
	BOOL	ActiveOutputs;			// Similar to MotorEnable, if false does not send the duty to the motor.
	BOOL 	MotorEnable;			// If FALSE, does not participate in the sequencing (
	// If MotorEnable==TRUE but ActiveOutputs==FALSE, the incoming messages will be updated, but no drive to motors.
	short 	MotorDirection;			// 1 => increasing counts is positive direction;  0=> decreasing counts is positive angle.

	byte	Feedback_index;
	int		Feedback_Msg_id;

private:
	float MaxRatedTorque;			// inch*lbs  around (1,546 for 82666)
	float StallCurrent;				// 	
	float Duty;						// 	
};


#endif
