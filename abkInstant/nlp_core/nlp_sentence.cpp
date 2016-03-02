//
//  nlp_sentence.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/16/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <stdlib.h>
#include <string>

#include "nlp_sentence.hpp"
#include "string_util.h"
#include "prefilter.hpp"
#include "nlp_extraction.hpp"



// Common to all Protocols :
WordGroup 	preposition_list;


Sentence::Sentence()
{
    m_raw_sentence  = NULL;
    m_sentence      = NULL;
    m_voice_response= false;
    m_question      = false;
    m_sentence_length = 0;
    m_tag           = NULL;
    m_words         = NULL;
    m_num_words = 0;
}
Sentence::Sentence(char* mSentence)
{
    set(mSentence);
}


Sentence::~Sentence()
{
    if (m_raw_sentence)
        delete m_raw_sentence;
}

/* Return:	true => everything okay.
 false => This is a Voice Response and not to be processed! */
bool Sentence::prefilter_text(  )
{
    char* result = strstr(m_sentence, "VR:");
    if (result != NULL)
    {
        m_voice_response = true;
        return false;
    }

    m_voice_response = false;
    trim_leading_space   ( m_sentence);
    trim_trail_space     ( m_sentence );
    convert_to_lower_case( m_sentence );
    
    // REMOVE PUNCTUATION
    m_sentence_length = strlen(m_sentence);                // re-eval after removing spaces.
    m_question = (m_sentence[m_sentence_length-1]=='?');
    bool remove = is_punctuation_mark(m_sentence[m_sentence_length-1]);
    if (remove)
    {
        m_sentence[m_sentence_length-1] = 0;
        m_sentence_length--;
    }
    printf ("Prefiltered sentence:|%s|\n", m_sentence );
    
    return true;
}


void Sentence::set( char*  mNew )
{
    // COPY THE RAW Sentence:
    size_t length   = strlen(mNew);
    m_raw_sentence  = new char[length];
    m_sentence      = new char[length];
    m_sentence      = new char[length];
    split_holder    = new char[length];
    strcpy(m_raw_sentence, mNew );
    strcpy(m_sentence,     mNew );

    // Prefilter:
    bool okay = prefilter_text();
    if (okay) {
        strcpy(split_holder, m_sentence);
        m_words = split( split_holder, ' ', &m_num_words );
        //printf("m_words|%s\n%s\n%s|", m_words[0], m_words[1], m_words[2]);
    }
}
/*
 return:    0 =>  Not found
            otherwise 1 indexed value of the first occurence of the word.
*/
int Sentence::is_found_in_sentence( const char* mSearchWord, bool mOrItsPlural )
{
    char plural[80];
    if (mOrItsPlural)
    {
        // Add an 's' to the word :
        strcpy(plural, mSearchWord);
        size_t length = strlen(plural);
        plural[length] = 's';
        plural[length+1] = 0;
    }
    
    int resultP = -1;
    for (int w=0; w<m_num_words; w++)
    {
        int result = strcmp( m_words[w], mSearchWord );
        if (result==0)
            return w+1;

        if (mOrItsPlural) {
            result = strcmp( m_words[w], plural );
            if (result==0)
                return w+1;
        }
    }
    return 0;
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
    int result = strcmp( m_words[m_last_search_found_index], mSearchWord );
    if (result==0)
    {
        m_last_search_found_index++;
        return 1;
    }
    return 0;
} // returns word index of first occurence.

/* Note: does not increment! */
int  Sentence::next_word_is_a_number( )
{
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
    
    int found = (strcmp(m_words[0], "are")==0);
//    remove_chars(m_raw_sentence, start_of_word, start_of_next_word);
    // Get start of word and next word:
//    int start_of_word = strstr();
 //   int start_of_next_word = strstr();
 //   remove_chars( start_of_word, start_of_next_word );

    
    return false;
}

/* Looks for inverted subject, verb:  such as "Are you going to help him?"  */
bool Sentence::check_verbage_question ( )
{
    m_verbage_question = false;
    m_verbage_question = (strcmp(m_words[0], "am")==0)   && (strcmp(m_words[1], "i")==0);         // personal identity  am I ugly?
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "will")==0) && (strcmp(m_words[1], "i")==0);       // possibility        will i be able to drive the car?
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "can")==0)  && (strcmp(m_words[1], "i")==0);        // permission
    
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "are")==0)  && (strcmp(m_words[1], "you")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "will")==0) && (strcmp(m_words[1], "you")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "can")==0)  && (strcmp(m_words[1], "you")==0);
    if (!m_verbage_question)
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "is")==0)  && (strcmp(m_words[1], "he")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "will")==0) && (strcmp(m_words[1], "he")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "can")==0)  && (strcmp(m_words[1], "he")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "is")==0)  && (strcmp(m_words[1],  "she")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "will")==0) && (strcmp(m_words[1], "she")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "can")==0)  && (strcmp(m_words[1], "she")==0);

    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "are")==0)  && (strcmp(m_words[1],  "we")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "will")==0) && (strcmp(m_words[1], "we")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "can")==0)  && (strcmp(m_words[1], "we")==0);
        
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "are")==0)  && (strcmp(m_words[1], "they")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "will")==0) && (strcmp(m_words[1], "they")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "can")==0)  && (strcmp(m_words[1], "they")==0);


    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "What's")==0)  && (strcmp(m_words[1], "is")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "What")==0)  && (strcmp(m_words[1], "are")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "Who" )==0)  && (strcmp(m_words[1], "are")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "What")==0)  && (strcmp(m_words[1], "are")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "When")==0)  && (strcmp(m_words[1], "are")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "Where")==0)  && (strcmp(m_words[1], "are")==0);

    // Quantity:
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "how")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "how much")==0)  && (strcmp(m_words[1], "is")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "how many")==0);
    if (!m_verbage_question)     m_verbage_question = (strcmp(m_words[0], "how many")==0)  && (strcmp(m_words[1], "are")==0);
    
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
char* Sentence::get_nth_word( int mIndex )
{
    if (mIndex>=m_num_words)  return NULL;
    
    return m_words[mIndex];
}

bool  Sentence::is_nth_word_a_number    ( int mIndex )
{
    if (mIndex>m_num_words) return false;
    printf("%d nth word=[%s] %lx\n", mIndex, m_words[mIndex], (long int)m_words[mIndex] );
    
    size_t length = strlen( m_words[mIndex]);    
    for (int i=0; i<length; i++)
    {
        if (
            (m_words[mIndex][i]=='0') || (m_words[mIndex][i]=='1') || (m_words[mIndex][i]=='2') ||
            (m_words[mIndex][i]=='3') || (m_words[mIndex][i]=='4') || (m_words[mIndex][i]=='5') ||
            (m_words[mIndex][i]=='6') || (m_words[mIndex][i]=='7') || (m_words[mIndex][i]=='8') ||
            (m_words[mIndex][i]=='9') || (m_words[mIndex][i]=='.') )
        {}
        else
            return false;
    }
    return true;
}

float Sentence::get_nth_word_as_a_number( int mIndex )
{
    float result = atof(m_words[mIndex] );
    return result;
}

// Use to find prepositions positions (or any word) :
int Sentence::get_word_index( char* mWord, int mSentenceStartingWordIndex )
{
    // FOR ALL WORDS IN THE SENTENCE:
    for (int i=0; i<m_num_words; i++)
    {
        size_t result = strcmp(m_words[i] , mWord);
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
    
    preposition_list.add_word( "to"   );
    preposition_list.add_word( "from" );
    preposition_list.add_word( "as" );
    preposition_list.add_word( "by" );
    preposition_list.add_word( "for");
    preposition_list.add_word( "in" );
}


