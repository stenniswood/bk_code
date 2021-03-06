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
#include <list>

#include "pican_defines.h"
#include "CAN_Interface.hpp"
#include "serverthread.hpp"
//#include "visual_memory.h"
#include "sway_memory.h"
#include "audio_memory.h"
#include "picamscan_memory.h"
#include "client_memory.hpp"
#include "CAN_memory.h"
#include "udp_transponder.hpp"
#include "GENERAL_protocol.hpp"
#include "client_to_socket.hpp"

#include "CAN_protocol.hpp"
#include "AUDIO_protocol.hpp"
#include "CAMERA_protocol.hpp"
#include "math_protocol2.hpp"
#include "self_identity.hpp"

#include "nlp_extraction.hpp"
#include "prefilter.hpp"
//#include "client.hpp"


using namespace std;

//#define NLP_DEBUG 1
bool connection_established = false; // replace this with ServerHandler info.
bool ClientRequestPending   = false;
char* CLIENT_Response       = NULL;

// These should be classes with synonyms:
static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
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
	/* These should be a composite of all verbs in all protocols!!
		Perhaps an automated way to do so.	
	*/
	verb_list.push_back( "open" );	
	verb_list.push_back( "route" );
	verb_list.push_back( "incoming" );
	verb_list.push_back( "send" );
	verb_list.push_back( "receive" );
	verb_list.push_back( "mute" );
	verb_list.push_back( "unmute" );
	verb_list.push_back( "silence"  );
	verb_list.push_back( "unsilence");			
	
	verb_list.push_back( "connect" );	
	verb_list.push_back( "disconnect" );

	verb_list.push_back( "close" 	 );
	verb_list.push_back( "stop" 	 );	
	verb_list.push_back( "end" 		 );
	verb_list.push_back( "terminate" );
	verb_list.push_back( "kill" 	 );			

	// camera related:	
	verb_list.push_back( "view" );				
	verb_list.push_back( "watch");			
	verb_list.push_back( "show" );
	verb_list.push_back( "spy"  );
	// audio related:	
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
	init_adjective_list();
	init_preposition_list();
}

void init_client_request()
{
	init_word_lists();
}

static std::string* subject = NULL;
static std::string* verb    = NULL;
static std::string* object  = NULL;
static std::string* adjective=NULL;
static std::string* preposition=NULL;

void disconnect_from_robot()
{
    connection_established = false;
    // Put back in when way to pass connfd is worked out.
    //close(connfd);
}

/* Note the client can do any of these:
	establish a connection
	audio  						(send and/or receive)
	file transfer				
	image transfer				
	HMI							
	CAN							
*/
void handle_client_request(ServerHandler* mh)
{
    char  relay_buffer[255];
    size_t   length=0;

	printf("handle_client_request() - ");
	if (ipc_memory_client==NULL)	return;
	ClientRequestPending = true;
    char* sentence = ipc_memory_client->Sentence;
    printf( "Client Sentence:|%s|\n", sentence );
    
    Sentence theSentence(sentence);
    if (theSentence.is_voice_response())  return ;      // no action!
    
    // Sjt Put back in when way of passing bytes_rxd,socket_buffer is available!
    //Parse_Statement( sentence, bytes_rxd, socket_buffer );        // GENERAL Protocol
    
    bool found = theSentence.is_found_in_sentence( "text" );
	//result = compare_word(subject, "text");
	if (found)
	{
		// Pass the request to the other end:
		strcpy (relay_buffer, ipc_memory_client->Sentence );
		length = strlen(relay_buffer);
		mh->SendTelegram( (BYTE*)relay_buffer, (int)length);
	}

   found= theSentence.is_found_in_sentence( "disconnect" );
//	result = compare_word(verb, "disconnect");
    if (found) {
		disconnect_from_robot( );
        mh->form_response("okay, disconnected");
    }
   found= theSentence.is_found_in_sentence( "sequencer" );
	if ( found )
	{
		// Pass the request to the other end:
		strcpy (relay_buffer, ipc_memory_client->Sentence );
		length = strlen(relay_buffer);
		mh->SendTelegram( (BYTE*)relay_buffer, (int)length);
	}
    
   found= theSentence.is_found_in_sentence( "connect" );
//	result = compare_word(verb, "connect");
	if (found) {
		printf("connecting..\n");
		char* space = strchr(ipc_memory_client->Sentence, ' ');
		if (space==NULL)  {  printf("space returning\n"); return;	};
		space++;

		REQUEST_client_connect_to_robot = TRUE;		// signal to serverthread.c
		REQUESTED_client_ip = space;				// ip address

		// Wait for the connection to happen.  want to acknowledge to client!
		while (REQUEST_client_connect_to_robot)		
		{};	// during this the socket msg "VR:Welcome" comes in.
		
		if (!connection_established)
		{
			// we can't get to it
			ClientRequestPending = false;	// don't want to wait for a response from other end.
			CLIENT_Response      = "No connection established!";
			// Positive response will come from the other end, in General_Protocol.
		} else {
			ClientRequestPending = false;	// don't want to wait for a response from other end.
			CLIENT_Response      = "Connected Welcome!";
		}
	}
   found= theSentence.is_found_in_sentence( "receive" );
//	result = compare_word(verb, "receive");
	if (found)
	{
		if (!connection_established)
		{
			// The request comes thru IPC Client memory.  So the response will be
			// placed there as well.
			CLIENT_Response= "No Connection!";			
			//printf("receive but no connection\n");
			//  will be done at end of this function!
		} 
		else 
		{
           found= theSentence.is_found_in_sentence( "can" );
	//	    result = compare_word(subject, "can");
		    if (found)
		    {
				// No amon at this end.
				int attached = can_connect_shared_memory(TRUE);	// allocate if not already.
                if (attached) {
                    CAN_ListeningOn   = TRUE;
                    set_tcp_receiving_flag_ipc_can();
        
                    printf("Requesting data... (ie. 'send can')\n");
                    
                    strcpy (relay_buffer, "send CAN");
                    size_t length = strlen( relay_buffer );
                    mh->SendTelegram( (BYTE*)relay_buffer, (int)length );
                }
			}
			found = theSentence.is_found_in_sentence( "audio" );
//			result = compare_word(subject, "audio");
			if (found)
			{
				// okay receive audio has been requested by client,			
				audio_listen(mh);		// AUDIO_protocol.c
	
				// Pass the request to the other end:
				strcpy (relay_buffer, "send audio to me");
				printf("Relaying: %s\n", relay_buffer);
				length = strlen(relay_buffer);
				mh->SendTelegram( (BYTE*)relay_buffer, length);
					
			}
			found = theSentence.is_found_in_sentence( "camera" );
//			result = compare_word(subject, "camera");
			if (found)
			{
				// okay receive audio has been requested by client,			
				camera_watch(mh);		// AUDIO_interface.c
	
				// Pass the request to the other end:
				strcpy (relay_buffer, "send camera to me");
				length = strlen(relay_buffer);
				mh->SendTelegram( (BYTE*)relay_buffer, length);
			
		    }
           found= theSentence.is_found_in_sentence( "file" );
//		    result = compare_word(subject, "file");
		    if (found)
		    {
				// Pass the request to the other end:
				strcpy (relay_buffer, "send file:  ");
				strcat (relay_buffer, "filename");
				length = strlen(relay_buffer);
				mh->SendTelegram( (BYTE*)relay_buffer, length);
		    }
		}
	}

	int result = ((compare_word(subject, "camera")==0) ||
		      (compare_word( subject, "web cam")==0)  ||
		      (compare_word( subject, "eyes")==0) );
	if (result)
	{
		if (!connection_established)
		{
			cli_ack_update_status();
			CLIENT_Response= "No Connection!";
			cli_ipc_write_response( CLIENT_Response, "instant" );
			ClientRequestPending= false;
			return;
		} 
	
		int verb_result = ((compare_word(verb, "receive") ==0) ||
							(compare_word(verb, "watch") ==0)  ||
							(compare_word(verb, "view") ==0)   ||
							(compare_word(verb, "spy") ==0)	   ||
							(compare_word(verb, "show") ==0)   ||
							(compare_word(verb, "send") ==0) );
		if (verb_result)
		{
			int cond_1 = ((compare_word(adjective, "my") ==0) ||
		  				  ((compare_word(preposition, "to")==0) && (compare_word(object, "you")==0)) );
			int cond_2 = ((compare_word(adjective, "your") ==0) ||
		  				  ((compare_word(preposition, "to")==0) && (compare_word(object, "me")==0)) );		
			if (cond_1)
			{
				send_camera(mh);
				strcpy (relay_buffer, "sending camera to you.");
				length = strlen(relay_buffer);
				mh->SendTelegram( (BYTE*)relay_buffer, length);
			}
			else if (cond_2)
			{
				strcpy 	   (relay_buffer, "send camera to me.");
				length = strlen(relay_buffer);
				mh->SendTelegram( (BYTE*)relay_buffer, length);
			}
		}
	}
	
	// 
	result = compare_word(verb, "mute");
	if (result==0)
	{
		result = compare_word(subject, "audio");
		if (result==0)
		{
			// Pass the request to the other end:
			strcpy (relay_buffer, ipc_memory_client->Sentence);
			length = strlen(relay_buffer);
			mh->SendTelegram( (BYTE*)relay_buffer, length);
		}
	}
	// 
	result = compare_word(verb, "unmute");
	if (result==0)
	{
		result = compare_word(subject, "audio");			
		if (result==0)
		{	
			// Pass the request to the other end:
			strcpy (relay_buffer, ipc_memory_client->Sentence);
			length = strlen(relay_buffer);
			mh->SendTelegram( (BYTE*)relay_buffer, length);
		}
	}
	
	// Remember this is this end.  So "send audio" means for
	// me to start transmitting audio over tcpip to some other end.
	result = compare_word(verb, "send");	
	if (result==0)
	{
        if (!connection_established)
        {
            // The request comes thru IPC Client memory.  So the response will be
            // placed there as well.
            CLIENT_Response= "No Connection!";
            //printf("receive but no connection\n");
            //  will be done at end of this function!
        } 
        else {
		result = compare_word(subject, "can");			
		if (result==0)
		{
			/* "send can" spoken to this local client - means:		*/
			start_amon();
			printf("sending..CAN\n");
			// this is the "token" to indicate can messages are coming.

            strcpy (relay_buffer, "receive CAN");
            size_t length = strlen(relay_buffer);
            mh->SendTelegram( (BYTE*)relay_buffer, (int)length);
            //Cmd_client_CAN_Start();		// in core/wifi/client.c
		}
		result = compare_word(subject, "audio");
		if (result==0)
		{
		    send_audio(mh);
			// Pass the request to the other end:
			strcpy (relay_buffer, "receive audio ");
			length = strlen(relay_buffer);
			mh->SendTelegram( (BYTE*)relay_buffer, length);
		    
		}
		result = compare_word(subject, "camera");			
		if (result==0)
		{
		    send_camera(mh);
		    
			// Pass the request to the other end:
			strcpy (relay_buffer, "receive camera ");
			length = strlen(relay_buffer);
			mh->SendTelegram( (BYTE*)relay_buffer, length);
		}
		result = compare_word(subject, "file");			
		if (result==0)
		{
				// Pass the request to the other end:
				strcpy (relay_buffer, "send file:  ");
				strcat (relay_buffer, "filename");
				length = strlen(relay_buffer);
				mh->SendTelegram( (BYTE*)relay_buffer, length);

		}
		result = compare_word(subject, "mouse");			
		if (result==0)
		{
		}
		result = compare_word(subject, "keyboard");
		if (result==0)
		{
        }
        }
        
	}

	result = compare_word(verb, "stop");
	if (result==0)
	{
		printf("stoping..\n");
		result = compare_word(subject, "can");			
		if (result==0)
		{
			//char coBuff[127];
			strcpy ((char*)relay_buffer, "stop CAN");
			length = strlen( (char*)relay_buffer );
			mh->SendTelegram( (BYTE*)relay_buffer, length);
			CAN_SendingOn = FALSE;
			CAN_ListeningOn = FALSE;			
			clear_tcp_transmitting_flag_ipc_can();
			clear_tcp_receiving_flag_ipc_can();		
		}
		result = compare_word(subject, "audio");
		if (result==0)
		{
		    audio_cancel(mh);
			// Pass the request to the other end:
			strcpy (relay_buffer, "stop audio ");
			length = strlen(relay_buffer);
			mh->SendTelegram( (BYTE*)relay_buffer, length);
		    
		}
		result = compare_word(subject, "camera");			
		if (result==0)
		{
			CAMERA_tcpip_SendingOn = FALSE;
			CAMERA_tcpip_WatchingOn = FALSE;

			// Pass the request to the other end:
			strcpy (relay_buffer, "stop camera ");
			length = strlen(relay_buffer);
			mh->SendTelegram( (BYTE*)relay_buffer, length);
		}
		result = compare_word(subject, "file");			
		if (result==0)
		{
		}
		result = compare_word(subject, "mouse");			
		if (result==0)
		{
		}
		result = compare_word(subject, "keyboard");
		if (result==0)
		{
		}	
	}

	// We came in here because we had a fresh update.  So now close it out:
	cli_ack_update_status();
	if (CLIENT_Response)
	{
		cli_ipc_write_response( CLIENT_Response, "instant" );
		ClientRequestPending= false;
	}
	printf("handle_client_request() done \n");		
		
}




