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
#include "bk_system_defs.h"
#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.hpp"
#include "CAMERA_protocol.hpp"
#include "CAMERA_device.hpp"
#include "CAMERA_util.h"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"
#include "nlp_sentence.hpp"


#define Debug 0

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
/* VIDEO refers to a video file.  Where as CAMERA refers to live cam. */

BOOL  CAMERA_tcpip_WatchingOn 	= FALSE;		
BOOL  CAMERA_tcpip_SendingOn  	= FALSE;
BOOL  CAMERA_tcpip_SendingMuted  	= FALSE;		// we send zerod out CAMERA
BOOL  CAMERA_save_requested 		= FALSE;		// incoming or outgoing? 
FILE* sending_camera_playback_file_fd = NULL;		// prerecorded cam (robot's history)


static std::list<std::string>  	subject_list;
static std::list<std::string> 	verb_list;
static std::list<std::string> 	adjective_list;
static std::list<std::string>  	object_list;


static void init_subject_list()
{
	subject_list.push_back( "camera" 		);
	subject_list.push_back( "web cam"	);	
	subject_list.push_back( "eyes" 	);
	subject_list.push_back( "camera capabilities" 	);
}

static void init_verb_list()
{
	verb_list.push_back( "open" );
	verb_list.push_back( "receive" );		
	verb_list.push_back( "route" );
	verb_list.push_back( "incoming" );
	verb_list.push_back( "send" );

	verb_list.push_back( "close" 	 );
	verb_list.push_back( "stop" 	 );	
	verb_list.push_back( "end" 		 );
	verb_list.push_back( "terminate" );
	verb_list.push_back( "kill" 	 );			

	verb_list.push_back( "view" );				
	verb_list.push_back( "watch");			
	verb_list.push_back( "show" );
	verb_list.push_back( "spy"  );
	
	verb_list.push_back( "are" );	
	verb_list.push_back( "have" );
	verb_list.push_back( "get" );	
	verb_list.push_back( "set" );	
	verb_list.push_back( "I am" );		
}


static void init_adjective_list()
{ 
	adjective_list.push_back( "highest" );
	adjective_list.push_back( "my" 		);
	adjective_list.push_back( "your" 	);
	adjective_list.push_back( "low" 	);
	adjective_list.push_back( "lowest"	);
	adjective_list.push_back( "VGA"	);
	adjective_list.push_back( "QVGA"	);	
	adjective_list.push_back( "hdmi" 	);
	adjective_list.push_back( "quality" );
	adjective_list.push_back( "hi-res"  );
}

static void init_object_list()
{   // Object might be a numerical value preceded by a preposition.
	// ie. "set camera tilt _to_ _25.5 degrees"
	// prepositions to look for :
	//		to by as 
	object_list.push_back( "tv" 	  	);		
	object_list.push_back( "hdmi"	);
	object_list.push_back( "you" 	);	
	object_list.push_back( "me"		);
}

static void init_word_lists()
{
	init_subject_list();
	init_verb_list();
	init_object_list();
	init_adjective_list();
}

void Init_Camera_Protocol()
{
    CAMERA_tcpip_WatchingOn 		= FALSE;		
    CAMERA_tcpip_SendingOn  		= FALSE;
    CAMERA_tcpip_SendingMuted  		= FALSE;		// we send zerod out CAMERA
    CAMERA_save_requested 			= FALSE;		// incoming or outgoing? 
    
    init_word_lists();
}


/**** ACTION INITIATORS:    (4 possible actions) *****/
void send_camera_file ( char* mFilename )
{	
    sending_camera_playback_file_fd  = fopen( mFilename, "r" );
    if (sending_camera_playback_file_fd == NULL)
    {
	nlp_reply_formulated=TRUE;
	sprintf ( NLP_Response, "Sorry, the camera file %s does not exist!", mFilename );
	printf( NLP_Response );
	return;
    }
    
	//printf( "Sending camera file over tcpip...\n");	
	CAMERA_tcpip_SendingOn = TRUE;

	nlp_reply_formulated=TRUE;
	sprintf ( NLP_Response, "Okay, I'm sending the camera file %s.", mFilename );
	printf( NLP_Response );
}

void send_camera()
{
	// Maybe want to verify the source IP address for security purposes
	// later on.  Not necessary now!
	//printf( "Sending my camera over tcpip...\n");

	// Fill in later...!  WaveHeader struct	
	CAMERA_tcpip_SendingOn = TRUE;

	nlp_reply_formulated=TRUE;
	strcpy(NLP_Response, "Okay, I will be sending you my camera.");
	printf("%s\n", NLP_Response);
}

void camera_watch(BOOL Save)
{	
	// Maybe want to verify the source IP address for security purposes
	// later on.  Not necessary now!
	//printf( "Watching for incoming tcpip camera...\n");
	CAMERA_tcpip_WatchingOn = TRUE;

	nlp_reply_formulated=TRUE;
	strcpy(NLP_Response, "Okay, I'm watching for your camera");
}

void camera_two_way()
{
	//printf( "Opening 2 way camera...\n");

	// Fill in with WaveHeader struct !
	//char* header=NULL;
	CAMERA_tcpip_SendingOn  = TRUE;
	CAMERA_tcpip_WatchingOn = TRUE;
	
	nlp_reply_formulated=TRUE;
	strcpy(NLP_Response, "Okay, we'll both see each other now.");
	printf("%s\n", NLP_Response);
}

void camera_cancel()
{
	//printf( "Cancelling camera connection.\n");
	CAMERA_tcpip_SendingOn = FALSE;
	CAMERA_tcpip_WatchingOn = FALSE;
	
	nlp_reply_formulated      = TRUE;
	strcpy(NLP_Response, "Okay, I am terminating our camera connection.");
	printf("%s\n", NLP_Response);
}


/*****************************************************************
Do the work of the Telegram :
return  -1	=> Not handled
		else number of extra bytes extracted from the mSentence buffer.
			- besides strlen(mSentence)! 
*****************************************************************/
int Parse_Camera_Statement( const char* mSentence, ServerHandler* mh )
{
	int retval=-1;

	dprintf("Parse_Camera_Statement\n");
	std::string* subject  	= extract_word( mSentence, &subject_list 	);	
	std::string* verb 		= extract_word( mSentence, &verb_list 	 	);
	std::string* object 	= extract_word( mSentence, &object_list  	);	
//	std::string* preposition= extract_word( mSentence, &preposition_list  );
	std::string* adjective	= extract_word( mSentence, &adjective_list  );	
//	int prepos_index      	= get_preposition_index( mSentence, preposition_list );
    diagram_sentence		( subject, verb, adjective, object, NULL );

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
                          (preposition_list.has_group_member("to") && (compare_word(object, "you")==0)) );
		  	
            int cond_2 = ((compare_word (adjective, "your") ==0) ||
		  				  ((preposition_list.has_group_member("to")==0) && (compare_word(object, "me")==0)) );
            
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
			cli_ipc_write_response( "Camera muted (electronically blocked)", "instant" );
		    retval = 0;		    
		}
		if ((compare_word(verb, "unmute") ==0) ||
			(compare_word(verb, "unblock") ==0))
		{
		    CAMERA_tcpip_SendingMuted = FALSE;	// Blocked, other side wont be able to see
			cli_ipc_write_response( "Camera live!", "instant" );
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
	if (retval>-1)  printf( "Parse_Camera_Statement done\n" );
	return retval;
}

