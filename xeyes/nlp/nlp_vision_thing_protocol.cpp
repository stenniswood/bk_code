/*	Process Verbal Commands : 

Watch my eyes.
How many object on the table?
Show red objects.
*/
#include <iostream>
#include <list>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <time.h> 
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
//#include "bk_system_defs.h"
#include "global.h"
#include "nlp_vision_things.hpp"
#include "color_protocol.hpp"


#define FALSE 0
#define TRUE  1
#define BOOL unsigned char

#include "serverthread.hpp"
#include "nlp_sentence.hpp"
#include "vision_logger.hpp"

using namespace std;

// Response language:
BOOL 			nlp_reply_formulated = FALSE;
string 			NLP_Response_str;

/*****************************************************************
 Do the work of the Telegram :
 return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void*      m_verbal_focus=0;

void Init_Thing_Protocol( )
{

}

extern int connfd;


/* Idea is to turn each question (asked several ways) into a unique id.
	This can be used for questions like, the last time you asked that...	
*/
int Get_thing_symantic_id( Sentence& theSentence )
{
	int retval=0;
	vector<int> answers;
	vector<int> remove_wi;		
	
	// on table?
	int what_on_table = theSentence.are_found_in_sentence( "what do you see on the table?" ); // any order.
	if (what_on_table)
		retval = 101;

	int what_on_table = theSentence.are_found_in_sentence( "is way clear?" ); // any order.
	if (what_on_table)
		retval = 104;
	int what_on_table = theSentence.are_found_in_sentence( "how far is the wall?" ); // any order.
	if (what_on_table)
		retval = 105;

	int  num_objects = theSentence.are_found_in_sentence( "how many (things|objects) on the table?" );
	int  num_tables = theSentence.are_found_in_sentence( "how many tables?" );	    
	if (num_times && num_tables) 
		retval = 111;

	int  num_times  = theSentence.are_found_in_sentence( "how many objects were there ?" );
	if (found && qualitative_time_duration)
		retval = 200;
	
	// 
	exp = "show (me|the) "+color;
	int finds = S.regex_find( exp, &answers, &remove_wi );
	if (finds)
		retval = 110;
	
	exp = "find circles";
	int finds = S.regex_find( exp, &answers, &remove_wi );
	if (finds)
		retval = 112;

	exp = "find squares";
	int finds = S.regex_find( exp, &answers, &remove_wi );
	if (finds)
		retval = 113;

	exp = "find cubes";
	int finds = S.regex_find( exp, &answers, &remove_wi );
	if (finds)
		retval = 114;

	exp = "is (the )?door open";
	int finds = S.regex_find( exp, &answers, &remove_wi );
	if (finds)
		retval = 115;
	return retval;
}

void process_thing_request( int question_id )
{
	switch(question_id)
	{
	case 101: 
				break;
	case 104: 
				break;
	case 105: 
				break;
	case 110: 
				break;
	case 111: 
				break;
	case 112: 
				break;
	case 113: 
				break;
	case 114: 
				break;
	case 115: 
				break;
	case 200: 
				break;
				
	default : 
				break;
	}
}

// Next time you see David...
int Get_future_thing_statement_symantic_id(Sentence& theSentence)
{
	int retval=-1;
	bool conditional = (theSentence.is_found_in_sentence("when") || theSentence.is_found_in_sentence("if"));
    bool did_see   = (theSentence.are_found_in_sentence( "you" ) && 
    		  	      theSentence.are_found_in_sentence( "see" ) );
	if (conditional && did_see)
		retval = 401;	
	return retval;
}

int process_future_thing_request( int question_id )
{
	int retval=-1;
	switch(question_id)
	{
	case 101: 
				break;
	default: 
				break;
	}
	return retval;
}
/*****************************************************************
Handle questions about objects in front of the robot's eyes.
	The cube, the fork the spoon, the plate, the ball. etc.
	Look for the reddish object,
 
*****************************************************************/
char* Parse_vision_things_Statement( Sentence theSentence, ServerHandler* mh )
{
    bool  vr 	= theSentence.is_voice_response();
 	char* end_of_telegram = mSentence + strlen(mSentence) +1 /*nullterminator*/;
	int   result 		  = -1;
	if (vr)
	{
		printf("Prefilter determined a VoiceResponse!\n");
		return end_of_telegram;
	}
	if (strcmp(mSentence, "I don't understand. Ignoring.")==0)
		return end_of_telegram;
    
    int question_id = Get_thing_symantic_id(theSentence);
    process_thing_request( question_id );
    
    switch(question_id)
    {
		case 101 :	// what's on table?
				break;
		case 104 :  // way clear?
				break;
		case 105 :  // far is the wall? 
				break;
		case 110 : // show color
				Parse_color_statement( theSentence, std::string& mReponse );
				filter_color( cv::Mat& mImage, Color mMin, Color mMax );
				
				break;
				
		case 111 : // how many on table?
				break;
		case 112 : // find circles (hough transform)
				break;
		case 113 : // find squares 
				break;
		case 114 : // find cubes
				break;
		case 115 : // find cubes
				break;

		case 200 :  // How many were there (past tense)
				break;
		default  : 
				break;
    }
    
    int future_action_request = Get_Future_Statement_symantic_id();
    switch (future_action_request)
    {
		case 100  : break;
		case 100  : break;
		case 100  : break;				
		default : break;
    }

	string Response;
	if (result==-1)
		result = Parse_color_statement( theSentence, Response );

	// Not handled:
	if (result==-1)
	    mh->form_response("I don't understand. Ignoring.");
	return end_of_telegram;
}
