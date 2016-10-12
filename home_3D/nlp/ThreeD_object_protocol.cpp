//
//  ThreeD_object_protocol.cpp
//                   This file will handle movement, joining, and separating commands.
//
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/20/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include "ThreeD_object_protocol.hpp"
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
#include <regex>

#include "client_memory.hpp"
#include "nlp_sentence.hpp"
#include "simulator_memory.h"
#include "parameter.hpp"
#include "object_primitive_creator.hpp"
#include "it.hpp"


extern class IPC_Exception IPC_Error;

#define Debug 0

// Prototype here.  Implementation is in 'robot_legs_protocol.cpp'
int  get_room_coord( Sentence& mSentence, MathVector& center );
bool nlp_reply_formulated = false;

/***********************************************************************
 
 ***********************************************************************/
void Init_ThreeD_Protocol()
{
    init_converter();
}

bool get_referred_object( Sentence& mSentence, VerbalObject*& mObjPtr )
{
    mObjPtr = referers.back();
    return true;
}

bool get_absolute_direction( Sentence& mSentence, MathVector& mDirection )
{
    
    //mDirection
    return true;
}

// Defined in main.cpp
extern GLdouble centerX,centerY,centerZ;
extern GLdouble eyeX,   eyeY,   eyeZ;

void move_foreward_backward( MathVector& mDelta, float mAmountForward )
{
    float F_vec_x = eyeX - centerX;
    float F_vec_y = eyeY - centerY;
    float F_vec_z = eyeZ - centerZ;
    float sum = fabs(F_vec_x) + fabs(F_vec_y) + fabs(F_vec_z);
    
    F_vec_x *= 2*mAmountForward/sum;
    F_vec_z *= 2*mAmountForward/sum;

    mDelta[0] 	= F_vec_x;
    mDelta[2]   = F_vec_z;
}

void move_left_right( MathVector& mDelta, float mAmountRight )
{
    MathVector forward(3);
    forward[0] = centerX - eyeX;
    forward[1] = centerY - eyeY;
    forward[2] = centerZ - eyeZ;
    
    MathVector perp = forward.get_perp_xz();
    perp.unitize();
    perp *= mAmountRight;
    
    mDelta[0] += perp[0];
    mDelta[2] += perp[2];
}

bool get_relative_direction( Sentence& mSentence, MathVector& mDirection )
{
    bool retval=false;
    /*  "lift 2 inches (higher)?"
        "raise by 2 inches"
        "move up 2 inches"
        "2 inches higher"
        "move 2 inches higher"
     */
    BidirectionalParameter height,sideways,frontback;
    height.m_type    = FLOAT;
    height.m_requested_unit = "inch";
    //height.m_name    = "(height|high|higher)";
    height.m_positive_regexpression = "(lift|raise|up|height|high|higher)( by )?";
    height.m_negative_regexpression = "(down|lower)";
    height.set_std_or_metric_length_units();
    height.construct_regex();
    height.parse          ( mSentence.m_sentence );

    sideways.m_type    = FLOAT;
    sideways.m_requested_unit = "inch";
    //sideways.m_name    = "(to the )?(left|right)";
    sideways.m_positive_regexpression = "(to the )?(left)";
    sideways.m_negative_regexpression = "(to the )?(right)";
    sideways.set_std_or_metric_length_units();
    sideways.construct_regex();
    sideways.parse ( mSentence.m_sentence );
    
    frontback.m_type    = FLOAT;
    frontback.m_requested_unit = "inch";
    frontback.m_name    = "(toward )?(front|back)";
    frontback.m_positive_regexpression = "(toward the )?(front|camera)";
    frontback.m_negative_regexpression = "((to the )?back|away(from )?)";
    frontback.set_std_or_metric_length_units();
    frontback.construct_regex();
    frontback.parse( mSentence.m_sentence );

    if (height.is_match() )
    {
        if (height.is_positive_direction())
            mDirection[1] += height.m_f_value;
        else
            mDirection[1] -= height.m_f_value;
        retval = true;
    }
    else if (frontback.is_match() )
    {
        if (frontback.is_positive_direction())
            move_foreward_backward( mDirection, frontback.m_f_value );         // height.m_f_value
        else
            move_foreward_backward( mDirection, -frontback.m_f_value );
        retval = true;
    }
    else if (sideways.is_match()) {
        if (sideways.is_positive_direction())
            move_left_right( mDirection, sideways.m_f_value );
        else
            move_left_right( mDirection, -sideways.m_f_value );
        retval = true;
    }
    return retval;
}

bool get_rotation( Sentence& mSentence, MathVector& mRotate )
{
    BidirectionalParameter rot_x,rot_y,rot_z;
    bool retval=false;          // No match
    
    string deg_exp = real_e + "(degrees )";
    
    rot_x.m_type    = FLOAT;
    rot_x.m_requested_unit = "degree";
    rot_x.m_positive_regexpression = "(clockwise )?(about x)( clockwise)?";
    rot_x.m_negative_regexpression = "(clockwise )?(about x)( counter clockwise|anticlockwise)";
    rot_x.set_permitted_units("(degree|radians)");
    rot_x.construct_regex();
    rot_x.parse ( mSentence.m_sentence );

    rot_y.m_type    = FLOAT;
    rot_y.m_requested_unit = "degree";
    rot_y.m_positive_regexpression = "(clockwise )?(about y|vertically)?( clockwise)?";
    rot_y.m_negative_regexpression = "(counter clockwise |anticlockwise )?(about y|vertically)?( counter clockwise| anticlockwise)?";
    rot_y.set_permitted_units("(degree|radians)");
    rot_y.construct_regex();
    rot_y.parse ( mSentence.m_sentence );

    rot_z.m_type    = FLOAT;
    rot_z.m_requested_unit = "degree";
    rot_z.m_positive_regexpression = "(about z)( clockwise)?";
    rot_z.m_negative_regexpression = "(about z)( counter clockwise| anticlockwise)";
    rot_z.set_permitted_units("(degree|radians)");
    rot_z.construct_regex();
    rot_z.parse ( mSentence.m_sentence );
    
    if (rot_x.is_match() || rot_y.is_match() || rot_z.is_match())
        retval = true;
    return retval;
}

void move_object    ( VerbalObject* obj, MathVector& mNewLocation )
{
    obj->relocate( mNewLocation );
}
void rotate_object( VerbalObject* obj, MathVector& mNewOrientation )
{
    obj->m_R_position = mNewOrientation;
}

/**** ACTION INITIATORS:    (4 possible actions) *****/

/*****************************************************************
 Do the work of the Telegram :
 return  -1	=> Not handled
 else number of extra bytes extracted from the mSentence buffer.
 - besides strlen(mSentence)!
 ServerHandler
 *****************************************************************/
int Parse_ThreeD_Statement( Sentence& mSentence, string* mh )
{
    static long last_object_id=-1;
    int         retval  = -1;
    float       h       = 10*12;
    string regexpression;
    
    if (Debug)  printf("Parse_Robot_Legs_Statement\n");
    mSentence.restore_reduced();
    
    // MOVE,
    regexpression = "(move|put|raise|lift|lower|pull|push)";
    if (mSentence.m_sentence.regex_find(regexpression))
    {
        MathVector NewLocation(3);
        MathVector Delta(3);
        VerbalObject* obj = referers.back();

        // Get Reffered Object :
        bool found = get_referred_object( mSentence, obj );
        if (found)
        {
            NewLocation = obj->m_position;      // Start at current position (for relative move)
        }
        MathVector direction(3);
        found = get_relative_direction( mSentence, direction );
        if (found)
        {
            NewLocation += direction;
        }
        found = get_absolute_direction( mSentence, NewLocation );
        if (found)
        {
        }
        move_object( obj, NewLocation );
        
        *mh = "moving";
        nlp_reply_formulated = true;
        retval = 0;
    }
    regexpression = "(delete)";
    if (mSentence.m_sentence.regex_find(regexpression))
    {
        long Object_id = last_object_id;
        sim_delete_object( Object_id );
        *mh = "dont mind if i do.";
        nlp_reply_formulated = true;
        retval = 0;
    }
    regexpression = "(rotate)";
    if (mSentence.m_sentence.regex_find(regexpression))
    {
        VerbalObject* obj = referers.back();
        MathVector NewOrientation;
        bool found = get_referred_object( mSentence, obj );
        if (found)
        {
            NewOrientation = obj->m_R_position;                 // Get Current orientation
            found = get_rotation( mSentence, NewOrientation );
            if (found)
            {
                rotate_object( obj, NewOrientation );
                *mh = "rotated" + obj->m_name;
                nlp_reply_formulated = true;
            }
            retval = 0;
        }
    }
    regexpression = "(stretch)";
    if (mSentence.m_sentence.regex_find(regexpression))
    {
        if (mSentence.is_found_in_sentence("it"))
        {
            enum eObjectTypes object_type = BOX;
            
            //sim_new_object( object_type, 1 );       // degrees
            *mh = "created new ibeam";
            nlp_reply_formulated = true;
            retval = 0;
        }
    }
    regexpression = "(call it|name it)";
    if (mSentence.m_sentence.regex_find(regexpression))
    {
    }
    
    // also change color
    regexpression = "(make thicker|make thinner)";
    if (mSentence.m_sentence.regex_find(regexpression))
    {
        if (mSentence.is_found_in_sentence("box"))
        {
            enum eObjectTypes object_type = BOX;
            
            //sim_new_object( object_type, 1 );       // degrees
            *mh="thinning new box";
            nlp_reply_formulated = true;
            retval = 0;
        }
    }
    
    if (mSentence.is_found_in_sentence("show camera"))
    {
        ipc_memory_sim->Command = COMMAND_CHANGE_GLOBAL_VAR;
        ipc_memory_sim->object_id      = 0;
        if (mSentence.is_found_in_sentence("don't") || mSentence.is_found_in_sentence("off"))
        {
            ipc_memory_sim->object_datum1  = 0;
            *mh = "okay, camera is off";
            nlp_reply_formulated = true;
        }
        else if (mSentence.is_found_in_sentence("show") || mSentence.is_found_in_sentence("on"))
        {
            ipc_memory_sim->object_datum1  = 1;
            *mh = "showing camera on wall inside apartment.";
            nlp_reply_formulated = true;
        }
        ipc_memory_sim->CommandCounter++;
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("robot view"))
    {
        ipc_memory_sim->Command = COMMAND_CHANGE_GLOBAL_VAR;
        ipc_memory_sim->object_id      = 1;
        if (mSentence.is_found_in_sentence("don't") || mSentence.is_found_in_sentence("off"))
        {
            ipc_memory_sim->object_datum1  = 0;
            *mh = "showing user's view point";
            nlp_reply_formulated = true;
        }
        else if (mSentence.is_found_in_sentence("show") || mSentence.is_found_in_sentence("on"))
        {
             ipc_memory_sim->object_datum1  = 1;
             *mh = "showing robot's viewpoint.";
             nlp_reply_formulated = true;
        }
        ipc_memory_sim->CommandCounter++;
        retval = 0;
    }
    

    if (retval>-1)  printf( "Parse_ThreeD_Statement done\n" );
    return retval;
}



