#ifndef _TOP_LEVEL_PROTOCOL_HPP_
#define _TOP_LEVEL_PROTOCOL_HPP_

#include "GENERAL_protocol.hpp"
#include "nlp_sentence.hpp"
#include "serverthread.hpp"
#include "server_handler.hpp"


const char* Parse_top_level_protocol( const char*  mSentence, ServerHandler* mh );

int pass_to_aux_apps( Sentence& theSentence, ServerHandler* mh );


#endif
