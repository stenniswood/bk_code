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
#include "protocol.h"
#include "devices.h"
#include "top_level_protocol.hpp"
#include "simulator_memory.h"
#include "visual_memory.hpp"
#include "vision_memory.h"
#include "client_to_socket.hpp"
#include "client_memory.hpp"
#include "sequencer_memory.hpp"

/* Sample sentences bkInstant would be able to respond to :
	What's your host name? what's your IP address?
	Start [your] camera show on [your display].
	Start [your] camera show on [my phone].
	
	Start [the] camera show on [my phone].
	Record [your] camera.
	Send camera video to harddrive.

	Pan your camera to the right [a little| about 10 degrees| a lot|all the way]
	Tilt your camera to the right [a little| about 10 degrees| a lot|all the way]
	
	We're going to start simple.  Each command will be statements as above.
	Not the numerous other ways of saying something:
		
	Let me see your camera on my phone.
	Send your camera over to the basement TV.
	Robot watch the camera in the bed room.  Notify me As soon as he wakes up.
	
*/

extern aVisualMemory avisual_mem;	// in main.cpp
extern SequencerIPC  sequencer_mem;		// see main.cpp

/* Auxiliary Apps could be anything like 
		xeyes 	- for vision (ie. "track red ball", "following my finger", etc.)
		home3D 	- for verbal object placement, creation, etc. (spatial temporal - "move scisors to living room")
		avisual	- for system commands ("show gyro for last 5 minutes", open "Audio Editor", etc)
*/
int pass_to_aux_apps( Sentence& theSentence, ServerHandler* mh )
{
	printf("Distributing telegram to shared memory.\n");    
	int result=0;
    // Pass to Simulator : 
    if ( is_sim_ipc_memory_available() )
    {
    	printf("Found simulator home3d shared memory. Delegating to its protocol.\n");
        /* New method is to pass the string into the shared memory.  Simulator
           software app will process the NLP and return a string.               
           Yes this means home3D will need files:
                    ThreeD_object_protocol,
                    nlp_sentence,
                    super_string,
                    (maybe more)
         */
        sim_ipc_write_sentence( theSentence.m_sentence.c_str() );
        result = sim_wait_for_response();
		// This should occur inside home3D (SIMULATOR)
		//    result = Parse_ThreeD_Statement( theSentence, mh );        
        if (result)
			return theSentence.m_sentence.length();
    }
    
    // Pass to "xeyes"    
    if (is_eyes_ipc_memory_available() )
    {
    	printf("Found XEYES shared memory. Delegating to its protocol.\n");    
    	eyes_write_client_command( theSentence.m_sentence.c_str() );
    	eyes_wait_for_acknowledgement();
    	//result = eyes_wait_for_response();    	    	
		return theSentence.m_sentence.length();
    }
    
    // Pass to "avisual"
    if ( avisual_mem.is_IPC_memory_available() )
    {
    	printf("Found AVISUAL shared memory. Delegating to its protocol.\n");    
    	avisual_mem.ipc_write_command_text( theSentence.m_sentence.c_str() );
    	// ipc_write_wait_for_response();
	    return (theSentence.m_sentence.length());
	}

    if ( sequencer_mem.is_IPC_memory_available() )
    {
    	printf("Found SEQUENCER shared memory. Delegating to its protocol.\n");    
    	sequencer_mem.write_sentence( theSentence.m_sentence.c_str() );
    	// ipc_write_wait_for_response();
	    return (theSentence.m_sentence.length());
	}	
}

const char* Parse_top_level_protocol( const char*  mSentence, ServerHandler* mh )
{
	if (mSentence==NULL) return mSentence; 
	printf( "Sentence:|%s|\n", mSentence );
    Sentence theSentence( mSentence );

    bool vr = theSentence.is_voice_response();
 	const char* end_of_telegram = mSentence + strlen(mSentence) +1 /*nullterminator*/;
	int result = -1;
	if (vr)
	{
		//printf("Prefilter determined a VoiceResponse!\n");
		if (ClientRequestPending)
		{
			// For those cases where the response comes from the other end:
			cli_ipc_write_response( mSentence, "instant" );	
			ClientRequestPending = false;
		}
		return end_of_telegram;
	}

	if (strcmp(mSentence, "I don't understand. Ignoring.")==0)
		return end_of_telegram;

	result = Parse_General_Statement( theSentence, mh );
	if (result>=0)
		printf("Parse_General_Statement - done\n");
	
	if (result<0) {
		result = pass_to_aux_apps( theSentence, mh );
	}
	if (result<0)		// Not handled:
	    mh->form_response("I don't understand. Ignoring.");

	return (end_of_telegram + result);
}


/* Objects the robot may have:
		camera
		pan/tilt servo
		buttons
		leds
		internet connection
		microphone
		speaker
		GPIO pin
 Actions:
		pan [camera]
		tilt [camera]		
		record
		start
		stop
		close
		play
		show
		graph						
*/
