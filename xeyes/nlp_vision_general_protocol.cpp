/*	Process Verbal Commands : 

When did you last see David?
Where you active yesterday around noon?
How long was he here for?

Watch my eyes.
Follow/Track Janet. Do what she asks.

*/
#include <iostream>
#include <list>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <time.h> 
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
//#include "bk_system_defs.h"
#include "nlp_vision_general_protocol.hpp"

#define FALSE 0
#define TRUE  1
#define BOOL unsigned char

#include "serverthread.hpp"
//#include "nlp_extraction.hpp"
//#include "prefilter.hpp"
#include "nlp_sentence.hpp"
#include "vision_logger.hpp"


// Response language:
BOOL 			nlp_reply_formulated = FALSE;
char			NLP_Response[255];
string 			NLP_Response_str;


/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
//using namespace std;
void*      m_verbal_focus=0;

void Init_General_Protocol( )
{
    //Init_vision_protocol();
//    Init
}

extern int connfd;



/*****************************************************************
Do the work of the Telegram :
PARAMS : 
	mSentence 		- pointer to a buffer which begins with text (data may follow end of string).
	mbegin_index	- index within the mSentence buffer (up to 5MB)

Return :   Pointer to next part of the telegram (mSentence).
 
//return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
const char* Parse_Statement(const char*  mSentence, ServerHandler* mh )
{	
	if (mSentence==NULL) return mSentence; 
	printf( "Sentence:|%s|\n", mSentence );
    Sentence theSentence( mSentence );

    bool vr = theSentence.is_voice_response();
 	const char* end_of_telegram = mSentence + strlen(mSentence) +1 /*nullterminator*/;
	int result =-1;	
	if (vr)
	{
		//printf("Prefilter determined a VoiceResponse!\n");
		return end_of_telegram;
	}

	if (strcmp(mSentence, "I don't understand. Ignoring.")==0)
		return end_of_telegram;
    
	// Person presence Question:
    bool foundS = theSentence.is_found_in_sentence( "When" ) && 
    		 theSentence.is_found_in_sentence( "last" ) && 
    		 theSentence.is_found_in_sentence( "see" );
	if (foundS)
	{
		string name = "Stephen";
		sql_logger.form_response__last_time_i_saw( name, NLP_Response_str );
		
	}		    

    foundS = theSentence.is_found_in_sentence( "show" ) && 
    		 theSentence.is_found_in_sentence( "last" ) && 
    		 theSentence.is_found_in_sentence( "objects" );

	//result = Parse_Camera_Statement( mSentence, mh );
	//if (result>=0)			return (end_of_telegram + result);
		
	//result = Parse_HMI_Statement   ( mSentence, mh ); 	/* ie mouse, keyboard, PS3 controller, etc. */
	//if (result>=0)          return (end_of_telegram + result);

	// Not handled:
    mh->form_response("I don't understand. Ignoring.");
	return end_of_telegram;
}
