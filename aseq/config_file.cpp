/* 
Sample Config file:
==========================================================
2							// Two vectors for this robot
Left Leg
Right Leg
0, 3, 31, 32, 33			// instance assignments
1, 3, 21, 22, 23
0, 3, 1,  1, -1				// Direction for increasing count.
1, 3, 1,  1, -1
0, 3, 0,  1,  1				// Enables
1, 3, 0,  1,  1
0, 3, 40, 2f, 42			// Zero Offsets
1, 3, 40, 2f, 42
0, 0, 1,  15.0, 0x002		// Stops
0, 0, 2, 175.0, 0x3C0		
0, 1, 1,  15.0, 0x002		
0, 1, 2, 175.0, 0x3C0		
0, 2, 1,  15.0, 0x002		
0, 2, 2, 175.0, 0x3C0		
1, 0, 1,  15.0, 0x002
1, 0, 2, 175.0, 0x3C0
1, 1, 1,  15.0, 0x002
1, 1, 2, 175.0, 0x3C0
1, 2, 1,  15.0, 0x002
1, 2, 2, 175.0, 0x3C0
==========================================================
Comments Should not appear in the file.
*/
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <vector> 
#include <string>

#include "bk_system_defs.h"
#include "can_eid.h"
#include "CAN_Interface.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "packer_motor.h"
#include "motor_vector.h"
#include "motor.hpp"
#include "motor_control.hpp"
#include "Appendage.hpp"
#include "vector_file.h"
#include "robot.hpp"
#include "config_file.h" 


using namespace std;

char  	line		  [255];
char 	ConfigFileName[] = "config.ini";


int* atoi_array( char** string, int num )
{
	int* is = (int*)malloc( num *sizeof(int) );
	for (int i=0; i<num; i++)
	{
		is[0] = atoi(string[i]);
	}	
	return is;
}

float* atof_array( char** string, int num )
{
	float* is = (float*)malloc( num *sizeof(float) );
	for (int i=0; i<num; i++)
	{
		is[i] = atof( string[i] );
		//printf("string[%d]: %s\t=%5.3f\n", i, string[i], is[i] );		
	}
	return is;
}

// Integer numbers.
void parse_number_line( char* text, int* numbers, int* num_params )
{
	char** params = split( text, ',', num_params );
	for (int i=0; i<*num_params; i++)
	{		
		numbers[i] = atoi(params[i]);
	}
}

void read_instance_line( FILE* f, Robot& mRobot )
{
	// READ FROM FILE:
	getLine( f, line );
	int numbers[64];
	int num_params=0;
	parse_number_line( line, numbers, &num_params );

	// PARSE DELIMINATORS INTO AN ARRAY : 
	byte limb_index         = numbers[0];		// Extract the Appendage Index : 
	byte actuators			= numbers[1];
	printf("Parsing Limb[%d] has %d actuators; Instances: ", limb_index, actuators);
	if (limb_index > mRobot.limbs.size())
	{
		printf("Error reading config : Supplied vector index is more than the robot has!\n");
		exit(0);
	}

	MotorControl* ni = NULL;
	for (int i=2; i<num_params; i++)
	{
		ni = new MotorControl();
		ni->Instance = numbers[i];
		mRobot.limbs[limb_index]->actuators.push_back( ni );
		printf("%d, ", numbers[i]);
	}
	printf("\n");
}

void read_direction_line( FILE* f, Robot& mRobot )
{
	// READ FROM FILE:
	getLine( f, line );
	int numbers[64];
	int num_params=0;
	parse_number_line( line, numbers, &num_params );
	
	// PARSE DELIMINATORS INTO AN ARRAY:
	byte limb_index         = numbers[0];		// Extract the Appendage Index
	byte actuators			= numbers[1];
	printf("Limb[%d] Actuator + Directions:\t", limb_index);
	for (int i=2; i<num_params; i++)
	{
		if (limb_index > mRobot.limbs.size())
		{
			printf("Error reading config : Supplied vector index is more than the robot has!\n");
			exit(0);
		}

		mRobot.limbs[limb_index]->actuators[i-2]->MotorDirection = numbers[i];
		printf("[%d]=%d, ", i-2, numbers[i]);
	}
	printf("\n");
}

void read_feedback_msg_ids( FILE* f, Robot& mRobot )
{
	// READ FROM FILE:
	getLine( f, line );
	int num_params=0;
	char** params = split( line, ',', &num_params );

	// PARSE MSG IDS INTO AN ARRAY :
	byte limb_index         = atoi(params[0]);		// Extract the Limb (Appendage) Index
	for (int i=1; i<num_params; i++)
	{		
		if (limb_index > mRobot.limbs.size())
		{
			printf("Error reading config : Supplied vector index %d is more than the robot has!\n", limb_index);
			exit(0);
		}
		char* s = params[i];
		while (*s == ' ') s++;
		int id = getID( s );
		mRobot.limbs[limb_index]->actuators[i-1]->Feedback_Msg_id = id;
		printf("Limb[%d][%d] Feedback Msg ID=|%s|%d\n", limb_index, i-1, s, id );
	}
}

/* if the Feedback Msg ID is ID_ANALOG_MEASUREMENT, then we need to know what 
   index (data[0]) corresponds to each actuator
*/
void read_feedback_indices( FILE* f, Robot& mRobot )
{
	// READ FROM FILE:
	getLine( f, line );
	int numbers[64];
	int num_params=0;
	parse_number_line( line, numbers, &num_params );
	
	// PARSE DELIMINATORS INTO AN ARRAY:
	byte limb_index         = numbers[0];		// Extract the Appendage Index 

	for (int i=1; i<num_params; i++)
	{
		if (limb_index > mRobot.limbs.size())
		{
			printf("Error reading config : Supplied vector index is more than the robot has!\n");
			exit(0);
		}
		printf("Limb[%d][%d] feedback index=%d\n", limb_index, i-1, numbers[i] );
		mRobot.limbs[limb_index]->actuators[i-1]->Feedback_index = numbers[i];
	}
}


void read_enable_line( FILE* f, Robot& mRobot )
{
	// READ FROM FILE:
	getLine( f, line );
	int numbers[64];
	int num_params=0;
	parse_number_line( line, numbers, &num_params );
	
	// PARSE DELIMINATORS INTO AN ARRAY:
	byte limb_index         = numbers[0];		// Extract the Appendage Index
	byte actuators			= numbers[1];
	printf("Limb[%d] Actuator Enables:\t", limb_index );
	for (int i=2; i<num_params; i++)
	{
		if (limb_index > mRobot.limbs.size())
		{
			printf("Error reading config : Supplied vector index is more than the robot has!\n");
			exit(0);
		}
		if (numbers[i])	numbers[i] = TRUE;		// 2,3,4,etc ==> 1		
		mRobot.limbs[limb_index]->actuators[i-2]->MotorEnable = numbers[i];
		mRobot.limbs[limb_index]->actuators[i-2]->ActiveOutputs = numbers[i];
		printf("[%d]=%d, ", i-2, numbers[i] );
	}
	printf("\n");
}

void read_zero_offsets( FILE* f, Robot& mRobot )
{
	// READ FROM FILE : 
	getLine( f, line );
	int numbers[64];
	int num_params=0;
	parse_number_line( line, numbers, &num_params );
	
	// PARSE DELIMINATORS INTO AN ARRAY:
	byte limb_index         = numbers[0];		// Extract the Appendage Index
	byte actuators			= numbers[1];
	printf("Limb[%d] Actuator Zeros:\t\t", limb_index );
	for (int i=2; i<num_params; i++)
	{
		if (limb_index > mRobot.limbs.size())
		{
			printf("Error reading config : Supplied vector index is more than the robot has!\n");
			exit(0);
		}
		mRobot.limbs[limb_index]->actuators[i-2]->ZeroOffset = numbers[i];
		printf("[%d]=%d, ", i-2, numbers[i] );
	}
	printf("\n");
}
		
		
		
/* Reads the motor stop position info from the config file.
	Each line has only 1 stop.  So 2 lines for each board.
*/
void read_stop_line(FILE* f, Robot& mRobot)
{
	// READ FROM FILE:
	getLine( f, line );
	int num_params=0;

	char** params = split( line, ',', &num_params );
	int numbers[64];
	
	// PARSE DELIMINATORS INTO AN ARRAY:
	// Extract the Appendage Index :
	byte limb_index         = atoi(params[0]);
	byte actuator_index		= atoi(params[1]);
	byte stop_index			= atoi(params[2]);
	printf("Limb[%d][%d] Stop #%d:  \t", limb_index, actuator_index, stop_index);
	if (limb_index > mRobot.limbs.size() )
	{
		printf("Error reading config : Supplied vector index is more than the robot has!\n");
		exit(0);
	}
	if ( actuator_index > (mRobot.limbs[limb_index]->actuators.size() ) )
	{
		printf("Error reading config : Supplied actuator index is more than the limb has!\n");
		exit(0);
	}

	float angle = atof(params[3]);
	word  value = strtol(params[4], NULL, 16);

	if (stop_index == 1)
	{
		mRobot.limbs[limb_index]->actuators[actuator_index]->stop1.Enabled  = TRUE;	
		mRobot.limbs[limb_index]->actuators[actuator_index]->stop1.Angle    = angle;
		mRobot.limbs[limb_index]->actuators[actuator_index]->stop1.PotValue = value;
	} else {
		mRobot.limbs[limb_index]->actuators[actuator_index]->stop1.Enabled  = TRUE;	
		mRobot.limbs[limb_index]->actuators[actuator_index]->stop2.Angle    = angle;
		mRobot.limbs[limb_index]->actuators[actuator_index]->stop2.PotValue = value;	
	}
	printf("Angle=%6.2f;  PotValue=%d\n", angle, value );			
	//printf("Done\n");
}


/* Main routine for reading the entire config file */
void read_config( char* mFilename, Robot& mRobot )
{
	printf("=== Reading Config file: %s ===\n", mFilename);
	FILE* f = fopen(mFilename, "r");

	// READ Number of Limbs:
	Appendage* ai;
	getLine( f, line );			// First Line has number of appendages.
	int Number_of_appendages = atoi( line );
	for (int i=0; i<Number_of_appendages; i++) {
		ai = new Appendage();
		mRobot.limbs.push_back( ai );
	}
	printf("Number of Robot Limbs = %d\n", mRobot.limbs.size() );

	// READ NAMES : 
	for (int i=0; i<Number_of_appendages; i++)
	{
		getLine( f, line );
		strcpy ( mRobot.limbs[i]->Name, line );
		printf("%s\n", mRobot.limbs[i]->Name );
	}
	// READ LIMB ENABLES : 	
	for (int i=0; i<Number_of_appendages; i++)
	{
		getLine( f, line );
		if (strcmp(line, "ENABLED")==0) {
			printf("limb[%d] : Enabled\n", i );
			mRobot.limbs[i]->Enable = true;
		} else {
			printf("limb[%d] : Disabled\n", i );
			mRobot.limbs[i]->Enable = false;
		}		
	}

	// READ ACTUATORS : 
	for (int i=0; i<Number_of_appendages; i++)
	{
		// We expect every limb to have a line of instances:
		// The function below allocates the struct and it's sub pointers and 
		// initializes with data from the file.		
		read_instance_line( f, mRobot );
	}
	printf("\n");
	// READ Feedback Message IDs
	for (int i=0; i<Number_of_appendages; i++)
		read_feedback_msg_ids( f, mRobot );
	// READ Feedback Indices:
	for (int i=0; i<Number_of_appendages; i++)
		read_feedback_indices( f, mRobot );
	printf("\n");
	
	// READ DEFAULT DIRECTIONS (-1 needed if feedback direction is opposite motor direction): 
	for (int i=0; i<Number_of_appendages; i++)
		read_direction_line( f, mRobot );
	// READ MOTOR ENABLES    
	for (int i=0; i<Number_of_appendages; i++)
		read_enable_line( f, mRobot );
	// READ ZERO OFFSETS : 
	for (int i=0; i<Number_of_appendages; i++)
		read_zero_offsets( f, mRobot );

	// For each Actuator : 
	for (int i=0; i<Number_of_appendages; i++)
	{
		for (int j=0; j<mRobot.limbs[i]->actuators.size(); j++)
		{
			read_stop_line( f, mRobot );	
			read_stop_line( f, mRobot );				
		}
	}
	printf("=== End of Config File ===\n");
}

/* COMPLETED:  Compiles & Reads Data correctly Sat April 5, 2014 
	Now Use to read Sequence file.
*/

