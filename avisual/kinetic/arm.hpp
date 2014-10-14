#ifndef _ARM_H_
#define _ARM_H_

#include "Container.hpp"
#include "motor.hpp"
#include "motor_pack.hpp"


/*******************************************
An arm is a Motor Pack that handles computations of torques.
Most obvious from jog the legs are the Ankles move so easily it's
trivial, knees okay, and hips require some strength to overcome the
static friction/gravity.
*******************************************/
class Arm : public MotorPack 
{
public:
	Arm( char* mName );

	virtual OneVector* calculate_duties( OneVector* positionVector );

	virtual void	SavePreferences	( Preferences* mpref		);		// stops and ...
	virtual void	LoadPreferences	( Preferences* mpref, BOOL mConstructObjects = TRUE	);	// load from file
	//virtual void 	construct_motors( byte mFirstInstanceNum, byte mNumMotors );
	
private:
		
};
 
#endif


