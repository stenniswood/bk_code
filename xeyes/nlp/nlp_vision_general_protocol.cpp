/*	Process Verbal Commands : 

// Person presence Question:
//	How do we extract the name?  It could be in most any position.

When did you see david last?
When did you last see david?
When was the last time you saw Stephen?

Have you seen David?
Where is David?

When did you last see David?
Where you active yesterday around noon?
How long was he here for?

Watch my eyes.
Follow/Track Janet. Do what she asks.

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
#include "nlp_vision_general_protocol.hpp"
#include "color_protocol.hpp"
#define FALSE 0
#define TRUE  1
#define BOOL unsigned char

#include "serverthread.hpp"
#include "nlp_sentence.hpp"
#include "vision_logger.hpp"
#include "qualitative_time.hpp"


extern int connfd;

// Response language:
BOOL 		nlp_reply_formulated = FALSE;
string 		NLP_Response_str;
void*      	m_verbal_focus=0;

struct tm 	start_time_bd;
struct tm 	end_time_bd;

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Init_General_Protocol( )
{
}

#define S theSentence.m_sentence

/* Idea is to turn each question (asked several ways) into a unique id.
	This can be used for questions like, the last time you asked that...	
*/
int Get_Statement_symantic_id( Sentence& theSentence )
{
	vector<int> answers;
	vector<int> remove_wi;	
	string exp,exp2;
	int retval=0;
	// Where were you when you saw David last...
	exp = "where were you";
    int where_were_you = theSentence.regex_find( exp );
	printf("%s\n", S.c_str() );
	
    bool when		   = theSentence.is_found_in_sentence ( "when" );
	int  num_times     = theSentence.are_found_in_sentence( "how many times"  );
	int  num_people    = theSentence.are_found_in_sentence( "how many people" );	    
    int  were_active   = theSentence.regex_find( "were you (last )?(active|activated)" );   
    int  were_deactive = theSentence.regex_find( "were you (last )?(deactive|deactivated)" );    
	//printf("num_times=%d;  w u act = %d; active=%d\n", num_times, were_deactive, parsed_qualitative_time );
	
	// Saw a person?
	exp = "was the (last)? time";
	int last_time = S.regex_find( exp, &answers, &remove_wi );
		
	exp = "(did )?you (last )?see";	  
	exp2 = "have you seen";
    int did_see = ( theSentence.regex_find(exp) || theSentence.regex_find(exp2) );
    
    bool foundS = when && (did_see || (theSentence.is_found_in_sentence( "saw" ) && last_time));
	if (foundS) {
		printf("REDUCED: %s\n", theSentence.m_reduced_sentence.c_str() );
		retval = 101;
	}
	if (num_times && did_see) 
		retval = 111;

	// Num Times activated during ___ and ___.
	if (when && were_active) 
		retval = 102;
	if (when && were_deactive) 
		retval = 103;

	// 	Active during the whole time        
	if (were_active && parsed_qualitative_time)
	{
		if (num_times)
			retval = 112;
		else 
			retval = 104;
	}
	// 	Active during the whole time
	if (were_deactive && parsed_qualitative_time) 
	{
		if (num_times)
			retval = 113;
		else 
			retval = 105;			
	}
    bool found = theSentence.are_found_in_sentence( "are you going to deactivate me" );
	if (found) 
		retval = 200;

	printf("Person question id = %d\n", retval );
	return retval;
}
/*
	Process response to question about user face detection.

*/
void process_person_query(int mquestion_id)
{
	int    	rows;
	time_t 	time;
	char* 	start_ptr;
	char* 	end_ptr;	
    string 	name,description;
    int 	num_activations=0;
    int 	num_sightings  =0;
    bool 	prior_activation;    
	string 	time_str;

    switch(mquestion_id)
    {
		case 101 : /* When did you last see David? */
				name = "Stephen";
				printf("REDUCED: %s\n", theSentence.m_reduced_sentence.c_str() );
				rows = sql_logger.form_response__last_time_i_saw( name, time );
				if (rows==0)
					NLP_Response_str = "According to my database, I have never met " + name;
				else {
					NLP_Response_str  = "The last time I saw " + name + " was ";					
					compose_time_qualitative( time, time_str );
					NLP_Response_str += time_str;
					NLP_Response_str += "\n";
				}
				result = 0;
				break;
		case 102 : /* When did you become active? How long have you been active?	*/
				rows = sql_logger.form_response__last_time_i_activated( time );
				compose_time_qualitative( time, time_str );
				if (rows==0)
					NLP_Response_str = "According to my database, I have never activated.";
				else {
					NLP_Response_str  = "The last time I activated was ";
					NLP_Response_str += time_str;
					NLP_Response_str += "\n";
				}
				result = 0;		
				break;
		case 103 : /* When were you deactived last? How long were you deactived? */
				rows = sql_logger.form_response__last_time_i_deactivated( time );
				if (rows==0)
					NLP_Response_str = "According to my database, I have never deactivated.";
				else {
					compose_time_qualitative( time, time_str );
					NLP_Response_str  = "The last time I deactivated was ";
					NLP_Response_str += time_str;
					NLP_Response_str += "\n";
				}
				result = 0;
				break;
		case 104 : // Were you active during...
				   //    To answer:  Any deactivations during time?
				   //			no, then was it Activation or Deactivation prior to the start time?
				   //           yes, then, "Not the whole time"
					description = "Vision System De-activated";
					name= "";
					num_activations = sql_logger.form_response__how_many_times( 
										name, description, start_time_bd, end_time_bd );
					start_ptr = asctime( &start_time_bd );
					end_ptr   = asctime( &end_time_bd   );

				   if (num_activations)
					   NLP_Response_str = "Not the whole time";
				   else {
				   		// Need to retrieve the last Activation or DeActivation and return true if active.
				   		prior_activation = true;
						if (prior_activation)
					   	   NLP_Response_str = "I was active the entire time";
				   		else 
						   NLP_Response_str = "I was not active at all during that time";
				   }
				   result = 0;
				break;
		case 105 : // Were you deactive during...
				   //    To answer:  Any deactivations during time?
				   //			no, then was it Activation or Deactivation prior to the start time?
				   //           yes, then, "I was deactivated the whole time"
				   //					   "I was deactivated (most|part|some) of the time"
					description = "Vision System Activated";
					name= "";
					num_activations = sql_logger.form_response__how_many_times( 
										name, description, start_time_bd, end_time_bd );				   
				   if (num_activations)
					   NLP_Response_str = "Deactive part of the time.";
				   else {
				   		prior_activation = true;
						if (prior_activation==false)
				   	   		NLP_Response_str = "I was deactive the entire time";
						else 
					   		NLP_Response_str = "I was active during that time";				   
				   }
				   result = 0;
				break;
		case 111 : // How many times see him/her ?
				description = "Face Detected";
				name = "David";
				num_sightings = sql_logger.form_response__how_many_times( 
								name, description, start_time_bd, end_time_bd );
				compose_time_period_qualitative( &start_time_bd, &end_time_bd, NLP_Response_str );
				if (num_sightings==0) {
					NLP_Response_str += ", I did not see him. ";				
				} else {
					NLP_Response_str += ", I saw him ";
					NLP_Response_str += std::to_string(num_sightings);
					NLP_Response_str += " times.\n";
				}
				result = 0;
				break;
		case 112 : // Num times activated 
				description = "Vision System Activated";
				num_activations = sql_logger.form_response__how_many_times( 
									name, description, start_time_bd, end_time_bd );
				start_ptr = asctime( &start_time_bd );
				end_ptr   = asctime( &end_time_bd   );
				compose_time_period_qualitative( &start_time_bd, &end_time_bd, NLP_Response_str );
				NLP_Response_str += ", I was activated ";
				NLP_Response_str += std::to_string(num_activations);
				NLP_Response_str += " times.\n";
				result = 0;				
				break;
				
		case 113 : // num times deactivate
				description = "Vision System De-activated";
				num_activations  = sql_logger.form_response__how_many_times( 
									name, description, start_time_bd, end_time_bd );
				compose_time_period_qualitative( &start_time_bd, &end_time_bd, NLP_Response_str );
				NLP_Response_str += ", I was deactivated ";
				NLP_Response_str += std::to_string(num_activations);
				NLP_Response_str += " times.\n";
				result = 0;				
				break;
		case 200 : 	/* Show me the picture ____?  (particular person at a particular time) */
		
				result=0;
				break;
		case 201 : 	/* Who did you meet today?  */
		
				result=0;
				break;

		case 202 : 	/* shutdown */
			result=0;
				break;
				
		default  : 	
				break;
    }    
}


// Next time you see David...
int Get_Future_Statement_symantic_id(Sentence& theSentence)
{
	int retval=0;
	bool conditional = (theSentence.is_found_in_sentence("when") || theSentence.is_found_in_sentence("if"));
    bool did_see   = (theSentence.are_found_in_sentence( "you" ) && 
    		  	      theSentence.are_found_in_sentence( "see" ) );
	if (conditional && did_see)
		retval = 401;	
		
	return retval;
}

void process_future_person_query(int future_action_request)
{
	int result = -1;
    switch (future_action_request)
    {
		case 100 : 	/* (next time|If you) see ____, tell him _____  [future] */
					
				result=0;
				break;

		case 101 : 	/* record video, next time you see ___. */
					
				result=0;
				break;

		case 102 : 	/* text me, next time you see ___. */
					
				result=0;
				break;
		default : break;
    }
    return result;
}

/*****************************************************************
Do the work of the Telegram :
PARAMS : 
	mSentence 		- pointer to a buffer which begins with text (data may follow end of string).
	mbegin_index	- index within the mSentence buffer (up to 5MB)

Return :   Pointer to next part of the telegram (mSentence).
 
//return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
char* Parse_Statement( char*  mSentence, ServerHandler* mh )
{	
	if (mSentence==NULL) return mSentence; 
	printf( "Sentence:|%s|\n", mSentence );
    Sentence  theSentence( mSentence );
	theSentence.prefilter_text();
 	char* end_of_telegram = mSentence + strlen(mSentence) +1 /*nullterminator*/;	
 	
	int   result = -1;
    bool  vr 	 = theSentence.is_voice_response();
	if (vr)
	{
		printf("Prefilter determined a VoiceResponse!\n");
		return end_of_telegram;
	}
	if (strcmp(mSentence, "I don't understand. Ignoring.")==0)
		return end_of_telegram;

    struct tm Time;
    int result2 = parse_qualitative_2_time( theSentence, start_time_bd, end_time_bd );    

    int question_id = Get_Statement_symantic_id(theSentence);
    process_person_query(question_id);
    
    int future_action_request = Get_Future_Statement_symantic_id(theSentence);
	process_future_person_query(future_action_request);

	if (result==-1)
		result = Parse_color_statement( theSentence, NLP_Response_str );

	char* end_ptr = Parse_vision_things_Statement( char*  mSentence, ServerHandler* mh );
	
		
	// Not handled:
	if (result==-1)
	    NLP_Response_str = "I don't understand. Ignoring.";
	
	if (mh)
		mh->form_response( NLP_Response_str.c_str() );	    
	return end_of_telegram;
}

//result = Parse_Camera_Statement( mSentence, mh );
//if (result>=0)			return (end_of_telegram + result);		
//result = Parse_HMI_Statement   ( mSentence, mh ); 	/* ie mouse, keyboard, PS3 controller, etc. */
//if (result>=0)          return (end_of_telegram + result);
