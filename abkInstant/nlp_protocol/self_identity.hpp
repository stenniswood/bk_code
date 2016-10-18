//
//  self_identity.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/16/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef self_identity_hpp
#define self_identity_hpp

#include <stdio.h>
#include "nlp_sentence.hpp"
#include "serverthread.hpp"

void Init_Self_Identity_Protocol  ( );
int  Parse_Self_Identity_Statement( Sentence& mSentence, ServerHandler* mh );




#endif /* self_identity_hpp */
