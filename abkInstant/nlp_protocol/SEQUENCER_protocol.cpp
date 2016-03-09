
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
#include <vector>

#include "protocol.h"
#include "devices.h"
#include "CAN_memory.h"
#include "client_memory.hpp"
#include "sequencer_memory.hpp"

#include "CAN_util.h"
#include "GENERAL_protocol.hpp"
#include "CAMERA_device.hpp"
#include "nlp_extraction.hpp"
#include "prefilter.hpp"
#include "string_util.h"
#include "nlp_sentence.hpp"


BOOL SEQ_ListeningOn;	// if true we will be receiving CAN Traffic.
BOOL SEQ_SendingOn;		// if true we will be sending CAN Traffic.

WordGroup  	subject_list;
WordGroup 	verb_list;

static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;

//#define NLP_DEBUG 1
#define Debug 0

static void init_subject_list()
{
	subject_list.add_word( "sequencer"      );
	subject_list.add_word( "sequence"       );
	subject_list.add_word( "robot" 	);
}

static void init_verb_list()
{
    Word raise, lower, rotate_left, walk, stop;
    
    raise.add_new("raise");
    raise.add_new("lift");

    lower.add_new("lower");
    lower.add_new("drop");

    rotate_left.add_new("rotate");
    rotate_left.add_new("twist ");
    rotate_left.add_new("turn");
    rotate_left.add_new("swivel");      // left/right comes from the object!
    
    walk.add_new("walk");
    walk.add_new("step");
    walk.add_new("come");
    
    stop.add_new( "stop"   );
    stop.add_new( "freeze" );
    stop.add_new( "end"    );

    verb_list.add_new( raise );
	verb_list.add_new( lower );
	verb_list.add_new( rotate_left );
    verb_list.add_new( walk );
    verb_list.add_new( stop );

/*    verb_list.add_word( "play" 	 );
	verb_list.add_word( "replay" );
	verb_list.add_word( "run" 	 );
	verb_list.add_word( "rerun" );
	verb_list.add_word( "set" ); */
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
	object_list.push_back( "knee"  );	
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
	init_adjective_list();	
}

/*****************************************************************
Initialize internal data structures for CAN protocol (protocol only, 
not all the CAN driving code):
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Init_Sequencer_Protocol()
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


static std::string* subject  = NULL;
static std::string* verb     = NULL;
static std::string* object   = NULL;
static std::string* adjective=NULL;




/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
		
return:	pointer to the next telegram (ie. after all our header and data bytes)
		this will be null if end of the received buffer (terminator added in serverthread.c
		by the number of bytes read).
*****************************************************************/
int Parse_Sequencer_Statement( Sentence& mSentence )
{
	int retval = -1;
    if (ipc_memory_client==NULL)  return FALSE;
    
	dprintf("Parse_Sequencer_Statement() ");	
    int subject_count	= subject_list.evaluate_sentence( mSentence.m_sentence );
    int verb_count		= verb_list.evaluate_sentence   ( mSentence.m_sentence );

    string* object 		= extract_word( mSentence.m_sentence, &object_list  	  );
    string* adjective	= extract_word( mSentence.m_sentence, &adjective_list    );
    //diagram_sentence		( subject, verb, adjective, object, preposition );
    
	/* Main thing we want for starters is:
	   "sequencer, queue up these named poses and "run", "animate", "play"
	   them.  Repeat 5 times."
	   
	   */
	if ( (compare_word( subject, "sequencer")==0) )
	{
		if ( (compare_word(verb, "queue") ==0) ||
			 (compare_word(verb, "store") ==0)  )
		{
			printf( "Storing sequence list for playback \n");
			retval=0;
		}
		if ( (compare_word(verb, "replay") ==0) ||
			 (compare_word(verb, "play") ==0)  )
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
			available = seq_connect_shared_sequencer_memory( TRUE );

			
			/* the messages will be pulled off of the Received buffer.
			   and stored in Recieved buffer at the other instant end.  */
			
			/* rather than creating a new thread, we are going to set a variable to
				send on the main server thread.
				Can we do that?  bidirectional - okay.  the receive would have to poll
				for bytes available and if none, then do any sending.  Yes.								
			*/
			retval=0;
		}
		if ( (compare_word(verb, "stop") ==0) ||
			 (compare_word(verb, "stopping") ==0)  )
		{
			// Leave connection to IPC and amon running.
			retval=0;
		}

		if (compare_word(verb, "how much") ==0)
		{
			//retval = TRUE;
		}
	}
	else if ( (compare_word( subject, "pose")==0) &&
		     (compare_word(adjective, "new") ==0)  )
	{
	}
	else if (compare_word( subject, "sequencer")==0)
	{
	}
	if (retval>-1)  printf("Parse_Sequencer_Statement() \n");
	return retval;
}


