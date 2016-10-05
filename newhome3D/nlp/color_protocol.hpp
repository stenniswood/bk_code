//
//  color_protocol.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 7/8/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef color_protocol_hpp
#define color_protocol_hpp

#include <stdio.h>
#include <string>
#include "nlp_sentence.hpp"

std::string create_color_regex( );

int Parse_color_statement( Sentence& mSentence, std::string& mReponse );


#endif /* color_protocol_hpp */
