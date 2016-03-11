#ifndef _GENERAL_protocol_
#define _GENERAL_protocol_

#include "bk_system_defs.h"
//typedef char BOOL;

void form_response(const char* mTextToSend);
extern BOOL nlp_reply_formulated;
extern char	NLP_Response[];


void 	Init_General_Protocol		( );
int 	find_subject				( char* mSubject  );
char* 	Parse_Statement				( char*  mCommand );




#endif
