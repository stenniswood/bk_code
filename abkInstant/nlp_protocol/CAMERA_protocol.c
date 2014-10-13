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
#include "CAMERA_protocol.h"
#include "CAMERA_device.h"
#include "thread_control.h"

/* Suggested statements:
	Show camera on [TV,robot,kitchen tv, phone, etc]
	stop camera.
	start record camera until you see someone.		// Recording will always imply harddrive.
	Use [highest,lowest,medium,HDMI,4x,etc] [resolution,quality,frame-rate]
	
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

static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string> 	preposition_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;


static void init_subject_list()
{
	subject_list.push_back( "audio"	 		);
	subject_list.push_back( "connection"	);	
	subject_list.push_back( "microphone" 	);
	subject_list.push_back( "recording" 	);
	subject_list.push_back( "music" 		);
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
	
	verb_list.push_back( "synthesize" );
	verb_list.push_back( "are" );	
	verb_list.push_back( "have" );
	verb_list.push_back( "get" );	
	verb_list.push_back( "set" );	
	verb_list.push_back( "I am" );		
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

void Init_Camera_Protocol()
{
	init_word_lists();
}
/*****************************************************************
Do the work of the Telegram :
return  TRUE = Telegram was Handled by this routine
		FALSE= Telegram not Handled by this routine
*****************************************************************/
BOOL Parse_Camera_Statement( char* mSentence )							
{
	BOOL retval = FALSE;
	std::string* subject  	= extract_word( mSentence, &subject_list 	);
	std::string* verb 		= extract_word( mSentence, &verb_list 	 	);
	std::string* object 	= extract_word( mSentence, &object_list  	);
	std::string* adjective	= extract_word( mSentence, &adjective_list  );	
	int prepos_index      	= get_preposition_index( mSentence );

	if (strcmp( subject->c_str(), "camera tilt")==0)
	{
		if (strcmp(verb->c_str(), "set") ==0)
		{
			float result = atof(object->c_str());
			set_camera_tilt_to( result );
		}
		if (strcmp(verb->c_str(), "lower") ==0)
		{
			float result = atof(object->c_str());
			lower_camera_by( result );
		}
		if (strcmp(verb->c_str(), "raise") ==0)
		{
			float result = atof(object->c_str());
			raise_camera_by( result );		
		}
		if (strcmp(verb->c_str(), "what is") ==0)
		{
			float result = atof(object->c_str());
			get_camera_tilt( result );
		}
	}
	else if (strcmp( subject->c_str(), "camera pan")==0)
	{
		if (strcmp(verb->c_str(), "set") ==0)
		{
			float result = atof(object->c_str());
			set_camera_pan_to( result );
		}
		if ((strcmp(verb->c_str(), "pan left") ==0)
		||  (strcmp(verb->c_str(), "move left") ==0)
		||  (strcmp(verb->c_str(), "rotate left") ==0))
		{
			float result = atof(object->c_str());
			move_camera_left_by( result );
		}
		if ((strcmp(verb->c_str(), "pan right") ==0)
		||  (strcmp(verb->c_str(), "move right") ==0)
		||  (strcmp(verb->c_str(), "rotate right") ==0))
		{
			float result = atof(object->c_str());
			move_camera_right_by( result );			
		}
		if (strcmp(verb->c_str(), "what is")==0)
		{
			float result = atof(object->c_str());
			get_camera_pan( result );
		}
	}
	else if (strcmp( subject->c_str(), "camera")==0)
	{
		/* It might be nice to redirect these statements to a subject of:
			camera tilt/pan.  This would get our subject more and more exacting.
			The preprocessor would know that Lowering a camera has to do with the tilt,
				and not any video properties.
							
		*/
		if (strcmp(verb->c_str(), "lower") ==0)
		{
			float result = atof(object->c_str());
			lower_camera_by( result );
		}		
		if (strcmp(verb->c_str(), "raise") ==0)
		{
			float result = atof(object->c_str());
			raise_camera_by( result );
		}		
		if (strcmp(verb->c_str(), "move left") ==0)
		{
			float result = atof(object->c_str());
			move_camera_left_by( result );		
		}		
		if (strcmp(verb->c_str(), "move right") ==0)
		{
			float result = atof(object->c_str());
			move_camera_left_by( result );		
		}
		
		if (strcmp(verb->c_str(), "start") ==0)
		if (strcmp(verb->c_str(), "show") ==0)
		{
			// Look for "with highest/medium/low resolution/quality"
			
			// Look for output device
						
			// Recording			
				// recording verb will take precedence over "start"
		}

		if (strcmp(verb->c_str(), "stop") ==0)
		{
		
		}

		if (strcmp(verb->c_str(), "change") ==0)
		{
			if (strcmp(object->c_str(), "resolution") ==0)
			{
			}
			if (strcmp(object->c_str(), "frame rate") ==0)
			{
			}			
			if (strcmp(object->c_str(), "width") ==0)
			{
			}
			if (strcmp(object->c_str(), "height") ==0)
			{
			}
		}		
	}
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

