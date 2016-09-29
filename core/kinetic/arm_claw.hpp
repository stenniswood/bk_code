#ifndef _ARM_H_
#define _ARM_H_

//#include "Container.hpp"
#include "vector_file.hpp"
#include "preferences.hpp"

#include "roboclaw.h"


/*******************************************
An arm is a Motor Pack that handles computations of torques.
Most obvious from jog the legs are the Ankles move so easily it's
trivial, knees okay, and hips require some strength to overcome the
static friction/gravity.
*******************************************/
class Arm 
{
public:
	Arm( const char* mName );
	Arm( );

	float 			get_shoulder_angle		();
	float 			get_shoudler_swing_angle();
	float 			get_elbow_angle			();
	float 			get_elbow_rotate_angle	();

	bool 			set_shoulder_duty		( float mFraction );
	bool 			set_shoulder_swivel_duty( float mFraction );
	bool 			set_elbow_duty			( float mFraction );
	bool 			set_elbow_swivel_duty	( float mFraction );
	
	void			stop_all_motors			();
	int				get_num_motors()	{ return 0; };
		
	virtual OneVector* calculate_duties( OneVector* positionVector );

	virtual void	SavePreferences	( Preferences* mpref		);		// stops and ...
	virtual void	LoadPreferences	( Preferences* mpref, BOOL mConstructObjects = TRUE	);	// load from file
	//virtual void 	construct_motors( byte mFirstInstanceNum, byte mNumMotors );
	
	RoboClaw	m_shoulder;
	RoboClaw	m_elbow;	
	
private:
	std::string m_name;
};
 
#endif


