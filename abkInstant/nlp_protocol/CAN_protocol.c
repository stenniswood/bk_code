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
#include "GENERAL_protocol.h"
#include "CAMERA_device.h"
#include "thread_control.h"


#ifdef  __cplusplus
extern "C" {
#endif

BOOL CAN_ListeningOn;	// if true we will be receiving CAN Traffic.
BOOL CAN_SendingOn;		// if true we will be sending CAN Traffic.

static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string> 	preposition_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;

//#define NLP_DEBUG 1


static void init_subject_list()
{
	subject_list.push_back( "CAN_message" 	);
	subject_list.push_back( "CAN"	 		);
	subject_list.push_back( "CAN data"		);
	subject_list.push_back( "CAN traffic" 	);
	
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

BOOL compare_subject( std::string* subject, const char* mstr )
{
	BOOL retval = (strcmp( subject->c_str(), mstr)==0);
	return retval;	
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
char* Parse_CAN_Statement( char* mSentence )
{
	std::string* subject  	= extract_word( mSentence, &subject_list 	);
	if (subject==NULL) return FALSE;  // subject matter must pertain.


	char* retval = mSentence + strlen(mSentence)+ 1/*nullterminator*/;

	std::string* verb 		= extract_word( mSentence, &verb_list 	 	);
	std::string* object 	= extract_word( mSentence, &object_list  	);
	std::string* adjective	= extract_word( mSentence, &adjective_list  );	
	//int prepos_index      = get_preposition_index( mSentence );

#ifdef NLP_DEBUG
	printf("Parse_CAN_Statement - ");
	diagram_sentence(subject, verb, adjective, object );
#endif

	if ( (strcmp( subject->c_str(), "CAN")==0) ||
		 (strcmp( subject->c_str(), "CAN traffic")==0) ||
		 (strcmp( subject->c_str(), "CAN data")==0) )
	{
		if ( (strcmp(verb->c_str(), "receive") ==0) ||
			 (strcmp(verb->c_str(), "incoming") ==0)  )
		{
			// Maybe want to verify the source IP address for security purposes
			// later on.  Not necessary now!
			printf( "Listening for incoming CAN data...\n");
			//create_CAN_thread( TRUE,  FALSE );
			CAN_ListeningOn = FALSE;
			//retval = TRUE;
		}
		if ( (strcmp(verb->c_str(), "send") ==0) ||
			 (strcmp(verb->c_str(), "route") ==0)  )
		{
			// Maybe want to verify the source IP address for security purposes
			// later on.  Not necessary now!
			printf( "Connecting to send CAN data...\n");
			int action;
			BOOL available = is_CAN_IPC_memory_available();
			if (!available)
			{
				printf("Starting fork for amon(itor)\n");
				//action = start_amon();
				//if (action)
				//	printf("No action for request to start amon!\n");
			} 
			int attached = can_connect_shared_memory(FALSE);
			CAN_SendingOn   = TRUE;
			set_tcp_transmitting_flag();
			
			/* the messages will be pulled off of the Received buffer.
			   and stored in Recieved buffer at the other instant end.  */
			
			/* rather than creating a new thread, we are going to set a variable to
				send on the main server thread.
				Can we do that?  bidirectional - okay.  the receive would have to poll
				for bytes available and if none, then do any sending.  Yes.								
			*/
			//retval = TRUE;
		}
		if ( (strcmp(verb->c_str(), "stop") ==0) ||
			 (strcmp(verb->c_str(), "stopping") ==0)  )
		{
			CAN_SendingOn   = FALSE;
			clear_tcp_transmitting_flag();			
			// Leave connection to IPC and amon running.
			//retval = TRUE;
		}

		if (strcmp(verb->c_str(), "how much") ==0)
		{
			//retval = TRUE;
		}
	}
	else if (strcmp( subject->c_str(), "CAN_message")==0)
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

		retval += bytes_extracted;		
	}
	else if (strcmp( subject->c_str(), "CANlkjh")==0)
	{
/*		if (strcmp(verb->c_str(), "send") ==0)
		{
			//float result = atof(mObject->c_str());			
		}
		if ((strcmp(verb->c_str(), "wait") ==0))
		{
			// check preposition "for" or "on" 
			// "then"
			// "plot"
		}
		if ((strcmp(verb->c_str(), "plot") ==0) ||
			(strcmp(verb->c_str(), "graph") ==0) )
		{
			// check preposition "for" or "on" 
			// "then"
			// "plot"
		}
		if (strcmp(verb->c_str(), "record")==0)
		{
			// "value"
		} */
	}
	return retval;
}


#ifdef  __cplusplus
}
#endif
