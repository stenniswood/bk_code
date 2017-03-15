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

#include "sequencer_memory.hpp"
#include "nlp_sentence.hpp"
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
    //Word raise, lower, rotate_left, walk, stop;
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

#if (MOTOR_DRIVER==USE_ROBOCLAW)
#include "roboclaw.hpp"
extern RoboClaw mot1;
extern RoboClaw mot2;

void set_motor_speed1( int speed )
{
	if (speed>0)
		mot1.ForwardM1( 0x80, speed );
	else 
		mot1.BackwardM1( 0x80, -speed );
}
void set_motor_speed2( int speed )
{
	if (speed>0)
		mot1.ForwardM2( 0x80, speed );
	else 
		mot1.BackwardM2( 0x80, -speed );
}
void set_motor_speed3( int speed )
{
	if (speed>0)
		mot2.ForwardM1( 0x80, speed );
	else 
		mot2.BackwardM1( 0x80, -speed );
}
void set_motor_speed4( int speed )
{
	if (speed>0)
		mot2.ForwardM2( 0x80, speed );
	else 
		mot2.BackwardM2( 0x80, -speed );
}
#elif (MOTOR_DRIVER==USE_BK_QM)

#endif

// No ServerHandler Response for these!  Just move the motors!
void parse_motor_speed_command(Sentence& mSentence, float mValues[], int& mSize)
{
	printf(" parse_motor_speed_command() \n");
	mSentence.m_sentence.split(',');
	int motors_found = mSentence.m_sentence.m_split_words.size();
	for (int i=0; i< motors_found; i++)
	{
		mSentence.m_sentence.m_split_words[i].split('=');
		string value = mSentence.m_sentence.m_split_words[i].m_split_words[1];
		int mot_number = atoi( &(mSentence.m_sentence.m_split_words[i].m_split_words[0][1]) );
		int speed = stoi( value );	
		printf("\t%d Mot%d:value =%d\n", i, mot_number, speed );	
		switch (mot_number)
		{
		case 1 : set_motor_speed1( speed );
					break;
		case 2 : set_motor_speed2( speed );
					break;
		case 3 : set_motor_speed3( speed );
					break;
		case 4 : set_motor_speed4( speed );
					break;
		default: break;
		}
	}
	
}

// No ServerHandler Response for these!  Just move the motors!
void parse_motor_position_command(Sentence& mSentence, float mValues[], int& mSize)
{
	printf(" parse_motor_position_command() \n");
	mSentence.m_sentence.split(',');
	int motors_found = mSentence.m_sentence.m_split_words.size();
	for (int i=0; i<motors_found; i++)
	{
		mSentence.m_sentence.m_split_words[i].split('=');
		string value = mSentence.m_sentence.m_split_words[i].m_split_words[1];
		printf("\t%d value =%s\n", i, value.c_str() );
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
		, ServerHandler* mh
*****************************************************************/
int Parse_Sequencer_Statement( Sentence& mSentence )
{
	int retval = -1;
    
	printf("Parse_Sequencer_Statement() ");
    mSentence.restore_reduced();
/* “Motor:Speed:Board=1:M1=211,M2=10,M3=127,M4=127”
   “Motor:Speed:Board=2:M1=211,M2=10,M3=127,M4=127”
   “Motor:Position:Board=1:M1=211,M2=10,M3=127,M4=127”
   “Motor:Position:Board=2:M1=211,M2=10,M3=127,M4=127”
   “Motor:home:Board=2:”
   “Motor:stop_all:Board=1:”   
*/
	mSentence.m_sentence.split(':');
	string key_word 	= mSentence.m_sentence.m_split_words[0];
	string cmd_word 	= mSentence.m_sentence.m_split_words[1];
	string board_select = mSentence.m_sentence.m_split_words[2];
	string data_words   = mSentence.m_sentence.m_split_words[3];
	Sentence data;
	data.set( data_words.c_str(), false);
	float Values[5];
	int   Size;
	 
	//printf("Recieved:  %s, %s, %s, %s \n", key_word.c_str(), cmd_word.c_str(), board_select.c_str(), data_words.c_str() );

	int okay = key_word.compare( "Motor" );
	if (okay==0) 
	{
		if (cmd_word.compare( "Speed" )==0)
			parse_motor_speed_command(data, Values, Size);
		else if (cmd_word.compare( "Position" )==0)
			parse_motor_position_command(data, Values, Size);
		else if (cmd_word.compare( "home" )==0)
		{
		} else if (key_word.compare( "stop" )==0)
    	{

    	} else if (key_word.compare( "resume" )==0)
		{
		}
	}
	if (retval>-1)  printf("Parse_Sequencer_Statement() ");
	return retval;
}


