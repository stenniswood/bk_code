//
//  robot_arms_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/18/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
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

#include "nlp_extraction.hpp"
#include "client_memory.hpp"
#include "nlp_sentence.hpp"
#include "robot_arms_protocol.hpp"
#include "simulator_memory.h"
#include "alias.hpp"

/* Suggested statements:
 Raise/lower your left/right arm [all the way up]
 arms [down] by your side.
 Straighten left/right elbow.
 Bend you elbow [little, all the way]
 raise arms right/straight in front of you.
 fingers straight/fist/sligtly curved.
 Arms out sideways.
 Go back to way it was.
 
 */

#define Debug 0

extern bool nlp_reply_formulated;

void form_response(string* mh, const char* mString )
{
    *mh += mString;
    nlp_reply_formulated = true;
}
void append_response(string* mh, const char* mString )
{
    *mh += mString;
    nlp_reply_formulated = true;
}


/***********************************************************************
 
 ***********************************************************************/
void Init_Robot_Arms_Protocol()
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
int servo_index;
int Robot_id=0;
enum eArmIndex {
    LEFT,
    RIGHT,
    BOTH
} selectedArm;

enum eArmIndex which_arm( Sentence& mSentence )
{
    if (mSentence.is_found_in_sentence("right"))
        return RIGHT;
    else
        return LEFT;
}
enum eArmJoint {
    SHOULDER,
    ELBOW,
    WRIST,
    THUMB,
    FINGER_1,
    FINGER_2,
    FINGER_3,
    NONE
};
enum eArmJoint which_joint( Sentence& mSentence )
{
    if (mSentence.is_found_in_sentence("elbow",true))
        return ELBOW;
    else if (mSentence.is_found_in_sentence("wrist",true))
        return WRIST;
    else if (mSentence.is_found_in_sentence("finger",true)) {
        if (mSentence.is_found_in_sentence("index"))
            return FINGER_1;
        else if (mSentence.is_found_in_sentence("middle"))
            return FINGER_2;
        else if (mSentence.is_found_in_sentence("last") || mSentence.is_found_in_sentence("pinky"))
            return FINGER_3;
    } else if (mSentence.is_found_in_sentence("thumb"))
        return THUMB;
    return NONE;
}

float how_much( Sentence& mSentence, float mMaxAngle )
{
    float degrees_to_move=30;
    if (mSentence.is_found_in_sentence("little",true))
        return 5.0;
    if (mSentence.is_found_in_sentence("lot",true))
        return 10.0;
    if ((mSentence.is_found_in_sentence("all the way",true)) || (mSentence.is_found_in_sentence("as far as possible",true)))
        return mMaxAngle;
    //if ((mSentence.is_found_in_sentence("match other limb",true)) || (mSentence.is_found_in_sentence("as far as possible",true)))
    //    return 5.0;
    //if ((mSentence.is_found_in_sentence("until touch nose",true))
    //    return 5.0;
    return degrees_to_move;
}

int Parse_arm_command( Sentence& mSentence, string* mh, int mbase_servo_index )
{
    int retval = -1;
    if (mSentence.is_found_in_sentence("raise") || mSentence.is_found_in_sentence( "lift"))
    {
        float raise_angle = -60;
        if (mSentence.is_found_in_sentence( "all the way"))
            raise_angle = -90;
        if (mSentence.is_found_in_sentence( "over") && mSentence.is_found_in_sentence( "head"))
            raise_angle = -180;
        if (mSentence.is_found_in_sentence("sideways"))
        {
            sim_robot_angle( Robot_id,  mbase_servo_index+1, raise_angle );       // degrees
        } else {
            sim_robot_angle( Robot_id,  mbase_servo_index, raise_angle );       // degrees
        }
        form_response(mh, "raising arms");
        retval = 0;
    }
    
    if (mSentence.is_found_in_sentence( "lower"))
    {
        if (mSentence.is_found_in_sentence( "all the way") || mSentence.is_found_in_sentence( "side"))
        {
            sim_robot_angle( Robot_id,  mbase_servo_index+1, 0.0 );       // degrees
            sim_robot_angle( Robot_id,  mbase_servo_index+2, 0.0 );       // degrees
        }
        
        sim_robot_angle( Robot_id,  mbase_servo_index, 0.0 );       // degrees
        form_response(mh, "lowering arms");
        retval = 0;
    }
    
    if (mSentence.is_found_in_sentence("straight") || mSentence.is_found_in_sentence("straighten"))
    {
        sim_robot_angle( Robot_id,  mbase_servo_index+3, 0.0 );
        sim_robot_angle( Robot_id,  mbase_servo_index+4, 0.0 );
        sim_robot_angle( Robot_id,  mbase_servo_index+5, 0.0 );
        form_response(mh, "arms straightened");
    }
    if (mSentence.is_found_in_sentence( "down"))
    {
        float angle = 0;
        sim_robot_angle( Robot_id,  mbase_servo_index,   angle );       // degrees
        sim_robot_angle( Robot_id,  mbase_servo_index+1, angle );       // degrees
        sim_robot_angle( Robot_id,  mbase_servo_index+3, angle );       // degrees
        form_response(mh, "arms down");
        retval = 0;
    }
    retval = 0;
    return retval;
}

int Parse_elbow_command( Sentence& mSentence, string* mh, int mbase_servo_index )
{
    int retval = -1;
    if (mSentence.is_found_in_sentence( "bend" ))
    {
        sim_robot_angle( Robot_id,  mbase_servo_index, 90.0 );       // degrees
        
        form_response(mh, "bending");
        retval = 0;
    }
    if (mSentence.is_found_in_sentence( "straighten"))
    {
        sim_robot_angle( Robot_id,  mbase_servo_index, 0.0 );       // degrees
        form_response(mh, "straightening");
        retval = 0;
    }
    return retval;
}
int Parse_wrist_command( Sentence& mSentence, string* mh, int mbase_servo_index )
{
    int retval = -1;
    if (mSentence.is_found_in_sentence( "bend" ))
    {
        if (mSentence.is_found_in_sentence( "up" ))
            sim_robot_angle( Robot_id,  mbase_servo_index, -45.0 );       // degrees
        else if (mSentence.is_found_in_sentence( "down" ))
            sim_robot_angle( Robot_id,  mbase_servo_index, 60.0 );       // degrees
        form_response(mh, "bending ");
        retval = 0;
    }
    if (mSentence.is_found_in_sentence( "straight"))
    {
        sim_robot_angle( Robot_id,  mbase_servo_index, 0.0 );       // degrees
        form_response(mh, "straight ");
        retval = 0;
    }
    return retval;
}

int Parse_Robot_Arms_Statement( Sentence& mSentence, string* mh )
{
    int retval=-1;
    
    if (Debug)  printf("Parse_Robot_arms_Statement\n");
    mSentence.restore_reduced();
    
    selectedArm = which_arm(mSentence);
    if (selectedArm==LEFT)
        servo_index = LEFT_SHOULDER_ROTATE;     // Base Servo
    else
        servo_index = RIGHT_SHOULDER_ROTATE;    // Base Servo

    enum eArmJoint joint = which_joint(mSentence);
    if (joint!=NONE){
        switch(joint) {
            case SHOULDER : servo_index += 0;
                break;
            case ELBOW :    servo_index += 3;
                break;
            case THUMB :    servo_index += 0;
                break;
            case WRIST :    servo_index += 4;
                break;
            case FINGER_1 : servo_index += 0;       /* save for a separate hand/gripper file */
                break;
            case FINGER_2:  servo_index += 0;
                break;
            case FINGER_3:  servo_index += 0;
                break;
            default:
                break;
        }
    }
    if (mSentence.is_found_in_sentence("arm",false))
    {
        Parse_arm_command( mSentence, mh, servo_index );
    }
    
    if (mSentence.is_found_in_sentence("arms"))
    {
        Parse_arm_command( mSentence, mh, servo_index );
        servo_index  += (RIGHT_SHOULDER_ROTATE-LEFT_SHOULDER_ROTATE);
        retval = Parse_arm_command( mSentence, mh, servo_index );
    }

    // ELBOW :
    if (mSentence.is_found_in_sentence("elbow")) {
        retval = Parse_elbow_command( mSentence, mh, servo_index );
        append_response(mh, " elbow");
    }
    if (mSentence.is_found_in_sentence("elbows"))
    { 
        Parse_elbow_command( mSentence, mh, servo_index );
        servo_index  +=(RIGHT_SHOULDER_ROTATE-LEFT_SHOULDER_ROTATE);
        retval = Parse_elbow_command( mSentence, mh, servo_index );
        append_response(mh, " elbows");
    }

    // WRISTS :
    if (mSentence.is_found_in_sentence("wrist")) {
        retval = Parse_wrist_command( mSentence, mh, servo_index );
        append_response(mh, " wrist");
    }
    if (mSentence.is_found_in_sentence("wrists"))
    {
        Parse_wrist_command( mSentence, mh, servo_index );
        servo_index  +=(RIGHT_SHOULDER_ROTATE-LEFT_SHOULDER_ROTATE);
        retval = Parse_wrist_command( mSentence, mh, servo_index );
        append_response(mh, " wrists");
    }
    
    if (retval>-1)  {
        printf( "Parse_Robot_Arms_Statement done\n" );
        bool simulator_available = is_sim_ipc_memory_available();
        if (simulator_available==false)
            form_response(mh,  "I am not a robot");
    }
    
    return retval;
}

