#ifndef _MOTOR_HPP_
#define _MOTOR_HPP_

/* NO LOAD:
Speed of the 82666 Motor with no load is:  0.683 sec/rev
==> 1.45 revs/sec.
==> 88   rpm
==> with 512 counts per rev encoder, it is:  15 counts / 20ms timeslice.

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

class Motor
{
public:
	Motor();
	Motor(float mMaxRatedTorque, float mStallCurrent);

	void	Initialize			( );
	float 	calc_motor_torque	( float mDuty );
	float 	compute_duty		( float mDestination );
	float	calc_gravity_boost	( );
	float 	compute_angle		( );
	int  	check_stops			( );
	int  	update_position		( struct sCAN* mMsg );	// handles incoming ID_MOTOR_VALUE & recomputes duty.
	bool	correct_direction_out_of_stop( float mDuty );

	bool 	destination_reached ( float mTolerance );
	bool 	within_tolerance	( float mDestination, float mTolerance );
	void  	send_speed_pid		(  );
	void  	send_speed			( float mDuty );
	void  	send_stop			( );
	void  	send_moveto_angle	( float mAngle=9999. );
	void  	send_config			( byte mindex, byte mValue, byte mMask=0xFF );
	void	print_positioning	( );
	void	print_stop			( int mStopNum );
	
	float  	calc_reaction_torque( Motor& mMotor, float mAlpha );
	
	// CHANGE FREQUENTLY (ie realtime) : 
	word	StartPotValue;		 // Reading when the send_speed() was called.
	word	DestinationPotValue; // Reading when the send_speed() was called.	
	word	CurrPotValue;		 // Latest reading
	bool	DestinationReached;	 // 
	int		MotorStopped;

	float	CurrAngle;			// in Degrees 
	float	NextAngleDeg;		// Destination when in Angle mode. in Degrees * 10 
	float	SpeedTimesTen;		// [-100.0 , 100.0] 
	word	CurrentTimesTen;	// fixed point

	struct timeval submitted;
	struct timeval completed;

	word 	Reads;				// number of CAN positioning reads done since duty was last updated
	word 	ReadsAllowed;		// number of CAN positioning reads before duty will be updated

	// Semi Constant Variables (change very infrequently)
	byte 	Instance;
	struct  sStopInfo stop1;
	struct  sStopInfo stop2;

	word	ZeroOffset;			// in counts.
	BOOL 	MotorEnable;		// If FALSE, does not participate in the sequencing
	short 	MotorDirection;		// 

	float	K_const;			// multiply by sin theta gives the torque.

private:
	float MaxRatedTorque;	// inch*lbs  around (1,546 for 82666)
	float StallCurrent;		// 	
	float Duty;				// 	
};


#endif
