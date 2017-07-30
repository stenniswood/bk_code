//
//  ThreeD_object_protocol.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/20/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef _UserDevs_protocol_hpp
#define _UserDevs_protocol_hpp

#include <stdio.h>
#include "nlp_sentence.hpp"
#include "serverthread.hpp"



void Init_UserDevs_Protocol  ( );
int  Parse_UserDevs_Statement( Sentence& mSentence, ServerHandler* mh );



#endif /* ThreeD_object_protocol_hpp */

