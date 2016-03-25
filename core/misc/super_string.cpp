//
//  super_string.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/17/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <string.h>
#include <string>
#include <vector>
#include "super_string.hpp"





SuperString& SuperString::operator=(const SuperString& mOp)
{
    string::operator=( *(string*)(&mOp) );
    return *this;
}

SuperString& SuperString::operator=(const char* mOp)
{
    string::operator=( mOp );
    return *this;
}

// HELPER FUNCTIONS:
void SuperString::trim_trail_space       ( )
{
    string whitespace = " \t";
    const auto strBegin = 0;
    if (strBegin == std::string::npos)
        return ; // no content
    
    const auto strEnd = find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    
    *(string*)this = substr(strBegin, strRange);
}
void SuperString::trim_leading_space     (  )
{
    string whitespace = " \t";
    const auto strBegin = find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ; // no content
    
    const auto strEnd = length();
    const auto strRange = strEnd - strBegin + 1;
    
    *(string*)this = substr(strBegin, strRange);
}
void SuperString::trim_duplicate_spaces  (  )
{ 
}      // between words of the sentence.
void SuperString::trim_spaces            ( )
{
    string whitespace = " \t";
    const auto strBegin = find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ; // no content
    
    const auto strEnd = find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    
    *(string*)this = substr(strBegin, strRange);
}      // leading,trailing, and no duplicates!

void SuperString::convert_to_lower_case  ( )
{
    int   i=0;
    char tmp;
    while ((*this)[i])
    {
        tmp = tolower( (*this)[i] );
        (*this)[i] = tmp;
        i++;
    }
}
bool SuperString::is_word_break          ( char mTest )
{
    if ((mTest==' ') || (mTest=='.') || (mTest==',') || (mTest==';') || (mTest=='?') || (mTest=='!'))
    {
        if (mTest=='.')
            printf("is_word_break\n");
        
        return true;
    }
    else
        return false;    
}
bool SuperString::is_punctuation_mark    ( char mTest )
{
    if ((mTest=='.') || (mTest==',') || (mTest=='?') || (mTest==';') || (mTest==':') || (mTest=='!'))
        return true;
    return false;
}     // ,.?;:!
void SuperString::remove_all_punctuations()
{
    size_t len = length();
    for (int c=0; c<len; c++)
        if (is_punctuation_mark((*this)[c]) )
            (*this).erase(c,1);
}

SuperString::SuperString()
:string()
{
}
SuperString::SuperString(char* mOriginal)
: string( mOriginal )
{
    
}
bool SuperString::char_is_a_number(char c)
{
    bool char_is_number = ((c=='0') || (c=='1') || (c=='2') || (c=='3') || (c=='4') ||
                           (c=='5') || (c=='6') || (c=='7') || (c=='8') || (c=='9') ||
                           (c=='.') || (c=='-'));
    return char_is_number;
}

bool SuperString::char_is_a_number_nd(char c)
{
    bool char_is_valid_ending = ((c=='n') || (c=='d') || (c=='t') || (c=='h') || (c=='s') || (c=='r'));
    return char_is_valid_ending;
}

bool SuperString::is_nth_word_a_number( int mIndex )
{
    if (mIndex>=m_split_words.size()) return false;
    printf("%d nth word=[%s]\n", mIndex, m_split_words[mIndex].c_str()  );
    
    size_t length = m_split_words[mIndex].length();
    for (int i=0; i<length; i++)
    {
        bool char_is_number       = char_is_a_number( m_split_words[mIndex][i] );
        if (char_is_number)
        { }
        else
            return false;
    }
    return true;
}

bool SuperString::is_nth_word_a_number_nd    ( int mIndex )
{
    if (mIndex>=m_split_words.size()) return false;
    
    printf("%d nth word=[%s]\n", mIndex, m_split_words[mIndex].c_str()  );
    
    size_t length = m_split_words[mIndex].length();
    for (int i=0; i<length; i++)
    {
        bool char_is_number       = char_is_a_number( m_split_words[mIndex][i] );
        bool char_is_valid_ending = char_is_a_number_nd( m_split_words[mIndex][i] );
        bool last_two_chars       = (i==(length-1)) || (i==(length-2));
        
        if ((char_is_number) || (char_is_valid_ending && last_two_chars))
        { }
        else
            return false;
    }
    return true;
}

int	 SuperString::split                  ( const char deliminator)
{
    //InPlace, replace deliminator's with zeros.
    //  and count how many sub strings with length >= 1 exist
    char* tmp = new char[length()+1];
    char* src_str_tmp = tmp;
    strcpy(src_str_tmp, c_str());
    char *last_src_str_tmp = src_str_tmp;
    
    while(*src_str_tmp)
    {
        if(*src_str_tmp == deliminator) {
            *src_str_tmp = 0;
            m_split_words.push_back(last_src_str_tmp);
            last_src_str_tmp = src_str_tmp+1;
        }
        src_str_tmp++;
    }
    //if
    m_split_words.push_back(last_src_str_tmp);
    delete[] tmp;
    return m_split_words.size(); 
}
void SuperString::trim_spaces_in_splits(  )      // leading,trailing, and no duplicates!
{
    for (int w=0; w<m_split_words.size(); w++)
    {
        m_split_words[w].trim_spaces();
    }
}

/*
    Searches all words in this class with all words in mWordList.
 */
int SuperString::any_one_word_found_in_sentence( SuperString& mWordList, int& mWhichWordIndex, int start_word )
{
    for (int w=start_word; w<m_split_words.size(); w++)
    {
        for (int sw=0; sw<mWordList.m_split_words.size(); sw++)
        {
            int result = mWordList.m_split_words[sw].compare(m_split_words[w]);
            if (result==0) 
            {
                mWhichWordIndex = sw;
                return w;
            }
        }
    }
    return -1;
}


/* do we want this in any order?  or sequential with words between the search words allowed. */
int SuperString::all_words_found_in_sentence             ( SuperString& mWordList )
{
    int start_word_index = -1;
    int sw=0;
    for (int sw=0; sw<m_split_words.size(); sw++)
    for (int w=0; w<m_split_words.size(); w++)
    {
        int result = mWordList.m_split_words[sw].compare(m_split_words[w]);
        if (result==0)
        {
            if (sw==0) start_word_index = w;
            sw++;
            if (sw>mWordList.m_split_words.size())
                return start_word_index;
        } else
            return -1;      // a word which was not in the sentence!
    }
    return -1;
}

/* even here words are allowed to slip between the search words. */
int SuperString::all_words_sequentially_found_in_sentence( SuperString& mWordList )
{
    int start_word_index = -1;
    int sw = 0;
    for (int w=0; w<m_split_words.size(); w++)
    {
        int result = mWordList.m_split_words[sw].compare(m_split_words[w]);
        if (result==0)
        {
            if (sw==0) start_word_index = w;
            sw++;
            if (sw==mWordList.m_split_words.size())
                return start_word_index;
        }
    }
    return -1;
}


