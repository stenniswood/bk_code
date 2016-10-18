//
//  math_protocol2.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 9/20/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef math_protocol2_hpp
#define math_protocol2_hpp

#include <stdio.h>
#include "nlp_sentence.hpp"
#include "serverthread.hpp"


void Init_Math_Protocol  ( );

int  Parse_Math_Statement( Sentence& mSentence, ServerHandler* mh );

#endif /* math_protocol2_hpp */
