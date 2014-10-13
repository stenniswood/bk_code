#ifndef _PENDULUM_H_
#define _PENDULUM_H_

#include "Container.hpp"


#define SWING_LEG 			0
#define STANCE_LEG 			1
#define WEIGHT_SHARED_LEG 	2		// both legs bearing weight.

/*******************************************
A Leg is a Motor Pack that handles computations of torques.
Most obvious from jog the legs are the Ankles move so easily it's
trivial, knees okay, and hips require some strength to overcome the
static friction/gravity.
*******************************************/
class Pendulum 
{
public:
	Pendulum( );


	virtual float 	get_angle_wrt_gravity()		{ return get_angle(); };
	
	// ACCESSOR FUNCTIONS:
	float 			get_angle			(			   )  { return angle;   	};
	void 			set_angle			( float mangle )  { angle = mangle; 	};

	void			set_to_swing_leg				()	  { mode = SWING_LEG;  };
 	void			set_to_stance_leg				()	  { mode = STANCE_LEG; };

	float 			calc_gravity_force_longitudinal	();
	float 			calc_gravity_force_transverse  	();
	float 			calc_centrifugal_force		   	();
	float			calc_normal_force			(				);
	float			get_joint_frictional_force	(				);

	float			calculate_friction_torque	(						);
	float			calc_gravity_torque			(						);
	float			calc_motor_torque			(float mDuty			);
	float			calc_sum_torques			(float mDuty 			);

/* Control requires that we specify desired position & velocity/accel.
   and achieve it by finding the motor_torque.   */
    float			find_motor_torque			(float mRequestedVelocity );
	/* Feedback allows us to adjust the filter constants. based on how 
		close we came to meeting our required speed.  Call this before 
		update_pendulumn_parameters() in leg.cpp   */
    float			adjust_constants			(float mRequestedVelocity, float mFinalPosition );    
    
	float			calc_net_accel				(float mDuty			);
	float			find_motor_duty				(float mRequestedAccel  );
	float			calc_avg_velocity			(float Initial_Velocity, float mAcceleration);

	float 			calc_final_velocity(float InitialVelocity, float Acceleration, float TimeSlicePeriod);
	float 			calc_acceleration(float RequestedDistance, float InitialVelocity, float Timeslice_Period);
	
	/* the parameters come from the vector file.  They are based on requested values */
	void 			timeslice_hip();

	void 		SavePreferences( Preferences* mpref );
	void 		LoadPreferences( Preferences* mpref );	


	// VARIABLES:	
	float 		angle;						
	float 		angular_velocity;				
	float 		angular_acceleration;			

	float		distance_to_center_of_mass;		// from the swing joint to CM (may be different than the length!)
	float		length;
	float		mass;							// kg or lbs ?
	float		inertia;						// these are proportionality constants.

	float 		mu_static;				
	float 		mu_rolling;			

	byte		mode;						// swing, stance, or shared (both legs bearing weight)
	
private:
	OneVector*	leg_attachment_position;	// used for finding knee and toe positions, etc.
};
 
#endif



/* So far we've not had any reference to the weight, lengths, or inertia's of 
   the parts of the leg.
   These should be stored into a config file.  Or they could be determined 
   algorithmically by trial runs of how leg responds to a duty.   
*/
