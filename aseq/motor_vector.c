/*  ***MotorPack is an equivalent cpp class***

	We may choose to setup several vectors for the boards.
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
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
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
#include "can_txbuff.h"
#include "config_file.h"

#define float_size 4
struct stVector Appendages[2];

byte LeftLegInstances [3] = { 33, 31, 32 };
byte RightLegInstances[3] = { 20, 21, 22 };

BOOL (*rx_vector_refreshed_callback)( byte VectorIndex) = NULL;	// Call back function for system msg processing 
/*********************************************************************
 Set callback handler when all elements of a vector have been received 
 with position data.
 RETURN:  none
 *********************************************************************/
void set_rx_vector_refreshed_callback( BOOL (*mCallback)(byte VectorIndex) )
{
	rx_vector_refreshed_callback = mCallback;
}

/************************************************************
INCOMING CAN BIGMOTOR MESSAGE CALLBACK(): 
	Call back from within the CAN ISR().  
************************************************************/
BOOL can_motor_position_responder( struct sCAN* mMsg )
{
	struct stMotorStateInfo tmp;
	int Vindex = 0;
	int element = 0;
	/* Is the CAN msg a MOTOR ANGLE? */
	if ( id_match( mMsg->id, create_CAN_eid			(ID_MOTOR_ANGLE, 0)) )
	{
		// FIND CORRESPONDING VECTOR & INDEX
		int result = scan_for_instance( Vindex, mMsg->id.group.instance );
		if (result == -1)
		{
			Vindex = 1;
			result = scan_for_instance( Vindex, mMsg->id.group.instance );
		}
		if (result == -1)
			return FALSE;	// Disregards b/c that board is not a leg vector.

		// PARSE MSG & put data into the Vector:
		printf("can_motor_position_responder: Vect=%d; result=%d, index=%d\n", Vindex, result, mMsg->id.group.instance);		
		can_parse_motor_angle( mMsg, &tmp.Angle, &tmp.CurrentTimesTen, &tmp.SpeedTimesTen );
		Appendages[Vindex].ElementsFilled |= (1<<result);		// Mark it.
		set_CurrAngle( Vindex, result, tmp.Angle );
		set_Current( Vindex, result, tmp.CurrentTimesTen );

		if (vector_fully_read(Vindex))
			rx_vector_refreshed_callback( Vindex );
		// Disregard Current & Speed for now		
		return TRUE;
	}
	else if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_VALUE, 0)) )
	{
		// GET CORRESPONDING VECTOR & INDEX
		int element = scan_for_instance( Vindex, mMsg->id.group.instance );
		if (element == -1)
		{
			Vindex = 1;
			element = scan_for_instance( Vindex, mMsg->id.group.instance );
		}
		if (element == -1)
			return FALSE;	// Disregards b/c that board is not a leg vector.

		// HERE We're given the POT values instead of the angles.
		// so we'll just keep them.
		word pot_value = 0;
		//printf("can_motor_position_responder: Vect=%d; index=%d, instance=%d\n", Vindex, element, mMsg->id.group.instance);		
		can_parse_motor_value( mMsg, &(tmp.PotValue), &(tmp.CurrentTimesTen), &(tmp.SpeedTimesTen) );
		tmp.Angle = tmp.PotValue;
		/*printf("%d %d : d[0]=%x d[1]=%x | tmp.Angle=%6.1f; Current=%d, Speed=%d\n", 
				Vindex, element, mMsg->data[0], mMsg->data[1],
				tmp.Angle, tmp.CurrentTimesTen, tmp.SpeedTimesTen ); */
		Appendages[Vindex].ElementsFilled |= (1<<element);		// Mark it
		set_CurrAngle( Vindex, element, tmp.Angle );
		//set_Current( Vindex, element, tmp.CurrentTimesTen );
		if (vector_fully_read(Vindex)) {
			rx_vector_refreshed_callback( Vindex );
			Appendages[Vindex].ElementsFilled = 0;
		}

		return TRUE;
	}
	else if ( id_match( mMsg->id, create_CAN_eid	(ID_MOTOR_SPEED, 0)) )
	{
		can_parse_motor_speed( mMsg, &tmp.Angle, &tmp.CurrentTimesTen, &tmp.SpeedTimesTen );
		return TRUE;
	}	
	return FALSE;
}
/*********************************************************************
 When all of the motors have been updated with a new Position...
 RETURN:  none
 *********************************************************************/
BOOL can_vector_read_complete_responder( byte VectorIndex )
{
	//printf("BOOL can_vector_read_complete_responder()\n");
	if (1==1)
			printf("Vector[%d] :  %6.1f, %6.1f, %6.1f \n", VectorIndex, 
			Appendages[VectorIndex].CurrAngle[0],
			Appendages[VectorIndex].CurrAngle[1], 
			Appendages[VectorIndex].CurrAngle[2] );
	return TRUE;
}
/*********************************************************************************
Compare the Elements filled to a complete.  When complete, a callback will be issued
	(ie to print the vector's lastest Angles)
RETURN:  TRUE 	- all elements of the vector have a fresh reading (from CAN msgs)
		 FALSE 	- all elements of the vector have a fresh reading (from CAN msgs)
 *********************************************************************************/
byte vector_fully_read( byte mVindex )
{
	byte bitfield = 0;
	for (int i=0; i<Appendages[mVindex].Dimension; i++)
		bitfield |= (1<<i);

	if (Appendages[mVindex].ElementsFilled == bitfield)
		return 	TRUE;
	return FALSE;
}

/*********************************************************************
 Initialize a vector for left & right legs.
 RETURN:  none
 *********************************************************************/
void init_appendages()
{
	for (int i=0; i<ConfigInfo.Number_of_appendages; i++)
	{
		Init_Appendage( i, "Left Leg",  ConfigInfo.LimbInfo[i]->Dimension );		
	}	
	// Config File does this for us!


//	CopyInstances( LEFT_LEG_INDEX , ConfigInfo.LimbInfo[i]->instances  );
//	CopyInstances( RIGHT_LEG_INDEX, RightLegInstances );	

//	printf("LeftLegInstances : %d %d %d \n", Appendages[0].Instances[0], Appendages[0].Instances[1], Appendages[0].Instances[1] );
//	printf("RightLegInstances: %d %d %d \n", Appendages[1].Instances[0], Appendages[1].Instances[1], Appendages[1].Instances[2] );	
}

/*********************************************************************
Init a vector of the array
INPUT :	 mVectIndex	- Element of the Appendages array.
	  :  mName      - Name for the vector.
	  :  mDimension - Number of elements in the vector.
RETURN:  none
 *********************************************************************/
void Init_Appendage( byte mVectIndex, char* mName, byte mDimension )
{
	// FILL IN INFO:
	Appendages[mVectIndex].Name      	= mName;
	Appendages[mVectIndex].Dimension 	= mDimension;	
	Appendages[mVectIndex].Instances	= malloc(mDimension * 1);
	Appendages[mVectIndex].CurrAngle	= malloc(mDimension * sizeof(float) );
	Appendages[mVectIndex].NextAngle	= malloc(mDimension * sizeof(float) );
	Appendages[mVectIndex].SpeedTimesTen = malloc(mDimension * sizeof(word));
}

/*********************************************************************
 Scan one of the Left & Right Leg Appendages to find the 
	one containing the instance number.  
INPUT :	 mVectIndex	- Element of the Appendages array.
	  :  mInstance  - Instance number to find.
RETURN:  Index within the vector which matches.  -1 for no match.
 *********************************************************************/
int scan_for_instance( byte mVectorIndex, byte mInstance )
{
	for (byte index=0; index < Appendages[mVectorIndex].Dimension; index++)
	{
		if ( Appendages[mVectorIndex].Instances[index] == mInstance )
			return index;
	}
	return -1;
}

/*********************************************************************
 Scan one of the Left & Right Leg Appendages to find the 
	one containing the instance number.  
INPUT :	 mVectIndex	- Element of the Appendages array.
		 mInstances - Array of indexes.
RETURN:  none
 *********************************************************************/
void CopyInstances( byte mVectorIndex, byte* mInstances )
{
	for (int i=0; i < Appendages[mVectorIndex].Dimension; i++)
		Appendages[mVectorIndex].Instances[i] = mInstances[i];
}

/*********************************************************************
 Scan one of the Left & Right Leg Appendages to find the 
	one containing the instance number.  
INPUT :	 mVectIndex	- Element of the Appendages array.
	  :  word* 		- mSpeeds
RETURN:  none
 *********************************************************************/
void set_Speeds( byte mVectorIndex, word* mSpeeds )
{
	for (int index=0; index < Appendages[mVectorIndex].Dimension; index++ )
	{
		Appendages[mVectorIndex].SpeedTimesTen[index] = mSpeeds[index];
	}
}

/*********************************************************************
 Set the angles for all elements of the vector
	one containing the instance number.  
INPUT :	 mVectIndex	- Element of the Appendages array.
RETURN:  none
 *********************************************************************/
void set_NextAngles( byte mVectorIndex, float* mAngles )
{
	for (int index=0; index < Appendages[mVectorIndex].Dimension; index++ )
		Appendages[mVectorIndex].NextAngle[index] = mAngles[index];
}

/*********************************************************************
 Scan one of the Left & Right Leg Appendages to find the 
	one containing the instance number.  
INPUT :	 mVectIndex	- Element of the Appendages array.
RETURN:  none 
 *********************************************************************/
void set_CurrAngle( byte mVectorIndex, byte index, float mAngle )
{
	*(Appendages[mVectorIndex].CurrAngle+index) = mAngle;
}
/*********************************************************************
 Scan one of the Left & Right Leg Appendages to find the 
	one containing the instance number.  
INPUT :	 mVectIndex	- Element of the Appendages array.
RETURN:  none 
 *********************************************************************/
void set_Current( byte mVectorIndex, byte index, float mAmpsTimesTen )
{
	Appendages[mVectorIndex].CurrAngle[index] = mAmpsTimesTen;	
}

/*********************************************************************
Send Motor_Speed commands to each instance in the vector.
INPUT :	 mVectIndex	- Element of the Appendages array.
RETURN:  none 
 *********************************************************************/
void send_speed_vector( byte mVectorIndex )
{
	for (int index=0; index < Appendages[mVectorIndex].Dimension; index++ )
	{
		pack_move_speed( &msg1, Appendages[mVectorIndex].Instances[index], 
					  	 Appendages[mVectorIndex].SpeedTimesTen[index] );
		print_message( &msg1 );
		AddToSendList( &msg1 );
	}
}

/*************************************************************************
  The vector will be mapped according to the MappedID in the board list 
  ie. find MappedID=0, retrieve instance for packing.
  The board list must have a sequential set of MappedID numbers.
  MappedID is the Vector index and the specific instance will be retrieved 
  from the list.
 INPUT :	mVectIndex	- Element of the Appendages array
 RETURN:  	none 
 *************************************************************************/
void send_angle_vector( byte mVectorIndex )
{
	printf("Instances=%x; NextAngle=%x SpeedTimesTen=%x\n", 
											Appendages[mVectorIndex].Instances, 
											Appendages[mVectorIndex].NextAngle,
											Appendages[mVectorIndex].SpeedTimesTen
											 );

	for (int index=0; index<Appendages[mVectorIndex].Dimension; index++ )
	{
		pack_move_to_angle( &msg1, Appendages[mVectorIndex].Instances[index], 
							Appendages[mVectorIndex].NextAngle[index], 
							Appendages[mVectorIndex].SpeedTimesTen[index]*10 );
		print_message( &msg1 );
		AddToSendList( &msg1 );
	}
}

/* 
Configure all BigMotor boards associated with this motor vector.
INPUT:
	mRate		: 
	mReports	: Which reports to send (VALUE, ANGLE, etc)
return 			: void
*/
void configure_motor_reports( byte mVectorIndex, byte mRate, byte mReports )
{
	for (int index=0; index<Appendages[mVectorIndex].Dimension; index++ )
	{
		configure_motor_set_update_rate( &msg1, Appendages[mVectorIndex].Instances[index], mRate, mReports);
		print_message( &msg1 );
		AddToSendList( &msg1 );
	}
}


//struct stVector* vectors_head = NULL;
//struct stVector* vectors_tail = NULL;

/** Allocates room for a new vector and adds it to the list of vectors. *
struct stVector* NewVector( char* mName, byte mDimension )
{
	//struct stVector* vector = malloc( 1 * sizeof(struct stVector) );
	// FILL IN INFO:
	vector->Name      = mName;
	vector->Dimension = mDimension;	
	vector->Instances	= malloc(mDimension * 1);
	vector->CurrPos		= malloc(mDimension * sizeof(float) );
	vector->NextPos		= malloc(mDimension * sizeof(float) );

//	vector->Prev 	  = vectors_tail;	// attach to end of list
//	vector->Next 	  = NULL;
/*	if (vectors_head == NULL) {
		vectors_head = vector;
	}
	if (vectors_tail != NULL)
		vectors_tail->Next = vector;	// this one is now end
	vectors_tail = vector;				// this one is now end
//	return vector;
} 
struct stVector* AppendVector( char* mName, byte mDimension )
{
	// Add later;
}
*/
