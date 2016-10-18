//
//  math_protocol2.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 9/20/16.
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
#include <string.h>
#include <string>
#include <sys/types.h>
#include <time.h>
#include <string>
#include <math.h>
#include <cfloat>
#include <iostream>

#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.hpp"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"

#include "alias.hpp"
#include "nlp_sentence.hpp"
#include "client_to_socket.hpp"
#include "math_protocol2.hpp"

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

string interrogative_expression = "(what is|how much is)";
string Plus_expression      = "(\\d+ )(plus|added to|add) (\\d+)";
string Minus_expression     = "(\\d+ )(minus|subtract|subtracted from) (\\d+)";
string Multiply_expression  = "(\\d+ )(times|multiply|multiplied by) (\\d+)";
string Divide_expression    = "(\\d+ )(divided by|divide|over) (\\d+)";          // "quotient of (\\d+) over (\\d+)";


/* equal, what is, what does, how much */
//Word mult("times, multiplied by, multiply" );   // this should work
void Init_Math_Protocol()
{

}

/**** ACTION INITIATORS:    (4 possible actions) *****/
void get_numbers( std::smatch matches, float& first_number, float& second_number )
{
    string tmp;
    tmp = matches[1];
    first_number = atoi( tmp.c_str() );
    tmp = matches[3];
    second_number = atoi( tmp.c_str() );
}
// The sentence may contain several operations.
// mFoundWord will be gauranteed to be part of the sentence (b/c it was from evaluate_sentence() )
int Parse_One_Statement( Sentence& mSentence, float& first_number, float& second_number, float& final_answer )
{
    string tmp;
    final_answer=0.0;
    
    int result = mSentence.regex_find(Plus_expression);
    if (result) {
        get_numbers( mSentence.m_sentence.regex_matches, first_number, second_number );
        final_answer = first_number + second_number;
    }
    
    result = mSentence.regex_find(Minus_expression);	// will find any matching multiply Word too.
    if (result)
    {
        get_numbers( mSentence.m_sentence.regex_matches, first_number, second_number );
        final_answer = first_number - second_number;
    }
    
    result = mSentence.regex_find( Multiply_expression );
    if (result) {
        get_numbers( mSentence.m_sentence.regex_matches, first_number, second_number );
        final_answer = first_number * second_number;
    }
    result = mSentence.regex_find( Divide_expression );
    if (result) {
        get_numbers( mSentence.m_sentence.regex_matches, first_number, second_number );
        if (second_number!=0.0)
            final_answer = first_number / second_number;
        else
            final_answer = FLT_MAX;
    }
    return final_answer;
}

string speed_exp = "(miles per hour|kilometers per hour|meters per second|feet per second)";
string weight_exp = "(pounds|kilograms|grams|ounces)";


int Parse_Unit_Conversion_Statement( Sentence& mSentence )
{
    int retval=-1;
    float final_answer=0.0;
    float first_number;
    float second_number;
    
    //float first_number  = extract_number(mSentence, 0);
    //float second_number = extract_number(mSentence, 1);
    int foundA = mSentence.regex_find( "(convert |what is )?(\\d+ )miles per hour (to |in kph|in metric|in kilometers per hour)" );
    int foundB = mSentence.regex_find( "(\\d+ )kilometers per hour" );
    if (foundA && foundB)
    {    final_answer = first_number * second_number;   retval = 0; }
    
    foundA = mSentence.regex_find( "(\\d+ )pounds" );
    foundB = mSentence.regex_find( "(\\d+ )kilograms" );
    if (foundA && foundB)
    {    final_answer = first_number + second_number;    retval = 0; }
    
    foundA = mSentence.regex_find( "(\\d+ )ounces" );
    foundB = mSentence.regex_find( "(\\d+ )grams" );
    if (foundA && foundB)
    {    final_answer = first_number - second_number;   retval = 0; }
    
    foundA = mSentence.regex_find( "(\\d+ )inches" );
    foundB = mSentence.regex_find( "(\\d+ )centimeters" );
    if (foundA && foundB)
    {    final_answer = first_number / second_number;   retval = 0; }
    
    foundA = mSentence.regex_find( "(\\d+ )feet" );
    foundB = mSentence.regex_find( "(\\d+ )meters" );
    if (foundA && foundB)
    {    final_answer = first_number / second_number;   retval = 0; }
    return retval;
}

void form_verbal_answer(Sentence& mSentence, ServerHandler* mh, float final_answer)
{
    float remainder = final_answer - trunc(final_answer);
    int integer = trunc(final_answer);
    if (remainder<0.001)
        sprintf ( NLP_Response, "%s is %d", mSentence.m_sentence.c_str(), integer );
    else if (remainder<0.099)
        sprintf ( NLP_Response, "%s is %6.3f", mSentence.m_sentence.c_str(), final_answer );
    else
        sprintf ( NLP_Response, "%s is %6.1f", mSentence.m_sentence.c_str(), final_answer );
    CLIENT_Response      = NLP_Response;
    ClientRequestPending = true;
    nlp_reply_formulated = TRUE;
    mh->form_response( NLP_Response );
}

/*****************************************************************
 Do the work of the Telegram :
 return  -1	=> Not handled
 else number of extra bytes extracted from the mSentence buffer.
 - besides strlen(mSentence)!
 *****************************************************************/
int Parse_Math_Statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval=-1;
    mSentence.restore_reduced();
    printf("Parse_Math_Statement\n");
    
    int operation_count=0;
    float first_number, second_number, final_answer;
    //while()
    {
        int op = Parse_One_Statement( mSentence, first_number, second_number, final_answer );
        if (op) {
            int result = mSentence.regex_find( interrogative_expression );
            if (result) {
                mSentence.m_sentence.regex_reduce();
            }
            form_verbal_answer(mSentence, mh, final_answer);
        }
        
        operation_count++;
        retval = 0;
    }
    
    if (retval>-1)  printf( "Parse_Math_Statement done\n" );
    return retval;
}

