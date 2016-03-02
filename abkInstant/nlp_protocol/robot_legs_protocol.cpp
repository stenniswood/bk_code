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

#include "Alias.hpp"

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

/***********************************************************************
All Incoming Video will also be saved to the hard-drive on those 
systems with enough capacity.  This allows the user to call back anything
in its memory banks.  Think star trek logs.  "There were two people outside
your side door yesterday...    Can you replay it?  
What was the topic of it?  How long was it?  A smart robot could even
use the info to answer questions.  On Sept 20th, you said you wanted to
ignore all upgrade notices.
***********************************************************************/
/* VIDEO refers to a video file.  Where as CAMERA refers to live cam. */


static WordGroup  	subject_list;		// we'll use this as "interrogative" listing the common ways of asking a math question.
static WordGroup 	verb_list;


static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;


/*static void init_subject_list()
{
	// these are a clue that math question might be coming.
	subject_list.add_word("leg");		// towards end of sentence.
	subject_list.add_word("knee");
	subject_list.add_word("foot");	
	subject_list.add_word("calf");	
	subject_list.add_word("ankle");	
	subject_list.add_word("ankle");
	
	// sentence must contain 2 or more numbers too.
}

static void init_verb_list()
{
    Word mult;
    Word divide;
    Word plus;
    Word minus;
	
	mult.add_new("rasise");
	mult.add_new("multiplied by");

	divide.add_new("divided by");
	divide.add_new("divide");
	divide.add_new("over");	
	divide.add_new("split");	
	//divide.add_new("separated");		// implied times.
	
	plus.add_new("plus");
	plus.add_new("added");
	plus.add_new("add");		
	
	minus.add_new("minus");			
	minus.add_new("subtract");			
	minus.add_new("subtracted");	
		
	verb_list.add_new(mult  );
	verb_list.add_new(divide);
	verb_list.add_new(plus  );
	verb_list.add_new(minus );
			
	//verb_list.add_new("for each of ");
	
}


static void init_adjective_list()
{ 

}

static void init_object_list()
{   // Object might be a numerical value preceded by a preposition.
	// ie. "set camera tilt _to_ _25.5 degrees"
	// prepositions to look for :
	//		to by as 
	object_list.push_back( "me" );
}

static void init_word_lists()
{
	init_subject_list();
	init_verb_list();
	init_object_list();
	init_adjective_list();
}*/

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

	if (Debug)  printf("Parse_Robot_Legs_Statement\n");
/*	int subject_count  	= subject_list.evaluate_sentence( mSentence.m_sentence );
	int verb_count 		= verb_list.evaluate_sentence   ( mSentence.m_sentence );
//	bool result = contains_two_or_more_numbers( mSentence );
 
	string* object 		= extract_word( mSentence.m_sentence, &object_list  	);
	string* adjective	= extract_word( mSentence.m_sentence, &adjective_list  );
    //diagram_sentence		( subject, verb, adjective, object, preposition ); */

	if (mSentence.is_found_in_sentence("leg"))
	{
		if (mSentence.is_found_in_sentence( "raise"))
		{
            form_response("raising leg");
            retval = 0;
		}
        
        if (mSentence.is_found_in_sentence( "lower"))
		{
            form_response("lowering leg");
            retval = 0;
		}
    }
    if (mSentence.is_found_in_sentence("knee"))
    {
        if (mSentence.is_found_in_sentence( "bend" ))
        {
            form_response("bending knee");
            retval = 0;
        }
        if (mSentence.is_found_in_sentence( "straighten"))
        {
            form_response("straightening knee");
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
        form_response("dont mind if i do.");
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
	
	if (retval>-1)  printf( "Parse_Robot_Legs_Statement done\n" );
	return retval;
}

