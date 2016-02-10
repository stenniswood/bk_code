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
#include "AUDIO_file_util.h"
#include "AUDIO_interface.h"
#include "AUDIO_device.h"

//#include "thread_control.h"
#include "nlp_extraction.hpp"

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
char AUDIO_PATH[] 		= "./AUDIO/";
int  transfer_size 			= 3844;			// default
int  audio_buffers_received 	= 0;

BOOL  AUDIO_tcpip_ListeningOn = FALSE;		
BOOL  AUDIO_tcpip_SendingOn  = FALSE;
BOOL  AUDIO_tcpip_SendingMuted  = FALSE;		// we send zerod out audio
BOOL  AUDIO_tcpip_ListeningSilenced  = FALSE;		// we do not play any incoming audio.
BOOL  AUDIO_save_requested = FALSE;			// incoming or outgoing? 

FILE* sending_audio_file_fd = NULL;


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
	subject_list.push_back( "buffer"	 	);	
}

static void init_verb_list()
{
	verb_list.push_back( "open"  );	
	verb_list.push_back( "route" );
	verb_list.push_back( "incoming");
	verb_list.push_back( "send"    );
	verb_list.push_back( "mute"    );
	verb_list.push_back( "unmute"   );
	verb_list.push_back( "silence"  );
	verb_list.push_back( "unsilence");			
	
	verb_list.push_back( "close" 	 );
	verb_list.push_back( "stop" 	 );	
	verb_list.push_back( "end" 		 );
	verb_list.push_back( "terminate" );
	verb_list.push_back( "kill" 	 );			
	
	verb_list.push_back( "hear" 	 );			
	verb_list.push_back( "listen" 	 );
	verb_list.push_back( "receive"  );
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
	// WordGroup prepositions ;
	// prepositions.add_word( "to" );
	// prepositions.add_word( "from" );
	
	
	
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
	adjective_list.push_back( "two way"  );
	adjective_list.push_back( "new"    );	
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
Show contents of buffer on the console.  Usefull for debugging.
*****************************************************************/
void DumpBuffer( BYTE* mbuff, int mlength)
{
	int i;
	printf("\nBuffer: %d bytes", mlength);
	for (i=0; i<mlength; i++)
	{
		if ((i%8)==0)
			printf("\n");
		printf(" %2x", *(mbuff+i) );
	}
	printf("\n");
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
	AUDIO_tcpip_ListeningOn = FALSE;		
	AUDIO_tcpip_SendingOn   = FALSE;
	init_word_lists();
}


/**** ACTION INITIATORS:    (4 possible actions) *****/
void send_audio_file ( char* mFilename )
{	
    sending_audio_file_fd  = fopen( mFilename, "r" );
    if (sending_audio_file_fd == NULL)
    {
	nlp_reply_formulated=TRUE;
	sprintf ( NLP_Response, "Sorry, the audio file %s does not exist!", mFilename );
	printf( NLP_Response );
	return;
    }
    
	//printf( "Sending audio file over tcpip...\n");	
	AUDIO_tcpip_SendingOn = TRUE;

	nlp_reply_formulated=TRUE;
	sprintf ( NLP_Response, "Okay, I'm sending the audio file %s.", mFilename );
	//printf( NLP_Response );
}

void send_audio()
{
	// Maybe want to verify the source IP address for security purposes
	// later on.  Not necessary now!
	//printf( "Sending my incoming audio over tcpip...\n");

	// Fill in later...!  WaveHeader struct
	char* header=NULL;
	AUDIO_tcpip_SendingOn = TRUE;

	nlp_reply_formulated=TRUE;
	strcpy(NLP_Response, "Okay, I will begin sending you my audio.");
}

void audio_listen(BOOL Save=FALSE)
{	
	// Fake out the other end, just verify the audio works:
	//play_api_test(22050, 16, 1, 0);
	int32_t result = audio_setup( 0, 22050, 1, 16 );
	
	
	//play_api_test(int samplerate, int bitdepth, int nchannels, int dest);
	
	//printf( "Listening for incoming tcpip audio...\n");
	AUDIO_tcpip_ListeningOn = TRUE;

	nlp_reply_formulated=TRUE;
	strcpy(NLP_Response, "Okay, I'm listening for your audio");
}

void audio_two_way()
{
	printf( "Opening 2 way audio...\n");
	// add echo cancelation algorithm here.

	// Fill in with WaveHeader struct !
	char* header=NULL;
	AUDIO_tcpip_SendingOn  = TRUE;
	AUDIO_tcpip_ListeningOn = TRUE;
	
	nlp_reply_formulated=TRUE;
	strcpy(NLP_Response, "Okay, we'll both hear each other now.");
	printf("%s\n", NLP_Response);
}

void audio_cancel()
{
	printf( "Cancelling audio connection.\n");
	//audio_terminate_requested = TRUE;
	AUDIO_tcpip_SendingOn = FALSE;
	AUDIO_tcpip_ListeningOn = FALSE;
	
	nlp_reply_formulated      = TRUE;
	strcpy(NLP_Response, "Okay, I am terminating our audio connection.");
	printf("%s\n", NLP_Response);
}


/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
BOOL Parse_Audio_Statement( char* mSentence )	
{
	BOOL retval = FALSE;
	std::string* subject  	= extract_word( mSentence, &subject_list );
	if (subject==NULL) return FALSE;  // subject matter must pertain.
	printf("Parse_Audio_Statement\n");
	
	std::string* verb 		= extract_word( mSentence, &verb_list 	 );
	std::string* object     = extract_word( mSentence, &object_list  );
	std::string* adjective  = extract_word( mSentence, &adjective_list );	
	std::string* preposition = extract_word( mSentence, &preposition_list );
	//int prepos_index     = get_preposition_index( mSentence );
	diagram_sentence(subject, verb, adjective, object, preposition );

	if ((compare_word( subject, "audio")==0) ||
		(compare_word( subject, "microphone")==0))
	{	    
	    //printf("Processing audio telegram. verb=%s\n", verb->c_str());
	    // implied subject (party being spoken to) is the other end (ie. sequencer from lcd rpi):
		if ((compare_word(verb, "upload"   ) ==0)  ||
		    (compare_word(verb, "eavesdrop") ==0)  ||
		    (compare_word(verb, "receive")==0) 	   ||
		    (compare_word(verb, "incoming" ) ==0) )
		{
			audio_listen();
			retval = TRUE;
		}
		if ((compare_word(verb, "hear") ==0) ||
		    (compare_word(verb, "listen") ==0))
		{
			if (compare_word(adjective, "your") ==0)
			{	audio_listen();		retval = TRUE;	}
		}
		if (compare_word(verb, "send") ==0)
		{
			//if (strcmp(object->c_str(), "me") ==0)
			{	send_audio();	retval = TRUE;	}
			//if (strcmp(object->c_str(), "you") ==0)
			//{	audio_listen();		retval = TRUE;	}
		}
		if ((compare_word(verb, "mute") ==0) )
		{
			
		    AUDIO_tcpip_SendingMuted = TRUE;
			retval = TRUE;		    

			nlp_reply_formulated = TRUE;
			strcpy (NLP_Response, "Muted.");			
		}
		if ((compare_word(verb, "unmute") ==0) )
		{
		    AUDIO_tcpip_SendingMuted = FALSE;
			retval = TRUE;		    

			nlp_reply_formulated = TRUE;
			strcpy (NLP_Response, "Unmuted.");			

		}
		if ((compare_word(verb, "silence") ==0) )
		{
		    AUDIO_tcpip_ListeningSilenced = TRUE;
			retval = TRUE;		    
			nlp_reply_formulated = TRUE;
			strcpy (NLP_Response, "Silenced.");			

		}
		if ((compare_word(verb, "unsilence") ==0) )
		{
		    AUDIO_tcpip_ListeningSilenced = FALSE;
		    retval = TRUE;
			nlp_reply_formulated = TRUE;
			strcpy (NLP_Response, "unSilenced.");			
		}
		
		if ((compare_word(verb, "close") ==0) ||
		    (compare_word(verb, "stop" ) ==0)  ||
		    (compare_word(verb, "end"  ) ==0)  ||		    
		    (compare_word(verb, "terminate") ==0)  ||		    		    
		    (compare_word(verb, "kill" ) ==0))
			{
				 audio_cancel(); 
				 retval = TRUE;
			}
	}
	if (compare_word( adjective, "two way")==0 )
	{
	    audio_two_way();
	    retval = TRUE;
	};

	if (compare_word( subject, "recording")==0)
	{
		if (compare_word(verb, "send") ==0) 	
		{
		    // Parse filename (somehow)
		    
		    // Query for time/date,  filename, request list
		    
		    if (compare_word(object, "me") ==0)
		    {
			send_audio_file("test.wav");
			retval = TRUE;
		    }
		}	    
	 }
	if ((compare_word( subject, "audio buffer")==0) &&
		( compare_word( adjective,  "new"  )==0))
	{
		BOOL handled = handle_audio_data( );
		retval = TRUE;	
	}
	if (compare_word( subject, "music")==0)
	{	        	    
		if (compare_word(verb, "send") ==0) 	
		{
		    // Parse filename (somehow)
		    
		    // Query for time/date,  filename, request list
		    if (compare_word(object, "me") ==0)
		    {
			send_audio_file("test.mp3");
			retval = TRUE;
		    }
		}
	}

	if ( (compare_word( subject, "audio")==0) ||
		 (compare_word( subject, "connection")==0))
	{
		if (object)  {
			if (compare_word( object, "connection")==0)
			{	audio_two_way();		retval = TRUE;	}
		}
	}

	printf( "Parse_Audio_Statement done\n" );	
	return retval;
}



/* CMD_AUDIO_START	:	printf(" COMMAND OPEN AUDIO "			);	break;
 CMD_AUDIO_DATA		:	printf(" COMMAND AUDIO DATA " 			);	break;
 CMD_AUDIO_END 		:	printf(" COMMAND AUDIO END " 			);	break;
 REQ_AUDIO_CAPABILITIES: printf(" REQUEST AUDIO CAPABILITIES "	);	break;
 REQ_AUDIO_LIVE		:	printf(" REQUEST LIVE AUDIO "			);	break;		
 REQ_AUDIO_STOP		:	printf(" REQUEST LIVE AUDIO STOP "		);	break;		
 REQ_AUDIO_TIMESTAMP:	printf(" REQUEST AUDIO FROM TIMESTAMP"	);	break;		
 REQ_AUDIO_FILE		:	printf(" REQUEST AUDIO FROM FILE "		);	break; */


