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
#include <math.h>
#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.hpp"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"

#include "Alias.hpp"
#include "nlp_sentence.hpp"
#include "client_to_socket.hpp"


/* Suggested statements:
		What is 197 times 12 ?
		What does 197 times 12 equal? 2364
		How much is 197 times 12?
		multiply 197 by 12.		[might imply hold answer w/o replying]
		
		if there are 197 people coming and each one brings 12 chairs, will we have enough?
*/

/***********************************************************************
All Incoming Video will also be saved to the hard-drive on those 
systems with enough capacity.  This allows the user to call back anything
in its memory banks.  Think star trek logs.  "There were two people outside
your side door yesterday...    Can you replay it?  
What was the topic of it?  How long was it?  A smart robot could even
use the info to answer questions.  On Sept 20th, you said you wanted to
ignore all upgrade notices.
***********************************************************************/


static WordGroup  	subject_list2;		// we'll use this as "interrogative" listing the common ways of asking a math question.

static WordGroup  	subject_list;		// we'll use this as "interrogative" listing the common ways of asking a math question.
static WordGroup 	verb_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;


void juk()
{
    Word tmpW;
    tmpW.add_new("hello");
    tmpW.add_new("bye");
    subject_list2.add_new(tmpW);
}

static void init_subject_list()
{
	// these are a clue that math question might be coming.
	subject_list.add_word("equal");		// towards end of sentence.
    subject_list.add_word("what is");
	subject_list.add_word("what does");
	subject_list.add_word("how much");
    int siez = subject_list.m_Words->size();

    
	// sentence must contain 2 or more numbers too.
}

static void init_verb_list()
{
    Word mult("times, multiplied by, multiply" );   // this should work
    Word divide,plus,minus;

    //
	mult.add_new("times");
	mult.add_new("multiplied by");
    mult.add_new("multiply");
    

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
	object_list.push_back( "me"	);
}

static void init_word_lists()
{
	init_subject_list();
	init_verb_list();
	init_object_list();
	init_adjective_list();
}

void Init_Math_Protocol()
{    
    init_word_lists();
}


/**** ACTION INITIATORS:    (4 possible actions) *****/
bool contains_two_or_more_numbers( Sentence& mSentence )
{
    int count=0;
    for(int i=0; i<mSentence.m_num_words; i++)
        if (mSentence.is_nth_word_a_number(i))
            count++;
    if (count>=2)
        return true;
    else
        return false;
}

// mOrdinal is 1 indexed!
float extract_number( Sentence& mSentence, int mOrdinal )
{
    int count=1;
    for(int i=0; i<mSentence.m_num_words; i++)
        if (mSentence.is_nth_word_a_number(i))
        {
            if (count==mOrdinal)
                return mSentence.get_nth_word_as_a_number(i);
            count++;
        }
    return 0.0;
}

// The sentence may contain several operations.
// mFoundWord will be gauranteed to be part of the sentence (b/c it was from evaluate_sentence() )
int Parse_One_Statement( Sentence& mSentence, Word& mFoundOperatingWord, float first_number, float second_number )
{
	int retval=-1;

	float final_answer=0.0;
    
	bool found = mFoundOperatingWord.is_synonym( "multiply" );	// will find any matching multiply Word too.
	if (found)
		final_answer = first_number * second_number;

	found = mFoundOperatingWord.is_synonym( "plus" );
	if (found)
		final_answer = first_number + second_number;
	
	found = mFoundOperatingWord.is_synonym( "minus" );
	if (found)
		final_answer = first_number - second_number;

	found = mFoundOperatingWord.is_synonym( "divide" );
	if ((found) && (second_number!=0.0))
		final_answer = first_number / second_number;

	return final_answer;
}

int Parse_Unit_Conversion_Statement( Sentence& mSentence )
{
    int retval=-1;
    float final_answer=0.0;

    float first_number  = extract_number(mSentence, 0);
    float second_number = extract_number(mSentence, 1);
    int foundA = mSentence.is_found_in_sentence( "miles per hour" );
    int foundB = mSentence.is_found_in_sentence( "kilometers per hour" );
    if (foundA && foundB)
        final_answer = first_number * second_number;
    
    foundA = mSentence.is_found_in_sentence( "pounds" );
    foundB = mSentence.is_found_in_sentence( "kilograms" );
    if (foundA && foundB)
        final_answer = first_number + second_number;

    foundA = mSentence.is_found_in_sentence( "ounces" );
    foundB = mSentence.is_found_in_sentence( "grams" );
    if (foundA && foundB)
        final_answer = first_number - second_number;
    
    foundA = mSentence.is_found_in_sentence( "inches" );
    foundB = mSentence.is_found_in_sentence( "centimeters" );
    if (foundA && foundB)
        final_answer = first_number / second_number;

    foundA = mSentence.is_found_in_sentence( "feet" );
    foundB = mSentence.is_found_in_sentence( "meters" );
    if (foundA && foundB)
        final_answer = first_number / second_number;
    
    return final_answer;
}

void form_verbal_answer(Sentence& mSentence, float final_answer)
{
    float remainder = final_answer - trunc(final_answer);
    int integer = trunc(final_answer);
    if (remainder<0.001)
        sprintf ( NLP_Response, "%s is %d", mSentence.m_sentence, integer );
    else if (remainder<0.099)
        sprintf ( NLP_Response, "%s is %6.3f", mSentence.m_sentence, final_answer );
    else
        sprintf ( NLP_Response, "%s is %6.1f", mSentence.m_sentence, final_answer );
    CLIENT_Response =  NLP_Response;
    ClientRequestPending = true;
    nlp_reply_formulated=TRUE;
    
}
/*****************************************************************
Do the work of the Telegram :
return  -1	=> Not handled
		else number of extra bytes extracted from the mSentence buffer.
			- besides strlen(mSentence)! 
*****************************************************************/
int Parse_Math_Statement( Sentence& mSentence )
{
	int retval=-1;
    
	int subject_count	= subject_list.evaluate_sentence( mSentence.m_sentence );
	int verb_count		= verb_list.evaluate_sentence   ( mSentence.m_sentence );
	bool two_or_more_numbers = contains_two_or_more_numbers( mSentence );
	
	if (!two_or_more_numbers)       // must have!
			return retval;
	if (verb_count==0)				// must have!
			return retval;

	printf("Parse_Math_Statement\n");
	string* object 		= extract_word( mSentence.m_sentence, &object_list  	  );
	string* adjective	= extract_word( mSentence.m_sentence, &adjective_list    );
    //diagram_sentence		( subject, verb, adjective, object, preposition );

    int operation_count=0;
    float first_number  = extract_number(mSentence, operation_count+1);
    while(operation_count < verb_list.m_all_extracted->size())
    {
        float second_number = extract_number(mSentence, operation_count+2);
        float result = Parse_One_Statement(mSentence, (*verb_list.m_all_extracted)[operation_count], first_number, second_number);
        first_number = result;
        operation_count++;
        retval = 0;
    }
	
    float final_answer = first_number;
    form_verbal_answer(mSentence, final_answer);
	if (retval>-1)  printf( "Parse_Math_Statement done\n" );
	return retval;
}

