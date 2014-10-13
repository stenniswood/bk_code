/* *** MOTION VECTOR FILE ***

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
#include "mcp2515.h"
#include "pican_defines.h"
#include "CAN_Interface.h"
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
//#include "motor_vector.h"
//#include "config_file.h"
#include "vector_file.h"


/* 
The robot will need multiple lists, not just 1 head/tail pointer list!
1 for each anatommical part.
This file will handle everything dealing with 1 list.  Making accessible
methods for adding multiple lists.

This lends itself to object oriented C++
*/

/************************************************************
**** LIST ROUTINES 
************************************************************/
struct sVectorList** VectorLists;


void init_lists()
{
	VectorLists = (struct sVectorList**) malloc( ConfigInfo.Number_of_appendages * sizeof(struct sVectorList*) );
	for (int i=0; i<ConfigInfo.Number_of_appendages; i++)
	{
		init_list(i, ConfigInfo.LimbInfo[i]->Dimension);
	}
}

/* Init the list structure.  If convert to C++ this is constructor */
void init_list( byte ListIndex, int Dimensionality )
{
	VectorLists[ListIndex] = (struct sVectorList*) malloc( sizeof(struct sVectorList) );
	VectorLists[ListIndex]->HeadPtr 	= NULL;
	VectorLists[ListIndex]->TailPtr 	= NULL;
	VectorLists[ListIndex]->NumVectors	= 0;
	VectorLists[ListIndex]->Dimension	= Dimensionality;
	VectorLists[ListIndex]->playback_period_ms = 40;		// default = 25 hz
	VectorLists[ListIndex]->data_type	= INT;
}

void set_playback_period_all_lists(int mPeriod)
{
	for (int i=0; i<ConfigInfo.Number_of_appendages; i++)
	{
		VectorLists[i]->playback_period_ms =  mPeriod;
	}
}
void set_data_type_all_lists(byte mType, word mSpeed)
{
	for (int i=0; i<ConfigInfo.Number_of_appendages; i++)
	{
		VectorLists[i]->data_type = mType;		// INT or FLOAT
		if (mSpeed)
			VectorLists[i]->data_type |= SPEED_VECTORS;
		else 
			VectorLists[i]->data_type |= POSITION_VECTORS;
	}
}

// Add the vector to end of the list.    
struct sOneVector* list_append_vector( struct sVectorList* mlist,
									   struct sOneVector* mv )
{
	if ((mv) && (mlist))
	{
		mv->Prev = mlist->TailPtr;
		mv->Next = NULL;
		if (mlist->TailPtr)
		{
			mlist->TailPtr->Next = mv;
			mlist->TailPtr       = mv;
		}
	}
	return mv;
}

void list_remove_all( struct sVectorList* mlist )
{
	struct sOneVector* tmp = mlist->HeadPtr;
	// Free up each vector's memory:
	while (tmp)
	{
		free(tmp->values.fData);	// free up data
		tmp = tmp->Next;
		free(tmp->Prev);			// free up the structure memory
	}
	mlist->HeadPtr = NULL;
	mlist->TailPtr = NULL;
	free(mlist);					// 
}

/************************************************************
**** VECTOR ROUTINES 
************************************************************/

/* Create a new vector in memory */
struct sOneVector* new_vector( byte Dimension, byte DataType )
{
	struct sOneVector* v = malloc( sizeof(struct sOneVector) );
	if (v)
	{
		if (DataType==FLOAT)
			v->values.fData = malloc( sizeof(float)*Dimension );
		else
			v->values.iData = malloc( sizeof(int)*Dimension );
	}
	return v;
}

/*float* atof_array(byte Dimension, char** mArray)
{
	float* Angles = malloc( Dimension*sizeof(float) );
	for (int i=0; i<Dimension; i++)
		Angles[i] = atof(mArray[i]);
	return Angles;
	}*/

/****************  FILE ACCESS ROUTINES  *********************/
FILE* f = NULL;

/* HEADER INFO:	*/
int   playback_period_ms;
BOOL  data_type = FLOAT;
BOOL  position_vectors = TRUE;

/* Vector file header */
void read_header(FILE* f)
{
	char  data[255];

	// TIME SLICE Period (rate of vector playback)	
	getLine(f, data);

	int playback_period_ms = atoi(data);
	//	set_playback_period_all_lists( playback_period_ms  );
	printf("playback_period = %d\n", playback_period_ms );

	// DATA TYPE = "float" or "int" 
	getLine(f, data);	
	printf("data type = %s\n", data);
	if (strcmp(data,"float")==0)
		data_type = FLOAT;
	else if (strcmp(data,"int")==0)
		data_type = INT;

	// POSITION OR SPEED VECTORS:
	getLine(f, data);	
	printf("data type = %s\n", data);
	if (strcmp(data,"position")==0)
		data_type = FLOAT;
	else if (strcmp(data,"speed")==0)
		data_type = INT;
}

/* 
Read 1 vector from the file : 
Return :  free floating vector (not attached to any list)
*/
struct sOneVector* read_line(FILE* f, byte mDataType )
{
	if (VectorLists==NULL) 	return NULL;
	if (feof(f)) 			return NULL;

 	char  data[255];
 	int number_strings;

 	getLine( f, data );  
 	if (data[0]==0) return NULL;

	char** array = split	 ( data,',', &number_strings );
 	if (number_strings<=1)   return NULL;
 	
	float* line  = atof_array( array, number_strings 	 );
//	int*  line 	 = atoi_array( char** string, int num 	 );

	// Limb index:
	int    limb_index = atoi( array[0] );
	int    dimension  = atoi( array[1] );
	printf("Vector: Limb[%d] dimen=%d ", limb_index, dimension );

	// Dimension
	//	VectorLists[limb_index]->Dimension = atoi( array[1] );
	//	printf("New vector dimension=%d\n", ConfigInfo.LimbInfo[limb_index]->Dimension );
	MotorPack* mp = Onesimus.get_limb( limb_index );
	int num_motors=0;
	if (mp)
		num_motors = getNumItems();

	struct sOneVector* v = new_vector( num_motors, mDataType );
	// ConfigInfo.LimbInfo[limb_index]->Dimension,

	// Datum (position or speed; float or int)
	// PUT VALUES INTO THE VECTOR:
	for (int i=0; i<VectorLists[limb_index]->Dimension; i++)
	  { 
		  if ((data_type & FLOAT)==FLOAT) {
				v->values.fData[i] = atof(array[i+2]);
				printf("%6.2f, ",v->values.fData[i]); 
		  } else {
				v->values.iData[i] = atoi(array[i+2]);
				printf("%d, ",v->values.iData[i]); 
		  }
	  }

	printf("\n");
	return v;
}


void read_vector_file( char* mFilename )
{
	init_lists();
	int num_vectors = 0;
	printf("=== Reading Motion Vector file %s ===\n", mFilename );
  
	f = fopen(mFilename, "r");
	if (f==NULL) {
		printf("Could not open file!\n");
	}
	read_header(f);

	// READ BODY:
	struct sOneVector* result = read_line(f, data_type );	// Datatype will come from header.
	while (result!=NULL)
	{
		//list_append_vector( VectorLists[0], result );
		num_vectors++;
		result = read_line(f, data_type);
	}
	printf("Read %d vectors\n", num_vectors);
    printf("=== End of Motion Vector file ===\n");
}




