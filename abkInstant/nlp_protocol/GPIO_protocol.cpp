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
#include <string>
#include <list>
#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.h"
#include "GPIO_protocol.h"
#include "GPIO_device.h"


static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string> 	preposition_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;


static void init_subject_list()
{
	subject_list.push_back( "GPIO"	 		);
	subject_list.push_back( "output"		);	
	subject_list.push_back( "pin" 			);
	subject_list.push_back( "pin number"	);	
	subject_list.push_back( "input" 		);
	subject_list.push_back( "LED" 			);
	subject_list.push_back( "RELAY" 		);	
	
	subject_list.push_back( "capabilities" 	);
}

static void init_verb_list()
{
	verb_list.push_back( "turn" 	);	
	verb_list.push_back( "toggle" 	);
	verb_list.push_back( "deactivate" 	);
	verb_list.push_back( "enable" 		);
	verb_list.push_back( "activate" 	);

	verb_list.push_back( "are" );	
	verb_list.push_back( "have" );
	verb_list.push_back( "get" );	
	verb_list.push_back( "set" );	
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
	adjective_list.push_back( "high" );
	adjective_list.push_back( "my" 		);
	adjective_list.push_back( "your" 	);
	adjective_list.push_back( "low" 	);
	adjective_list.push_back( "lowest"	);
	adjective_list.push_back( "best" 	);
	adjective_list.push_back( "quality" );

}

static void init_object_list()
{   // Object might be a numerical value preceded by a preposition.
	// ie. "set camera tilt _to_ _25.5 degrees"
	// prepositions to look for :
	//		to by as 
	object_list.push_back( "on" 	  	);		
	object_list.push_back( "off"	  	);
	object_list.push_back( "input"	);
	object_list.push_back( "output" );
	object_list.push_back( "tristate" 	 );
	object_list.push_back( "impediance" 	 );		
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
void Init_GPIO_Protocol()
{
	init_word_lists();
}

/* ACTION FUNCTIONS: */

int extract_pin_number( std::string* mSentence )
{
	// find first [0..9] after either "GPIO" or "pin"	
}
			
/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Parse_GPIO_Acknowledgement( sObject* mSubject, sObject* mVerb, 
								 sObject* mObject, std::string* mSentence )
{
	int pin = extract_pin_number( mSentence );
	
	if (strcmp( mSubject->name.c_str(), "GPIO")==0)
	{
		if (strcmp(mVerb->name.c_str(), "set") ==0)
		{
			if (strcmp(mObject->name.c_str(), "input") ==0)
			{
				set_GPIO_DDR( pin, TRUE );
			}
			if (strcmp(mObject->name.c_str(), "output") ==0)
			{
				set_GPIO_DDR( pin, FALSE );
			}
			if ((strcmp(mObject->name.c_str(), "1") ==0)
			||  (strcmp(mObject->name.c_str(), "high") ==0)
			||  (strcmp(mObject->name.c_str(), "on") ==0))			
			{
				set_GPIO( pin, 1 );
			}
			if ((strcmp(mObject->name.c_str(), "0") ==0)
			||  (strcmp(mObject->name.c_str(), "low") ==0)
			||  (strcmp(mObject->name.c_str(), "off") ==0))			
			{
				set_GPIO( pin, 0 );			
			}
		}
		if ((strcmp(mVerb->name.c_str(), "read")==0)
		||  (strcmp(mVerb->name.c_str(), "what is")==0))
		{
			int value = read_GPIO( pin );
		}
	}
}

/*****************************************************************
Do the work of the Telegram :
We might have multiple actions such as:  start and record
So:   list<char*> mAction

Adjectives may be black & white.  Low res, high res, HDMI,

return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/

