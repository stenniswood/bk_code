#ifndef _MOTOR_HPP_
#define _MOTOR_HPP_


#include "preferences.hpp"

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

float radians(float mDegrees);

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

	virtual void	Initialize		( 						);
	virtual bool	read_config_data( Preferences& mprefs, int mIndex );	
	bool	is_destination_greater	( 						);
	
	float 	compute_motor_torque	( float mDuty 			);
	float 	compute_angle			( word  mPotValue 		);
	int 	compute_position		( float mAngle  		);

	int  	check_stops				( 						);
	bool	correct_direction_out_of_stop( float mDuty		);
	bool 	within_tolerance		( float mDestination, float mTolerance );
		
	int  	update_position			( 						);	// computes time delta, angle, angular speed, check stops 
	virtual int  	handle_CAN_message( struct sCAN* mMsg 	);	// handles incoming msg

	void  	send_speed				( float mDuty 			);
	void  	send_stop				( 						);
	void  	send_moveto_angle		( float mAngle=9999. 	);
	void  	send_config				( byte mindex, byte mValue, byte mMask=0xFF );

	void	print_average			( );
	void	print_speeds			( 				);
	void	print_positioning		( 				);
	void	print_state				( 				);	// Position, speed, stops active, torque applied, etc.  1 liner.
	void	print_stop				( int mStopNum 	);
     
	/*** VARIABLES ***/
	// CHANGE FREQUENTLY (ie realtime) : 
	word	PrevCount;			 // Latest reading
	word	CurrCount;			 // Latest reading
	long	SumCurrCounts;		 // for averaging the last n.
	float	average_CurrCounts;

	int		MotorStopped;		 // Indicator of which Stop is in: 0 none, 1, 2.
	float	Duty;				 // 
	
	float	PrevAngle;
	float	CurrAngle;				// in Degrees 
	float	NextAngleDeg;			// Destination when in Angle mode. in Degrees * 10 

	float	DutyPercent;			// [-100.0 , 100.0] 
	float	RequestedSpeed;			// Counts per second
	float	MeasuredSpeed;			// Counts per second	

	// MOTOR CURRENT DRAW:
	word	CurrentTimesTen;		// Motor current draw (fixed point)
	float 	StallCurrent;			// 
	
	struct timeval CurrTime;
	struct timeval PrevTime;

	word 	Reads;					// number of CAN positioning reads done since duty was last updated
	word 	ReadsAllowed;			// number of CAN positioning reads before duty will be updated

	// Semi Constant Variables (change very infrequently)
	byte 	Instance;
	bool	use_stops;				// if true, then always check limits.
	struct  sStopInfo stop1;
	struct  sStopInfo stop2;

	float	gravity_angle;			// of the portion more torso direction + CurrAngle = next limb angle wrt gravity!

	word	ZeroOffset;				// in counts.
	BOOL	ActiveOutputs;			// Similar to MotorEnable, if false does not send the duty to the motor.
	BOOL 	MotorEnable;			// If FALSE, does not participate in the sequencing (
	// If MotorEnable==TRUE but ActiveOutputs==FALSE, the incoming messages will be updated, but no drive to motors.
	short 	MotorDirection;			// 1 => increasing counts is positive direction;  0=> decreasing counts is positive angle.

	byte	Feedback_index;
	int		Feedback_Msg_id;
	int		Missing_Msg;
	char	Name[40];

private:
	float MaxRatedTorque;			// inch*lbs  around (1,546 for 82666)

};


#endif
