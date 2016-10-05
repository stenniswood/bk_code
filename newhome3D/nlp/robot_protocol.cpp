//
//  robot.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/6/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef _ROBOT_PROTCOL_HPP_
#define _ROBOT_PROTCOL_HPP_
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <string.h>

#include "robot.hpp"
#include "robot_protocol.hpp"
#include "robot_arms_protocol.hpp"
#include "robot_legs_protocol.hpp"
#include "ThreeD_object_protocol.hpp"
#include "create_object_nlp.hpp"
#include "scene_nlp.hpp"


void InitRobotTopLevelProtocol( )
{
    Init_Robot_Legs_Protocol();
    Init_Robot_Arms_Protocol();
    Init_Create_Protocol    ();
    Init_ThreeD_Protocol    ();
    Init_Scene_Protocol     ();
}

const char* RobotTopLevelProtocol( Sentence& mSentence, string* mh )
{
    //if (mSentence==NULL) return mSentence;
    printf( "Sentence:|%s|\n", mSentence.m_sentence.c_str() );

    bool vr = mSentence.is_voice_response();
    const char* end_of_telegram = mSentence.m_sentence.data() + mSentence.m_sentence.length() +1 /*nullterminator*/;
    int result =-1;
    if (vr)
    {
        return end_of_telegram;
    }
    //if (strcmp(mSentence.m_sentence, "I don't understand. Ignoring.")==0)
    //return end_of_telegram;

    result = Parse_Robot_Legs_Statement( mSentence, mh );
    if (result>=0)          return (end_of_telegram + result);
        
    result = Parse_Robot_Arms_Statement( mSentence, mh );
    if (result>=0)          return (end_of_telegram + result);
    
    result = Parse_Create_Statement( mSentence, mh );
    if (result>=0)          return (end_of_telegram + result);

    Parse_ThreeD_Statement         ( mSentence, mh );
    if (result>=0)          return (end_of_telegram + result);
    
    result = Parse_Scene_Statement ( mSentence, mh );
    if (result>=0)          return (end_of_telegram + result);

    
    // Not handled :
    *mh = "I don't understand. Ignoring.";
    //nlp_response_formulated = true;
    return end_of_telegram;
}


#endif
