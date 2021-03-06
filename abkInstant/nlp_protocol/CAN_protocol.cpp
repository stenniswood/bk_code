#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h> 
#include <string.h>
#include <string>
#include <list>

#include "protocol.h"
#include "devices.h"
#include "CAN_memory.h"
#include "CAN_util.h"
#include "GENERAL_protocol.hpp"
#include "CAMERA_device.hpp"
#include "CAN_protocol.hpp"
#include "nlp_extraction.hpp"
#include "prefilter.hpp"

#define Debug 0

BOOL CAN_ListeningOn;	// if true we will be receiving CAN Traffic.
BOOL CAN_SendingOn;		// if true we will be sending CAN Traffic.

static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;

//#define NLP_DEBUG 1



static void init_subject_list()
{
	subject_list.push_back( "can_message" 	);
	subject_list.push_back( "can"	 		);
	subject_list.push_back( "can data"		);
	subject_list.push_back( "can traffic" 	);
	subject_list.push_back( "capabilities" 	);
}

static void init_verb_list()
{
	verb_list.push_back( "open" );	
	verb_list.push_back( "route" );
	verb_list.push_back( "incoming" );
	verb_list.push_back( "receive" );
	verb_list.push_back( "send" );

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


/*static void init_adjective_list()
{ 
	adjective_list.push_back( "highest" );
	adjective_list.push_back( "my" 		);
	adjective_list.push_back( "your" 	);
	adjective_list.push_back( "low" 	);
	adjective_list.push_back( "lowest"	);
	adjective_list.push_back( "best" 	);
	adjective_list.push_back( "quality" );
	adjective_list.push_back( "stereo"  );
}*/

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
void Init_CAN_Protocol()
{
	init_word_lists();
}

//extern int start_amon();	// extern from instant_main.cpp
#if (PLATFORM==Mac)
static char amon_command[] = "~/bk_code/amonitor/amon";
#elif (PLATFORM==RPI)
static char amon_command[] = "sudo /home/pi/bk_code/amonitor/amon";
#elif (PLATFORM==linux_desktop)
static char amon_command[] = "sudo /home/steve/bk_code/amonitor/amon";
#endif

int start_amon()
{
    int pid;
    switch (pid=fork()) {
        case -1:
			printf("fork() = -1 %s\n", strerror(errno) );
            return 0;
        case 0:
            execvp(amon_command, NULL);
            printf("returned from ececvp\n");
        default:
            return 0;
    }
    return 1;
}


//extern void dump_buffer(BYTE* buffer, int bufSize);

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
		
return:	pointer to the next telegram (ie. after all our header and data bytes)
		this will be null if end of the received buffer (terminator added in serverthread.c
		by the number of bytes read).
*****************************************************************/
int Parse_CAN_Statement( Sentence& theSentence, ServerHandler* mh )
{
	if (Debug) printf("Parse_CAN_Statement - \n");
	//char* retval = mSentence + strlen(mSentence)+ 1/*nullterminator*/;
	int retval = -1;
	char* mSentence = theSentence.m_raw_sentence;
	
	//extract_nlp_words( mSentence, &subject_list, &verb_list, &object_list, &adjective_list );

	std::string* subject  	= extract_word( mSentence, &subject_list 	);
	std::string* verb 		= extract_word( mSentence, &verb_list 	 	);
	//std::string* object 	= extract_word( mSentence, &object_list  	);
	//std::string* adjective	= extract_word( mSentence, &adjective_list  );
	//int prepos_index      = get_preposition_index( mSentence );

#ifdef NLP_DEBUG
	diagram_sentence(subject, verb, adjective, object );
#endif

	if ( (compare_word( subject, "can")==0) ||
		 (compare_word( subject, "can traffic")==0) ||
		 (compare_word( subject, "can data")==0) )
	{		
		if ( (compare_word( verb, "receive") ==0) ||
			 (compare_word( verb, "incoming") ==0)  )
		{
			printf( "Listening for incoming CAN data...\n");
			CAN_ListeningOn = FALSE;
			retval=0;
		}
		if ( (compare_word( verb, "send") ==0) ||
			 (compare_word( verb, "route") ==0)  )
		{
			//printf( "preparing to send CAN data...\n");
			int action;
			BOOL available = is_CAN_IPC_memory_available();
			if (!available)
			{
				printf("CAN IPC not present; Starting fork for amon(itor)\n");
				action = start_amon();
				if (action==0)
				{
					printf("No action for request to start amon!\n");
					mh->form_response ("Sorry, CAN is not available.");
					//nlp_reply_formulated = TRUE;
					//strcpy (NLP_Response, "Sorry, CAN is not available.");
					retval=0;					
					return retval;
				}						
			}
			int attached  = can_connect_shared_memory(FALSE);
            if (attached) {
                CAN_SendingOn = TRUE;
                set_tcp_transmitting_flag_ipc_can();
                /* rather than creating a new thread, we are going to set a variable to
                    send on the main server thread.			*/
          
          		mh->form_response("Okay, I will be sending you my CAN traffic.");      
               // nlp_reply_formulated = TRUE;
        //        strcpy (NLP_Response, "Okay, I will be sending you my CAN traffic.");
                retval=0;
            }
			/* the messages will be pulled off of the Received buffer.
			   and stored in Recieved buffer at the other instant end.  */
		}
		if ( (compare_word( verb, "stop") ==0) ||
			 (compare_word( verb, "stopping") ==0)  )
		{
			CAN_SendingOn   = FALSE;
			clear_tcp_transmitting_flag_ipc_can();
			// Leave connection to IPC and amon running.
			mh->form_response("Okay, No more CAN traffic will be transmitted over tcpip.");
			//nlp_reply_formulated = TRUE;
			//strcpy (NLP_Response, "Okay, No more CAN traffic will be transmitted over tcpip.");
			retval=0;			
		}

		if (compare_word( verb, "how much") ==0)
		{
		}
	}
	else if (compare_word( subject, "can_message")==0)
	{
		static struct sCAN msg;
		//dump_buffer(mSentence, 25); 
		byte* ptr = ((byte*)mSentence + strlen("CAN_message")+1);		
		int bytes_extracted = extract_CAN_msg( &msg, ptr );
		//printf("extract_CAN_msg() %d bytes\n", bytes_extracted );
		if (bytes_extracted) {
			AddToRxList( &msg );	// goes into the received buffer.
			print_rx_position();
		}
		retval = bytes_extracted;		
	}
    if (retval>-1) printf("Parse_CAN_Statement - done\n");
	return retval;
}


