#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <list>
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
string room_name;

int get_room_name( Sentence& mSentence )
{
    int found = mSentence.are_found_in_sentence("first bedroom");
    if (found)    {   room_name="first bedroom"; return 1;   }
    
    found = mSentence.are_found_in_sentence("master bedroom");
    if (found)    {   room_name="master bedroom";  return 1; }
    
    found = mSentence.is_found_in_sentence("bathroom");
    if (found)    {   room_name="bathroom"; return 1;        }
    
    found = mSentence.are_found_in_sentence("dining room");
    if (found)    {   room_name="dining room";  return 1;    }
    
    found = mSentence.is_found_in_sentence("kitchen");
    if (found)    {   room_name="kitchen";  return 1;        }
    
    found = mSentence.are_found_in_sentence("living room");
    if (found)    {   room_name="living room";return 1;      }
    
    found = mSentence.are_found_in_sentence("entry way") || mSentence.is_found_in_sentence("entrance") || mSentence.is_found_in_sentence("entryway");
    if (found)    {   room_name="entry way"; return 1;       }
    
    found = mSentence.are_found_in_sentence("storage closet");       // bedroom walk-in
    if (found)    {   room_name="storage closet"; return 1;  }
    
    found = mSentence.is_found_in_sentence("closet");       // bedroom walk-in
    if (found)    {   room_name="closet"; return 1;          }
    
    return 0;
}

/* Do this b/c likely this will be part of a database.
 Return :   1 => success
 */
int get_room_coord( Sentence& mSentence, MathVector& center )
{
    center.dimension(3);
    center[1] = 5;

    int found = mSentence.are_found_in_sentence("first bedroom");
    if (found)    {   room_name="first bedroom"; center[0]=225;  center[2]=75;  return 1; }
    
    found = mSentence.are_found_in_sentence("master bedroom");
    if (found)    {   center[0]=350;  center[2]=75;  return 1; }
    
    found = mSentence.is_found_in_sentence("bathroom");
    if (found)    {   center[0]=330;  center[2]=250;  return 1; }
    
    found = mSentence.are_found_in_sentence("dining room");
    if (found)    {   center[0]=125;  center[2]=250;  return 1; }
    
    found = mSentence.is_found_in_sentence("kitchen");
    if (found)    {   center[0]=210;  center[2]=250;  return 1; }
    
    found = mSentence.are_found_in_sentence("living room");
    if (found)    {   center[0]=75;  center[2]=75;  return 1; }
    
    found = mSentence.are_found_in_sentence("entry way") || mSentence.is_found_in_sentence("entrance") || mSentence.is_found_in_sentence("entryway");
    if (found)    {   center[0]=40;  center[2]=200;  return 1; }

    found = mSentence.are_found_in_sentence("storage closet");       // bedroom walk-in
    if (found)    {   center[0]=40;  center[2]=250;  return 1; }
    
    found = mSentence.is_found_in_sentence("closet");       // bedroom walk-in
    if (found)    {   center[0]=375;  center[2]=200;  return 1; }

    int numNums = mSentence.number_of_numbers_present();
    if (numNums==3)
    {
        int Windex=0;
        for ( ; Windex < mSentence.get_number_words(); Windex++ )
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
int Parse_Robot_Legs_Statement( Sentence& mSentence, ServerHandler* mh )
{
	int retval=-1;
    enum ServoIndices servo_index;
    int Robot_id=0;
    
	if (Debug)  printf("Parse_Robot_Legs_Statement\n");
    mSentence.restore_reduced();    
    if (mSentence.is_found_in_sentence( "raise"))
    {
        if (mSentence.is_found_in_sentence("legs"))
        {
            servo_index = LEFT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 60.0 );       // degrees
            // need a wait for ack in here!
            
            servo_index = RIGHT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 60.0 );       // degrees
            mh->form_response("raising legs");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence("leg"))
		{
            if (mSentence.is_found_in_sentence("left"))
                servo_index = LEFT_HIP_FB_SWING;
            else
                servo_index = RIGHT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 60.0 );       // degrees

            mh->form_response("raising leg");
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
            mh->form_response("raising legs");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence("leg"))
        {
            if (mSentence.is_found_in_sentence("left"))
                servo_index = LEFT_HIP_FB_SWING;
            else
                servo_index = RIGHT_HIP_FB_SWING;
            sim_robot_angle( Robot_id,  servo_index, 0.0 );       // degrees

            mh->form_response("lowering leg");
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

            mh->form_response("bending knee");
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

            mh->form_response("straightening knee");
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

            mh->form_response("extending");
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
            
            mh->form_response("flexing");
            retval = 0;
        }
        if (mSentence.are_found_in_sentence( "keep level"))
        {
            mh->form_response("holding level");
            retval = 0;
        }

    }
    if (mSentence.is_found_in_sentence("squat"))
    {
        float fraction = 0.5;
        if (mSentence.are_found_in_sentence("as you can") ||
            mSentence.are_found_in_sentence("all the way") ||
            mSentence.is_found_in_sentence("full"))   // "as much/far as you can"
            //robot.squat_fraction( 1.0 );
            fraction=1.0;
        else if (mSentence.is_found_in_sentence("inch") || mSentence.is_found_in_sentence("inches"))
            fraction =1.0;
        else if (mSentence.is_found_in_sentence("little")) {
            fraction =0.10;
            if (mSentence.is_found_in_sentence("more")) {
                //fraction = robot.get_squat_amount();
                fraction += 0.1;
            }
        }
        sim_robot_predefined(Robot_id, SQUAT, fraction*100);
        mh->form_response("squatting");
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("fold") && mSentence.is_found_in_sentence("arms"))
    {
        sim_robot_predefined(Robot_id, FOLDED_ARMS, 1);
        mh->form_response("folding arms");
        retval = 0;
    }
    if ((mSentence.is_found_in_sentence("zombie") && mSentence.is_found_in_sentence("arms")) ||
        (mSentence.is_found_in_sentence("arms") && mSentence.is_found_in_sentence("front")))
    {
        sim_robot_predefined(Robot_id, ZOMBIE_ARMS, 1);
        mh->form_response("i am a zombie.");
        retval = 0;
    }
    if ((mSentence.are_found_in_sentence("straight up")) && (mSentence.is_found_in_sentence("arms")))
    {
        sim_robot_predefined(Robot_id, ARM_STRAIGHT_UP, 1);
        mh->form_response("arms up");
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("arm") &&
         (mSentence.are_found_in_sentence("out left side") ||(mSentence.is_found_in_sentence("stretched") && mSentence.is_found_in_sentence("left"))))
    {
        sim_robot_predefined(Robot_id, ARM_STRETCHED_SIDE_LEFT, 1);
        mh->form_response("left arm stretched");
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("arm") &&
        (mSentence.are_found_in_sentence("out right side") ||(mSentence.is_found_in_sentence("stretched") && mSentence.is_found_in_sentence("right"))))
    {
        sim_robot_predefined(Robot_id, ARM_STRETCHED_SIDE_RIGHT, 1);
        mh->form_response("right arm stretched");
        retval = 0;
    }
                
    if (mSentence.is_found_in_sentence("assume") &&
        (mSentence.is_found_in_sentence("pose") || mSentence.is_found_in_sentence("position")) )
    {
        int word_index = mSentence.get_word_index("pose");
        int mMoveSequenceIndex= mSentence.get_nth_word_as_a_number(word_index+1);
        int repetitions=1;
        sim_robot_predefined ( Robot_id,   mMoveSequenceIndex,  repetitions);
        mh->form_response("standard pose");
        retval = 0;
    }
    if (mSentence.are_found_in_sentence("go to") || mSentence.are_found_in_sentence("walk to"))
    {
        MathVector Source;      // not used.  Robot moves from where it currently is.
        MathVector Destination(3);
        int dest_found = get_room_coord( mSentence, Destination );
        if (dest_found) {
            get_room_name ( mSentence );
            sim_move_robot( Robot_id,  Destination,  Source, false );
            string tmp = "walking to ";  tmp += room_name;
            mh->form_response(tmp.c_str() );
            retval = 0;
        }
    }
    if (mSentence.is_found_in_sentence("where") && mSentence.is_found_in_sentence("you"))
    {
        sim_where_is ( 48 );
        sim_wait_for_acknowledgement();
        mh->form_response(ipc_memory_sim->Response);
        
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("walk"))
    {
        mh->form_response("walking");
        retval = 0;
    }
    
    if (mSentence.is_found_in_sentence("step"))
    {
        mh->form_response("stepping");
        retval = 0;
    }

    if (mSentence.is_found_in_sentence("sit"))
    {
        enum PredefinedPoses mMoveSequenceIndex = SIT;
        sim_robot_predefined ( Robot_id,   mMoveSequenceIndex,  1 );
        
        mh->form_response("don't mind if i do.");
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("stand"))
    {
        enum PredefinedPoses mMoveSequenceIndex = STAND;
        sim_robot_predefined ( Robot_id,   mMoveSequenceIndex,  1);
        
        mh->form_response("standing");
        retval = 0;
    }
    
    if (mSentence.is_found_in_sentence("stop"))
    {
        mh->form_response("Stopping by your command!");
        sim_stop_robot(Robot_id);
        retval = 0;
    }

    if (mSentence.are_found_in_sentence("run sequence"))
    {
        mh->form_response("sequence executing");
        retval = 0;
    }

    if (retval>-1)  {
        printf( "Parse_Robot_Legs_Statement done\n" );
        // OVERRIDE IF SIM not running :
        bool simulator_available = is_sim_ipc_memory_available();
        if (simulator_available==false)
            mh->form_response("I am not a robot");
    }
	return retval;
}

