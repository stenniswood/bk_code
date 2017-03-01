#ifndef _ROUTE_PROTOCOL_HPP_
#define _ROUTE_PROTOCOL_HPP_

#include "nlp_sentence.hpp"
#include "serverthread.hpp"

void Init_Routing_Protocol  ( );
int  Parse_Routing_Statement( Sentence& mSentence, ServerHandler* mh );



#endif
