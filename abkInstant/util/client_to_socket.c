/*************************************************************************
  CLIENT to SOCKET Translation
   Accepts Client commands and passes onto the other end server
   when necessary.
 *************************************************************************/
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string>
#include <vector>
#include <pthread.h>
#include "pican_defines.h"
#include "CAN_Interface.hpp"
#include "can_txbuff.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "catagorization.h"
#include "serverthread.h"
#include "client.h"
#include "visual_memory.h"
#include "sway_memory.h"
#include "audio_memory.h"
#include "picamscan_memory.h"
#include "client_memory.hpp"
#include "CAN_memory.h"
#include "udp_transponder.hpp"
#include "GENERAL_protocol.h"
#include "client_to_socket.h"

#include "CAN_protocol.h"
#include "AUDIO_protocol.h"
#include "CAMERA_protocol.h"

//#define NLP_DEBUG 1

// These should be classes with synonyms:
static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string> 	preposition_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;

static void init_subject_list()
{
	subject_list.push_back( "whoami" 	);
	subject_list.push_back( "list"	 	);
	subject_list.push_back( "text" 	);
	subject_list.push_back( "can" 	);
	subject_list.push_back( "audio" 	);		
	subject_list.push_back( "camera" 	);
	subject_list.push_back( "file" 	);
	subject_list.push_back( "mouse" 	);
	subject_list.push_back( "keyboard" );
}

static void init_verb_list()
{
	verb_list.push_back( "open" );	
	verb_list.push_back( "route" );
	verb_list.push_back( "incoming" );
	verb_list.push_back( "send" );
	
	verb_list.push_back( "connect" );	
	verb_list.push_back( "disconnect" );

	verb_list.push_back( "close" 	 );
	verb_list.push_back( "stop" 	 );	
	verb_list.push_back( "end" 		 );
	verb_list.push_back( "terminate" );
	verb_list.push_back( "kill" 	 );			
	
	verb_list.push_back( "hear" 	 );			
	verb_list.push_back( "listen" 	 );
	verb_list.push_back( "eavesdrop" );
	
	verb_list.push_back( "are" );	
	verb_list.push_back( "have" );
	verb_list.push_back( "get" );	
	verb_list.push_back( "set" );	
	verb_list.push_back( "I am" );		
	verb_list.push_back( "how much" );			
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
	init_object_list();
	init_preposition_list();
}

std::string* subject = NULL;
std::string* verb = NULL;
std::string* object = NULL;
std::string* adjective=NULL;

void trim_trail_space( char* str )
{
	int   len = strlen(str)-1;
	char* ptr = &(str[len]);
	while(( len>0 ) && (*ptr==' '))
	{
		*ptr = 0;
		ptr--;  len--;
	}
}

BOOL extract_nlp_words()
{
    if (ipc_memory_client==NULL)  return;

    char* mSentence = ipc_memory_client->Sentence;    
    trim_trail_space( ipc_memory_client->Sentence );

    subject = extract_word( mSentence, &subject_list );
    if (subject==NULL)   // subject matter must pertain.
    {
		printf("Reply:  What do you want me to do with %s\n", subject->c_str());
		return FALSE;
    }

    verb 	= extract_word( mSentence, &verb_list );
    if (verb==NULL)
    {
		printf("Reply:  What do you want me to do with %s\n", subject->c_str());
		return FALSE;
    }

    object    = extract_word( mSentence, &object_list  );
    adjective = extract_word( mSentence, &adjective_list  );	
    diagram_sentence(subject, verb, adjective, object );
    return TRUE;    
}


/* Note the client can do any of these:
	establish a connection
	audio  						(send and/or receive)
	file transfer				
	image transfer				
	HMI							
	CAN							
*/
void handle_client_request()
{
    char relay_buffer[255];
    int length=0;
    
	if (ipc_memory_client==NULL)	return;
	printf("handle_client_request()\n");
	
 	printf ("Sentence:%s|\n", ipc_memory_client->Sentence );    
	int result = extract_nlp_words();
	if (result==FALSE) return;

	result = strcmp(subject->c_str(), "whoami");
	if (result==0)
		printf("whoami\n");

	result = strcmp(subject->c_str(), "list");
	if (result==0)
		printf("list\n");

	result = strcmp(subject->c_str(), "text");
	if (result==0)
	{
			// Pass the request to the other end:
			strcpy (relay_buffer, ipc_memory_client->Sentence );
			length = strlen(relay_buffer);
			SendTelegram( relay_buffer, length);
	}
	
	result = strcmp(verb->c_str(), "disconnect");
	if (result==0)
		result = disconnect_from_robot( );

	// AFTER THIS BEGIN 2+ WORD COMMANDS:
	char* space = strchr(ipc_memory_client->Sentence, ' ');
	if (space==NULL)  {  return;	};
	*space = 0;
	space++;

	if ( (strcmp( subject->c_str(), "sequencer")==0) )
	{
		// Pass the request to the other end:
		strcpy (relay_buffer, ipc_memory_client->Sentence );
		length = strlen(relay_buffer);
		SendTelegram( relay_buffer, length);
	}

	result = strcmp(verb->c_str(), "connect");
	if (result==0) {
		printf("connecting..\n");
		REQUEST_client_connect_to_robot = TRUE;		// signal to serverthread.c
		REQUESTED_client_ip = space;				// ip address
	}
	result = strcmp(verb->c_str(), "receive");
	if (result==0)
	{
		if (!connection_established)
		{
			// The request comes thru IPC Client memory.  So the response will be
			// placed there as well.
			printf("No Connection!!\n\n");
			return;
		} 
		else 
		{
		    result = strcmp(subject->c_str(), "can");			
		    if (result==0)
		    {
			// No amon at this end.
			int attached = can_connect_shared_memory(TRUE);	// allocate if not already.
			CAN_ListeningOn   = TRUE;
			set_tcp_receiving_flag_ipc_can();

			printf("Requesting data...");
			result = strcmp(space, "can");                  
			if (result==0)
			{  
			    printf(" (ie. 'send can')\n");
			    Cmd_client_CAN_listen();  // request for other end to send CAN.  in core/wifi$
			}
		    }
		    result = strcmp(subject->c_str(), "audio");
		    if (result==0)
		    {
			// okay receive audio has been requested by client,			
			audio_listen();		// AUDIO_protocol.c

			// Pass the request to the other end:
			strcpy (relay_buffer, "send audio to me");
			length = strlen(relay_buffer);
			SendTelegram( relay_buffer, length);
		    }
		    result = strcmp(subject->c_str(), "camera");			
		    if (result==0)
		    {
			// okay receive audio has been requested by client,			
			camera_watch();		// AUDIO_interface.c

			// Pass the request to the other end:
			strcpy (relay_buffer, "send camera to me");
			length = strlen(relay_buffer);
			SendTelegram( relay_buffer, length);
			
		    }
		    result = strcmp(subject->c_str(), "file");			
		    if (result==0)
		    {
			send_file_transmit_request();
		    }
		
		}
	}

	// Remember this is this end.  So "send audio" means for
	// me to start transmitting audio over tcpip to some other end.
	result = strcmp(verb->c_str(), "send");	
	if (result==0)
	{
		result = strcmp(subject->c_str(), "can");			
		if (result==0)
		{
			/* "send can" spoken to this local client - means:		*/
			start_amon();
			printf("sending..CAN\n");
			// this is the "token" to indicate can messages are coming.
			Cmd_client_CAN_Start();		// in core/wifi/client.c
		}
		result = strcmp(subject->c_str(), "audio");
		if (result==0)
		{
		    send_audio();
			// Pass the request to the other end:
			strcpy (relay_buffer, "receive audio ");
			length = strlen(relay_buffer);
			SendTelegram( relay_buffer, length);
		    
		}
		result = strcmp(subject->c_str(), "camera");			
		if (result==0)
		{
		    send_camera();
		    
			// Pass the request to the other end:
			strcpy (relay_buffer, "receive camera ");
			length = strlen(relay_buffer);
			SendTelegram( relay_buffer, length);
		}
		result = strcmp(subject->c_str(), "file");			
		if (result==0)
		{
			send_file_transmit_request();
		}
		result = strcmp(subject->c_str(), "mouse");			
		if (result==0)
		{
		}
		result = strcmp(subject->c_str(), "keyboard");
		if (result==0)
		{
		}		
	}

	result = strcmp(ipc_memory_client->Sentence, "stop");
	if (result==0)
	{
		printf("stoping..\n");
		result = strcmp(subject->c_str(), "can");			
		if (result==0)
		{
			//char coBuff[127];
			strcpy ((char*)relay_buffer, "stop CAN");
			length = strlen( (char*)relay_buffer );
			SendTelegram( relay_buffer, length);
			CAN_SendingOn = FALSE;
			CAN_ListeningOn = FALSE;			
			clear_tcp_transmitting_flag_ipc_can();
			clear_tcp_receiving_flag_ipc_can();		
		}
		result = strcmp(subject->c_str(), "audio");
		if (result==0)
		{
		    audio_cancel();
			// Pass the request to the other end:
			strcpy (relay_buffer, "stop audio ");
			length = strlen(relay_buffer);
			SendTelegram( relay_buffer, length);
		    
		}
		result = strcmp(subject->c_str(), "camera");			
		if (result==0)
		{
		    camera_cancel();
			// Pass the request to the other end:
			strcpy (relay_buffer, "stop camera ");
			length = strlen(relay_buffer);
			SendTelegram( relay_buffer, length);
		    
		}
		result = strcmp(subject->c_str(), "file");			
		if (result==0)
		{
		}
		result = strcmp(subject->c_str(), "mouse");			
		if (result==0)
		{
		}
		result = strcmp(subject->c_str(), "keyboard");
		if (result==0)
		{
		}	
	}
	printf("handle_client_request() done\n");	
}




