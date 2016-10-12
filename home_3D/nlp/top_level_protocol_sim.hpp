//
//  top_level_protocol_sim.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 2/19/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef top_level_protocol_sim_hpp
#define top_level_protocol_sim_hpp

#include <stdio.h>
#include "nlp_sentence.hpp"
#include "serverthread.hpp"


void Init_Sim_Protocol  ( );

char*  Parse_Sim_Statement( char* mSentence, ServerHandler* mh );


#endif /* top_level_protocol_sim_hpp */
