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
#include "CAN_protocol.hpp"
#include "GENERAL_protocol.hpp"
#include "CAMERA_device.hpp"
#include "nlp_extraction.hpp"
#include "prefilter.hpp"
#include "string_util.h"
#include "nlp_sentence.hpp"

#define Debug 0

BOOL SEQ_ListeningOn;	// if true we will be receiving CAN Traffic.
BOOL SEQ_SendingOn;		// if true we will be sending CAN Traffic.


//#define NLP_DEBUG 1

static void init_subject_list()
{
	string subjects = "(sequencer|sequence|robot)";
}

// Object might be a numerical value preceded by a preposition.
// ie. "set camera tilt _to_ _25.5 degrees"
// prepositions to look for :
//		to by as 
static void init_word_lists()
{
    Word raise, lower, rotate_left, walk, stop;
	string action = "(raise|lift|lower|drop|rotate|twist|turn|swivel|walk|step|come|stop|freeze|end)";
	string action2 = "(play|replay|run|rerun|set)";
	string adjective = "(straight|highest|fast|slowly|low|lowest|best|backward|forward)";
	string objects = "(leg|arm|foot|ankle|knee|hip|step|elbow|wrist|hand|finger)";	
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

extern int start_seq();

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

// No ServerHandler Response for these!  Just move the motors!
void parse_motor_speed_command(Sentence& mSentence, float& mValues[], int& mSize)
{
	mSentence.split(";");
	int okay = mSentence.m_split_words[0].compare( "MOTOR SPEED:" );
	if (okay !=0) return;
	
	int motors_found = mSentence.m_split_words.size()-1;
	for (int i=1; i<motors_found; i++)
	{
		mSentence.m_split_words[i].fprintf ();		
		mValues[i-1] = blah;
	}
}

// No ServerHandler Response for these!  Just move the motors!
void parse_motor_position_command(Sentence& mSentence, float& mValues[], int& mSize)
{
	mSentence.split(";");
	int okay = mSentence.m_split_words[0].compare( "MOTOR POSITION:" );
	if (okay !=0) return;

	int motors_found = mSentence.m_split_words.size()-1;
	for (int i=1; i<motors_found; i++)
	{
		mSentence.m_split_words[i].fprintf("M");
		mValues[i-1] = blah;
//		mSentence.m_split_words[i].fprintf("M");
	}
}

extern SequencerIPC  sequencer_mem;		// see main.cpp

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
		
return:	pointer to the next telegram (ie. after all our header and data bytes)
		this will be null if end of the received buffer (terminator added in serverthread.c
		by the number of bytes read).
*****************************************************************/
int Parse_Sequencer_Statement( Sentence& mSentence, ServerHandler* mh )
{
	int retval = -1;
    if (ipc_memory_client==NULL)  return FALSE;
    
	if (Debug) printf("Parse_Sequencer_Statement() ");
    mSentence.restore_reduced();
    
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
			//int action;
			//sequencer_mem.connect_shared_memory(TRUE);			
			int available = 1; //seq_connect_shared_sequencer_memory( TRUE );
			if (!available)
			{
				printf("Starting fork for amon(itor)\n");
				//action = start_sequencer();
				//if (action)
				//	printf("No action for request to start amon!\n");
			}
			//available = seq_connect_shared_sequencer_memory( TRUE );

			
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
	if (retval>-1)  printf("Parse_Sequencer_Statement() ");
	return retval;
}


