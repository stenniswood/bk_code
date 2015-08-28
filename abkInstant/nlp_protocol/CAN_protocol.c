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


static void init_subject_list()
{
	subject_list.push_back( "CAN"	 		);
	subject_list.push_back( "CAN data"		);
	subject_list.push_back( "CAN traffic" 	);
	subject_list.push_back( "CAN message" 	);
	
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

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
BOOL Parse_CAN_Statement( char* mSentence )
{
	std::string* subject  	= extract_word( mSentence, &subject_list 	);
	if (subject==NULL) return FALSE;  // subject matter must pertain.
	printf("Parse_CAN_Statement\n");
			
	std::string* verb 		= extract_word( mSentence, &verb_list 	 	);
	std::string* object 	= extract_word( mSentence, &object_list  	);
	std::string* adjective	= extract_word( mSentence, &adjective_list  );	
	//int prepos_index      	= get_preposition_index( mSentence );

	diagram_sentence(subject, verb, adjective, object );

	BOOL retval = FALSE;
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
			create_CAN_thread( TRUE,  FALSE );
			retval = TRUE;
		}
		if ( (strcmp(verb->c_str(), "send") ==0) ||
			 (strcmp(verb->c_str(), "route") ==0)  )
		{
			// Maybe want to verify the source IP address for security purposes
			// later on.  Not necessary now!
			printf( "Connecting to send CAN data...\n");
			create_CAN_tx_thread( TRUE,  FALSE );
			retval = TRUE;
		}

		if (strcmp(verb->c_str(), "how much") ==0)
		{
		}
	}
	else if (strcmp( subject->c_str(), "CAN message")==0)
	{
		if (strcmp(verb->c_str(), "send") ==0)
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
		}
	}
	return retval;
}


#ifdef  __cplusplus
}
#endif
