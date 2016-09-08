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
#include "tk_colors.h"

std::string create_color_regex( );


void   init_color_map		( );
int    Parse_color_statement( Sentence& mSentence, std::string& mReponse );
Color  Get_Spoken_color		( int index);


#endif /* color_protocol_hpp */
