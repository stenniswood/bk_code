#ifndef _LEG_H_
#define _LEG_H_

#include "Container.hpp"
#include "motor.hpp"
#include "motor_pack.hpp"

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
	//Leg( char* mName, byte mFirstInstance );

/* So far we've not had any reference to the weight, lengths, or inertia's of 
   the parts of the leg.
   These should be stored into a config file.  Or they could be determined 
   algorithmically by trial runs of how leg responds to a duty.   
*/

	float 			get_hip_angle			();
	float 			get_knee_angle			();
	float 			get_ankle_angle			();

	float 			get_hip_angular_speed	();
	float 			get_knee_angular_speed	();
	float 			get_ankle_angular_speed	();

	/* the parameters come from the vector file.  They are based on requested values */
	virtual OneVector* calculate_duties ( OneVector* mPos, OneVector* mVelocity, OneVector* mAcceleration );
	void 			   apply_vector		( 						    	   );

	virtual void	SavePreferences		( Preferences* mpref			   );		// stops and ...
	virtual void	LoadPreferences		( Preferences* mpref, BOOL mConstructObjects = TRUE	);	
	
private:
		
		
};
 
#endif
