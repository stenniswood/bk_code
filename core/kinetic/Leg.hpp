#ifndef _LEG_H_
#define _LEG_H_

#include "Container.hpp"
#include "motor.hpp"
#include "motor_pack.hpp"
#include "pendulumn.hpp"

#define LEG_LOCATION_NONE 0
#define LEG_LOCATION_TOE  1
#define LEG_LOCATION_HEEL 2
#define LEG_LOCATION_KNEE 3

#define SWING_LEG 			0
#define STANCE_LEG 			1
#define WEIGHT_SHARED_LEG 	2		// both legs bearing weight.

/*******************************************
A Leg is a Motor Pack that handles computations of torques.
Most obvious from jog the legs are the Ankles move so easily it's
trivial, knees okay, and hips require some strength to overcome the
static friction/gravity.
*******************************************/
class Leg : public MotorPack 
{
public:
	Leg( char* mName );

	// ACCESSOR FUNCTIONS:
	float 			get_hip_angle			();
	float 			get_knee_angle			();
	float 			get_ankle_angle			();
	void 			set_hip_angle			(float mAngle);
	void 			set_knee_angle			(float mAngle);
	void 			set_ankle_angle			(float mAngle);
	float 			get_hip_angular_speed	();
	float 			get_knee_angular_speed	();
	float 			get_ankle_angular_speed	();

	void			set_to_swing_leg		();
 	void			set_to_stance_leg		();
 	void			set_to_standing_leg		();

	// 
	OneVector*		get_robot_coordinate_leg_attachment_position();
	void 			set_robot_coordinate_leg_attachment_position(OneVector* mAttachementPosition );

	// This can be used to specify "toe to <x,y,z>" or "heel to <xyz>" or even "knee to <xyz>"
	void 			inverse_kinematic( OneVector* mPosition, byte mLegLocation 	   );
	OneVector*		getKneePosition();
	OneVector*		getHeelPosition();
	OneVector*		getToePosition ();

	virtual void	SavePreferences		( Preferences* mpref			   );		// stops and ...
	virtual void	LoadPreferences		( Preferences* mpref, BOOL mConstructObjects = TRUE	);	

	/* the parameters come from the vector file.  They are based on requested values */
	void			update_pendulumn_parameters();	
	virtual OneVector* calculate_duties ( OneVector* mPos, OneVector* mVelocity, OneVector* mAcceleration );
	void 			   apply_vector		( VectorSequence* mvs );
	
private:
	OneVector*	leg_attachment_position;	// used for finding knee and toe positions, etc.
	
	Pendulum	upper_leg;
	Pendulum	lower_leg;
	Pendulum	foot;		

	float	ankle_height;		// distance from heel to ankle.	
	byte	leg_mode;			// swing, stance, or shared (both legs bearing weight)
};
 
#endif



/* So far we've not had any reference to the weight, lengths, or inertia's of 
   the parts of the leg.
   These should be stored into a config file.  Or they could be determined 
   algorithmically by trial runs of how leg responds to a duty.   
*/
