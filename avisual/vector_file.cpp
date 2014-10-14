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
#include "board_list_oop.hpp"
#include "packer_motor.h"
//#include "motor_vector.h"
//#include "config_file.h"
#include "vector_file.hpp"
#include "string_util.h"
#include "robot.hpp"

/* 
The robot will need multiple lists, not just 1 head/tail pointer list!
1 for each anatommical part.
This file will handle everything dealing with 1 list.  Making accessible
methods for adding multiple lists.

This lends itself to object oriented C++
*/

/************************************************************
**** OneVector
************************************************************/
OneVector::OneVector( int Dimension )
: dimension(Dimension)
{
	fData = new float[Dimension];
}
OneVector::OneVector( OneVector* original )		// copy ctor	
{
	dimension = original->get_dimension();
	fData     = new float[dimension];
	for (int i=0; i<dimension; i++)
		fData[i] = original->get_element(i);
}	
OneVector::~OneVector()
{
	delete fData;
}

void OneVector::set_element(  float mValue, int mIndex )
{
	if (fData==NULL) return;
	
	fData[mIndex] = mValue;	
}

float OneVector::get_element( int mIndex )
{
	if (fData==NULL) return 0.;		// at least we're not accessing it.
	return fData[mIndex];
}		

float OneVector::operator[]( int   mIndex )
{
	if (fData==NULL) return 0.;		// at least we're not accessing it.
	return fData[mIndex];
}
void OneVector::operator-=( OneVector* mSub	)
{
	if ((fData==NULL) || (mSub==NULL)) return;		// at least we're not accessing it.
	for (int i=0; i<dimension; i++)
		fData[i] -= mSub->get_element(i);		
}
float	OneVector::x	(  	)
{
	if (fData==NULL) return 0.;		// at least we're not accessing it.
	return fData[0];
}
float	OneVector::y	(  	)
{
	if (fData==NULL) return 0.;		// at least we're not accessing it.
	return fData[1];
}
float	OneVector::z	(  	)
{
	if (fData==NULL) return 0.;		// at least we're not accessing it.
	return fData[2];
}

float* OneVector::atof_array( char** mStrArray )
{
	for (int i=0; i<dimension; i++)
		fData[i] = atof( mStrArray[i] );
	return fData;
}

float OneVector::get_magnitude(	)
{
	float mag = 0.0;
	for (int i=0; i<dimension; i++)
		mag += ( fData[i]*fData[i] );
	
	return sqrt(mag);
}

void OneVector::to_radians( )
{
	for (int i=0; i<dimension; i++)
		fData[i] =  fData[i] * (M_PI/180.0);
}
void OneVector::to_degrees( )
{
	for (int i=0; i<dimension; i++)
		fData[i] = fData[i] * (180.0/M_PI);
}


/************************************************************
**** VECTOR SEQUENCE
************************************************************/
VectorSequence::VectorSequence(int mDimension)
:Dimension(mDimension)
{	
	CurrentIndex = 0;
}

VectorSequence::~VectorSequence()
{

}

void VectorSequence::set_mode( int mMode  )
{
	Mode = mMode;
}

OneVector* VectorSequence::get_vector( )
{
	//printf("get_vector:CurrIndex=%d\n", CurrentIndex );
	if (CurrentIndex >= getNumItems())
	{
		switch (Mode)
		{
		case MODE_ONCE_THRU: return NULL;  
		case MODE_LOOP	   : 
							CurrentIndex = 0;
							break;	
		default: break;
		}
	}
	return (OneVector*)(getItem( CurrentIndex )->getData());
}

OneVector* VectorSequence::get_ivector ( int mIndex ) 
{
	return (OneVector*)( getItem(mIndex)->getData() );
}

/* Gives the speed expected at the instant of the position. 
	ie based on previous position (not future position).
   Caller must delete returned object when done to prevent memory leaks.
*/
OneVector* VectorSequence::get_ivelocity_vector( int mIndex )
{
	if (mIndex==-1) mIndex = CurrentIndex;
	if (mIndex==0) return NULL;
	
	OneVector* prevPos =  (OneVector*)( getItem(mIndex-1)->getData() );
	OneVector* currPos =  (OneVector*)( getItem(mIndex  )->getData() );	
	OneVector* speed = new OneVector(currPos);	
	*speed -= prevPos;
	return speed;
}

/* Gives the speed expected at the instant of the position. 
	ie based on previous position (not future position).
   Caller must delete returned object when done to prevent memory leaks.
*/
OneVector* VectorSequence::get_iaccel_vector( int mIndex )
{
	if (mIndex==-1) mIndex = CurrentIndex;
	if (mIndex <= 1) 	  return NULL;
	
	OneVector* prevVel =  get_ivelocity_vector( mIndex-1 );
	OneVector* currVel =  get_ivelocity_vector( mIndex );
	OneVector* accel   =  new OneVector( currVel );
	*accel -= prevVel;
	return accel;
}

OneVector* VectorSequence::new_vector()
{
	return new OneVector(Dimension); 
}

Item* VectorSequence::add_vector( OneVector* mVector )
{
	Item* newItem = new Item();
	newItem->setData( mVector );
	Add( newItem );
	return newItem;
}

OneVector* VectorSequence::new_add_vector( )
{
	OneVector* nvector = new_vector();
	add_vector( nvector );
	return nvector;
}

BOOL VectorSequence::move_to( int mIndex )
{
	if (mIndex > getNumItems())
		return FALSE;

	CurrentIndex = mIndex;
	return TRUE;		
}

/************************************************************
**** VECTOR GROUP SEQUENCE
************************************************************/
VectorGroupSequence::VectorGroupSequence()
{
	fd=NULL;
	data_is_radians = FALSE;
	playback_period_ms = 50.0;
}

void VectorGroupSequence::add_new_vector_sequence( int dimension )
{
	VectorSequence* tmp_vs   = new VectorSequence( dimension );
	Item*			tmp_item = new Item( );
	tmp_item->setData( tmp_vs );	
	Add( tmp_item );
}

void VectorGroupSequence::copy_robot_structure(Robot *mRobot)
{
	// Create a VectorSequence for each limb.	
	VectorSequence* tmp;
	int dimension = 0;
	int num = mRobot->get_number_limbs();
	printf("copy_robot_structure: allocating for %d limbs\n", num);
	for (int i=0; i<num; i++)
	{
		dimension = ((MotorPack*)mRobot->get_limb(i))->getNumItems();
		printf("adding VectorSequence(dimension=%d)\n", dimension);
		add_new_vector_sequence(dimension);			
	}
	//printf("vector group : copying %d limbs\n", getNumItems() );
}

VectorGroupSequence::~VectorGroupSequence()
{
	FreeList();
/*	Item* item = NULL;
	VectorSequence* tmp;
	int num = getNumItems();
	for (int i=0; i<num; i++)
	{
		item = getItem(i);
		tmp = item->getData();
		delete tmp;
		delete item;
	}		*/
}


void VectorGroupSequence::set_data_type_all_lists( byte mType, word mSpeed )
{
	VectorSequence* tmp;
	for (int i=0; i<getNumItems(); i++)
	{
		tmp = (VectorSequence*)(getItem(i)->getData());
		tmp->data_type = mType;			// INT or FLOAT
		if (mSpeed)
			tmp->data_type |= SPEED_VECTORS;
		else 
			tmp->data_type |= POSITION_VECTORS;
	}
}


/************************************************************
**** VECTOR ROUTINES 
************************************************************/



/****************  FILE ACCESS ROUTINES  *********************/
/* HEADER INFO:	*/
int   playback_period_ms;
BOOL  data_type = FLOAT;
BOOL  position_vectors = TRUE;

/* Vector file header */
void VectorGroupSequence::read_header( )
{
	char  data[255];

	// TIME SLICE Period (rate of vector playback)	
	getLine_nb(fd, data);

	int playback_period_ms = atoi(data);
	//	set_playback_period_all_lists( playback_period_ms  );
	printf("playback_period = %d\n", playback_period_ms );

	// DATA TYPE = "float" or "int" 
	getLine_nb(fd, data);	
	printf("data type = %s\n", data);
	if (strcasecmp(data,"float")==0)
		data_type = FLOAT;
	else if (strcasecmp(data,"int")==0)
		data_type = INT;

	// RADIANS or DEGREES:
	getLine_nb(fd, data );
	
	if (strcasecmp(data,"radians")==0)
	{	data_is_radians = TRUE;		
		printf("data is radians!\n");
	}
	else if (strcasecmp(data,"degrees")==0)
	{	data_is_radians = FALSE;	
		printf("data is degrees!\n");
	}

	// POSITION OR SPEED VECTORS:
	getLine_nb(fd, data);	
	printf("data type = %s\n", data);
	if (strcasecmp(data,"position")==0)
		data_type = FLOAT;
	else if (strcasecmp(data,"speed")==0)
		data_type = INT;
}

void VectorGroupSequence::set_mode( int mMode  )
{
	MotorPack* mp  = NULL;
	int num_groups = getNumItems();
	VectorSequence* vs;

	// Go thru each limb:
	for (int i=0; i<num_groups; i++)
	{
		vs = get_limb_sequence(i);
		vs->set_mode( mMode );
	}
}
VectorSequence*	VectorGroupSequence::get_limb_sequence( byte mIndex )
{
	return (VectorSequence*)(getItem(mIndex)->getData());
}

/*   */
OneVector*	VectorGroupSequence::get_vector_deg( int mLimbIndex )
{
	VectorSequence* vs = get_limb_sequence( mLimbIndex );
	if (vs)
	{
		OneVector* ov     = vs->get_vector();
		OneVector* deg_ov = new OneVector( ov );
		if (data_is_radians)
		{
			deg_ov->to_degrees();
		}
		return deg_ov;
	}
	return NULL;
}

/* The caller must delete the returned object after done of memory leaks will occurr!! */
OneVector*	VectorGroupSequence::get_vector_rad( int mLimbIndex )
{
	VectorSequence* vs = get_limb_sequence( mLimbIndex );
	if (vs)
	{
		OneVector* ov     = vs->get_vector();
		OneVector* rad_ov = new OneVector(ov);
		if (data_is_radians==FALSE)
		{
			rad_ov->to_radians();
		}
		return rad_ov;
	}
	return NULL;
}

OneVector*	VectorGroupSequence::get_velocity_deg	 ( int mLimbIndex, int mIndex )
{
	VectorSequence* vs = get_limb_sequence( mLimbIndex );
	vs->get_ivelocity_vector( mIndex );
}

OneVector*	VectorGroupSequence::get_acceleration_deg( int mLimbIndex, int mIndex )
{
	VectorSequence* vs = get_limb_sequence( mLimbIndex );
	vs->get_iaccel_vector( mIndex );	
}

/* 
Read 1 vector from the file : 
Return :  free floating vector (not attached to any list)
*/
OneVector* VectorGroupSequence::read_line( byte mDataType )
{
	if (feof(fd)) 			return NULL;

 	char  data[255];
 	int number_strings;

 	getLine_nb( fd, data );  
 	if (data[0]==0) return NULL;
	printf("VectorGroupSequence: %s \n", data );

	char** array = split( data,',', &number_strings );
 	if (number_strings<=1)   return NULL;
	 	
	// Limb index:
	int    limb_index = atoi( array[0] );
	int    dimension  = atoi( array[1] );
	//printf("Vector: Limb[%d] dimen=%d\n", limb_index, dimension );

	// GET Limb Dimension:
	Item* limb_item    = getItem(limb_index);
	VectorSequence* vs = (VectorSequence*)(limb_item->getData());
	int num_motors=0;
	if (vs)
		num_motors = vs->get_dimension();
	OneVector* v = vs->new_add_vector();

	// INCOMING VECTOR HAS TO MATCH SIZE OF THE LIMB:
	if (dimension < num_motors )
	{
		printf("Vector size=%d, but %d limb motors; padding with zeros!\n", dimension, num_motors);
		// really want to pad with previous vectors values. if it's the first vector, 
		// then pad with 0's.
		for (int i=0; i < num_motors; i++)
	    { 
	      if (i<dimension)
		  	  v->set_element( atof(array[2+i]), i );
		  else 
			  v->set_element( 0.0, i );
	  	}
	  	//printf("done!\n");
	  	return v;
	} else if (dimension > num_motors )
	{
		printf("Vector size=%d, but %d limb motors; ignoring extra values!\n", dimension, num_motors);	
		// trunc happens automatically in the atof_array()!
	}

	if ((data_type & FLOAT)==FLOAT) 
	{
		v->atof_array( &(array[2]) );
	} else { 
		v->atof_array( &(array[2]) );
	}

	printf("limb=%d;  %d vector added!\n", limb_index, vs->getNumItems() );
	return v;
}

void VectorGroupSequence::read_one_vector( byte mDataType  )
{
	// for all groups	
}

void VectorGroupSequence::read_vector_file( char* mFilename )
{
	int num_vectors = 0;
	printf("========= Reading Motion Vector file %s =========\n", mFilename );
  
	fd = fopen(mFilename, "r");
	if (fd==NULL) {
		printf("Could not open file!\n");
		return;
	}
	read_header();

	// READ BODY:
	OneVector* result = read_line( data_type );	// Datatype will come from header.
	while (result!=NULL)
	{
		num_vectors++;
		result = read_line( data_type );
	}
	
	// 
	for (int i=0; i<getNumItems(); i++)
	{
		Item* item = getItem(i);
		VectorSequence* vs = (VectorSequence*)item->getData();
		num_vectors = vs->getNumItems();
		printf("Limb %d, read %d vectors\n", i, num_vectors);
	}
    printf("========= End of Motion Vector file =========\n");
}




// Add the vector to end of the list.    
/*struct sOneVector* list_append_vector( struct sVectorList* mlist,
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
}*/

/*void list_remove_all( struct sVectorList* mlist )
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
}*/
/* Create a new vector in memory */
/*struct sOneVector* new_vector( byte Dimension, byte DataType )
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
}*/
/*void VectorGroupSequence::init(  )
{
    //VectorLists = (struct sVectorList**) malloc( mNumLimbs * sizeof(struct sVectorList*) );
	for (int i=0; i<mNumLimbs; i++)
	{
		init_list(i, ConfigInfo.LimbInfo[i]->Dimension);
	}
}
void set_playback_period_all_lists(int mPeriod)
{
	for (int i=0; i<ConfigInfo.Number_of_appendages; i++)
	{
		VectorLists[i]->playback_period_ms =  mPeriod;
	}
}*/
