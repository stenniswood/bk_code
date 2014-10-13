#ifndef _MOTOR_PACK_
#define _MOTOR_PACK_

#include "Container.hpp"
#include "motor.hpp"

class OneVector;


/************************************************
A Motor Pack contains any number of Motor
objects.  It groups then for commonly related
functions.  Such as starting position reporting.
Simultaneous halt.  Grouping all the Angle values, 
etc.
*************************************************/
class MotorPack : public Container 
{
public:
	MotorPack( char* mName );

	void	 add_motor			( Motor* NewMotor 			);
	Motor*	 get_motor			( int mIndex 				);
	char*	 get_name			( ) { return pack_name; };

	// Accessors : 
	void	 set_angle_vector	( float* mData,    int n,   BOOL mAnimationOnly=FALSE );
	void	 set_angle_onevector( OneVector* mOV,  			BOOL mAnimationOnly=FALSE );
	void 	 set_duty_vector	( float* mFractions, int n 	);
	//void 	 set_duty_onevector ( OneVector* mOV 		   	);
	void 	 get_angle_vector	( float* mData, int* n 	   	);	// Last received Angles

	void	 send_stops  		( 							);
	void	 verify_stops		( 							);
	int  	 distribute_CAN_msg	( struct sCAN* mMsg 		);	// dispatches to all connected motors.

	// STOPS (different for each motor)
	// Save/Load to/from a .ini file on the Raspberry Pi.
	virtual void	SavePreferences	( Preferences* mpref		);		// stops and ...
	virtual void	LoadPreferences	( Preferences* mpref, BOOL mConstructObjects = TRUE	);	// load from file
	virtual void	construct_motors( byte mFirstInstanceNum, byte mNumMotors );
	void			ActivateStops	( char* mConfigFileName		);	// stops in the object to the boards.
	void			RetrieveStops	( char* mConfigFileName		);	// read stops from the boards.

	// CONFIGURE:	(usually apply to all motors)
	// void			RetrieveConfig	( char* mConfigFileName		);	// read stops from the boards

	// For all boards!
	void	select_reports		( byte Speed, byte ReportType )	 { config_bytes[1] = Speed|ReportType; };	// For all boards!
	void	start_reporting		( );
	void	stop_reporting		( );	
	void	set_config_byte		( byte mindex, byte mvalue    );
	void	distribute_config	( );		// currently set configs dispersed to all motors.

	//void configure_all			( byte mFlag, BOOL mOn 		);
	//void configure_set_update_rate( byte mRate, byte mReports );
	//void pack_configure_motor		( byte mFlags 				);

private:
	char* pack_name;		// Name used to save preferences under.

	byte config_bytes[4];
};


/**************************************************************
    We also declare an abstract base class for limbs.	
	A generic limb takes in a sequence of position vectors
	and has an algorithm that determines the duty cycle needed.
***************************************************************/
class Limb : public MotorPack 
{
public:
	Limb( char* mName );

	virtual OneVector* calculate_duties( OneVector* positionVector );
	void	   apply_duties	   ( OneVector* dutyVector 	   );

private:
				
};

 
#endif

