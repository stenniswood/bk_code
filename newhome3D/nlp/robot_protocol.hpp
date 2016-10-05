//
//  robot.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/6/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef robot_hpp
#define robot_hpp

#include <stdio.h>
#include "nlp_sentence.hpp"


void InitRobotTopLevelProtocol  ( );

const char* RobotTopLevelProtocol ( Sentence& mSentence, string* mh );


#endif /* robot_hpp */
