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
#include "protocol.h"
#include "devices.h"
#include "utilities.h"
#include "package_commands.h"
#include "GENERAL_protocol.h"
#include "AUDIO_protocol.h"
#include "audio_thread.h"
#include "thread_control.h"

/* Suggested Statements:

	start 2 way audio to me.
	I want to [hear/listen to] your microphone.
	Listen to my [microphone/music/audio].
	I want to listen [in].					// pre-stage should add the implied (your microphone)
	I want to eavesdrop.

	Open audio connection to the bedroom. Listen to the baby.
	Open audio connection to the bedroom. play my voice on cue.
	[let them] Listen to me.
	
	send me your audio.			[subject=audio][verb=send][object=me]
	send your audio to me.		[subject=audio][verb=send][object=me]
		
	All of the above result in 1 of 3 actions:
		[create_audio_thread, create_audio_tx_thread, or both]
		
	show me a power meter.
	What are your audio capabilities?
	How many microphones do you have?
	How many speakers do you have?
	stereo

*/

/***********************************************************************
All Incoming Audio will also be saved to the hard-drive on those 
systems with enough capacity.  This allows the user to call back anything
in its memory banks.  Think star trek logs.  "There was an incoming 
message from so and so on October 31 2006".  Can you replay it?  
What was the topic of it?  How long was it?  A smart robot could even
use the info to answer questions.  On Sept 20th, you said you wanted to
ignore all upgrade notices.
***********************************************************************/
char AUDIO_PATH[] 			= "./AUDIO/";
int  transfer_size 			= 3844;			// default
int  audio_buffers_received = 0;


static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string> 	preposition_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;


static void init_subject_list()
{
	subject_list.push_back( "audio"	 		);
	subject_list.push_back( "connection"	);	
	subject_list.push_back( "microphone" 	);
	subject_list.push_back( "recording" 	);
	subject_list.push_back( "music" 		);
	subject_list.push_back( "capabilities" 	);
}

static void init_verb_list()
{
	verb_list.push_back( "open" );	
	verb_list.push_back( "route" );
	verb_list.push_back( "incoming" );
	verb_list.push_back( "send" );

	verb_list.push_back( "close" 	 );
	verb_list.push_back( "stop" 	 );	
	verb_list.push_back( "end" 		 );
	verb_list.push_back( "terminate" );
	verb_list.push_back( "kill" 	 );			
	
	verb_list.push_back( "hear" 	 );			
	verb_list.push_back( "listen" 	 );
	verb_list.push_back( "eavesdrop" );
	
	verb_list.push_back( "synthesize" );
	verb_list.push_back( "are" );	
	verb_list.push_back( "have" );
	verb_list.push_back( "get" );	
	verb_list.push_back( "set" );	
	verb_list.push_back( "I am" );		
}

static void init_preposition_list()
{   // Object might be a numerical value preceded by a preposition.
	// ie. "set camera tilt _to_ _25.5 degrees"
	// prepositions to look for :
	//		to by as 
	preposition_list.push_back( "to" );
	preposition_list.push_back( "from" );	
	preposition_list.push_back( "as" );	
	preposition_list.push_back( "by" );		
	preposition_list.push_back( "for");
	preposition_list.push_back( "in" );
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
	init_adjective_list();
	init_object_list();
	init_preposition_list();
}

/*****************************************************************
Initialize internal data structures for CAN protocol (protocol only, 
not all the CAN driving code):
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Init_Audio_Protocol()
{		
	audio_buffers_received = 0;
	init_word_lists();
}


/* 3 possible actions:  */
void audio_listen(BOOL Play=TRUE, BOOL Save=FALSE)
{
	//BOOL Play = TRUE;
	//BOOL Save = TRUE;
	
	// Maybe want to verify the source IP address for security purposes
	// later on.  Not necessary now!
	printf( "Listening for incoming audio...\n");
	create_audio_thread( Play,  Save );
	
	nlp_reply_formulated=TRUE;
	strcpy(NLP_Response, "Okay, I'm listening for your audio");
	
}
void send_audio()
{
	// Maybe want to verify the source IP address for security purposes
	// later on.  Not necessary now!
	printf( "Sending my incoming audio...\n");

	// Fill in later...!  WaveHeader struct
	char* header=NULL;
	create_audio_tx_thread( header );

	nlp_reply_formulated=TRUE;
	strcpy(NLP_Response, "Okay, I am attempting to send you my audio.");
}
void audio_cancel()
{
	printf( "Cancelling audio connection.\n");
	audio_terminate_requested = TRUE;
	nlp_reply_formulated      = TRUE;
	strcpy(NLP_Response, "Okay, I am terminating our audio connection.");
	printf( "Cancel initiated.\n");
}
void audio_two_way()
{
	printf( "Opening 2 way audio...\n");
	create_audio_thread( FALSE,  TRUE );
	// add echo cancelation algorithm here.

	// Fill in with WaveHeader struct !
	char* header=NULL;
	create_audio_tx_thread( header );

	nlp_reply_formulated=TRUE;
	strcpy(NLP_Response, "Okay, we'll both hear each other now.");
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
BOOL Parse_Audio_Statement( char* mSentence )	
{
	int retval = FALSE;
	std::string* subject  	= extract_word( mSentence, &subject_list );
	if (subject==NULL) return FALSE;  // subject matter must pertain.
	printf("Parse_Audio_Statement\n");
	
	std::string* verb 		= extract_word( mSentence, &verb_list 	 );
	if (verb==NULL)
	{	printf("Reply:  What do you want me to do with %s\n", subject->c_str());
		return FALSE;
	}

	std::string* object 	= extract_word( mSentence, &object_list  );
	std::string* adjective	= extract_word( mSentence, &adjective_list  );	
	//int prepos_index      = get_preposition_index( mSentence );

	diagram_sentence(subject, verb, adjective, object );

	if ((strcmp( subject->c_str(), "audio")==0) ||
		(strcmp( subject->c_str(), "microphone")==0) ||
		(strcmp( subject->c_str(), "recording")==0) ||
		(strcmp( subject->c_str(), "music")==0))
	{
			
		//printf("Processing audio telegram. verb=%s\n", verb->c_str());
		if ((strcmp(verb->c_str(), "upload") ==0) 	   ||
			(strcmp(verb->c_str(), "eavesdrop") ==0)  ||
		    (strcmp(verb->c_str(), "incoming") ==0) )
		{
			audio_listen();
			retval = TRUE;
		}
		if (strcmp(verb->c_str(), "send") ==0) 	
		{
			if (strcmp(object->c_str(), "me") ==0)
			{	send_audio();	retval = TRUE;	}				
			if (strcmp(object->c_str(), "you") ==0)
			{	audio_listen();		retval = TRUE;	}
		}
		if ((strcmp(verb->c_str(), "hear") ==0) ||
		    (strcmp(verb->c_str(), "listen") ==0))
		{
			if (strcmp(adjective->c_str(), "your") ==0)
			{	audio_listen();		retval = TRUE;	}
		}

		if ((strcmp(verb->c_str(), "close") ==0) ||
		    (strcmp(verb->c_str(), "stop" ) ==0)  ||
		    (strcmp(verb->c_str(), "end"  ) ==0)  ||		    
		    (strcmp(verb->c_str(), "terminate") ==0)  ||		    		    
		    (strcmp(verb->c_str(), "kill" ) ==0))
			{
				 audio_cancel(); 
				 retval = TRUE;
			}
	}
	//printf( "Parse_Audio_Statement almost done\n" );

	if ( (strcmp( subject->c_str(), "audio")==0) ||
		 (strcmp( subject->c_str(), "connection")==0))
	{
		if (object)  {
			if (strcmp( object->c_str(), "connection")==0)
			{	audio_two_way();		retval = TRUE;	}
		}
	}

	//printf( "Parse_Audio_Statement done\n" );	
	return retval;
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Formulate_AUDIO_Response(UINT mToken)
{
}


/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
BOOL Process_AUDIO_Telegrams( UINT  mToken, byte* mheader, int DataLength, int mconnfd )
{
	struct DEVICE_CAPS* dc;
	BOOL retval = FALSE;
	int samples = 0;
	float seconds = 0.0;
	int   osize = 0;
	char* fn = NULL;
	char* Name = NULL;
	
			
}



/* CMD_AUDIO_START	:	printf(" COMMAND OPEN AUDIO "			);	break;
 CMD_AUDIO_DATA		:	printf(" COMMAND AUDIO DATA " 			);	break;
 CMD_AUDIO_END 		:	printf(" COMMAND AUDIO END " 			);	break;
 REQ_AUDIO_CAPABILITIES: printf(" REQUEST AUDIO CAPABILITIES "	);	break;
 REQ_AUDIO_LIVE		:	printf(" REQUEST LIVE AUDIO "			);	break;		
 REQ_AUDIO_STOP		:	printf(" REQUEST LIVE AUDIO STOP "		);	break;		
 REQ_AUDIO_TIMESTAMP:	printf(" REQUEST AUDIO FROM TIMESTAMP"	);	break;		
 REQ_AUDIO_FILE		:	printf(" REQUEST AUDIO FROM FILE "		);	break; */
