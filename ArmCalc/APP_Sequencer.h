/********* CONSTANT DECLARATIONS *******************************/
#define SEQUENCE_START_ADDRESS 		0xF800		// EEPROM ADDRESS.
#define MAX_POSITIONS_PER_SEQUENCE 	25
#define SEQUENCE_SIZE   			(1+MAX_POSITIONS_PER_SEQUENCE+MAX_POSITIONS_PER_SEQUENCE*2)			/*sizeof(stSequence)*/



struct stSequence
{
	U8  NumOfPositions;									// in this sequence.
	U8  IndexArray[MAX_POSITIONS_PER_SEQUENCE];
	U16 Time[MAX_POSITIONS_PER_SEQUENCE];				// Depends on the previous position right.
	U8  PauseCount[MAX_POSITIONS_PER_SEQUENCE];		// Pauses begin once position is reached.
	
/* Definitions for "Time":  
 *	RELATIVE TIME TO COMPLETE - Time from moment of starting the move till all positions are 
 *			  reached.  With this time alotment the motors will change faster or slower
 *			  depending on how close the previous position is to this one.
 *			  This type of timing may not be desireable for the above reason (rate
 *			  of change may be too big in some cases.
 *
 *  CONTROLLED MOTOR SPEED TIME - 
 *			The specified time will be duration of a 60 degree movement.
 *			No motor may move faster than this.
 *	
 */
};
typedef struct stSequence*  stSequencePtr;


extern U8  SEQ_NumStoredSequences;
extern U8  SEQ_Request;



U16 SEQ_Store(stSequencePtr mSequence, U8 mSequenceIndex );
U8* SEQ_Retrieve( U8 mSequenceIndex );
int SEQ_EraseAll();



