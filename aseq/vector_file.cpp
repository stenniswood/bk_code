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
#include <string>

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

void sOneVector::print_counts()
{
	printf(" Vector (counts): ");
	//printf("data_type=%d:%x\n", data_type, limb );	
	for (int i=0; i<Data.size(); i++)
		printf(" %5d:%5.2f", get_count(i), Data[i] );
	printf("\n");
}

void sOneVector::print_vector()
{
	printf("Vector (angles): ");
	for (int i=0; i<Data.size(); i++)
	    printf(" %5.1f ", get_angle(i) );
	printf("\n");
}

float sOneVector::get_angle(byte index)
{	
	float retval=-1;
	if (data_type==POSITION_VECTORS_ANGLE)
		retval = Data[index];
	else if (data_type==POSITION_VECTORS_COUNT)
		retval = limb->actuators[index].compute_angle(Data[index]);
	return retval;
}

/* Note:  negative counts are not allowed.  A/D can't go negative voltages.
   and the cast on RPI automatically seems to make any negative number a 0.
*/
int sOneVector::get_count(byte index)
{
	int retval = -1;
	if (data_type==POSITION_VECTORS_ANGLE)
		retval = limb->actuators[index].compute_position( Data[index] );
	else if (data_type==POSITION_VECTORS_COUNT)
		retval = (word)(Data[index]);
	return retval;
}

/************************************************************
**** sVectorSet SET ********
************************************************************/
sVectorSet::sVectorSet()
{
	playback_period_ms = 40;		// default = 25 hz
	data_type		   = POSITION_VECTORS_ANGLE;
}

// updates all vectors.
void sVectorSet::set_data_type( byte mDataType )	
{
	data_type = mDataType;
	for (int v=0; v<vectors.size(); v++)
		vectors[v].data_type = mDataType;	
}

void sVectorSet::set_limb( Appendage* mlimb )
{
	for (int i=0; i<vectors.size(); i++)
		vectors[i].limb = mlimb;
}

/* Speed between 2 consequtive vectors 
Return	:	speed (counts per second)
*/
float sVectorSet::calc_average_speed_cps( int mStartIndex, int mActuator )
{
	size_t  size = vectors.size();
	if (mStartIndex >= size) 	mStartIndex = size-1;

	float distance = vectors[mStartIndex+1].get_count(mActuator) - 
					 vectors[mStartIndex].get_count(mActuator);
	//printf("calc_average_speed_cps() distance=", distance);	
	float speed    = distance / (playback_period_ms/1000.);
	return speed;
}

/* Prints entire set of all vectors! */
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

/*
INPUT:
	mType	:	FLOAT or INT
	mSpeed	:	POSITION_VECTORS_ANGLE, POSITION_VECTORS_COUNT, SPEED_VECTORS
*/
void sRobotVector::set_data_type_all_lists(byte mType)
{
	data_type  = mType ;
	for (int i=0; i<limbs.size(); i++)
		limbs[i].set_data_type( data_type );
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
	printf("playback_period = %d\n", playback_period_ms );

	// DATA TYPE = "float" or "int" 
	getLine(f, data);	
	printf("data type = %s\n", data);

	// POSITION OR SPEED VECTORS:
	getLine(f, data);

	if (strcmp(data,"counts")==0)
		data_type = POSITION_VECTORS_COUNT;
	else if (strcmp(data,"angles")==0)
		data_type = POSITION_VECTORS_ANGLE;
	else if (strcmp(data,"speeds")==0)	
		data_type = SPEED_VECTORS;
	printf ("data type = %s:%d\n", data, data_type);
	
}

/* 
Read 1 vector from the file : 
*/
int sRobotVector::read_line(FILE* f, byte mdata_type)
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

	// Limb index : 
	int    limb_index = atoi( array[0] ); 
	int    dimension  = atoi( array[1] ); 
	//printf("Vector: n=%d, Limb[%d] dimen=%d \n", number_strings, limb_index, dimension ); 

	// Dimension
	struct sOneVector v;
	float tmp;

	// Datum (position or speed; float or int)
	//printf("Datum: ");
	for (int i=0; i<dimension; i++)
	{
		  tmp = atof(array[i+2]);
		  //printf(" %6.2f ", tmp );
		  v.Data.push_back( tmp );
	}
	//printf("\n");  
	list_append_vector( limb_index, v );
	//printf("read done.\n");
	return 1; 
}

/* Note: If you wish to print based on the Appendages the robot
	actually has enabled.  Use the Robot.print_vector() function. */
void sRobotVector::print_vector( int mIndex, bool mAngles )
{
	if (mIndex==-1)  mIndex=Current_Vindex;

	printf("Vector[%d] \n", mIndex );
	if (mAngles)
		for (int l=0; l<limbs.size(); l++)		
			limbs[l].vectors[mIndex].print_vector( );
	else
		for (int l=0; l<limbs.size(); l++)
			limbs[l].vectors[mIndex].print_counts( );	
}

void sRobotVector::set_limbs( Robot& mrobot )
{
	for (int l=0; l<limbs.size(); l++)
		limbs[l].set_limb( &(mrobot.limbs[l]) );
}
	
void sRobotVector::init_limbs(  )
{
	class sVectorSet list;
	list.playback_period_ms = 40;
	list.data_type = 0;	

	for (int i=0; i<robot.limbs.size(); i++)
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
	printf("data_type= %d\n", data_type );
	printf("Read %d vectors\n", limbs[0].vectors.size() );
	// POSITION_VECTORS_ANGLE, POSITION_VECTORS_COUNT
	set_data_type_all_lists( data_type );
    printf("=== End of Motion Vector file ===\n");
    /*limbs[0].print_vectors( );
      printf("-------------------------");
      limbs[1].print_vectors( );
      printf("=== End of Motion Vector file ===\n");
	*/
}
