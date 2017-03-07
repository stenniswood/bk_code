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
#include "super_string.hpp"

using namespace std;
extern WordGroup 	preposition_list;
void init_preposition_list();


class Sentence
{
public:
    Sentence ();
    Sentence (const char* mSentence);
    ~Sentence();

    void			set                     ( const char*  mNew );
    void			set_word_pointer        ( int  mWordIndex );
    int             get_word_pointer        ( );
    int             get_number_words        ( )  { return (int)m_sentence.m_split_words.size(); };

    bool            is_voice_response       ( );            // VR: preceeds the text
    bool            is_question             ( );
    bool            check_verbage_question  ( );            // result stored internally
    bool            remove_verbage_question_words( );       //

    int             are_exactly_found_in_sentence( SuperString& Tsearches, bool mOrItsPlural );
    int             regex_find              ( string  mRegexpression  );
    
    int             is_found_in_sentence    ( const char* mSearchWord, bool mOrItsPlural = false ); // returns word index of first occurence.
    int             are_found_in_sentence   ( const char* Tsearches,   bool mOrItsPlural = false ); // returns word index of first occurence.
    int             are_found_in_sentence   ( SuperString& Tsearches,   bool mOrItsPlural = false ); // returns word index of first occurence.

    int             any_one_word_found_in_sentence( SuperString& mWordList ); // returns word index of first occurence.
    int             next_word_matches       ( const char* mSearchWord, bool mOrItsPlural = false ); // returns word index of first occurence.
    bool            next_word_is_a_number   ( ); // returns word index of first occurence.
    int             number_of_numbers_present();    // counts how many words in the sentence are numbers.
    
    const char*     get_nth_word            ( int mIndex );
    int             get_word_index          ( const char* mWord, int mSentenceStartingWordIndex=0 );

    bool            is_nth_word_a_number_nd ( int mIndex );     // a number with st, nd, rd, or th after it.  ie. 1st, 2nd, 3rd, 4th, etc.
    bool            is_nth_word_a_number    ( int mIndex );
    float           get_nth_word_as_a_number( int mIndex );
    bool            prefilter_text          ( );
    void            restore_reduced         ( );
    void            reduce_sentence         ( vector<int> mWordIndices );

    char*         m_raw_sentence;
    SuperString   m_sentence;
    SuperString   m_reduced_sentence;       // after some words have been "used up", this will hold the remaining portion. (for multiple menu items for instance.
    
    bool        m_question;
    bool        m_verbage_question;     // 
    bool        m_voice_response;
	bool		m_eat_words;			// after a good match.
	bool		m_always_convert_to_lowercase;
	
    int*        m_tag;
    int         m_last_search_found_index;  // private var used for next word matches
    
    vector<int> answers;
    vector<int> remove_wi;
};



#endif /* nlp_sentence_hpp */
