#ifndef  _ANGULAR_VECTORS_H_
#define _ANGULAR_VECTORS_H_

/* Ab, Hip Knee, & Foot may be better done as object oriented.  However, need to check the gnu++ compiler
   and the interactions with regular C code. */

extern float g ;				// gravity 1g

struct sAbInfo
{	
	// These parameters will refer to the upperleg section only!
	// Additional torques & inertia from knee will be added separately.
	BOOL Stance_leg;		// stance means weight placed on the leg.
	float distance_to_center_of_mass;
	float Mass;			// mass of upper leg
	float Inertia;			// inertia of leg
	float Angle;				// Rad
	float AngularSpeed;			// Rad / Second
	float mu_static;			// coefficient of friciton when stationary.
	float mu_kinetic;			// coefficient of friction when moving.
	struct sKneeInfo* KneeLink;	
} ;

struct sHipInfo
{	
	// These parameters will refer to the upperleg section only!
	// Additional torques & inertia from knee will be added separately.
	BOOL Stance_leg ;		// stance means weight placed on the leg.
	float distance_to_center_of_mass ;
	float Mass   ;			// mass of upper leg
	float Inertia;			// inertia of leg
	float Angle;				// Rad
	float AngularSpeed;			// Rad / Second
	float mu_static;			// coefficient of friciton when stationary.
	float mu_kinetic;			// coefficient of friction when moving.
	struct sKneeInfo* KneeLink;	
} ;


struct sKneeInfo
{	
	// These parameters will refer to the upperleg section only!
	// Additional torques & inertia from knee will be added separately.
	BOOL Stance_leg ;		// stance means weight placed on the leg.
	float distance_to_center_of_mass;
	float Mass   ;			// mass of upper leg
	float Inertia;			// inertia of leg
	float Angle;				// Rad
	float AngularSpeed;			// Rad / Second
	float mu_static;			// coefficient of friciton when stationary.
	float mu_kinetic;			// coefficient of friction when moving.
	//struct sKneeInfo* KneeLink;	
} ;

extern struct sHipInfo HipInfo;


/* Functions for calcualting Hip motor duty cycle. 		*/
float calc_gravity_force_parallel_hip(					);
float calc_gravity_force_perpendicular_hip(				);
float calc_normal_force			(						);
float calculate_friction_torque	(						);
float calc_gravity_torque		( float mAngle 			);
float calc_motor_torque			( float mDuty 			);
float calc_sum_torques			( float mDuty 			);
float calc_net_accel			( float mDuty			);
float find_motor_duty			( float mRequestedAccel );
float calc_avg_velocity			( float Initial_Velocity,  float mAcceleration 						);
float calc_final_velocity		( float InitialVelocity,   float Acceleration, float TimeSlicePeriod);
float calc_acceleration			( float RequestedDistance, float InitialVelocity, float Timeslice_Period);
void  timeslice_hip				();


/* Functions for calcualting Knee motor duty cycle. 		*/



#endif
