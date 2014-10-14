#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include "bk_system_defs.h"
#include "can_eid.h"
#include "can.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "../pican_defines.h"
#include "leds.h"
#include "can_id_list.h"
#include "system_msgs_callback.h"
#include "../board_list.h"
#include "motor_vector.h"


struct stVector* vectors_head = NULL;
struct stVector* vectors_tail = NULL;

/*  We may choose to setup several vectors for the boards.
	ie. LeftLeg, Right Leg, LeftArm, Right Arm

	how many vectors and how many boards in each depends on the application.
	So how can we make this dynamically changeable?
	
	Dimension a vector as in Matlab.   
			vec1 = motorVect(3);
			vec2 = motorVect(3);
			vec3 = motorVect(5);
			vec4 = motorVect(5);
	Then assign board instance numbers
	
	Each vector then consists of only an Array of Instance numbers.		
*/

/* Allocates room for a new vector and adds it to the list of vectors. */
struct stVector* NewVector( char* mName, byte mSize )
{
	struct stVector* vector = malloc( 1 * sizeof(struct stVector) );
	// FILL IN INFO:
	vector->Name      = mName;
	vector->Instances = malloc(1 * mSize);
	vector->Dimension = mSize;
	vector->Prev 	  = vectors_tail;	// attach to end of list
	vector->Next 	  = NULL;

	if (vectors_head == NULL) {
		vectors_head = vector;
	}
	if (vectors_tail != NULL)
		vectors_tail->Next = vector;	// this one is now end
	vectors_tail = vector;				// this one is now end
	return vector;
}

void AssignBoards( struct stVector* mVector, byte* mInstances )
{
	for (int i=0; i < mVector->Dimension; i++)
		mVector->Instances[i] = mInstances[i];
}

/* The vector will be mapped according to the MappedID in the board list 
	ie. find MappedID=0, retrieve instance for packing.
	The board list must have a sequential set of MappedID numbers.
	MappedID is the Vector index and the specific instance will be retrieved from the list.		
*/
void send_angle_vector( struct stVector* mVector, float* mAngles, float* mSpeeds )
{
	for (int VectorIndex=0; VectorIndex< mVector->Dimension; VectorIndex++)
	{
		pack_move_to_angle( &msg1, mVector->Instances[VectorIndex], mAngles[VectorIndex], mSpeeds[VectorIndex] );
		AddToSendList( &msg1 );
	}
}


