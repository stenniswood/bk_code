#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "vector_file.hpp"

#include "Container.hpp"
#include "motor.hpp"
#include "tilt_sensor.hpp"
#include "board_list_oop.hpp"
#include "ini_file.hpp"
#include "Leg.hpp"
#include "arm.hpp"

class BoardList;
class VectorGroupSequence;


#define CALIBRATE_MODE	0x01
#define NORMAL_MODE   	0x02
#define SLEEP_MODE    	0x03
#define LOGIC_ONLY_MODE	0x04
#define RESERVED_MODE 	0x05


/*******************************************
Top Level Object for Robot

*******************************************/
class Robot
{
public:
	Robot( char* mName = "Mr T" );
	
	// Accessors:
	Arm*		 get_left_arm	(  	);
	Arm*		 get_right_arm	(  	);
	Leg*		 get_left_leg	(  	);
	Leg* 		 get_right_leg	(  	);

	MotorPack*	 new_limb		( char* mName, char* mType );
	int 		 find_limb_index( char* mName 		);
	MotorPack*	 find_limb		( char* mName 		);
	MotorPack*	 get_limb		( int mIndex 		);
	int		 	 get_number_limbs( )  { return Limbs.getNumItems();	 };
	
	// CHANGE MODES:
	void 	setMode				( int mMode			);
	void 	enterNormalMode		( );	// sequencing upon request.
	void 	enterCalibrateMode	( );
	void 	enterLogicOnlyMode	( );
	void 	enterSleepMode		( );

	// Sequencer:
	void	applyVector			( VectorGroupSequence* vgs 	);
	void	timeslice			( VectorGroupSequence* vgs 	);

	// Dispatches received CAN msgs to all objects;
	int  	distribute_CAN_msg	( struct sCAN* mMsg );	
	void	query_devices		(					);
	void	configure_boards	(					);

	// Save/Load to/from a .ini configuration file on the Raspberry Pi.
	void	InitializeModel		(  					);
	void	SavePreferences		( 					);
	void	LoadPreferences		( BOOL Construct = FALSE );
	void	MakeStandardRobot	( 					);
	void	SendStops			( );
	
	TiltSensor	tilt;
	
private:
	FILE*		vector_fd;		// For reading vectors
	FILE*		pref_fd;		// 
	
	BoardList* 	board_list;		// 
	int			Mode;
	Container   Limbs;			// MotorPacks
	char*		PersonalName;	// Frank, harry, etc.

/*	ServoPack Head;	*/
};

extern Preferences 			robotPref;
extern Robot 				Onesimus;
extern VectorGroupSequence 	vgs;
 
#endif


