//
//  robot_arms_protocol.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/18/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef robot_arms_protocol_hpp
#define robot_arms_protocol_hpp

#include <stdio.h>
#include "serverthread.hpp"

void Init_Robot_Arms_Protocol();
int Parse_Robot_Arms_Statement( Sentence& mSentence, ServerHandler* mh );


#endif /* robot_arms_protocol_hpp */
