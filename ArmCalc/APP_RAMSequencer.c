/*********************************************************************
 *											   						 *
 *    DESCRIPTION:  Arm sequencing - Fixed positions			 	 *
 *											  						 *
 *	  CATEGORY	:  Low level APP									 *
 *											  						 *
 *    AUTHOR	:  Stephen Tenniswood, Embedded Engineer 	 		 *
 *											   						 *
 *    PURPOSE	:  Setup an ADC channel for the Cds light sensor	 *
 *			   	   and Thermistor.									 *
 *																	 *
 *	  DATE		:  Sept 25,2008										 *
 *********************************************************************/
#include <stdlib.h>
#include "skp_bsp.h"
#include "APP_ArmPositionStorage.h"
#include "APP_Sequencer.h"
#include "MID_FilterServo.h"
#include "DEV_flash-26-ew1.h"
#include "MID_FilterServo.h"

/************** VARIABLE DEFINITIONS ************************/
U8 	 SEQ_Request=0;						// 
U8 	 SEQ_NumStoredSequences;			// Keeps the total number of taught coordinates.  
										// This is stored first 2 bytes of EEPROM.

struct stServoSet[MAX_POSITIONS];		// 

/*****************************************************************************
Name		:   SEQ_GetEEPROMLocation()
Parameters	:	
Returns 	:   TRUE when finished
Description	:   Get the addresss for the saved arm position to EEPROM.
*****************************************************************************/
stSequencePtr SEQ_GetEEPROMLocation(int mIndex)
{
	return (stSequencePtr)(SEQUENCE_START_ADDRESS + mIndex*SEQUENCE_SIZE );	
}


/**********************************************************************************
Name		:   SEQ_Store()
Parameters	:	mCoordinateIndex			[0..200]
				mServoNumber				must be: [0,2,4,6]
				mCompressedServoValue1		The compressed byte for Servo n
				mCompressedServoValue2		The compressed byte for Servo n+1
Returns 	:   TRUE when finished
Description	:   This will store the compressed positions for 2 servo motors.
				Since 6 servos are supported, will need to call this multiple times
				for the complete arm position vector.
************************************************************************************/
U16 SEQ_Store(stSequencePtr mSequence, U8 mSequenceIndex )
{
	U32 Location;
	U16 retval2 = 0;
	int i=0;
	U8  *Temp ;
	
    /* Enable flash data blocks - disabled by default after reset */
    prc1 = 1;  					// Unlock PM1
    pm10 = 1;					// Enable flash data block access
    prc1 = 0;					// Lock PM1

	Location  = (U32)SEQ_GetEEPROMLocation(mSequenceIndex);
	Temp = (U8*)mSequence;
		
	/* Add this delay to ensure the ICD does not disrupt flash operations */
	for(i=0;i<256;i++)
		_asm("NOP");

	retval2 = FlashWrite( 	Location, 	// Flash address write to (MUST be an even address)
						 	Temp, 
							SEQUENCE_SIZE );	
										// Address of buffer containing data to write
					 					// Number of bytes in buffer (MUST be even number)
										// Interrupts are disabled during in this function
		
	SEQ_NumStoredSequences++;
	return retval2;
}

/**********************************************************************************
Name		:   SEQ_Retrieve()
Parameters	:	mSequenceIndex
Returns 	:   TRUE when finished
Description	:   This returns a pointer to the structure.
************************************************************************************/
U8* SEQ_Retrieve( U8 mSequenceIndex )
{	
    /* Enable flash data blocks - disabled by default after reset */
    prc1 = 1;  					// Unlock PM1
    pm10 = 1;					// Enable flash data block access
    prc1 = 0;					// Lock PM1

	return (U8*)SEQ_GetEEPROMLocation(mSequenceIndex);
}

/**********************************************************************************
Name		:   SEQ_MovetoStoredPosition()
Parameters	:	mSequenceIndex
Returns 	:   TRUE when finished
Description	:   This retrieves an arm position and therefore moves the servos
				to the desired arm position.
************************************************************************************/
void SEQ_MovetoStoredPosition(stSequencePtr mPtr, U8 mStopNumber)
{
	// Calculate Timings and Set in motion MID_FilterServo.c
	//mTimeToReach = Calc_TimeToReach_DefaultRate( MAX_SERVO_1_PW, MIN_SERVO_1_PW );

	// Retrieve Position with timing
	RetrieveArmPosition( mPtr->IndexArray[mStopNumber], mPtr->Time[mStopNumber] );
}

/*****************************************************************************
Name		:   EraseAllArmPositions()
Parameters	:	returns TRUE when finished.  		
Description	:   
*****************************************************************************/
int SEQ_EraseAll()
{
    /* Enable flash data blocks - disabled by default after reset */
    prc1 = 1;  	// Unlock PM1
    pm10 = 1;	// Enable flash data block access
    prc1 = 0;	// Lock PM1

	SEQ_NumStoredSequences = 0;
	
	return FlashErase( 4 );	// Erase Block 5 (0xF800 - 0xFFFE)s
							// Note that interrupts are disabled during this function
}

/**********************************************************************************
Name		:   Perform_Sequence()
Parameters	:	mSequenceIndex
Returns 	:   TRUE when finished
Description	:   
************************************************************************************/
void Perform_Sequence( int mSequenceNumber )
{	
}

/**********************************************************************************
Name		:   SEQ_Initiate_Position()
Parameters	:	mSequenceIndex
Returns 	:   TRUE when finished
Description	:   Takes a servo set and puts it into the Servo Filtered move.
************************************************************************************/
void SEQ_Initiate_Position( int mSequenceNumber )
{	
	
}

/**********************************************************************************
Name		:   SEQ_TimeSlice()
Parameters	:	mSequenceIndex
Returns 	:   TRUE when finished
Description	:   
************************************************************************************/
void SEQ_TimeSlice()
{
	static U8 state = 0;
	static stSequencePtr Ptr;
	static U16 PauseCount=0;
	static U8 StopNumber=0;
	
	switch (state)
	{
		case 0:	 Ptr = NULL; 
				 // Here we may be waiting for a sensor trigger done in another task.
				 // Other task only has to queue up the desired sequence.  Then this
				 // statemachine will take care of the rest.

				 if (SEQ_Request)			// IDLE - Waiting for command
				 {
					 if (SEQ_Request <= SEQ_NumStoredSequences)		// Valid request range
					 {
						 Ptr = SEQ_GetEEPROMLocation(SEQ_Request);		// Retrieve new Sequence
						 StopNumber=0;
				 		 state=1;
					 }
				 }			 
				 break;
				 
		case 1:	 // Retrieve 1st coordinate and queue movements
				 SEQ_MovetoStoredPosition(Ptr, StopNumber);
				 state=2;
				 break;

		case 2:	 // Wait for position to be reached.
				 if (FS_Position_Reached)
				 {
				 	state=3;
					PauseCount = Ptr->PauseCount[StopNumber];
				 }
				 break;	
		
		case 3:	 // PAUSE
				 if (PauseCount-- == 0)
				 	state = 4;
				 break;

		case 4:	 // Reset and go to IDLE.
				 StopNumber++;
				 if (StopNumber == Ptr->NumOfPositions)
				 	state = 0;
				 else
				 	state = 1;		// Continue in the Sequence.
				 break;
				 
		default:
				 break;	
	};
}


