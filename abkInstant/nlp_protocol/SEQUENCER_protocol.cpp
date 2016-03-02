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

BOOL SEQ_ListeningOn;	// if true we will be receiving CAN Traffic.
BOOL SEQ_SendingOn;		// if true we will be sending CAN Traffic.

static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string> 	preposition_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;

//#define NLP_DEBUG 1


static void init_subject_list()
{
	subject_list.push_back( "sequencer" 	);
	subject_list.push_back( "sequence" 	);
	subject_list.push_back( "capabilities" 	);
}

static void init_verb_list()
{
	verb_list.push_back( "open" 	 );
	verb_list.push_back( "queue" );
	verb_list.push_back( "store" 	 );
	verb_list.push_back( "repeat" );

	verb_list.push_back( "close" 	 );
	verb_list.push_back( "stop" 	 );	
	verb_list.push_back( "end" 	 );
	verb_list.push_back( "terminate" );
	verb_list.push_back( "kill" 	 );			
	
	verb_list.push_back( "play" 	 );			
	verb_list.push_back( "replay" );
	verb_list.push_back( "run" 	 );
	verb_list.push_back( "rerun" 	 );
	
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

static void init_adjective_list()		// and adverbs
{ 
	adjective_list.push_back( "straight" );	
	adjective_list.push_back( "highest" );
	adjective_list.push_back( "fast" 	);
	adjective_list.push_back( "slowly"	);
	adjective_list.push_back( "low" 	);
	adjective_list.push_back( "lowest"	);
	adjective_list.push_back( "best" 	);
	adjective_list.push_back( "backward" );
	adjective_list.push_back( "forward"  );
}

static void init_object_list()
{   // Object might be a numerical value preceded by a preposition.
	// ie. "set camera tilt _to_ _25.5 degrees"
	// prepositions to look for :
	//		to by as 
	object_list.push_back( "leg" 	);		
	object_list.push_back( "arm"	);
	object_list.push_back( "foot"	);
	object_list.push_back( "ankle" );
	object_list.push_back( "knee" );	
	object_list.push_back( "hip"	);
	object_list.push_back( "step"	);
	
	object_list.push_back( "elbow"	);
	object_list.push_back( "wrist"	);
	object_list.push_back( "hand"	);
	object_list.push_back( "finger"	);
	
}

static void init_word_lists()
{
	init_subject_list();
	init_verb_list();
	init_object_list();
	init_preposition_list();
	init_adjective_list();	
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

#if (PLATFORM==Mac)
static char aseq_command[] = "";
#elif (PLATFORM==RPI)
static char aseq_command[] = "sudo /home/pi/bk_code/aseq/seq";
#elif (PLATFORM==linux_desktop)
static char aseq_command[] = "";
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
int start_sequencer() 
{
    int pid;
    switch (pid=fork()) {
        case -1:
	    printf("fork() = -1 %s\n", strerror(errno) );
            return 0;
        case 0:
            execvp(aseq_command, NULL);
            printf("returned from ececvp\n");
        default:
            return 0;
    }
    return 1;
}


std::string* subject = NULL;
std::string* verb = NULL;
std::string* object = NULL;
std::string* adjective=NULL;

BOOL  extract_nlp_words()
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

    verb 	= extract_word( mSentence, &verb_list 	 );
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


/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
		
return:	pointer to the next telegram (ie. after all our header and data bytes)
		this will be null if end of the received buffer (terminator added in serverthread.c
		by the number of bytes read).
*****************************************************************/
char* Parse_Sequencer_Statement( char* mSentence )
{
	printf("Parse_Sequencer_Statement() ");	
	BOOL result = extract_nlp_words();
	if (result==FALSE) 	return;

	char* retval = mSentence + strlen(mSentence)+ 1/*nullterminator*/;	

	/* Main thing we want for starters is:

	   "sequencer, queue up these named poses and "run", "animate", "play"
	   them.  Repeat 5 times."
	   
	   */
	if ( (strcmp( subject->c_str(), "sequencer")==0) )
	{
		if ( (strcmp(verb->c_str(), "queue") ==0) ||
			 (strcmp(verb->c_str(), "store") ==0)  )
		{
			printf( "Storing sequence list for playback \n");

			retval = TRUE;
		}
		if ( (strcmp(verb->c_str(), "replay") ==0) ||
			 (strcmp(verb->c_str(), "play") ==0)  )
		{
			// Maybe want to verify the source IP address for security purposes
			// later on.  Not necessary now!
			printf( "Initiating robot sequence... (Watch for objects in path of limbs!)\n");
			int action;
			int available = seq_connect_shared_sequencer_memory( TRUE );
			if (!available)
			{
				printf("Starting fork for amon(itor)\n");
				//action = start_sequencer();
				//if (action)
				//	printf("No action for request to start amon!\n");
			}
			int available = seq_connect_shared_sequencer_memory( TRUE );			

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
	else if ( (strcmp( subject->c_str(), "pose")==0) &&
		     (strcmp(adjective->c_str(), "new") ==0)  )
	{
	/*	static struct sPose msg;
		//dump_buffer(mSentence, 25); 
		byte* ptr = ((byte*)mSentence + strlen("new pose")+1);		

	//	int bytes_extracted = extract_pose( &pose, ptr );
		printf("extract_pose() %d bytes\n", bytes_extracted );
		
		if (bytes_extracted) {
		    // add name & angles to SQL.
			print_rx_position();
		}

		retval += bytes_extracted;	*/
	}
	else if (strcmp( subject->c_str(), "sequencer")==0)
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
	printf("Parse_Sequencer_Statement() ");
	return retval;
}


#ifdef  __cplusplus
}
#endif
