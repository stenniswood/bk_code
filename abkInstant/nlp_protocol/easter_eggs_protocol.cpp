//
//  easter_eggs_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/18/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#include "easter_eggs_protocol.hpp"
#include "GENERAL_protocol.hpp"



void Init_EasterEggs_Protocol  ( )
{
    
}
int  Parse_EasterEggs_Statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval =-1;
    mSentence.restore_reduced();
    
    int foundS = mSentence.is_found_in_sentence( "what" ) || mSentence.is_found_in_sentence( "what's" );
    int foundA = mSentence.is_found_in_sentence( "meaning" );
    int foundO = mSentence.is_found_in_sentence( "life" );
    bool cond1 = (foundS && foundA && foundO);
    if (cond1)
    {
        mh->form_response( "The meaning of my life is to calculate anything and everything that makes life easier for humans." );
        retval = 0;
    }
    
    foundS = mSentence.is_found_in_sentence( "how" ) && mSentence.is_found_in_sentence( "spell" );
    if (foundS)
    {
        int spell_index = mSentence.get_word_index("spell");
        string word_to_spell = mSentence.get_nth_word(spell_index+1);
        string spelling = word_to_spell + ", is spelled ";
        for (int i=0; i<word_to_spell.length(); i++)
        {
            spelling += word_to_spell[i]; spelling += ", ";
        }
        mh->form_response( spelling.c_str() );
        retval = 0;
    }
    return retval;
}
