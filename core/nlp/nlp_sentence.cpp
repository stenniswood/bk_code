//
//  nlp_sentence.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/16/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <stdlib.h>
#include <string>
#include "global.h"
#include "nlp_sentence.hpp"
#include "string_util.h"
#include "prefilter.hpp"
#include "nlp_extraction.hpp"
#include "global_funcs.h"


#define Debug 0


// Common to all Protocols :
//WordGroup 	preposition_list;

Sentence::Sentence()
{
    m_raw_sentence  = NULL;
    m_sentence      = NULL;
    m_voice_response= false;
    m_question      = false;
    m_tag           = NULL;
}
Sentence::Sentence(const char* mSentence)
{
    set(mSentence);
}

Sentence::~Sentence()
{
    if (m_raw_sentence) {
        delete m_raw_sentence;
        m_raw_sentence = NULL;
    }
}

/* Return:	true => everything okay.
 false => This is a Voice Response and not to be processed! */
bool Sentence::prefilter_text(  )
{
	char pre[] = "VR:";
	char* str = (char*)m_sentence.c_str();
    char* result = strstr( str, pre );
    if (result != NULL)
    {
        m_voice_response = true;
        return false;
    }
    m_voice_response = false;
    
    m_sentence.trim_spaces();
    m_sentence.convert_to_lower_case();
    
    // REMOVE PUNCTUATION
    m_question = (m_sentence[m_sentence.length()-1]=='?');
    m_sentence.remove_all_punctuations();

    //m_sentence_length = strlen(m_sentence);                // re-eval after removing spaces.
/*    bool remove = is_punctuation_mark(m_sentence[m_sentence_length-1]);
    if (remove)
    {
        m_sentence[m_sentence_length-1] = 0;
        m_sentence_length--;
    }*/
    Dprintf ("Prefiltered sentence:|%s|\n", m_sentence.c_str() );
    return true;
}

void Sentence::restore_reduced()
{
    m_reduced_sentence = m_sentence;
}

void Sentence::set( const char*  mNew )
{
    // COPY THE RAW Sentence :
    size_t length   = strlen(mNew)+1;
    m_raw_sentence  = new char[length];
    
    strcpy(m_raw_sentence, mNew );
    m_sentence      = m_raw_sentence;
    
    // Prefilter:
    bool okay = prefilter_text();
    if (okay) {
        m_sentence.split( ' ' );
    }
    m_reduced_sentence = m_sentence;
    m_reduced_sentence.split(' ');
}

/*  Remove words from the m_reduced_sentence :
INPUT  :  vector<int>     indices into the m_reduced_sentence.
RETRUN :  m_reduced_sentence is modified.
 */
void Sentence::reduce_sentence( vector<int> mWordIndices )
{
    m_reduced_sentence.reduce_string(mWordIndices);
}

int Sentence::regex_find( string  mRegexpression )
{
    return m_sentence.regex_find(mRegexpression, &answers, &remove_wi );
    
    // Now remove words from m_reduced_sentence
    //for (auto x:remove_wi)
}

/*
   No Multiple words (such as "go to") should be passed in!!
   Return:    0 =>  Not found
              otherwise 1 indexed value of the first occurence of the word.
*/
int Sentence::is_found_in_sentence( const char* mSearchWord, bool mOrItsPlural )
{
    if (mSearchWord==NULL)
        return 0;
    
    vector<int> remove_wi;
    SuperString sw ( mSearchWord );
    
    // Also search for it's plural:
    int match_count = m_reduced_sentence.is_found_in_sentence( mSearchWord, 0, &remove_wi, mOrItsPlural );
    if (match_count>0) {
        set_word_pointer( remove_wi[0]);
        reduce_sentence ( remove_wi );
    }
    return match_count;
}

// returns:  0 =>  Not found
//                 word index of first occurence
int Sentence::are_found_in_sentence( const char* Tsearches, bool mOrItsPlural )
{
    SuperString tmp;
    tmp = Tsearches;
    //size_t length = tmp.length();
    tmp.convert_to_lower_case ();
    tmp.split(' ');
    tmp.trim_spaces_in_splits ();
    return are_found_in_sentence( tmp, mOrItsPlural );
}

/*************************************************************************
 Searches for all words in Tsearches against the m_reduced sentence.
 
 return:   0 =>  Not found
 otherwise 1 indexed value of the first occurence of the word.
 ************************************************************************/
int Sentence::are_found_in_sentence( SuperString& Tsearches, bool mOrItsPlural )
{
    // When Multiple words are passed in:  such as "go to", the result must match all of them.
    vector<int> remove_wi;

    // Find in the sentence :
    int match_count = m_reduced_sentence.all_words_sequentially_found_in_sentence( Tsearches, 0, &remove_wi );
    if(match_count>0)
    {
        set_word_pointer( remove_wi[0] );
        reduce_sentence(remove_wi);
    }
    return match_count;
}

/*************************************************************************
 Searches for all words in Tsearches against the m_reduced sentence.
 
 return:   0 =>  Not found
           number of matched words (Note this is a change!)
                                        //otherwise 1 indexed value of the first occurence of the word
 ************************************************************************/
int Sentence::are_exactly_found_in_sentence( SuperString& Tsearches, bool mOrItsPlural )
{
    // When Multiple words are passed in:  such as "go to", the result must match all of them.
    vector<int> remove_wi;

    // Find in the sentence :
    int match_count = m_reduced_sentence.all_words_exact_match( Tsearches, 0, &remove_wi );
    if (match_count>0)
    {
        set_word_pointer(remove_wi[0]);
        reduce_sentence(remove_wi);
    }
    return match_count;
}

// Returns: 0 not found;
//          word index of first occurence;
int  Sentence::any_one_word_found_in_sentence( SuperString& mWordList )
{
    vector<int> removals;
    int match_count = m_sentence.any_one_word_found_in_sentence( mWordList, 0, &removals );
    if (match_count) {
        set_word_pointer( removals[0] );
        reduce_sentence ( removals    );
    }
    return match_count;
}

void Sentence::set_word_pointer( int  mWordIndex )
{
    m_last_search_found_index = mWordIndex;
}

int  Sentence::get_word_pointer ( )
{
    return m_last_search_found_index;
}
/* Note: Increments pointer! */
int  Sentence::next_word_matches( const char* mSearchWord, bool mOrItsPlural )
{
    int result = m_sentence.m_split_words[m_last_search_found_index].compare( mSearchWord );
    if (result==0)
    {
        m_last_search_found_index++;
        return 1;
    }
    return 0;
} // returns word index of first occurence

/* Note: does not increment! */
bool  Sentence::next_word_is_a_number( )
{
    if (++m_last_search_found_index >= m_sentence.m_split_words.size()) return false;
    return is_nth_word_a_number(m_last_search_found_index);
} // returns word index of first occurence.

void    remove_chars( char* mString, int mStart, int mStart2 )
{
    strcpy (mString+mStart, mString+mStart2);
}

/* to assist in restating the question as an answer */
bool Sentence::remove_verbage_question_words( )
{
    // Want to remove it from the raw sentence, and then reporocess.
    vector<int> removals;
    
    int found = (m_reduced_sentence.m_split_words[0].compare("are")==0);
    if (!found)
        found = (m_reduced_sentence.m_split_words[0].compare("have")==0);
    if (!found)
        found = (m_reduced_sentence.m_split_words[0].compare("would")==0);
    if (!found)
        found = (m_reduced_sentence.m_split_words[0].compare("can")==0);
    if (!found)
        found = (m_reduced_sentence.m_split_words[0].compare("how")==0);
    if (found) {
        removals.push_back( 0 );
        m_reduced_sentence.reduce_string( removals );
    }
    return found;
}

/* Looks for inverted subject, verb:  such as "Are you going to help him?"  */
bool Sentence::check_verbage_question ( )
{
    m_verbage_question = false;
    m_verbage_question = (m_sentence.m_split_words[0].compare("am")==0)   && (m_sentence.m_split_words[1].compare( "i")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare("will")==0) && (m_sentence.m_split_words[1].compare( "i")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "can")==0)  && (m_sentence.m_split_words[1].compare( "i")==0);
    
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "are")==0)  && (m_sentence.m_split_words[1].compare( "you")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "will")==0) && (m_sentence.m_split_words[1].compare( "you")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "can")==0)  && (m_sentence.m_split_words[1].compare( "you")==0);

    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "is")==0)  && (m_sentence.m_split_words[1].compare( "he")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "will")==0) && (m_sentence.m_split_words[1].compare( "he")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "can")==0)  && (m_sentence.m_split_words[1].compare( "he")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "is")==0)  && (m_sentence.m_split_words[1].compare(  "she")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "will")==0) && (m_sentence.m_split_words[1].compare( "she")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "can")==0)  && (m_sentence.m_split_words[1].compare( "she")==0);

    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "are")==0)  && (m_sentence.m_split_words[1].compare(  "we")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "will")==0) && (m_sentence.m_split_words[1].compare( "we")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "can")==0)  && (m_sentence.m_split_words[1].compare( "we")==0);
        
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "are")==0)  && (m_sentence.m_split_words[1].compare( "they")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "will")==0) && (m_sentence.m_split_words[1].compare( "they")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "can")==0)  && (m_sentence.m_split_words[1].compare( "they")==0);

    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "What's")==0)  && (m_sentence.m_split_words[1].compare( "is")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "What")==0)  && (m_sentence.m_split_words[1].compare( "are")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "Who" )==0)  && (m_sentence.m_split_words[1].compare( "are")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "What")==0)  && (m_sentence.m_split_words[1].compare( "are")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "When")==0)  && (m_sentence.m_split_words[1].compare( "are")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "Where")==0)  && (m_sentence.m_split_words[1].compare( "are")==0);

    // Quantity:
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "how")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "how much")==0)  && (m_sentence.m_split_words[1].compare( "is")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "how many")==0);
    if (!m_verbage_question)     m_verbage_question = (m_sentence.m_split_words[0].compare( "how many")==0)  && (m_sentence.m_split_words[1].compare( "are")==0);
    
    return m_verbage_question;
}


bool Sentence::is_question ( )
{
    return m_question;
}
bool Sentence::is_voice_response( )           // VR: preceeds the text.
{
    return m_voice_response;
}
const char* Sentence::get_nth_word( int mIndex )
{
    if (mIndex>=m_sentence.m_split_words.size())  return NULL;
    
    return m_sentence.m_split_words[mIndex].c_str();
}

bool  Sentence::is_nth_word_a_number_nd    ( int mIndex )
{
//    if (mIndex>m_sentence.m_split_words.size())
//        return false;
    return m_sentence.is_nth_word_a_number_nd(mIndex);
}

bool  Sentence::is_nth_word_a_number    ( int mIndex )
{
//    if (mIndex>m_sentence.m_split_words.size())
//        return false;
    return m_sentence.is_nth_word_a_number(mIndex);
}

float Sentence::get_nth_word_as_a_number( int mIndex )
{
    float result = atof(m_sentence.m_split_words[mIndex].c_str() );
    return result;
}

// counts how many words in the sentence are numbers.
int Sentence::number_of_numbers_present()
{
    bool isNum=false;
    int count=0;
    for (int w=0; w<m_sentence.m_split_words.size(); w++)
    {
        isNum = is_nth_word_a_number(w);
        if (isNum)
            count++;
    }
    return count;
}

// Use to find prepositions positions (or any word) :
int Sentence::get_word_index( const char* mWord, int mSentenceStartingWordIndex )
{
    // FOR ALL WORDS IN THE SENTENCE:
    for (int i=0; i<m_sentence.m_split_words.size(); i++)
    {
        size_t result = m_sentence.m_split_words[i].compare( mWord );
        if (result == 0)
        {
            printf("Found at [%d]\n", i );
            return (int)i;
        }
    }
    return (-1);
}


void init_preposition_list()
{   // Object might be a numerical value preceded by a preposition.
    // help to establish order of operations:
    //    ie.  subtract 21 from 101.
    //      or what is 101 subtract 21
    
/*    preposition_list.add_word( "to"   );
    preposition_list.add_word( "from" );
    preposition_list.add_word( "as" );
    preposition_list.add_word( "by" );
    preposition_list.add_word( "for");
    preposition_list.add_word( "in" ); */
}


