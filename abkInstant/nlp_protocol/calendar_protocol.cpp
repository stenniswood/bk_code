//
//  calendar_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/17/16.
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
#include <vector>
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

#include "calendar_protocol.hpp"

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


static WordGroup  	subject_list;		// we'll use this as "interrogative" listing the common ways of asking a Calendar question.
static WordGroup 	verb_list;
static WordGroup  	object_list;
static WordGroup 	adjective_list;

// does verb_list extraction match an Word word.


/*static void init_subject_list()
{
    // these are a clue that Calendar question might be coming.
    subject_list.add_word("name");
    subject_list.add_word("who" );
    subject_list.add_word("what");
    subject_list.add_word("self");
    subject_list.add_word("how much");
    
    // sentence must contain 2 or more numbers too.
}

static void init_verb_list()
{
    Word tobe;
    Word created;
    Word plus;
    Word minus;
    
    tobe.add_new("are");
    tobe.add_new("is");
    
    created.add_new("made");
    created.add_new("created");
    created.add_new("designed");
    
    //divide.add_new("separated");		// implied times.
    
    plus.add_new("plus");
    plus.add_new("added");
    plus.add_new("add");
    
    minus.add_new("minus");
    minus.add_new("subtract");
    minus.add_new("subtracted");
    
    verb_list.add_new(tobe  );
    verb_list.add_new(created);
    verb_list.add_new(plus  );
    verb_list.add_new(minus );
    
    //verb_list.add_new("for each of ");
}


static void init_adjective_list()
{
    adjective_list.add_word( "highest"  );
    adjective_list.add_word( "my" 		);
    adjective_list.add_word( "your" 	);
    adjective_list.add_word( "low"      );
    adjective_list.add_word( "lowest"	);
    adjective_list.add_word( "VGA"      );
    adjective_list.add_word( "QVGA"     );
    adjective_list.add_word( "hdmi" 	);
    adjective_list.add_word( "quality"  );
    adjective_list.add_word( "hi-res"   );
}

static void init_object_list()
{   // Object might be a numerical value preceded by a preposition.
    // ie. "set camera tilt _to_ _25.5 degrees"
    // prepositions to look for :
    //		to by as
    object_list.add_word( "me"		);
    object_list.add_word( "you"     );
    object_list.add_word( "time"	);
    object_list.add_word( "date"	);
    object_list.add_word( "year"	);
}

static void init_word_lists()
{
    init_subject_list();
    init_verb_list();
    init_object_list();
    init_adjective_list();
} */

void Init_Calendar_Protocol()
{
    //init_word_lists();
}


/**** ACTION INITIATORS:    (4 possible actions) *****/
int Parse_Time_from_now_Statement( Sentence& mSentence )
{
    int retval=-1;
    bool foundP   = mSentence.is_found_in_sentence( "in" );
    bool foundNum = mSentence.next_word_is_a_number();
    int   index=-1;
    float Number=0.0;
    if (foundNum)
    {
        index  = mSentence.get_word_pointer();
        Number = mSentence.get_nth_word_as_a_number(index);
    }
    // in X days
    // in X hours
    // in X minutes
    bool foundA = mSentence.is_found_in_sentence( "a" );
    bool foundMo= mSentence.is_found_in_sentence( "month", true  );
    bool foundW = mSentence.is_found_in_sentence( "week", true   );
    bool foundD = mSentence.is_found_in_sentence( "day", true    );
    bool foundH = mSentence.is_found_in_sentence( "hour", true   );
    bool foundM = mSentence.is_found_in_sentence( "minute", true );
    bool foundS = mSentence.is_found_in_sentence( "second", true );
    int cond2 = (foundMo || foundW || foundD || foundH || foundM || foundS );
    
    if ((foundNum || foundA) && (cond2))
    {
        time_t t = time(0);                 // get time now
        float delta_time=0.0;

        if (foundMo)
            delta_time = Number*60*60*24*30;        // this needs fixing.  In 5 months
        if (foundW)
            delta_time = Number*60*60*24*7;
        if (foundD)
            delta_time = Number*60*60*24;
        if (foundH)
            delta_time = Number*60*60;
        if (foundM)
            delta_time = Number*60;
        t += delta_time;
        
        sprintf( NLP_Response, "%6.1f seconds from now. is ", delta_time );
        form_response(NLP_Response);
        retval=0;
    }
    return retval;
}

int Parse_date_Statement( Sentence& mSentence )
{
    return -1;
}


/*****************************************************************
 Do the work of the Telegram :
 return  -1	=> Not handled
 else number of extra bytes extracted from the mSentence buffer.
 - besides strlen(mSentence)!
 *****************************************************************/
int Parse_Calendar_Statement( Sentence& mSentence )
{
    int retval=-1;
    
/*    int subject_count	= subject_list.evaluate_sentence( mSentence.m_sentence );
    int verb_count		= verb_list.evaluate_sentence   ( mSentence.m_sentence );
    int object_count    = object_list.evaluate_sentence ( mSentence.m_sentence 	 );
    int adjective_count = adjective_list.evaluate_sentence( mSentence.m_sentence );
    
    printf("Parse_Calendar_Statement\n");
    //diagram_sentence		( subject, verb, adjective, object, preposition );
*/
    int foundS = 0;
    int foundV = 0;
    int foundO = 0;
    int foundA = 0;
    
    foundS = mSentence.is_found_in_sentence ( "what" );
    foundO = mSentence.is_found_in_sentence ( "time" );
    if (foundS && foundO)
    {
        time_t t = time(0);                 // get time now
        struct tm* now = localtime( & t );
        
        strftime( (char*)NLP_Response, 80, "The time is now %l %M %p.",now);
        CLIENT_Response =  NLP_Response;
        ClientRequestPending = true;
        nlp_reply_formulated = TRUE;
        retval = 0;
    }
    
    foundS = (mSentence.is_found_in_sentence( "what" ) || mSentence.is_found_in_sentence( "what's" ));
    foundO = mSentence.is_found_in_sentence ( "date" );
    if (foundS && foundO)
    {
        time_t t = time(0);   // get time now
        struct tm* now = localtime( & t );
        strftime( NLP_Response, 80, "Today is %A, %B, %e ", now );
        
        CLIENT_Response =  NLP_Response;
        ClientRequestPending = true;
        nlp_reply_formulated = TRUE;
        retval = 0;
    }
    foundS = mSentence.is_found_in_sentence( "what" );
    foundO = mSentence.is_found_in_sentence ( "year" );
    if (foundS && foundO)
    {
        time_t t = time(0);   // get time now
        struct tm* now = localtime( & t );
        strftime( NLP_Response, 80, "It is %Y", now );
        
        CLIENT_Response =  NLP_Response;
        ClientRequestPending = true;
        nlp_reply_formulated = TRUE;
        retval = 0;
    }
    
    bool new_calendar_entry_desired = false;
    
    foundS = mSentence.is_found_in_sentence( "add" ) || mSentence.is_found_in_sentence( "new" );
    foundO = mSentence.is_found_in_sentence ( "appointment" ) || mSentence.is_found_in_sentence ( "meeting" );
    if (foundS && foundO)
    {
        bool new_calendar_entry_desired = true;
        retval = 0;
    }
    
    int result = Parse_Time_from_now_Statement( mSentence );
    if (result>-1)
        retval=0;
    else {
        result = Parse_date_Statement( mSentence );
        if (result>-1)
            retval = 0;
    }
    if (new_calendar_entry_desired)
    {
        
    }
    if (nlp_reply_formulated)
    	printf("Response: %s\n", CLIENT_Response );
        
    if (retval>-1)   printf( "Parse_Calendar_Statement done\n" );
    return retval;
}

