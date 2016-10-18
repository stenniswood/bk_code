//
//  self_identity.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/16/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <list>
#include <sys/types.h>
#include <time.h>
#include <string>
#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.hpp"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"
#include "alias.hpp"
#include "nlp_sentence.hpp"
#include "client_to_socket.hpp"

#include "self_identity.hpp"


#define Debug 0

/***********************************************************************

 Suggested statements:
 
        I am viki.  [response to who are you, what's your name, etc.]
        My creator is ...
        I am here to help you.  [purpose]
        My favorite color is
        I have __,__, and ___ tasks on my list todo.
        I have lived with you for x years now.
        I am composed of 24 servo motors,
        My capabilities are:

        I am able to vacume, sweep, paint, etc.
 ***********************************************************************/


void Init_Self_Identity_Protocol()
{
}


/**** ACTION INITIATORS:    (4 possible actions) *****/


/*****************************************************************
 Do the work of the Telegram :
 return  -1	=> Not handled
 else number of extra bytes extracted from the mSentence buffer.
 - besides strlen(mSentence)!
 *****************************************************************/
int Parse_Self_Identity_Statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval=-1;
    
/*    int subject_count	= subject_list.evaluate_sentence( mSentence.m_sentence.c_str() );
    int verb_count		= verb_list.evaluate_sentence   ( mSentence.m_sentence.c_str() );
    int object_count    = object_list.evaluate_sentence ( mSentence.m_sentence.c_str() 	 );
    int adjective_count = adjective_list.evaluate_sentence( mSentence.m_sentence.c_str() ); */
    
    if (Debug) printf("Parse_Self_Identity_Statement\n");
    //diagram_sentence		( subject, verb, adjective, object, preposition );
    
    bool foundS = mSentence.is_found_in_sentence ( "who" );
    bool foundV = mSentence.is_found_in_sentence ( "are" );
    bool foundO = mSentence.is_found_in_sentence ( "you" );
    bool cond1  = (foundS && foundV && foundO);

    foundS = mSentence.is_found_in_sentence( "name" );
    bool foundA = mSentence.is_found_in_sentence( "your" );
    bool cond2 = (foundS && foundA);
    
    if ((cond1) || (cond2))
    {        
        //strcpy ( (char*)NLP_Response, "I am Vicki (Virtual Interactive Kinetic Intelligence. How may I assist you?");
        mh->form_response( "I am Vicki (Virtual Interactive Kinetic Intelligence. How may I assist you?" );
        retval = 0;
    }
    if (mSentence.is_found_in_sentence("hi") || mSentence.is_found_in_sentence( "hello" ))
    {
        mh->form_response( "Hello. " );
        if ( (mSentence.is_found_in_sentence("viki")) || mSentence.is_found_in_sentence("vicky") )
        {
            mh->form_response( "Hello. With whom am I speaking?" );
        }
        retval=0;
    }
    if ((mSentence.is_found_in_sentence("who") || mSentence.is_found_in_sentence("who's")) && (mSentence.is_found_in_sentence( "owner" ) || mSentence.are_found_in_sentence( "belong to" )))        
    {
        mh->form_response( "I belong to the zootopia police department!" );
        retval=0;
    }
    if (mSentence.are_found_in_sentence("how are you") || mSentence.are_found_in_sentence( "how do you do" ))
    {        
        mh->form_response( "Great all systems working!" );
        
       /* form_response( "I am critically ill." );
        form_response( "I am okay, but..." );
        form_response( "My motor control CAN network is down." );
        form_response( "My 3D imagination is down." );
        form_response( "My 3D imagination is extremely slow." );        
        form_response( "My vision system is down." );
        
        form_response( "Beyond Kinetics internet site is down." );  */
        
        retval = 0;
    }
        

    
    foundS = mSentence.is_found_in_sentence( "what" );
    foundV = mSentence.is_found_in_sentence( "are" );
    foundO = mSentence.is_found_in_sentence( "you" );
    cond2 = (foundS && foundV && foundO);
    if (cond2)
    {
        mh->form_response( "I exist in many different forms as a cell phone assistant, and as a robot." );
        retval = 0;
    }

    foundS = mSentence.is_found_in_sentence( "who"  ) || mSentence.is_found_in_sentence( "who's"  );
    foundA = mSentence.is_found_in_sentence( "your" );
    foundO = mSentence.is_found_in_sentence( "creator" ) || mSentence.is_found_in_sentence( "maker" );
    cond1 = (foundS && foundA && foundO);
    foundS = mSentence.is_found_in_sentence( "who" );
    foundA = mSentence.is_found_in_sentence( "created" ) || mSentence.is_found_in_sentence( "made" );
    foundO = mSentence.is_found_in_sentence( "you" );
    cond2 = (foundS && foundA && foundO);
    if (cond1 || cond2)
    {
        mh->form_response( "Beyond Kinetic's - Mr Tenniswood created me" );
        retval = 0;
    }

    foundS = mSentence.is_found_in_sentence( "what" ) || mSentence.is_found_in_sentence( "what's" );
    foundA = mSentence.is_found_in_sentence( "meaning" );
    foundO = mSentence.is_found_in_sentence( "life" );
    cond1 = (foundS && foundA && foundO);
    if (cond1)
    {
        mh->form_response( "The meaning of my life is to calculate anything and everything that makes life easier for humans." );
        retval = 0;
    }
    
    if (retval>-1)     printf( "Parse_Self_Identity_Statement done\n" );
    return retval;
}

