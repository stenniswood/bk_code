// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "bk_system_defs.h"
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_txbuff.hpp"
#include "packer.h"
#include "can_id_list.h"
#include "vector_math.h"
#include "cmd_process.h"
#include "parser_tilt.h"
#include "buttons.h"
#include "fuse_ag.h"
#include "fuse_accel.h"
#include "fuse_magnet.h"
#include "fuse_gyro.h"
#include "fuse_log.h"
#include "dataset.hpp"
#include "leveler.hpp"
#include "display.h"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "histogram.hpp"
#include "stick_leg.hpp"
#include "window_layouts.hpp"
#include "callbacks.hpp"
#include "motor.hpp"
#include "e_robot.hpp"
#include "Leg.hpp"
#include "arm.hpp"
#include "vector_file.hpp"
#include <iostream>

using namespace std;

// Preferences has to be constructed before Robot!!
Preferences robotPref("robot.ini");
Robot 		Onesimus("Ronny");
VectorGroupSequence vgs;

 
Robot::Robot( const char* mName )
//:LeftLeg("Left"), get_right_leg()->("Right")
{
	//string  my_str("Hello world!\n");
	//cout << my_str;			Yep!  this works!
	board_list   = &bl;
	Mode 		 = CALIBRATE_MODE;
	PersonalName = strdup(mName);	
}

Arm*	Robot::get_left_arm( )
{	return (Arm*)find_limb("LeftArm");	}
Arm*	Robot::get_right_arm( )
{	return (Arm*)find_limb("RightArm");	}

Leg*	Robot::get_left_leg( )
{	return (Leg*)find_limb("LeftLeg");	}
Leg*	Robot::get_right_leg( )
{	return (Leg*)find_limb("RightLeg");	}

MotorPack* Robot::new_limb( char* mName, char* mType )
{
	Item* item = new Item();
	MotorPack* mp;
	if (strcasecmp(mType, "Leg")==0)
		mp = new Leg(mName);	// this will NOT create the motors objects, done in respective LoadPreferences()
	if (strcasecmp(mType, "Arm")==0)
		mp = new Arm(mName);

	item->setData(mp);
	Limbs.Add( item );
	return mp;
}

MotorPack* Robot::find_limb( char* mName )
{
	int index  = find_limb_index( mName );
	Item* item = Limbs.getItem( index );
	return (MotorPack*)item->getData();
}

int Robot::find_limb_index( char* mName )
{
	int		 index=0;
	Item*    item = Limbs.getHead();
	MotorPack* mp = (MotorPack*)item->getData();
	char*    name = mp->get_name();

	while ( strcasecmp(name, mName) != 0 )
	{
		index++;
		item = item->getNext();
		mp = (MotorPack*)item->getData();
		name = mp->get_name ();
	}
	return index;
}

MotorPack* Robot::get_limb( int mIndex )
{
	Item*    item = Limbs.getHead();
	int n = Limbs.getNumItems();
	int c = 0;
	while ( c<n )
	{
		if (c==mIndex)
			return (MotorPack*)item->getData();
		item = item->getNext();
		c++;
	}
	return NULL;
}

// Dispatches received CAN msgs to all objects;
int  	Robot::distribute_CAN_msg	( struct sCAN* mMsg )
{
	get_left_leg()->distribute_CAN_msg ( mMsg );
	get_right_leg()->distribute_CAN_msg( mMsg );
	
	board_list->process_CAN_msg( mMsg );
	tilt.process_CAN_msg	   ( mMsg );
	
/*	LeftArm.distribute_CAN_msg ( mMsg );
	RightArm.distribute_CAN_msg( mMsg );
	future  head, etc */	
}

void Robot::query_devices(  )
{
	board_list->RequestBoardsPresent();
}

void Robot::configure_boards(  )
{
	if (Mode == CALIBRATE_MODE)
	{
		// While Calibrating, we need to have the boards configured in FreeSpin mode
		// other wise it wouldn't let us jog the motors if we're past a unset stop value.
		get_left_leg()->set_config_byte ( 1, 0x08 );	// FreeSpin Mode
		get_right_leg()->set_config_byte( 1, 0x08 );
		
		printf("===BuildLeg: Start reporting Left & Right all motors!\n");	
		get_left_leg()->select_reports ( MODE_SEND_UPDATES_100ms, MODE_SEND_POSITION_RAW );
		get_right_leg()->select_reports( MODE_SEND_UPDATES_100ms, MODE_SEND_POSITION_RAW );
		get_left_leg()->start_reporting();
		get_right_leg()->start_reporting();	
	}
	else if (Mode == LOGIC_ONLY_MODE)
	{	
		get_left_leg()->stop_reporting();
		get_right_leg()->stop_reporting();
	}
	else if (Mode == SLEEP_MODE)
	{	
		get_left_leg()->stop_reporting();
		get_right_leg()->stop_reporting();
	}
	else if (Mode == NORMAL_MODE)
	{	
		// Send info from .ini to the attached boards.
		// Refresh by LoadPreferences()
		
		get_left_leg()->set_config_byte ( 1, 0x00 );	// FreeSpin Mode
		get_right_leg()->set_config_byte( 1, 0x00 );
		get_left_leg()->distribute_config ( );	// use what was load in preferences
		get_right_leg()->distribute_config( );
		
		get_left_arm()->get_motor(0)->setAngle( M_PI/2.0 );
		get_right_arm()->get_motor(0)->setAngle( M_PI/2.0 );

		printf("===BuildLeg: Start reporting Left & Right all motors!\n");	
		get_left_leg()->select_reports ( MODE_SEND_UPDATES_100ms, MODE_SEND_POSITION_RAW );
		get_right_leg()->select_reports( MODE_SEND_UPDATES_100ms, MODE_SEND_POSITION_RAW );
		get_left_leg()->start_reporting();
		get_right_leg()->start_reporting();
	}
}

// Save/Load to/from a .ini configuration file on the Raspberry Pi.
void	Robot::SavePreferences( )
{
	// This is top level for robot preferences
	// Other prefs stored in other file. Such as display setup
	//pref_fd = fopen("robot.ini", "a+");
	robotPref.open();
	char str[255];
	
	sprintf(str, "\nrobot_family_name=Onesimus\npersonal_name=%s\nNumber_of_limbs=%d\n", 
					PersonalName, Limbs.getNumItems() );
	robotPref.write( str );

	// SAVE ALL LIMBS:
	Item*    item = Limbs.getHead();
	Limb* mp = (Limb*)item->getData();
	while ( item )
	{
		mp   = (Limb*)item->getData();
		mp->SavePreferences( &robotPref );
		item = item->getNext();
	}
	printf("All limbs saved!\n");
	robotPref.close();	
	printf("Robot Preferences Saved!\n");
}

void	Robot::LoadPreferences( BOOL Construct )
{
	char str[255];
	printf("=== Robot LoadPreferences()\n");
	
	robotPref.open(TRUE);			
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
			num_motors = robotPref.getString( info[1] ); */
		} else {
			printf("Unknown Limb type: Expecting Leg or Arm!\n");
			return ;
		}

		mp = new_limb( "name", type );		// creates new arm/leg instances
		//printf("name=%s:%s\n", name, type );
		mp->LoadPreferences( &robotPref );
	}

	robotPref.close();
	printf("=== Robot: LoadPreferences() completed!\n\n");
}

void Robot::applyVector( VectorGroupSequence* vgs )
{
	OneVector* posv = NULL;
	Limb* 	   l    = NULL;
	int num_groups  = vgs->getNumItems();

	// Go thru each limb:
	for (int i=0; i<num_groups; i++)
	{
		posv = vgs->get_vector_deg(i);		// This returns angle in degrees always!
		l    = get_limb(i);
		if (posv)
		{
			//posv->print();
			/*  When putting in the physics equations, we'll need to calculate the 
				duties required to acheive the position:			   
			*/
			//OneVector* dutyVector = l->calculate_duties( posv );
			//mp->set_duty_onevector( dutyVector );
			l->set_angle_onevector( posv, TRUE );
			delete posv;
		} else printf("posv=NULL\n");
	}
}

void	Robot::SendStops( )
{
	printf("Robot: Transfering Stops to Boards...\n");
	get_left_leg()->send_stops();
	get_right_leg()->send_stops(); 	

	get_left_leg()->verify_stops();
	get_right_leg()->verify_stops(); 
}

/* If there's no config file, we need to make one. */
void	Robot::MakeStandardRobot	( )
{
//	Leg* LeftL  = new Leg("LeftLeg" , 21 );
//	Leg* RightL = new Leg("RightLeg", 31 );

	MotorPack* mp;
	mp = new_limb( "LeftLeg",  "Leg");	mp->construct_motors( 21, 3 );
	mp = new_limb( "RightLeg", "Leg");	mp->construct_motors( 31, 3 );
	
	mp = new_limb( "LeftArm",  "Arm" );	mp->construct_motors( 41, 7 );
	mp = new_limb( "RightArm", "Arm" );	mp->construct_motors( 51, 7 );
	printf("made 4 limbs...\n");	
	printf("Standard robot created!\n");
}

void Robot::setMode				( int mMode			)
{
	Mode = mMode;
}
void Robot::enterCalibrateMode	( )
{
	Mode = CALIBRATE_MODE;
}
void Robot::enterLogicOnlyMode	( )
{
	Mode = LOGIC_ONLY_MODE;	// No motor movements
}
void Robot::enterSleepMode	( )
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
	if (found)
		LoadPreferences(TRUE);
	else {
		MakeStandardRobot();		
		SavePreferences();
	}
}
