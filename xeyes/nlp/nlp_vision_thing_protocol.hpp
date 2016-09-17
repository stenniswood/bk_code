#ifndef _NLP_VISION_THING_PROTOCOL_HPP_
#define _NLP_VISION_THING_PROTOCOL_HPP_


#include "nlp_sentence.hpp"
#include "serverthread.hpp"


int 	Get_thing_symantic_id ( Sentence& theSentence );
int		process_thing_request ( Sentence& theSentence, int question_id, string& Response );

int		Get_future_thing_statement_symantic_id( Sentence& theSentence );
int		process_future_thing_request		  ( int question_id 	  );

char*	Parse_vision_things_Statement( Sentence& theSentence, ServerHandler* mh );


#endif
