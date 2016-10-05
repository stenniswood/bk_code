// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "CAN_Interface.hpp"
#include "can_id_list.h"
#include "vector.hpp"
#include "vector_math.h"
#include "buttons.h"
#include "robot_claw.hpp"
#include "leg_claw.hpp"
#include "arm.hpp"
#include <iostream>

using namespace std;

// Preferences has to be constructed before Robot!!
Preferences robotPref("robot.ini");
VectorGroupSequence vgs;
//Robot 		Onesimus("Ronny");


MathVector	sit1(4);
MathVector	sit2(4);
MathVector	stand1(4);
MathVector	stand2(4);
MathVector	leg_forward1(4);
MathVector	leg_forward2(4);

void init_duties()
{
	printf("init_duties()\n");
	sit1[0] = 0.0;	sit1[1] = -0.5;	sit1[2] = 0.5;	sit1[3] = 0.2;
	sit2[0] = 0.0;	sit2[1] = -0.5;	sit2[2] = 0.5;	sit2[3] = 0.2;

	stand1[0] = 0.0;	stand1[1] = 0.5;	stand1[2] = -0.5;	stand1[3] = -0.2;
	stand2[0] = 0.0;	stand2[1] = 0.5;	stand2[2] = -0.5;	stand2[3] = -0.2;

	leg_forward1[0] = 0.0;	leg_forward1[1] = 0.5;	leg_forward1[2] = 0.3;	leg_forward1[3] = -0.2;
	leg_forward2[0] = 0.0;	leg_forward2[1] = 0.5;	leg_forward2[2] = 0.3;	leg_forward2[3] = -0.2;	
	
}

Robot::Robot( const char* mName )
: left_leg("/dev/ttyACM0","/dev/ttyACM1"), right_leg("/dev/ttyACM2","/dev/ttyACM3")
//left_arm(), right_arm()
{
	Mode 		 = CALIBRATE_MODE;
	PersonalName = strdup(mName);	
}

void Robot::query_devices(  )
{
	//board_list->RequestBoardsPresent();
}

void Robot::configure_boards(  )
{
	if (Mode == CALIBRATE_MODE)
	{
		// While Calibrating, we need to have the boards configured in FreeSpin mode
		// other wise it wouldn't let us jog the motors if we're past a unset stop value.
//		left_leg.set_config_byte ( 1, 0x08 );	// FreeSpin Mode
//		right_leg.set_config_byte( 1, 0x08 );
		
		printf("===BuildLeg: Start reporting Left & Right all motors!\n");	
//		left_leg.select_reports ( MODE_SEND_UPDATES_100ms, MODE_SEND_POSITION_RAW );
//		right_leg.select_reports( MODE_SEND_UPDATES_100ms, MODE_SEND_POSITION_RAW );
//		left_leg.start_reporting();
//		right_leg.start_reporting();	
	}
	else if (Mode == LOGIC_ONLY_MODE)
	{	
//		left_leg.stop_reporting();
//		right_leg.stop_reporting();
	}
	else if (Mode == SLEEP_MODE)
	{	
		//left_leg.stop_reporting();
		//right_leg.stop_reporting();
	}
	else if (Mode == NORMAL_MODE)
	{	
		// Send info from .ini to the attached boards.
		// Refresh by LoadPreferences()
		
/*		left_leg.set_config_byte ( 1, 0x00 );	// FreeSpin Mode
		right_leg.set_config_byte( 1, 0x00 );
		left_leg.distribute_config ( );	// use what was load in preferences
		right_leg.distribute_config( );
		
		left_arm.get_motor(0)->setAngle( M_PI/2.0 );
		right_arm.get_motor(0)->setAngle( M_PI/2.0 );

		printf("===BuildLeg: Start reporting Left & Right all motors!\n");	
		left_leg.select_reports ( MODE_SEND_UPDATES_100ms, MODE_SEND_POSITION_RAW );
		right_leg.select_reports( MODE_SEND_UPDATES_100ms, MODE_SEND_POSITION_RAW );
		left_leg.start_reporting();
		right_leg.start_reporting(); */
	}
}

// Save/Load to/from a .ini configuration file on the Raspberry Pi.
/*void	Robot::SavePreferences( )
{
	// This is top level for robot preferences
	// Other prefs stored in other file. Such as display setup
	//pref_fd = fopen("robot.ini", "a+");
	robotPref.open();
	char str[255];
	
	sprintf(str, "\nrobot_family_name=Onesimus\npersonal_name=%s\nNumber_of_limbs=%d\n", 
					PersonalName, 4 );
	robotPref.write( str );

	// SAVE ALL LIMBS:
	left_leg.SavePreferences ( &robotPref );
	right_leg.SavePreferences( &robotPref );
	
	left_arm.SavePreferences ( &robotPref );
	right_arm.SavePreferences( &robotPref );

	printf("All limbs saved!\n");
	robotPref.close();	
	printf("Robot Preferences Saved!\n");
}

void	Robot::LoadPreferences( BOOL Construct )
{
	char str[255];
	printf("=== Robot LoadPreferences()\n");
	
/*	robotPref.open(TRUE);			
	robotPref.readln_nb( str );		// Robot Family name=onesimus
	robotPref.readln_nb( str );		//
	PersonalName = robotPref.getString( str );

	robotPref.readln_nb( str );
	int num_limbs = robotPref.getIntValue( str );
	printf("Robot Name=%s; %d limbs;\n", PersonalName, num_limbs);
	MotorPack* mp=NULL;
	char* name;

	// CREATE LIMBS:
	for (int i=0; i<num_limbs; i++)
	{
		// Get Limb Type	
		robotPref.readln_nb( str );
		char* type = robotPref.getString( str );
		char** info;
		int num_motors=0;
		int n=0;

		//printf("Limb Type=%s\n", type);
		if ((strcasecmp(type, "Leg") == 0) || (strcasecmp(type, "Arm") == 0))
		{
		/*	robotPref.readln_nb( str );
			info = robotPref.split(str, ';', &n);		// Should be 5 entries	
			name = robotPref.getString( info[0] );
			num_motors = robotPref.getString( info[1] ); *
		} else {
			printf("Unknown Limb type: Expecting Leg or Arm!\n");
			return ;
		}

		//mp = new_limb( "name", type );		// creates new arm/leg instances
		//printf("name=%s:%s\n", name, type );
		left_leg.LoadPreferences( &robotPref );
//		right_leg.LoadPreferences( &robotPref );		
//		left_arm.LoadPreferences( &robotPref );
//		right_arm.LoadPreferences( &robotPref );						
	}

	robotPref.close();

	printf("=== Robot: LoadPreferences() completed!\n\n");
} */

/*void Robot::applyVector( VectorGroupSequence* vgs )
{
	OneVector* posv = NULL;
	//Limb* 	   l    = NULL;
	int num_groups  = vgs->getNumItems();

	// Go thru each limb:
	for (int i=0; i<num_groups; i++)
	{
		posv = vgs->get_vector_deg(i);		// This returns angle in degrees always!
		//l    = get_limb(i);
		if (posv)
		{
			//posv->print();
			*  When putting in the physics equations, we'll need to calculate the 
				duties required to acheive the position:			   
			*
			//OneVector* dutyVector = l->calculate_duties( posv );
			//mp->set_duty_onevector( dutyVector );
			//l->set_angle_onevector( posv, TRUE );
			delete posv;
		} else printf("posv=NULL\n");
	}
}*/

void	Robot::SendStops( )
{
	printf("Robot: Transfering Stops to Boards...\n");
/*	left_leg.send_stops();
	right_leg.send_stops(); 	
	left_leg.verify_stops();
	right_leg.verify_stops();  */
}


void Robot::setMode					( int mMode			)
{
	Mode = mMode;
}
void Robot::enterCalibrateMode		( )
{
	Mode = CALIBRATE_MODE;
}
void Robot::enterLogicOnlyMode		( )
{
	Mode = LOGIC_ONLY_MODE;	// No motor movements
}
void Robot::enterSleepMode			( )
{
	// Low power mode.  SHUTDOWN issued on CAN (all boards in low power)
	// All camera's & displays off
	// Only Central command RPI on.
	// No motor movements	
	Mode = SLEEP_MODE;	
}
void Robot::enterNormalMode		( )
{
	Mode = NORMAL_MODE;
}	// sequencing upon request

// Don't call until CAN is up and running.
void	Robot::InitializeModel(  )
{
	// Read INI file:
	BOOL found = robotPref.file_exists();
	//printf("Robot.ini file found=%d\n", found);
	//if (found)
		//LoadPreferences(TRUE);
	//else 
	{
	//	SavePreferences();
	}
}

// Dispatches received CAN msgs to all objects;
int  	Robot::distribute_CAN_msg	( struct sCAN* mMsg )
{	
	//board_list->process_CAN_msg( mMsg );
	//tilt.process_CAN_msg	   ( mMsg );
/*	left_arm.distribute_CAN_msg ( mMsg );
	right_arm.distribute_CAN_msg( mMsg );
	left_leg.distribute_CAN_msg ( mMsg );
	right_leg.distribute_CAN_msg( mMsg );
	head.distribute_CAN_msg( mMsg );  */
}

void Robot::stop_all_motors	()
{
	printf("stop_all_motors	()\n");
	
	left_leg.stop_all_motors();
	right_leg.stop_all_motors();	

//	left_arm.stop_all_motors();
//	right_arm.stop_all_motors();
}


void Robot::Sit		()
{
	left_leg.set_duty_vector (sit1);
	right_leg.set_duty_vector(sit1);
	usleep(200000);
	left_leg.read_currents();
	left_leg.read_status();	
	usleep(1000000);
	
	left_leg.set_duty_vector (sit2);
	right_leg.set_duty_vector(sit2);
	usleep(200000);
	left_leg.read_currents();
	left_leg.read_status();	
	usleep(500000);
	
}

// From sitting:
void Robot::Stand_up		()
{
	left_leg.set_duty_vector (stand1);
	right_leg.set_duty_vector(stand1);
	usleep(100000);
	left_leg.read_currents();
	left_leg.read_status();	
	usleep(100000);
	
	left_leg.set_duty_vector (stand2);
	right_leg.set_duty_vector(stand2);
	usleep(100000);
	left_leg.read_currents();
	left_leg.read_status();	
	usleep(100000);
}

void Robot::Put_leg_forward1	()
{
	
}
void Robot::Put_leg_forward2	()
{

}
