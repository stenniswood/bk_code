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
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include "bk_system_defs.h"
#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.hpp"
#include "HMI_protocol.hpp"
//#include "thread_control.h"
//#include "hmi_thread.h"
#include "nlp_extraction.hpp"

/* Sample Sentences:
	receive my [mouse/touch/[and]keyboard] input
	receive my mouse input
	
	send me your [mouse/touch/keyboard]
*/

static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;


static void init_subject_list()
{
	subject_list.push_back( "mouse"	 		);
	subject_list.push_back( "keyboard"		);	
	subject_list.push_back( "touch" 		);
	subject_list.push_back( "controller" 	);
	subject_list.push_back( "pendant" 		);
	subject_list.push_back( "capabilities" 	);
}

static void init_verb_list()
{
	verb_list.push_back( "open" 	);	
	verb_list.push_back( "route" 	);
	verb_list.push_back( "incoming" );
	verb_list.push_back( "send" 	);
	verb_list.push_back( "sending" 	);
	
	verb_list.push_back( "close" 	 );
	verb_list.push_back( "stop" 	 );	
	verb_list.push_back( "end" 		 );
	verb_list.push_back( "terminate" );
	verb_list.push_back( "kill" 	 );			

	verb_list.push_back( "are" );	
	verb_list.push_back( "have" );
	verb_list.push_back( "get" );	
	verb_list.push_back( "set" );	
	verb_list.push_back( "I am" );		
}


static void init_adjective_list()
{ 
	adjective_list.push_back( "highest" );
	adjective_list.push_back( "my" 		);
	adjective_list.push_back( "your" 	);
	adjective_list.push_back( "low" 	);
	adjective_list.push_back( "lowest"	);
	adjective_list.push_back( "best" 	);
	adjective_list.push_back( "quality" );
	adjective_list.push_back( "stereo"  );
}

static void init_object_list()
{   // Object might be a numerical value preceded by a preposition.
	// ie. "set camera tilt _to_ _25.5 degrees"
	// prepositions to look for :
	//		to by as 
	object_list.push_back( "tv" 	  	);		
	object_list.push_back( "hdmi"	  	);
	object_list.push_back( "speaker"	);
	object_list.push_back( "headphones" );
	object_list.push_back( "you" 		);	
	object_list.push_back( "me"			);
}

static void init_word_lists()
{
	init_subject_list();
	init_verb_list();
	init_object_list();
}

/*****************************************************************
Initialize internal data structures for CAN protocol (protocol only, 
not all the CAN driving code):
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Init_HMI_Protocol()
{		
	init_word_lists();
}

/* ACTION FUNCTIONS HERE */


/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
int Parse_HMI_Statement(char* mSentence)
{
	printf("Parse_HMI_Statement\n");
	int retval = -1;	
	
	std::string* subject  	= extract_word( mSentence, &subject_list );
	std::string* verb 		= extract_word( mSentence, &verb_list 	 );
	std::string* object 	= extract_word( mSentence, &object_list  );
	std::string* adjective	= extract_word( mSentence, &adjective_list  );	

	if ((compare_word(subject, "mouse")==0) ||
		(compare_word(subject, "touch")==0) ||
		(compare_word(subject, "keyboard")==0) ||
		(compare_word(subject, "controller")==0))
	{
		//printf("Processing hmi telegram. verb=%s\n", verb->c_str());
		if ((compare_word(verb, "upload") ==0) ||
		    (compare_word(verb, "transfer") ==0)   ||
		    (compare_word(verb, "incoming") ==0)   ||
		    (compare_word(verb, "receive") ==0)	 ||
		    (compare_word(verb, "save") ==0))
		{
			//float result = atof(mObject->name.c_str());
			// Maybe want to verify the source IP address for security purposes
			// later on.  Not necessary now!
			printf( "Listening for incoming HMI...\n");
			
			retval = 0;
		}

		if ( (compare_word(verb,   "send") ==0) && // preposition "to"
			 (compare_word(object, "you") ==0) )
		{
			printf( "Listening for HMI...\n");
			
			retval = 0;			
		}	

		if ((compare_word(verb, "close") ==0) ||
		    (compare_word(verb, "stop" ) ==0)  ||
		    (compare_word(verb, "end"  ) ==0)  ||		    
		    (compare_word(verb, "terminate") ==0)  ||		    		    
		    (compare_word(verb, "kill" ) ==0))
			{
				 //hmi_terminate_requested = TRUE; 
				 retval=0;				 
			}
	}
    if (retval>-1)  printf("Parse_HMI_Statement - done\n");    
	return retval;
}


/* The need for an FILE telegram is questionable.  We could have the client
	send us the image as a file.  Then push some HTML code which this would 
	open a window and display.  Since a "live" image would really be a live
	video.  What we might be looking for is:  requesting the robot to send us
	a snapshot.  We send a message to the robot's server side with a token.
	The server responds by sending us the picture and html.

	Now take the perspective that this is the robot.  We need to accept a token 
	REQ_FILE_LIVE and respond with CMD_FILE_TRANSFER (picture & html), 
	CMD_SHOW_HTML_FILE.
	
	Notice this wouldn't work with audio/video since it is continual.
*/
