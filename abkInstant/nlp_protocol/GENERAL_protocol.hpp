#ifndef _GENERAL_protocol_
#define _GENERAL_protocol_


#include "bk_system_defs.h"
#include "serverthread.hpp"


extern void*    m_verbal_focus;
extern BOOL 	nlp_reply_formulated;
extern char		NLP_Response[];


void        Init_General_Protocol		( );
int         find_subject				( char* mSubject  );
const char* Parse_Statement				( const char*  mCommand, ServerHandler* mh );


#endif