#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <vector>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <time.h> 
#include <string>

#include "protocol.h"
#include "devices.h"

#include "GENERAL_protocol.hpp"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"
#include "nlp_sentence.hpp"
#include "client_to_socket.hpp"
#include "alias.hpp"

#include "simulator_memory.h"

//#include "walking_robot.h"
//extern glWalkingRobot      robot;


/* Suggested statements:
		Raise your left/right leg [all the way up]
		Straighten left/right knee.
		Bend you knee [little, all the way]
		Keep foot level with floor.
		Toes up.
		Extend foot
		Turn/Swivel leg right/left.
		Take two steps forward/backward.
		Go back to way it was.
		Walk.
		Walk, then turn left 45 degrees, take 3 more steps.
				
*/

#define Debug 0


/* Do this b/c likely this will be part of a database.  
 Return :   1 => success
 */
int get_room_coord( Sentence& mSentence, MathVector& center )
{
    center.dimension(3);
    center[1] = 5;

    int found = mSentence.is_found_in_sentence("first bedroom");
    if (found)    {   center[0]=225;  center[2]=75;  return 1; }
    
    found = mSentence.is_found_in_sentence("master bedroom");
    if (found)    {   center[0]=350;  center[2]=75;  return 1; }
    
    found = mSentence.is_found_in_sentence("bathroom");
    if (found)    {   center[0]=330;  center[2]=250;  return 1; }
    
    found = mSentence.is_found_in_sentence("dining room");
    if (found)    {   center[0]=125;  center[2]=250;  return 1; }
    
    found = mSentence.is_found_in_sentence("kitchen");
    if (found)    {   center[0]=210;  center[2]=250;  return 1; }
    
    found = mSentence.is_found_in_sentence("living room");
    if (found)    {   center[0]=75;  center[2]=75;  return 1; }
    
    found = mSentence.is_found_in_sentence("entry way") || mSentence.is_found_in_sentence("entrance") || mSentence.is_found_in_sentence("entryway");
    if (found)    {   center[0]=40;  center[2]=200;  return 1; }

    found = mSentence.is_found_in_sentence("storage closet");       // bedroom walk-in
    if (found)    {   center[0]=40;  center[2]=250;  return 1; }
    
    found = mSentence.is_found_in_sentence("closet");       // bedroom walk-in
    if (found)    {   center[0]=375;  center[2]=200;  return 1; }

    int numNums = mSentence.number_of_numbers_present();
    if (numNums==3)
    {
        int Windex=0;
        for ( ; Windex < mSentence.m_num_words; Windex++ )
            if (mSentence.is_nth_word_a_number( Windex ))
                break;
        
        center[0] = mSentence.get_nth_word_as_a_number( Windex );
        center[1] = mSentence.get_nth_word_as_a_number( Windex+1 );
        center[2] = mSentence.get_nth_word_as_a_number( Windex+2 );
        return 1;
    }
    return 0;
}



/***********************************************************************

 ***********************************************************************/

void Init_Robot_Legs_Protocol()
{    
   // init_word_lists();
} 


/**** ACTION INITIATORS:    (4 possible actions) *****/


/*****************************************************************
Do the work of the Telegram :
return  -1	=> Not handled
		else number of extra bytes extracted from the mSentence buffer.
			- besides strlen(mSentence)! 
*****************************************************************/
int Parse_Robot_Legs_Statement( Sentence& mSentence )
{
	int retval=-1;
    enum ServoIndices servo_index;
    int Robot_id=0;
    
	if (Debug)  printf("Parse_Robot_Legs_Statement\n");

    if (mSentence.is_found_in_sentence( "raise"))
    {
        if (mSentence.is_found_in_sentence("legs"))
        {
            servo_index = LEFT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 60.0 );       // degrees
            // need a wait for ack in here!
            
            servo_index = RIGHT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 60.0 );       // degrees
            form_response("raising legs");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence("leg"))
		{
            if (mSentence.is_found_in_sentence("left"))
                servo_index = LEFT_HIP_FB_SWING;
            else
                servo_index = RIGHT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 60.0 );       // degrees

            form_response("raising leg");
            retval = 0;
		}
    }
    
    if (mSentence.is_found_in_sentence( "lower"))
    {
        if (mSentence.is_found_in_sentence("legs"))
        {
            servo_index = LEFT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 0.0 );       // degrees
            servo_index = RIGHT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 0.0 );       // degrees
            form_response("raising legs");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence("leg"))
        {
            if (mSentence.is_found_in_sentence("left"))
                servo_index = LEFT_HIP_FB_SWING;
            else
                servo_index = RIGHT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 0.0 );       // degrees

            form_response("lowering leg");
            retval = 0;
        }
    }
    if (mSentence.is_found_in_sentence("knee"))
    {
        if (mSentence.is_found_in_sentence( "bend" ))
        {
            if (mSentence.is_found_in_sentence("left"))
                servo_index = LEFT_KNEE;
            else
                servo_index = RIGHT_KNEE;
            // how far?  all the way, 90, little=45, etc.
            sim_robot_angle( Robot_id,  servo_index, 60.0 );       // degrees

            form_response("bending knee");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "straighten"))
        {
            if (mSentence.is_found_in_sentence("left"))
                servo_index = LEFT_KNEE;
            else
                servo_index = RIGHT_KNEE;
            // how far?  all the way, 90, little=45, etc.
            sim_robot_angle( Robot_id,  servo_index, 0.0 );       // degrees

            form_response("straightening knee");
            retval = 0;
        }
    }
    if (mSentence.is_found_in_sentence("foot"))
    {
        if (mSentence.is_found_in_sentence( "extend" ))
        {
            if (mSentence.is_found_in_sentence("left"))
                servo_index = LEFT_ANKLE_SWING;
            else
                servo_index = RIGHT_ANKLE_SWING;
            // how far?  all the way, 90, little=45, etc.
            sim_robot_angle( Robot_id,  servo_index, 45.0 );       // degrees

            form_response("extending");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "pull"))
        {
            if (mSentence.is_found_in_sentence("left"))
                servo_index = LEFT_ANKLE_SWING;
            else
                servo_index = RIGHT_ANKLE_SWING;
            // how far?  all the way, 90, little=45, etc.
            sim_robot_angle( Robot_id,  servo_index, -30.0 );       // degrees
            
            form_response("flexing");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "keep level"))
        {
            form_response("holding level");
            retval = 0;
        }

    }
    if (mSentence.is_found_in_sentence("assume") &&
        (mSentence.is_found_in_sentence("pose") || mSentence.is_found_in_sentence("position")) )
    {
        int word_index = mSentence.get_word_index("pose");
        int mMoveSequenceIndex= mSentence.get_nth_word_as_a_number(word_index+1);
        int repetitions=1;
        sim_robot_predefined ( Robot_id,   mMoveSequenceIndex,  repetitions);
        form_response("standard pose");
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("go to"))
    {
        MathVector Source;      // not used.  Robot moves from where it currently is.
        MathVector Destination(3);
        int dest_found = get_room_coord( mSentence, Destination );
        if (dest_found) {
            sim_move_robot( Robot_id,  Destination,  Source, false );
            form_response("walking to a new position");
            retval = 0;
        }
    }
    if (mSentence.is_found_in_sentence("walk"))
    {
        form_response("walking");
        retval = 0;
    }

    if (mSentence.is_found_in_sentence("step"))
    {
        form_response("stepping");
        retval = 0;
    }

    if (mSentence.is_found_in_sentence("sit"))
    {
        enum PredefinedPoses mMoveSequenceIndex = SIT;
        sim_robot_predefined ( Robot_id,   mMoveSequenceIndex,  1);
        
        form_response("dont mind if i do.");
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("stand"))
    {
        enum PredefinedPoses mMoveSequenceIndex = STAND;
        sim_robot_predefined ( Robot_id,   mMoveSequenceIndex,  1);
        
        form_response("standing");
        retval = 0;
    }
    
    if (mSentence.is_found_in_sentence("stop"))
    {
        form_response("Stopping by your command!");
        retval = 0;
    }

    if (mSentence.is_found_in_sentence("run sequence"))
    {
        form_response("sequence executing");
        retval = 0;
    }

    // OVERRIDE IF SIM not running : 
    bool simulator_available = is_sim_ipc_memory_available();
    if ((simulator_available==false) && (retval==0))
    {
        form_response("Sorry the simulator is not currently running.");
    }
	
	if (retval>-1)  printf( "Parse_Robot_Legs_Statement done\n" );
	return retval;
}

