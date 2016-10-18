//
//  calendar_protocol.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/17/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef calendar_protocol_hpp
#define calendar_protocol_hpp

#include <stdio.h>
#include "nlp_sentence.hpp"
#include "serverthread.hpp"


void Init_Calendar_Protocol  ( );

int  Parse_Calendar_Statement( Sentence& mSentence, ServerHandler* mh );



#endif /* calendar_protocol_hpp */
