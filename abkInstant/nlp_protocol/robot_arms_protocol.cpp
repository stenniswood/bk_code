//
//  robot_arms_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/18/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
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
//#include "protocol.h"
//#include "devices.h"
#include "GENERAL_protocol.hpp"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"
#include "nlp_sentence.hpp"
//#include "client_to_socket.hpp"
#include "robot_arms_protocol.hpp"
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
int Parse_Robot_Arms_Statement( Sentence& mSentence )
{
    int retval=-1;
    
    if (Debug)  printf("Parse_Robot_arms_Statement\n");
    
    if (mSentence.is_found_in_sentence("arm",true))
    {
        if (mSentence.is_found_in_sentence( "raise"))
        {
            form_response("raising arms");
            retval = 0;
        }
        
        if (mSentence.is_found_in_sentence( "lower"))
        {
            form_response("lowering arm");
            retval = 0;
        }
    }
    if (mSentence.is_found_in_sentence("elbow"))
    {
        if (mSentence.is_found_in_sentence( "bend" ))
        {
            form_response("bending elbow");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "straighten"))
        {
            form_response("straightening elbow");
            retval = 0;
        }
    }
    if (mSentence.is_found_in_sentence("foot"))
    {
        if (mSentence.is_found_in_sentence( "extend" ))
        {
            form_response("extending");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "pull"))
        {
            form_response("flexing");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "keep level"))
        {
            form_response("holding level");
            retval = 0;
        }
    }
    
    if (mSentence.is_found_in_sentence("step"))
    {
        form_response("stepping");
        retval = 0;
    }
    
    if (mSentence.is_found_in_sentence("sit"))
    {
        form_response("don't mind if i do.");
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("stand"))
    {
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
    
    if (retval>-1)  printf( "Parse_Robot_arms_Statement done\n" );
    return retval;
}

