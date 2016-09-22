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
#include "nlp_vision_thing_protocol.hpp"
#include "color_protocol.hpp"
#include "tk_colors.h"


#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "image_proc_misc.hpp"

#define FALSE 0
#define TRUE  1
#define BOOL unsigned char

#include "serverthread.hpp"
#include "nlp_sentence.hpp"
#include "vision_logger.hpp"

using namespace std;

/*****************************************************************
 Do the work of the Telegram :
 return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
extern void*      m_verbal_focus;

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
	int num_times     = theSentence.regex_find( "how many " );
	int what_on_table = theSentence.regex_find( "what do you see on the table?" ); // any order.
	if (what_on_table)
		retval = 101;

	int way_clear = theSentence.regex_find( "is way clear?" ); 	
	if (way_clear)
		retval = 103;

	int distance_to_wall = theSentence.regex_find( "how far is the wall?" ); 
	if (distance_to_wall)
		retval = 105;

	int  objects = theSentence.regex_find( "(things|objects) on the table" );
	int  tables  = theSentence.regex_find( "tables" );	    
	if (num_times && objects) 
		retval = 107;
	if (num_times && tables) 
		retval = 109;	
	// 
	//exp = "show (me|the) "+color;
	string exp = "show red";
	int show_color = theSentence.regex_find( exp );
	if (show_color)
		retval = 110;

	exp = "find circles";
	int find_circle = theSentence.regex_find( exp );
	if (find_circle)
		retval = 112;

	exp = "find squares";
	int find_squares = theSentence.regex_find( exp );
	if (find_squares)
		retval = 113;

	exp = "find cubes";
	int find_cubes = theSentence.regex_find( exp );
	if (find_cubes)
		retval = 114;

	exp = "is (the )?door open";
	int door = theSentence.regex_find( exp );
	if (door)
		retval = 115;
	return retval;
}

int process_thing_request( Sentence& theSentence, int question_id, string& Response )
{
	int retval=-1;
	Color mMin;
	Color mMax;
	cv::Mat Image;	
	
    switch(question_id)
    {
		case 101 :	// what's on table?
				break;
		case 104 :  // way clear?
				break;
		case 105 :  // far is the wall? 
				break;
		case 110 : // show color
				Parse_color_statement( theSentence, Response );
				//filter_color         ( Image, mMin, mMax );		
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
    return retval;
}

// Next time you see David...
int Get_future_thing_statement_symantic_id(Sentence& theSentence)
{
	int retval=-1;
	bool conditional = (theSentence.is_found_in_sentence("when") || theSentence.is_found_in_sentence("if"));
	bool did_see     = (theSentence.are_found_in_sentence( "you" ) && 
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
char* Parse_vision_things_Statement( Sentence& theSentence, ServerHandler* mh )
{
    bool  vr 	= theSentence.is_voice_response();
 	const char* end_of_telegram = theSentence.m_sentence.data() + theSentence.m_sentence.length() +1 /*nullterminator*/;
	int   result 		  = -1;
	if (vr)
	{
		printf("Prefilter determined a VoiceResponse!\n");
		return (char*)end_of_telegram;
	}
	if (theSentence.m_sentence.compare( "I don't understand. Ignoring.")==0)
		return (char*)end_of_telegram;
    
    int question_id = Get_thing_symantic_id(theSentence);
    result = process_thing_request( theSentence, question_id, mh->NLP_Response );
    
    int future_action_request = Get_future_thing_statement_symantic_id(theSentence);
    result = process_future_thing_request( future_action_request );    

	if (result==-1)
		result = Parse_color_statement( theSentence, mh->NLP_Response );

	// Not handled:
	if (result==-1)
	    mh->NLP_Response="I don't understand. Ignoring.";
	return (char*)end_of_telegram;
}

