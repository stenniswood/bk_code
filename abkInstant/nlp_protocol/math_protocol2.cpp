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
 ***********************************************************************/
string interrogative_expression = "(what is|how much is)";
string Plus_expression      = "(\\d+ )(\\+|plus|added to|add) (\\d+)";
string Minus_expression     = "(\\d+ )(\\-|minus|subtract|subtracted from) (\\d+)";
string Multiply_expression  = "(\\d+ )(\\*|times|multiply|multiplied by) (\\d+)";
string Divide_expression    = "(\\d+) (\\/|divided by|divide|over) (\\d+)";          // "quotient of (\\d+) over (\\d+)";

/* equal, what is, what does, how much */
//Word mult("times, multiplied by, multiply" );   // this should work
void Init_Math_Protocol()
{

}

/**** ACTION INITIATORS:    (4 possible actions) *****/
void get_number( std::smatch matches, float& first_number, int match_index=1)
{
    string tmp;
    tmp = matches[match_index];
    first_number = atoi( tmp.c_str() );
}
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
//    	printf("Found PLUS\t");
        get_numbers( mSentence.m_sentence.regex_matches, first_number, second_number );
        final_answer = first_number + second_number;
        return result;
    }
    
    result = mSentence.regex_find(Minus_expression);	// will find any matching multiply Word too.
    if (result)
    {
//    	printf("Found MINUS\n");
        get_numbers( mSentence.m_sentence.regex_matches, first_number, second_number );
        final_answer = first_number - second_number;
        return result;
    }
    
    result = mSentence.regex_find( Multiply_expression );
    if (result) {
        get_numbers( mSentence.m_sentence.regex_matches, first_number, second_number );
        final_answer = first_number * second_number;
	    return result;        
    }
    result = mSentence.regex_find( Divide_expression );
    if (result) {
        get_numbers( mSentence.m_sentence.regex_matches, first_number, second_number );
        if (second_number!=0.0)
            final_answer = first_number / second_number;
        else
            final_answer = FLT_MAX;
	    return result;            
    }
    return result;
}

string speed_exp = "(miles per hour|kilometers per hour|meters per second|feet per second)";
string weight_exp = "(pounds|kilograms|grams|ounces)";

const float Miles_to_Km_per_Hour = 1.60934;
const float Km_to_Miles_per_Hour = 0.621371;

const float Pound_to_Kg = 0.453592;
const float Kg_to_Pound = 2.20462;

const float ounces_to_g = 28.3495;
const float g_to_ounces = 0.035274;

const float inches_to_cm = 2.54;
const float cm_to_inches = 0.393701; 

const float feet_to_meters = 0.3048;
const float meters_to_feet = 3.28084; 

int Parse_Unit_Conversion_Statement( Sentence& mSentence, float& final_answer )
{
    int retval=-1;
    final_answer=0.0;
    float first_number;
    float second_number;
	float scale;

    int foundA = mSentence.regex_find( "(\\d+ )(km\\/hr|kph|metric|kilometers? per hour) (to|into|in|as) (miles? per hour|mph|miles?/hr)" );
    if (foundA)	{ scale = Km_to_Miles_per_Hour;  get_number( mSentence.m_sentence.regex_matches, first_number); }    
    int foundB = mSentence.regex_find( "(\\d+ )(miles? per hour|mph|miles?/hr) (to|into|in|as) (km/hr|kph|metric|kilometers? per hour)" );
    if (foundB)	{ scale = Miles_to_Km_per_Hour;  get_number( mSentence.m_sentence.regex_matches, first_number); }    
    if (foundA||foundB) 
    {
    	final_answer = first_number * scale;   
		//printf("%6.3f = %6.3f", first_number, final_answer );    	
    	return 1;
    }

    foundA = mSentence.regex_find( "(\\d+) (pounds?|lbs) (to|into|in|as) (kilograms?|kg|metric)" );
    if (foundA)	{ scale = Pound_to_Kg; get_number( mSentence.m_sentence.regex_matches, first_number); }
    foundB = mSentence.regex_find( "(\\d+ )(kilograms?|kg|metric) (to|into|in|as) (pounds?|lbs)" );
    if (foundB)	{ scale = Kg_to_Pound;  get_number( mSentence.m_sentence.regex_matches, first_number); }
    if (foundA || foundB)
    {
        final_answer = first_number * scale;    
        return 1;
    }

    foundA = mSentence.regex_find( "(\\d+ )(ounces?) (to|into|in|as) (grams?|g)" );
    if (foundA)	{ scale = ounces_to_g; get_number( mSentence.m_sentence.regex_matches, first_number); }
    foundB = mSentence.regex_find( "(\\d+ )(grams?|g) (to|into|in|as) (ounces?)" );        
    if (foundB)	{ scale = g_to_ounces;  get_number( mSentence.m_sentence.regex_matches, first_number); }
    if (foundA||foundB)
    {
        final_answer = first_number * scale;   
    	return 1;    	
    }
    
    foundA = mSentence.regex_find( "(\\d+ )(inches?|in) (to|into|in|as) (centimeters?|cm)" );    
    if (foundA)	{ scale = inches_to_cm; get_number( mSentence.m_sentence.regex_matches, first_number); }
    foundB = mSentence.regex_find( "(\\d+ )(centimeters?|cm) (to|into|in|as) (inches?|in)" );
    if (foundB)	{ scale = cm_to_inches;  get_number( mSentence.m_sentence.regex_matches, first_number); }    
    if (foundA||foundB)
    {    
    	final_answer = first_number * scale;   
    	return 1;    	
    }
    
    foundA = mSentence.regex_find( "(\\d+) (feets?|foot) (to|into|in|as) (meters?)" );
    if (foundA)	{ scale = feet_to_meters; get_number( mSentence.m_sentence.regex_matches, first_number); }    
    foundB = mSentence.regex_find( "(\\d+) (meters?) (to|into|in|as) (feet|foot)" );
    if (foundB)	{ scale = meters_to_feet;  get_number( mSentence.m_sentence.regex_matches, first_number); }        
    if (foundA||foundB)
    {
    	final_answer = first_number * scale;   
    	return 1;
    }

    //printf("%6.3f %6.2f = %6.3f", first_number, second_number, final_answer );
    return retval;
}

int Parse_shape_geometry( Sentence& mSentence, float& final_answer )
{
	float first_number=0.0;
	printf("parse shape geometry\n");

    int foundA = mSentence.regex_find( "area (of )?(a )?circle with radius (of )?(\\d+)" );
    if (foundA)	{ 
    	int msize = mSentence.m_sentence.regex_matches.size();
    	get_number( mSentence.m_sentence.regex_matches, first_number, msize-1); 
    	printf("  %6.2f\n", first_number );
    	final_answer = M_PI * first_number*first_number;
  		return 1;    	
    }    

    foundA = mSentence.regex_find( "area (of )?(a )?circle with (a )?diameter (of )?(\\d+)" );
    if (foundA)	{ 
    	int msize = mSentence.m_sentence.regex_matches.size();    
    	get_number( mSentence.m_sentence.regex_matches, first_number,msize-1); 
    	printf("  %6.2f\n", first_number );    	
    	final_answer = M_PI * first_number*first_number/4.0;
  		return 1;    	
    }    

    foundA = mSentence.regex_find( "circumference (of )?(a )?(circle )?(with )?(a )?radius (of )?(\\d+)" );
    if (foundA)	{ 
    	int msize = mSentence.m_sentence.regex_matches.size();    
    	get_number( mSentence.m_sentence.regex_matches, first_number,msize-1); 
    	final_answer = M_PI * first_number*2;
  		return 1;    	
    }    

    foundA = mSentence.regex_find( "circumference (of )?(a )?(circle )?(with )?(a )?diameter (of )?(\\d+)" );
    if (foundA)	{ 
    	int msize = mSentence.m_sentence.regex_matches.size();    
    	get_number( mSentence.m_sentence.regex_matches, first_number,msize-1); 
    	final_answer = M_PI * first_number;
  		return 1;
    }    
	return 0;
}

void form_verbal_answer(Sentence& mSentence, ServerHandler* mh, float final_answer)
{
    int integer = trunc(final_answer);
    float remainder = final_answer - integer;

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
    //printf("Parse_Math_Statement: %s\n", mSentence.m_raw_sentence );
    
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
    
	int result = Parse_Unit_Conversion_Statement( mSentence, final_answer );
	if (result>0) {
		form_verbal_answer(mSentence, mh, final_answer);		
		retval=0;
		return retval;            
	} 	
	
	result = Parse_shape_geometry( mSentence, final_answer );
	if (result>0) {
		form_verbal_answer(mSentence, mh, final_answer);		
		return retval;            
	}

    int foundA = mSentence.regex_find( "area (of )?(a )?circle" );
    if (foundA)	{ 
		mh->form_response( "Area of a circle is pi times the radius squared." );	
		return 1;	
    }    
    foundA = mSentence.regex_find( "circumference (of )?(a )?circle" );
    if (foundA)	{ 		
		mh->form_response( "Circumference of a circle is pi times the diameter." );	
		return 1;	
    }    
	
    if (retval>-1)  printf( "Parse_Math_Statement done\n" );
}

void test_math_protocol()
{
	printf("======= MATH PROTOCOL UNIT TESTS ==========\n");
	
	ServerHandler mh;
	Sentence sent;
	// TEST ADDITION:
	sent.set( "what is 5 plus 5 equal?");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );
	sent.set( "what is 5 + 5 equal?");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );
	sent.set( "what is 125 plus 250 plus 101 plus 201 equal ?");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() ); 
	sent.set( "what is 125 plus 250 equal ?");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

	// TEST SUBTRACTION:
	sent.set( "what is 25 minus 10 equal ?");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );
	sent.set( "what is 25 - 10 equal ?");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

	// TEST MULTIPLICATION:
	sent.set( "what is 256 times 4 equal?");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

	// TEST DIVISION:
	sent.set( "what is 512 divided by 16 equal?");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );
	sent.set( "what is 64 / 16 equal?");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

	// UNIT CONVERSION:
	sent.set( "Convert 230 km/hr to miles/hr");	
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

	sent.set( "Convert 130 miles per hour to km/hr");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );
	
	sent.set( "Convert 185 lbs to kg");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );
	sent.set( "Convert 100 kilograms to pounds");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

	sent.set( "Convert 16 ounces to grams");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );
	sent.set( "Convert 10 grams to ounce");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

	sent.set( "Convert 6 inches to cm");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );
	sent.set( "Convert 50 cm to inches");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

	sent.set( "Convert 5280 feet to meters");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );
	sent.set( "Convert 100 meters to feet");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

	sent.set( "Convert 100 meters to feet");
	Parse_Math_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );

}