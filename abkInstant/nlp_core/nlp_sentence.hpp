//
//  nlp_sentence.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/16/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef nlp_sentence_hpp
#define nlp_sentence_hpp

#include <stdio.h>
#include <string>
#include "alias.hpp"

using namespace std;

extern WordGroup 	preposition_list;
void init_preposition_list();



class Sentence
{
public:
    Sentence();
    Sentence(char* mSentence);
    
    ~Sentence();
    
    void			set                     ( char*  mNew );
    void			set_word_pointer        ( int  mWordIndex );
    int             get_word_pointer        ( );
    
    bool            is_voice_response       ( );           // VR: preceeds the text.
    bool            is_question             ( );
    bool            check_verbage_question  ( );            // result stored internally.
    bool            remove_verbage_question_words( );       //


    int             is_found_in_sentence    ( const char* mSearchWord, bool mOrItsPlural = false ); // returns word index of first occurence.
    int             next_word_matches       ( const char* mSearchWord, bool mOrItsPlural = false ); // returns word index of first occurence.
    int             next_word_is_a_number   ( ); // returns word index of first occurence.

    
    char*           get_nth_word( int mIndex );
    int             get_word_index( char* mWord, int mSentenceStartingWordIndex=0 );
    
    bool            is_nth_word_a_number    ( int mIndex );
    float           get_nth_word_as_a_number( int mIndex );
    bool            prefilter_text( );
    
    
    char*       m_raw_sentence;
    char*       m_sentence;
    int         m_sentence_length;      // the filtered length;
    bool        m_question;
    bool        m_verbage_question;     // 
    bool        m_voice_response;

    char*       split_holder;
    char**      m_words;
    int*        m_tag;
    int         m_num_words;

    int         m_last_search_found_index;  // private var used for next word matches
};



#endif /* nlp_sentence_hpp */
