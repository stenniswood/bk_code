//
//  scene_nlp.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/14/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include "scene_nlp.hpp"
#include "physical_world.h"
#include "scene.hpp"


void Init_Scene_Protocol ( )
{
    
}

int get_scene_id( Sentence& mSentence )
{
    return 1;
}

int  Parse_Scene_Statement ( Sentence& mSentence, string* mh )
{
    int     retval  = -1;
    string  regexpression;
    mSentence.restore_reduced();

    regexpression = "(new scene|erase (scene|everything)|start over)";
    if (mSentence.m_sentence.regex_find(regexpression))
    {
        theWorld.erase_scene();
    }

    regexpression = "(load scene)";
    if (mSentence.m_sentence.regex_find(regexpression))
    {
        int scene_id = get_scene_id( mSentence );
        theScene.load( scene_id );
    }

    regexpression = "(save scene)";
    if (mSentence.m_sentence.regex_find(regexpression))
    {
        int scene_id = get_scene_id( mSentence );
        theScene.save( scene_id );
    }

    if (retval>-1)  printf( "Parse_Scene_Statement done\n" );
    return retval;
}


//sql_load_scene( scene_id );