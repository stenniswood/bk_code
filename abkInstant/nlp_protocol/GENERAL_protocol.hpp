#ifndef _GENERAL_protocol_
#define _GENERAL_protocol_


#include "bk_system_defs.h"
#include "serverthread.hpp"
#include "nlp_sentence.hpp"
#include "serverthread.hpp"
#include "server_handler.hpp"


extern void*    m_verbal_focus;

int         find_subject				( char* mSubject  );

void        Init_General_Protocol		( );
int 		Parse_General_Statement		( Sentence& theSentence, ServerHandler* mh );

//const char* Parse_General_Statement		( const char*  mCommand, ServerHandler* mh );

//extern BOOL 	nlp_reply_formulated;
//extern char		NLP_Response[];

#endif