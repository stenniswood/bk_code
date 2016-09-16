#include <stdlib.h>
#define BOOL unsigned char

#include "serverthread.hpp"
#include "vision_memory.h"
#include "nlp_vision_general_protocol.hpp"
#include "cal_screen.hpp"


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

