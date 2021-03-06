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
#include "FILE_protocol.hpp"
//#include "thread_control.h"
#include "nlp_extraction.hpp"

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
//using namespace std;
static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string>  	object_list;

static void init_subject_list()
{
	// Also need aliases (ie synonyms):
	subject_list.push_back( "file"		 );
	subject_list.push_back( "files"		 );
	subject_list.push_back( "directory"	 );
	subject_list.push_back( "path"		 );	

	subject_list.push_back( "tv"		 );		
	subject_list.push_back( "display"	 );	
	subject_list.push_back( "hdmi"		 );	
}

static void init_verb_list()
{
	verb_list.push_back( "login" 	);
	verb_list.push_back( "close" 	);
	verb_list.push_back( "power" 	);
	verb_list.push_back( "route" 	);
	verb_list.push_back( "incoming" );
	verb_list.push_back( "end" 		);
	verb_list.push_back( "send" 	);
	verb_list.push_back( "synthesize");
	verb_list.push_back( "press" 	);
	verb_list.push_back( "mouse" 	);
	verb_list.push_back( "are" 		);	
	verb_list.push_back( "have" 	);
	verb_list.push_back( "get" 		);	
	verb_list.push_back( "set" 		);	
	verb_list.push_back( "I am" 	);		
}

static void init_object_list()
{ // Object might be a numerical value preceded by a preposition.
	// ie. "set camera tilt _to_ _25.5 degrees"
	// prepositions to look for :
	//		to by as 
	object_list.push_back( "you" 	);
	object_list.push_back( "me" 	);
}
static void init_word_lists()
{
	init_verb_list   ();
	init_subject_list();
	init_object_list ();
}


/*****************************************************************
Initialize internal data structures for CAN protocol (protocol only, 
not all the CAN driving code):
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Init_FILE_Protocol()
{
	init_word_lists();
}

/* ACTION FUNCTIONS */
char* file_inform_client()
{ return NULL;
}
char* file_inform_user  ()
{ return NULL;
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
int Parse_File_Statement(const char* mSentence, ServerHandler* mh)
{
	int retval = -1;
	Dprintf("Parse_FILE_Statement\n");
	
	std::string* subject  	= extract_word( mSentence, &subject_list 	);
	std::string* verb 		= extract_word( mSentence, &verb_list 	 	);

    if ((compare_word(subject, "file"   ) ==0)      ||
        (compare_word(subject, "files")==0)     ||
        (compare_word(subject, "path")==0)      ||
        (compare_word(subject, "directory")==0))
	{
		//printf("Processing audio telegram. verb=%s\n", verb->c_str());
		if ((compare_word(verb, "upload") ==0) ||
		    (compare_word(verb, "transfer") ==0)   ||
		    (compare_word(verb, "incoming") ==0)   ||
		    (compare_word(verb, "receive") ==0)	 ||
		    (compare_word(verb, "show") ==0)	 ||		/* even a show will create the thread for listening */
		    (compare_word(verb, "save") ==0))
		{
			// Maybe want to verify the source IP address for security purposes
			// later on.  Not necessary now!
			printf( "Listening for incoming file...\n");
			//create_file_thread( FALSE,  TRUE, BASE_FILE_PORT );
			retval = 0;
		}
	}
	if (retval>-1)  printf("Parse_FILE_Statement done\n");
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
