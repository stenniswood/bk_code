//
//  easter_eggs_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/18/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <vector>

#include "easter_eggs_protocol.hpp"
#include "GENERAL_protocol.hpp"



void Init_EasterEggs_Protocol  ( )
{
    
}
int  Parse_EasterEggs_Statement( Sentence& mSentence )
{
    int retval =-1;
    int foundS = mSentence.is_found_in_sentence( "what" ) || mSentence.is_found_in_sentence( "what's" );
    int foundA = mSentence.is_found_in_sentence( "meaning" );
    int foundO = mSentence.is_found_in_sentence( "life" );
    bool cond1 = (foundS && foundA && foundO);
    if (cond1)
    {
        form_response( "The meaning of life is to calculate anything and everything that makes life easier for humans." );
        retval = 0;
    }
    return retval;
}
