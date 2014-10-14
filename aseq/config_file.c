/* 
Sample Config file:
==========================================================
2							// Two vectors for this robot
0, 3, 31, 32, 33				// instance assignments
1, 3, 21, 22, 23				// instance assignments
0, Left Leg
1, Right Leg
0, 0, 1,  15.0, 0x002		// calibrated stops	vector 0
0, 0, 2, 175.0, 0x3C0		
0, 1, 1,  15.0, 0x002		
0, 1, 2, 175.0, 0x3C0		
0, 2, 1,  15.0, 0x002		
0, 2, 2, 175.0, 0x3C0		
1, 0, 1,  15.0, 0x002		// calibrated stops vector 1
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
#include "config_file.h"


char  				line[255];
char 				ConfigFileName[] = "sequencer.ini";
struct sConfigInfo	ConfigInfo;

int* atoi_array( char** string, int num )
{
	int* is = malloc( num *sizeof(int) );
	for (int i=0; i<num; i++)
	{
		is[0] = atoi(string[i]);
	}	
	return is;
}

float* atof_array( char** string, int num )
{
	float* is = malloc( num *sizeof(float) );
	for (int i=0; i<num; i++)
	{
		is[0] = atof( string[i] );
	}
	return is;
}

void read_instance_line( FILE* f )
{
	// READ FROM FILE:
	getLine( f, line );
	int numbers[64];

	int num_params=0;

	char** params = split( line, ',', &num_params );
	for (int i=0; i<num_params; i++)
	{		
		numbers[i] = atoi(params[i]);
	}
	
	// PARSE DELIMINATORS INTO AN ARRAY:
	//	byte num_elements_read = getIntArray( line, numbers );

	// Extract the Appendage Index :
	byte limb_index         = numbers[0];
	byte actuators			= numbers[1];
	printf("Parsing Limb[%d] has %d actuators; Instances: ", limb_index, actuators);
	for (int i=2; i<num_params; i++)
		printf("%d, ", numbers[i]);
	printf("\n");

	if (limb_index > ConfigInfo.Number_of_appendages)
	{
		printf("Error reading config : Supplied vector index is more than the robot has!\n");
		exit(0);
	}

	// Each appendage can have an arbitrary number of motors.  So the 2nd
	// item in the instance line tells that number.	
	ConfigInfo.LimbInfo[limb_index] = construct_LimbInfo( actuators, "noname" );
	ConfigInfo.LimbInfo[limb_index]->Dimension = (actuators);		// set dimension for this vector
		
	// Copy the instances to a more permanent location 
	for (int j=0; j< (numbers[1]); j++)
	{
		// (don't include the appendage index | dimension)
		ConfigInfo.LimbInfo[limb_index]->instances[j] = numbers[j+2];
	}
}

/* Reads the motor stop position info from the config file.
	Each line has only 1 stop.  So 2 lines for each board.
*/
void read_stop_line(FILE* f)
{
	// READ FROM FILE:
	getLine( f, line );
	int num_params=0;

	char** params = split( line, ',', &num_params );
//	data[0] = atoi( column[0] );
	int numbers[64];
	
	// PARSE DELIMINATORS INTO AN ARRAY:
	//	byte num_elements_read = getIntArray( line, numbers );

	// Extract the Appendage Index :
	byte limb_index         = atoi(params[0]);
	byte actuator_index		= atoi(params[1]);
	byte stop_index			= atoi(params[2]);
	printf("Parsing Limb:%d Actuator:%d Stop:%d  \t", limb_index, actuator_index, stop_index);
	if (limb_index > ConfigInfo.Number_of_appendages)
	{
		printf("Error reading config : Supplied vector index is more than the robot has!\n");
		exit(0);
	}

	// Each appendage can have an arbitrary number of motors.  So the 2nd
	// item in the instance line tells that number.	
	float angle = atof(params[3]);
	word value = strtol(params[4], NULL, 16);
	printf("Angle=%6.2f;  PotValue=%d\n", angle, value );
	
	ConfigInfo.LimbInfo[limb_index]->CalInfos[actuator_index+stop_index-1].Angle    = angle;
	ConfigInfo.LimbInfo[limb_index]->CalInfos[actuator_index+stop_index-1].PotValue = value;
	//printf("Done\n");
}

struct sAppendageInfo* construct_LimbInfo(byte Dimension, char* mName)
{
	static struct sAppendageInfo* ptr;
	ptr = (struct sAppendageInfo*) malloc( sizeof(struct sAppendageInfo) );
	ptr->Dimension = Dimension;
	ptr->instances = (byte*)malloc(Dimension*sizeof(byte));

	// NAME:
	ptr->Name = (char*)malloc(strlen(mName)*1);
	strcpy( ptr->Name, mName );	
	
	// MAKE ROOM FOR CALIBRATION INFO:
	ptr->CalInfos = (struct sCalInfo*)malloc(Dimension*2*sizeof(struct sCalInfo) );	
	return ptr;
}

/* Main routine for reading the entire config file */
void read_config( )
{
	printf("=== Reading Config file: %s ===\n", ConfigFileName);
	FILE* f = fopen(ConfigFileName, "r");

	// READ Number of Limbs:
	getLine( f, line );			// First Line only has num vectors
	ConfigInfo.Number_of_appendages = atoi( line );
	printf("Number of Robot Limbs = %d\n", ConfigInfo.Number_of_appendages );

	// CREATE an array of Pointers to the structures.
	ConfigInfo.LimbInfo	= (struct sAppendageInfo**) malloc( 
							ConfigInfo.Number_of_appendages * sizeof(struct sAppendageInfo*) );

	// We expect every limb to have a line of instances:
	for (int i=0; i<ConfigInfo.Number_of_appendages; i++)
	{
		// The function below allocates the struct and it's sub pointers and 
		// initializes with data from the file.		
		read_instance_line( f );        		
	}

	for (int i=0; i<ConfigInfo.Number_of_appendages; i++)
	{
		for (int j=0; j<ConfigInfo.LimbInfo[i]->Dimension; j++)
		{
			read_stop_line( f );	
			read_stop_line( f );				
		}
	}
	printf("=== End of Config File ===\n");
}

/* COMPLETED:  Compiles & Reads Data correctly Sat April 5, 2014 
	Now Use to read Sequence file.
*/

