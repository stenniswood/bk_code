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
#include "Alias.hpp"
#include "nlp_sentence.hpp"
#include "client_to_socket.hpp"

#include "self_identity.hpp"


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


static WordGroup  	subject_list;		// we'll use this as "interrogative" listing the common ways of asking a Self_Identity question.
static WordGroup 	verb_list;
static WordGroup  	object_list;
static WordGroup 	adjective_list;

// does verb_list extraction match an Word word.


static void init_subject_list()
{
    // these are a clue that Self_Identity question might be coming.
    subject_list.add_word("name");
    subject_list.add_word("who" );
    subject_list.add_word("what");
    subject_list.add_word("what's");
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
}

void Init_Self_Identity_Protocol()
{
    init_word_lists();
}


/**** ACTION INITIATORS:    (4 possible actions) *****/


/*****************************************************************
 Do the work of the Telegram :
 return  -1	=> Not handled
 else number of extra bytes extracted from the mSentence buffer.
 - besides strlen(mSentence)!
 *****************************************************************/
int Parse_Self_Identity_Statement( Sentence& mSentence )
{
    int retval=-1;
    
    int subject_count	= subject_list.evaluate_sentence( mSentence.m_sentence );
    int verb_count		= verb_list.evaluate_sentence   ( mSentence.m_sentence );
    int object_count    = object_list.evaluate_sentence ( mSentence.m_sentence 	 );
    int adjective_count = adjective_list.evaluate_sentence( mSentence.m_sentence );
    
    printf("Parse_Self_Identity_Statement\n");
    //diagram_sentence		( subject, verb, adjective, object, preposition );
    
    bool foundS = mSentence.is_found_in_sentence ( "who" );
    bool foundV = mSentence.is_found_in_sentence ( "are" );
    bool foundO = mSentence.is_found_in_sentence ( "you" );
    bool cond1  = (foundS && foundV && foundO);

    foundS = mSentence.is_found_in_sentence( "name" );
    bool foundA = adjective_list.was_found_in_sentence( "your" );
    bool cond2 = (foundS && foundA);
    
    if ((cond1) || (cond2))
    {        
        //strcpy ( (char*)NLP_Response, "I am Vicki (Virtual Interactive Kinetic Intelligence. How may I assist you?");
        form_response( "I am Vicki (Virtual Interactive Kinetic Intelligence. How may I assist you?" );
        retval = 0;
    }

    foundS = mSentence.is_found_in_sentence( "who"  );
    foundA = mSentence.is_found_in_sentence( "your" );
    foundO = mSentence.is_found_in_sentence( "creator" ) || mSentence.is_found_in_sentence( "maker" );
    cond1 = (foundS && foundA && foundO);
    foundS = mSentence.is_found_in_sentence( "what" );
    foundS = mSentence.is_found_in_sentence( "are" );
    foundO = mSentence.is_found_in_sentence( "you" );
    cond2 = (foundS && foundA && foundO);
    if (cond1 || cond2)
    {
        form_response( "I exist in many different forms as a cell phone assistant, and as a robot." );
        retval = 0;
    }
    
    
    foundS = mSentence.is_found_in_sentence( "who" );
    foundA = mSentence.is_found_in_sentence( "your" );
    foundO = mSentence.is_found_in_sentence( "creator" ) || mSentence.is_found_in_sentence( "maker" );
    cond1 = (foundS && foundA && foundO);
    foundS = mSentence.is_found_in_sentence( "who" );
    foundA = mSentence.is_found_in_sentence( "created" ) || mSentence.is_found_in_sentence( "made" );
    foundO = mSentence.is_found_in_sentence( "you" );
    cond2 = (foundS && foundA && foundO);
    if (cond1 || cond2)
    {
        form_response( "Beyond Kinetic's - Mr Tenniswood created me" );
        retval = 0;
    }

    foundS = mSentence.is_found_in_sentence( "what" ) || mSentence.is_found_in_sentence( "what's" );
    foundA = mSentence.is_found_in_sentence( "meaning" );
    foundO = mSentence.is_found_in_sentence( "life" );
    cond1 = (foundS && foundA && foundO);
    if (cond1)
    {
        form_response( "The meaning of life is to calculate anything and everything that makes life easier for humans." );
        retval = 0;
    }
    
    if (retval)     printf( "Parse_Self_Identity_Statement done\n" );
    return retval;
}

