//
//  positioner_2d.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/28/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "nlp_sentence.hpp"
#include "serverthread.hpp"

#include "positioner_2d.hpp"


#define Debug 0

class Rectangle;

/*
 raise the check box
 lower the list box
 put below the [___]
 push to the left of [___]
 
 */

/* MOVEMENT ACTIONS - THESE ACTUALLY DO THE WORK */
void relocate_higher  ( Rectangle& mRect, float mDeltaAmount )
{
    
}
void relocate_lower   ( Rectangle& mRect, float mDeltaAmount )
{
    
}
void relocate_to_right( Rectangle& mRect, float mDeltaAmount )
{
    
}
void relocate_to_left ( Rectangle& mRect, float mDeltaAmount )
{
    
}

/* MOVEMENT LOCATOR HELPERS - THESE HELP FIND THE DESIRED LOCATION */
/* Source is the one left of */
void get_left_of ( Rectangle& mDest, Rectangle& mSource, float mDeltaAmount )
{
    //const float padding = 5;
/*    float left = mDest.left;
    mSource.left  = mDest.left  - mSource.get_width();
    mSource.right = mDest.left - padding;
*/
}
void get_right_of ( Rectangle& mRect, float mDeltaAmount )
{
    //const float padding = 5.0;
    /*    float left = mDest.left;
     mSource.left  = mDest.left  - mSource.get_width();
     mSource.right = mDest.left - padding;
     */
    
}
void get_top_of ( Rectangle& mRect, float mDeltaAmount )
{
    //const float padding = 5.0;
    /*    float left = mDest.left;
     mSource.left  = mDest.left  - mSource.get_width();
     mSource.right = mDest.left - padding;
     */
}
void get_bottom_of ( Rectangle& mRect, float mDeltaAmount )
{
    //const float padding = 5.0;
    /*    float left = mDest.left;
     mSource.left  = mDest.left  - mSource.get_width();
     mSource.right = mDest.left - padding;
     */
}


int Parse_2D_Positioning_Statement( Sentence& mSentence, ServerHandler* mh )
{
    //static long last_object_id=-1;
    int retval=-1;
    
    if (Debug)  printf("Parse_2D_positioning_Statement\n");
    mSentence.restore_reduced();
    string tmp = "move";
    if ( mSentence.m_reduced_sentence.regex_find( tmp ) )
    {
        tmp = "(up)";
        if ( mSentence.m_reduced_sentence.regex_find( tmp ) )
        {
            
        }
        tmp = "(down)";
        if ( mSentence.m_reduced_sentence.regex_find( tmp ) )
        {
            
        }
    }
    
    tmp="(move|lift|lower|)";
    if ( mSentence.m_reduced_sentence.regex_find( tmp ) )
    {
        
    }
 
    return retval;
}

