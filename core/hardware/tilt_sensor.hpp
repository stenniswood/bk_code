#ifndef _TILT_SENSOR_H_
#define _TILT_SENSOR_H_

#include "Container.hpp"



/***********************************************
Wrapper object for a Tilt sensor on the CAN bus.
************************************************/
class TiltSensor
{
public:
	TiltSensor( );
	
	// Accessors:
	float*		getOrientation();
	float*		getPosition()	;
	
	float*		getAngularVelocity();
	float*		getLinearVelocity ();
	
	// Sequencer:
	// none	

	// RECEIVE CAN MESSAGE : 
	int  process_CAN_msg	  ( struct sCAN* mMsg );
	byte can_parse_motor_angle( struct sCAN* mMsg );
	byte can_parse_motor_value( struct sCAN* mMsg );

	// Save/Load to/from a .ini configuration file on the Raspberry Pi.
	void	SavePreferences		( FILE* mfd		);		
	void	LoadPreferences		( FILE* mfd		);		// sensitivity, last known position
	
private:
	
};
 
#endif
