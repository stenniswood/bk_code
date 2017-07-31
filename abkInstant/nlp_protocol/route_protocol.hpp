#ifndef _ROUTE_PROTOCOL_HPP_
#define _ROUTE_PROTOCOL_HPP_

#include "nlp_sentence.hpp"
#include "serverthread.hpp"
#include "server_handler.hpp"

void Init_Routing_Protocol  ( );

int  Parse_Cancel_Routing_Statement( char* mSentence, ServerHandler* mh );
int  Parse_Routing_Statement( Sentence& mSentence, ServerHandler* mh );
  


#endif
