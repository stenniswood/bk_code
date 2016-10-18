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
#include "GENERAL_protocol.hpp"
#include "GPIO_protocol.hpp"
#include "GPIO_device.hpp"
#include "nlp_extraction.hpp"


static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
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


/*static void init_adjective_list()
{ 
	adjective_list.push_back( "high" );
	adjective_list.push_back( "my" 		);
	adjective_list.push_back( "your" 	);
	adjective_list.push_back( "low" 	);
	adjective_list.push_back( "lowest"	);
	adjective_list.push_back( "best" 	);
	adjective_list.push_back( "quality" );
}*/

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

int extract_pin_number( std::string& mSentence )
{
	// find first [0..9] after either "GPIO" or "pin"
    return -1;
}


/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
int Parse_GPIO_Acknowledgement( char* mSentence, ServerHandler* mh )
{
    int retval=-1;
    std::string Sentence(mSentence);
	int pin = extract_pin_number( Sentence );
	
    std::string* subject  	= extract_word( mSentence, &subject_list 	);
    std::string* verb 		= extract_word( mSentence, &verb_list 	 	);
    std::string* object 	= extract_word( mSentence, &object_list  	);
    //std::string* adjective	= extract_word( mSentence, &adjective_list  );
    //int prepos_index      = get_preposition_index( mSentence, preposition_list );
    
#ifdef NLP_DEBUG
    diagram_sentence(subject, verb, adjective, object );
#endif
    
    
	if (compare_word( subject, "GPIO")==0)
	{
		if (compare_word(verb, "set") ==0)
		{
			if (compare_word(object, "input") ==0)
			{
				set_GPIO_DDR( pin, TRUE );
                retval=0;
			}
			if (compare_word(object, "output") ==0)
			{
				set_GPIO_DDR( pin, FALSE );
                retval=0;
            }
			if ((compare_word(object, "1") ==0)
			||  (compare_word(object, "high") ==0)
			||  (compare_word(object, "on") ==0))
			{
				set_GPIO( pin, 1 );
                retval=0;
			}
			if ((compare_word(object, "0") ==0)
			||  (compare_word(object, "low") ==0)
			||  (compare_word(object, "off") ==0))
			{
				set_GPIO( pin, 0 );
                retval=0;
			}
		}
		if ((compare_word(verb, "read")==0)
		||  (compare_word(verb, "what is")==0))
		{
			//int value = read_GPIO( pin );
            retval=0;
		}
	}
    if (retval>-1)  printf("Parse_GPIO_Statement - done\n");

    return retval;
}

/*****************************************************************
Do the work of the Telegram :
We might have multiple actions such as:  start and record
So:   list<char*> mAction

Adjectives may be black & white.  Low res, high res, HDMI,

return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/

