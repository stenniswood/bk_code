#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <time.h> 
#include <string>
#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.h"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"

#include "alias.hpp"

/* Suggested statements:
		What is 197*12 ?
		What does 197*12 equal?
		How much is 197*12?
		multiply 197 by 12.		[might imply hold answer w/o replying]
		
		if there are 197 people coming and each one brings 12 chairs, will we have enough?
		

*/

/***********************************************************************
All Incoming Video will also be saved to the hard-drive on those 
systems with enough capacity.  This allows the user to call back anything
in its memory banks.  Think star trek logs.  "There were two people outside
your side door yesterday...    Can you replay it?  
What was the topic of it?  How long was it?  A smart robot could even
use the info to answer questions.  On Sept 20th, you said you wanted to
ignore all upgrade notices.
***********************************************************************/
/* VIDEO refers to a video file.  Where as CAMERA refers to live cam. */

BOOL  CAMERA_tcpip_WatchingOn 	= FALSE;		
BOOL  CAMERA_tcpip_SendingOn  	= FALSE;
BOOL  CAMERA_tcpip_SendingMuted  	= FALSE;		// we send zerod out CAMERA
BOOL  CAMERA_save_requested 		= FALSE;		// incoming or outgoing? 
FILE* sending_camera_playback_file_fd = NULL;		// prerecorded cam (robot's history)


static WordGroup  	subject_list;		// we'll use this as "interrogative" listing the common ways of asking a math question.
static WordGroup 	verb_list;

static std::list<std::string> 	preposition_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;


static void init_subject_list()
{
	// these are a clue that math question might be coming.
	subject_list.add_word("equal");		// towards end of sentence.
	subject_list.add_word("what is");
	subject_list.add_word("what does");	
	subject_list.add_word("how much");	
	
	// sentence must contain 2 or more numbers too.
}

static void init_verb_list()
{
	Alias mult,divide,plus,minus;
	
	mult.add_new("times");
	mult.add_new("multiplied by");

	divide.add_new("divided by");
	divide.add_new("divide");
	divide.add_new("over");	
	divide.add_new("split");	
	//divide.add_new("separated");		// implied times.
	
	plus.add_new("plus");
	plus.add_new("added");
	plus.add_new("add");		
	
	minus.add_new("minus");			
	minus.add_new("subtract");			
	minus.add_new("subtracted");	
		
	verb_list.add_new(mult  );
	verb_list.add_new(divide);
	verb_list.add_new(plus  );
	verb_list.add_new(minus );
			
	//verb_list.add_new("for each of ");
	
}

static void init_preposition_list()
{   // Object might be a numerical value preceded by a preposition.
	// help to establish order of operations:
	//    ie.  subtract 21 from 101.
	//      or what is 101 subtract 21
	// 	

	preposition_list.push_back( "to" );
	preposition_list.push_back( "from" );	
	preposition_list.push_back( "as" );	
	preposition_list.push_back( "by" );		
	preposition_list.push_back( "for");
	preposition_list.push_back( "in" );
}

static void init_adjective_list()
{ 

}

static void init_object_list()
{   // Object might be a numerical value preceded by a preposition.
	// ie. "set camera tilt _to_ _25.5 degrees"
	// prepositions to look for :
	//		to by as 
	object_list.push_back( "me"		);
}

static void init_word_lists()
{
	init_subject_list();
	init_verb_list();
	init_object_list();
	init_adjective_list();
	init_preposition_list();
}

void Init_Math_Protocol()
{    
    init_word_lists();
}


/**** ACTION INITIATORS:    (4 possible actions) *****/
bool contains_two_or_more_numbers( char* mSentence )
{
	/* blah */
}

/*****************************************************************
Do the work of the Telegram :
return  -1	=> Not handled
		else number of extra bytes extracted from the mSentence buffer.
			- besides strlen(mSentence)! 
*****************************************************************/
int Parse_Math_Statement( char* mSentence )
{
	int retval=-1;

	printf("Parse_Math_Statement\n");
	string* subject  	= subject_list.extract_word( mSentence );	
	string* verb 		= verb_list.extract_word   ( mSentence );
	bool result = contains_two_or_more_numbers( mSentence );
	if (result == false)		// must have!
			return retval;
	if (verb==NULL)				// must have!
			return retval;
				
	string* object 		= extract_word( mSentence, &object_list  	);	
	string* preposition = extract_word( mSentence, &preposition_list  );
	string* adjective	= extract_word( mSentence, &adjective_list  );	
	int prepos_index      	= get_preposition_index( mSentence );
    diagram_sentence		( subject, verb, adjective, object, preposition );

	if (compare_word( subject, "camera tilt")==0)
	{
		if (compare_word(verb, "set") ==0)
		{
			float result = atof(object->c_str());
			set_camera_tilt_to( result );
			retval=0;
		}
		if (compare_word(verb, "lower") ==0)
		{
			float result = atof(object->c_str());
			lower_camera_by( result );
			retval=0;
		}
		if (compare_word(verb, "raise") ==0)
		{
			float result = atof(object->c_str());
			raise_camera_by( result );		
			retval=0;
		}
		if (compare_word(verb, "what is") ==0)
		{
			float result = atof(object->c_str());
			get_camera_tilt( result );
			retval=0;
		}
	}
	else if (compare_word( subject, "camera pan")==0)
	{
		if (compare_word(verb, "set") ==0)
		{
			float result = atof(object->c_str());
			set_camera_pan_to( result );
			retval=0;
		}
		if ((compare_word(verb, "pan left") ==0)
		||  (compare_word(verb, "move left") ==0)
		||  (compare_word(verb, "rotate left") ==0))
		{
			float result = atof(object->c_str());
			move_camera_left_by( result );
			retval=0;
		}
		if ((compare_word(verb, "pan right") ==0)
		||  (compare_word(verb, "move right") ==0)
		||  (compare_word(verb, "rotate right") ==0))
		{
			float result = atof(object->c_str());
			move_camera_right_by( result );			
			retval=0;
		}
		if (compare_word(verb, "what is")==0)
		{
			float result = atof(object->c_str());
			get_camera_pan( result );
			retval=0;
		}
	}
	else if ((compare_word( subject, "camera")==0)  ||
		     (compare_word( subject, "web cam")==0)  ||
		     (compare_word( subject, "eyes")==0) )
	{
		printf("matched subject\n");
		
		/* It might be nice to redirect these statements to a subject of:
			camera tilt/pan.  This would get our subject more and more exacting.
			The preprocessor would know that Lowering a camera has to do with the tilt,
				and not any video properties.							
		*/
		if ((compare_word(verb, "receive") ==0) ||
			(compare_word(verb, "watch") ==0)  ||
			(compare_word(verb, "view") ==0)   ||
			(compare_word(verb, "spy") ==0)	   ||
			(compare_word(verb, "show") ==0)   ||
			(compare_word(verb, "send") ==0) )
		{	
			printf("verb matched!\n");
			int cond_1 = ((compare_word (adjective, "my")==0) ||
		  				  ((compare_word(preposition, "to")==0) && (compare_word(object, "you")==0)) );
			int cond_2 = ((compare_word (adjective, "your") ==0) ||
		  				  ((compare_word(preposition, "to")==0) && (compare_word(object, "me")==0)) );
			printf("cond1=%d; cond2=%d;\n", cond_1, cond_2 );
			if (cond_1)
			{    camera_watch();		retval = 0;		}					  				  
			else if (cond_2)
			{    send_camera();			retval = 0;		}
		}
		if (compare_word( adjective, "two way")==0 )
		{
		    camera_two_way();
		    retval = 0;
		};
		
		if ((compare_word(verb, "mute") ==0) ||
			(compare_word(verb, "block") ==0) )
		{
		    CAMERA_tcpip_SendingMuted = TRUE;	// Blocked, other side wont be able to see
			cli_ipc_write_response( "Math muted (electronically blocked)" );			    
		    retval = 0;		    
		}
		if ((compare_word(verb, "unmute") ==0) ||
			(compare_word(verb, "unblock") ==0))
		{
		    CAMERA_tcpip_SendingMuted = FALSE;	// Blocked, other side wont be able to see
			cli_ipc_write_response( "Math live!" );			    		    
		    retval = 0;
		}

		if ((compare_word(verb, "close") ==0) ||
		    (compare_word(verb, "stop" ) ==0)  ||
		    (compare_word(verb, "end"  ) ==0)  ||		    
		    (compare_word(verb, "terminate") ==0)  ||		    		    
		    (compare_word(verb, "kill" ) ==0))
		{
		    camera_cancel();
		    retval = 0;
		};
		
		if (compare_word(verb, "lower") ==0)
		{
			float result = atof(object->c_str());
			lower_camera_by( result );
			retval=0;
		}		
		if (compare_word(verb, "raise") ==0)
		{
			float result = atof(object->c_str());
			raise_camera_by( result );
			retval=0;
		}		
		if (compare_word(verb, "move left") ==0)
		{
			float result = atof(object->c_str());
			move_camera_left_by( result );		
			retval=0;
		}		
		if (compare_word(verb, "move right") ==0)
		{
			float result = atof(object->c_str());
			move_camera_left_by( result );		
			retval=0;
		}
		
		if (compare_word(verb, "start") ==0)
		if (compare_word(verb, "show") ==0)
		{
			// Look for "with highest/medium/low resolution/quality"
			
			// Look for output device
						
			// Recording			
				// recording verb will take precedence over "start"
			retval=0;
		}

		if (compare_word(verb, "stop") ==0)
		{
			retval=0;		
		}

		if (compare_word(verb, "change") ==0)
		{
			if (compare_word(object, "resolution") ==0)
			{
			}
			if (compare_word(object, "frame rate") ==0)
			{
			}			
			if (compare_word(object, "width") ==0)
			{
			}
			if (compare_word(object, "height") ==0)
			{
			}
			retval=0;			
		}		
	}
	printf( "Parse_Math_Statement done\n" );
	return retval;
}

