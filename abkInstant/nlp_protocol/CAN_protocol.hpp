#ifndef _CAN_PROTOCOL_HPP_
#define _CAN_PROTOCOL_HPP_


#include "serverthread.hpp"
#include "server_handler.hpp"
#include "nlp_sentence.hpp"


extern BOOL CAN_ListeningOn;	// if true we will be receiving CAN Traffic.
extern BOOL CAN_SendingOn;		// if true we will be sending   CAN Traffic.

void 	Init_CAN_Protocol();
int 	Parse_CAN_Statement( Sentence& theSentence, ServerHandler* mh );

int		start_amon();


#endif
