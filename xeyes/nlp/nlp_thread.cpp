#include <stdlib.h>
#define BOOL unsigned char

#include "serverthread.hpp"
#include "vision_memory.h"
#include "nlp_vision_general_protocol.hpp"
#include "cal_screen.hpp"
#include "vision_logger.hpp"


void* nlp_handler( void* mconnfd )
{
	const char* 	next_sentence = NULL;
	ServerHandler*  mh = new ServerHandler();
	while (1)
	{
		if (eyes_new_command_available())
		{		
			next_sentence = Parse_Statement( (char*)ipc_memory_eyes->client_command, mh );
			eyes_acknowledge_command();

			printf("NLP Response= %s\n", mh->NLP_Response.c_str() );
			eyes_write_server_event( mh->NLP_Response );
			mh->NLP_Response[0] = ' ';
			mh->NLP_Response[1] = ' ';
			text_to_speech_festival( mh->NLP_Response.c_str() );
		}
	}
}

void run_one( const char* mStatement, ServerHandler* mh  )
{	
	const char* next_sentence = Parse_Statement( (char*)mStatement, mh );

	mh->NLP_Response[0] = ' ';
	mh->NLP_Response[1] = ' ';
	printf("NLP Response= %s\n\n", mh->NLP_Response.c_str() );
	//text_to_speech_festival( mh->NLP_Response.c_str() );
}

void nlp_test( )
{
	const char* 	next_sentence = NULL;
	ServerHandler*  mh = new ServerHandler();

	sql_logger.connect_to_logger_db();

	run_one("when did you last see Steve?", mh );
	run_one("how long ago did you see Steve?", mh );
	run_one("how long was he around?", mh );
	run_one("how many times did you see Steve today?", mh );
	run_one("how many times did you see Steve yesterday?", mh );
	run_one("how many times did you see Steve 3 days ago?", mh );
	
	run_one("how many times were you activated today?",   mh );
	run_one("how many times were you deactivated today?", mh );
	run_one("how long have you been active?",   		  mh );
	run_one("how many hours were you active yesterday?",   mh );
	run_one("how many hours were you active 3 days ago?",   mh );	
	run_one("how many hours were you deactive yesterday?",   mh );

	run_one("was anyone with Steve?", 					 mh );
	run_one("Did you see anyone you didn't know today?", mh );
	run_one("Did you meet anyone new today?", 			 mh );
	run_one("Did you meet anyone new yesterday?", 		 mh );
	run_one("Did you meet anyone new while i was gone?", mh );
	run_one("Show me the video sequence of the unknown/unrecognized person.", mh );		// context of the last person and time mentioned.
	
	run_one("Show me the pictures.", mh );		// context of the last person and time mentioned.
	exit(1);  
}

/*
 Later add - and before that?  after that time but before i got back did you see anyone?
*/
