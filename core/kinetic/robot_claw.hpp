#ifndef _ROBOT_H_
#define _ROBOT_H_

//#include "vector_file.hpp"
//#include "ini_file.hpp"
//#include "preferences.hpp"
#include "tilt_sensor.hpp"
#include "leg_claw.hpp"
#include "arm_claw.hpp"
#include "serial_loadcell.hpp"


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
	Robot( const char* mName = "Mr T" );

	// CHANGE MODES:
	void 	setMode				( int mMode			);
	void 	enterNormalMode		( );	// sequencing upon request.
	void 	enterCalibrateMode	( );
	void 	enterLogicOnlyMode	( );
	void 	enterSleepMode		( );

	// Sequencer:
//	void	applyVector			( VectorGroupSequence* vgs 	);
//	void	timeslice			( VectorGroupSequence* vgs 	);

	// Dispatches received CAN msgs to all objects;
	int  	distribute_CAN_msg	( struct sCAN* mMsg );	
	void	query_devices		(					);
	void	configure_boards	(					);

	// Save/Load to/from a .ini configuration file on the Raspberry Pi.
	void	InitializeModel		(  					);
//	void	SavePreferences		( 					);
//	void	LoadPreferences		( BOOL Construct = FALSE );
	void	SendStops			( );

	void	stop_all_motors		();
	void	Sit					();
	void	Stand_up			();
	
	//TiltSensor	tilt;
	Arm			left_arm;
	Arm			right_arm;
	Leg			left_leg;
	Leg			right_leg;
	
	LoadCell_SerialInterface left_foot;
	LoadCell_SerialInterface right_foot;
	
protected:
	FILE*		vector_fd;		// For reading vectors
	FILE*		pref_fd;		// 
	
	int			Mode;
	char*		PersonalName;	// Frank, harry, etc.
	//BoardList* 	board_list;
	/*	ServoPack Head;	*/
};

extern Preferences 			robotPref;
extern Robot 				Onesimus;
extern VectorGroupSequence 	vgs;
 
#endif
