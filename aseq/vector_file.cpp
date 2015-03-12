/**** MOTION VECTOR FILE ****

PURPOSE:  This file handles reading the vectors from a file, creating a 
		   linked list of the data, and associating with one of the limbs.

Text file .csv format.
First parameter specifies the anatomical part it belongs to.  ie 
 		0=> left leg,
		1=> right leg,
		2=> left arm,
		3=> right arm

The dimension comes next. ie 3 elements.
Next comes the data.  All of this is ',' separated.

Line Description:
[anatomical part] [dimension] [datum1], [datum2], [datum3], ...

Example line:
1, 4, 0.0, 0.0, 0.0, 0.0
or 
1, 3, 10.0, -5.0, -5.0				// for right leg slight forward

The timeslice will only be incremented when a subsequent vector of
the same anatomical part is given.
For example,
0, 3, 0, 0, 0
1, 3, 0, 0, 0
will share same timeslice
0, 3, 10, -5, -5		will be next time slice.
if an anatomical part is missing for a timeslice, then it's previous vector will
be duplicated.

This is a sample of the vector file :

	A) malloc entire array
	B) malloc 1 vectors worth;  
		add to an array of pointers
		Not so good because length is unknown at start
	c) malloc 1 vectors worth;
		use linked list
		Yes.
*/
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <signal.h>
#include <time.h>
#include <wiringPi.h>
#include <string.h>
#include <vector>

#include "gpio.h"
#include "mcp2515.h"
#include "pican_defines.h"
#include "gpio.h"
#include "CAN_Interface.h"
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
#include "motor.hpp"
#include "vector_file.h"
#include "motor_vector.h"
#include "Appendage.hpp"
#include "robot.hpp"
#include "config_file.h"


/* 
The robot will need multiple lists, not just 1 !
1 for each anatommical part.
This file will handle everything dealing with 1 list.  Making accessible
methods for adding multiple lists.

This lends itself to object oriented C++
*/
sOneVector::sOneVector()
{	
	limb     = NULL;
	data_type = POSITION_VECTORS_COUNT;	// default
}

void sOneVector::print_vector()
{
	printf(" Vector : ");
	for (int i=0; i<Data.size(); i++)
		printf(" %5.1f ", Data[i] );
	printf("\n");
}

float sOneVector::get_angle(byte index)
{	
	if (data_type==POSITION_VECTORS_ANGLE)
		return Data[index];
	else if (data_type==POSITION_VECTORS_COUNT)
		return (Data[index] - limb->actuators[index].ZeroOffset)* DegreesPerCount;
	else if (data_type==POSITION_VECTORS_ANGLE)
		return -1;
}

word sOneVector::get_count(byte index)
{
	if (data_type==POSITION_VECTORS_ANGLE)
		return Data[index];
	else if (data_type==POSITION_VECTORS_COUNT)
		return (Data[index] - limb->actuators[index].ZeroOffset)* DegreesPerCount;
	else if (data_type==POSITION_VECTORS_ANGLE)
		return -1;	
}


/************************************************************
**** sVectorSet SET ********
************************************************************/
sVectorSet::sVectorSet()
{
	playback_period_ms = 40;		// default = 25 hz
	data_type		   = POSITION_VECTORS_ANGLE;
}

void sVectorSet::print_vectors( )
{
	printf("Vectors: \n");	
	for (int i=0; i<vectors.size(); i++)
	{
	  	for (int j=0; j<vectors[i].Data.size(); j++)
			printf(" %5.3f ", vectors[i].Data[j] );
		printf("\n");
	}
}

/************************************************************
****  ROBOT VECTOR SET 		sRobotVector			 ********
*************************************************************/
sRobotVector::sRobotVector()
{	
		
	Current_Vindex 		 =  0;
	iterations_requested = -1;	
	iterations_completed =  0;
}

void sRobotVector::set_playback_period_all_lists(int mPeriod)
{
	for (int i=0; i<limbs.size(); i++)
	{
		limbs[i].playback_period_ms =  mPeriod;
	}
}

void sRobotVector::set_data_type_all_lists(byte mType, word mSpeed)
{
	for (int i=0; i<limbs.size(); i++)
	{
		limbs[i].data_type = mType;		// INT or FLOAT
		if (mSpeed)
			limbs[i].data_type |= SPEED_VECTORS;
		else 
			limbs[i].data_type |= POSITION_VECTORS;
	}
}

float sRobotVector::get_destination( int mAppendageIndex, int mActuator )
{
	return limbs[mAppendageIndex].vectors[Current_Vindex].Data[mActuator];
}

// Add the vector to end of the list 
void sRobotVector::list_append_vector( int mListIndex, struct sOneVector& mv )
{
	limbs[mListIndex].vectors.push_back( mv );
}

/****************  FILE ACCESS ROUTINES  *********************/
FILE* f = NULL;

/* Vector file header */
void sRobotVector::read_header(FILE* f)
{
	char  data[255];

	// TIME SLICE Period (rate of vector playback)	
	getLine(f, data);
	
	int playback_period_ms = atoi(data);
	
	set_playback_period_all_lists( playback_period_ms );
	//printf("playback_period = %d\n", limbs[0].playback_period_ms );

	// DATA TYPE = "float" or "int" 
	getLine(f, data);	
	printf("data type = %s\n", data);
	if (strcmp(data,"float")==0)
		data_type = FLOAT;
	else if (strcmp(data,"int")==0)
		data_type = INT;

	// POSITION OR SPEED VECTORS:
	getLine(f, data);
	printf ("data type = %s\n", data);
	if (strcmp(data,"position")==0)
		data_type = FLOAT;
	else if (strcmp(data,"speed")==0)
		data_type = INT;

	// POSITION_VECTORS_ANGLE, POSITION_VECTORS_COUNT
	set_data_type_all_lists( data_type, 0 );
}

/* 
Read 1 vector from the file : 
*/
int sRobotVector::read_line(FILE* f, byte mdata_type )
{
	if (feof(f)) 			return 0;
 	char  data[255];
 	int   number_strings;

 	getLine( f, data );  
 	printf("readline: %s\n", data );
 	
 	if (data[0]==0) return 0;
 	
	char** array = split	 ( data,',', &number_strings );
 	if (number_strings<=1)   return 0;
	
	float* line  = atof_array( array, number_strings 	 );
//	int*   line  = atoi_array( array, number_strings 	 );

	// Limb index : 
	int    limb_index = atoi( array[0] ); 
	int    dimension  = atoi( array[1] ); 
	//printf("Vector: n=%d, Limb[%d] dimen=%d \n", number_strings, limb_index, dimension ); 

	// Dimension
	//	printf("New vector dimension=%d\n", ConfigInfo.LimbInfo[limb_index]->Dimension );
	struct sOneVector v;
	float tmp;
	
	// Datum (position or speed; float or int)
	for (int i=0; i<dimension; i++)
	{
		  tmp = atof(array[i+2]);
		  //printf(" %6.2f ", tmp );
		  v.Data.push_back( tmp );

		 /* if ((data_type & FLOAT)==FLOAT) {
			  printf("%6.2f, ",v.Data[i]);
		  } else { 
			  v->.iData[i] = atoi(array[i+2]); 
			  printf("%d, ",v.Data[i] );
		  } */
	}
	//printf("\t");
	list_append_vector( limb_index, v );
	//printf("read done.\n");
	return 1; 
}
void sRobotVector::init_limbs(  )
{
	class sVectorSet list;
	list.playback_period_ms = 40;
	list.data_type = 0;	
	//printf("%d Appendages\n", Appendages.size() );
	for (int i=0; i<Appendages.size(); i++)
		limbs.push_back( list );

}
void sRobotVector::read_vector_file( char* mFilename )
{	
	init_limbs();
	
	printf("=== Reading Motion Vector file:  %s ===\n", mFilename );
	f = fopen(mFilename, "r");
	if (f==NULL) {
		printf("Could not open file!\n");
	}
	read_header(f);

	// READ BODY:
	int result = read_line(f, data_type );	// data_type will come from header
	while (result!=0)
	{
		result = read_line( f, data_type );
	}
	printf("Read %d vectors\n", limbs[0].vectors.size() );
    printf("=== End of Motion Vector file ===\n");
    //print_vectors( 1 );
}


/*float* atof_array(byte Dimension, char** mArray)
{
	float* Angles = malloc( Dimension*sizeof(float) );
	for (int i=0; i<Dimension; i++)
		Angles[i] = atof(mArray[i]);
	return Angles;
	}*/
