//
//  super_string.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/17/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <string>
#include <vector>
#include <math.h>
#include <regex>
#include <iostream>
#include "bk_system_defs.h"
#include "super_string.hpp"


#define Debug 1


SuperString& SuperString::operator=(const SuperString& mOp)
{
    string::operator=( *(string*)(&mOp) );
    m_regex             = mOp.m_regex;
    regex_matches       = mOp.regex_matches;
    m_wordlist_choices  = mOp.m_wordlist_choices;         // For any & all word lists.

    m_split_words                = mOp.m_split_words;
    m_split_words_original_index = mOp.m_split_words_original_index;
    return *this;
}

SuperString& SuperString::operator=(const char* mOp)
{
    string::operator=( mOp );
    return *this;
}
SuperString& SuperString::append_float(float mOp)
{
    char tmp[12];
    sprintf(tmp, "%6.2f", mOp);
    string::operator+=(tmp);
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

vector<float> SuperString::convert_to_float()
{
    float tmp;
    vector<float> vf;
    for (int i=0; i<m_split_words.size(); i++)
    {
        tmp = stof( m_split_words[i].c_str() );
        vf.push_back( tmp );
        i++;
    }
    return vf;
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
SuperString::SuperString(string mOriginal)
:string (mOriginal)
{
    split(' ');
}
SuperString::SuperString( const char* mOriginal )
: string( mOriginal )
{
    //split(' ');
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
    if (m_split_words[mIndex].length()==0) return false;
    dprintf("%d nth word=[%s]\n", mIndex, m_split_words[mIndex].c_str()  );
    
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
    
    dprintf("%d nth word=[%s]\n", mIndex, m_split_words[mIndex].c_str()  );
    
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
/* 
    Splits a string of "(abc|def|ghi)" removing the ( and ).
 */
void SuperString::split_wordlist( )  // string remains intact.  non destructive!
{
    split('|');
    trim_spaces();
    
    size_t words            = m_split_words.size();
    size_t last_word_length = m_split_words[words-1].length();
    
    m_split_words[0].erase( 0, 1 );                        // Remove (
    m_split_words[words-1].erase( last_word_length-1 );    // Remove )
}

int	 SuperString::split                  ( const char deliminator)
{
    //InPlace, replace deliminator's with zeros.
    //  and count how many sub strings with length >= 1 exist
    m_split_words.clear();
    m_split_words_original_index.clear();
    char* tmp = new char[length()+1];
    char* src_str_tmp = tmp;
    strcpy(src_str_tmp, c_str());
    char *last_src_str_tmp = src_str_tmp;
    int index = (int)(src_str_tmp-tmp);
    
    while(*src_str_tmp)
    {
        if(*src_str_tmp == deliminator)  {
            *src_str_tmp = 0;
            m_split_words.push_back(last_src_str_tmp);
            index = (int)(last_src_str_tmp-tmp);
            m_split_words_original_index.push_back( index );      // starting char index
            last_src_str_tmp = src_str_tmp+1;
        }
        src_str_tmp++;
    }
    m_split_words.push_back(last_src_str_tmp);
    index = (int)(last_src_str_tmp-tmp);
    m_split_words_original_index.push_back( index );      // starting char index
    
    delete[] tmp;
    return (int)m_split_words.size();
}

void SuperString::trim_spaces_in_splits(  )      // leading,trailing, and no duplicates!
{
    for (int w=0; w<m_split_words.size(); w++)
    {
        m_split_words[w].trim_spaces();
    }
}

/*
 Return:    0  ==> Not Found
            0..n    first index.
 */
int SuperString::is_found_in_sentence( const char* mWord, int start_word, vector<int>* remove_wi, bool mOrItsPlural )
{
    int  match_count = 0;
    char plural[80];
    if (remove_wi)    remove_wi->clear();
    if (start_word<0) start_word=0;

    strcpy(plural, mWord);
    size_t len = strlen(mWord);
    if (len<79) {
        plural[len]   = 's';
        plural[len+1] =  0 ;
    }
    // For all words in this string :
    for (int w=start_word; w < m_split_words.size(); w++)
    {
        int result = m_split_words[w].compare(mWord);
        int result_plural = m_split_words[w].compare(plural);
        if ((mOrItsPlural) && (result_plural==0))
            result=0;
        if (result==0)
        {
            match_count++;
            if (remove_wi)
                remove_wi->push_back(w);
        }
    }
    return match_count;
}
/* 
 Such as of "(all|everything|order)"
 */
int SuperString::compare_wordlist( SuperString mWord, int& mSelectedWord, int start_word, vector<int>* remove_wi )
{
//    mWord.erase( 0,1     );             // Remove (
//    mWord.erase( mWord.length()-1 );    // Remove )
//    mWord.split('|');
    int match_count = 0;
    mSelectedWord   =-1;
    
    for (int wl=0; wl < mWord.m_split_words.size(); wl++)
    {
        match_count += is_found_in_sentence( mWord.m_split_words[wl].c_str(), start_word, remove_wi );
        if ((mSelectedWord==-1) && (match_count))
            mSelectedWord = wl;
    }
    return match_count;
}

void SuperString::replace_all( char mChar, char mDest )
{
	char text[255];
	strcpy (text, c_str() );
    char* ptr = strchr( text, mChar );
    while (ptr)
    {
        *ptr = mDest;
        ptr = strchr( ptr+1, mChar );
    }
}

int SuperString::regex_find( string&  mRegexpression, vector<int>* answers, vector<int>* remove_wi )
{
    m_regex = mRegexpression;
    regex regex  ( m_regex );
    regex_search ( *this, regex_matches, regex );

    if (Debug) {
        for (auto x:regex_matches)
            std::cout << x <<" "<< std::endl;
    }
    return (int)regex_matches.size();
}

/* After performing a regex_find() with an expression that has "()" in it.  */
int SuperString::print_matches(  )
{
    size_t   valid_responses = regex_matches.size();
	printf("%d Matches \n", valid_responses);
    for (int i=0; i<valid_responses; i++)
        printf( " %s \n", regex_matches[i] );
    return (int)valid_responses;
}

/* After performing a regex_find() with an expression that has "()" in it.  */
int SuperString::extract_wordlist_results(  )
{
    //size_t valid_response = regex_matches.size();
    for (int i=0; i<regex_matches.size(); i++)
        m_wordlist_choices.push_back( (int)regex_matches.position(i) );
    
    return (int)regex_matches.size();
}

/* After performing a regex_find() with an expression that has "()" in it.  */
SuperString SuperString::extract_results(  )
{
    SuperString ss;
    for (int m=0; m<regex_matches.size(); m++)
        ss.m_split_words.push_back( substr(regex_matches.position(m), regex_matches[m].length()) );
    
    return ss;
}

int SuperString::count_wordlists(  )
{
    int count = 0;
    size_t pos = 0;
    while ( (pos = find('(', pos)) )
        count++;

    return count;
}




/* May be composed of "cancel (all|everything|order) "
Return:     0 ==> at least one word did not match
            1+ ==> pattern matches.
 */
int SuperString::regex_found_in_sentence( SuperString mWord, int start_word, vector<int>* answers, vector<int>* remove_wi )
{
    int match_count=0;
    int sw = 0;
    SuperString express;
    size_t length;
    int wordlist;
    int result = 0;
    int compare_result;
    int selected_index=-1;
    if (answers)
        answers->clear();

    // For all words in this sentence :
    for (int w=start_word; w<m_split_words.size(); w++)
    {
        length   = mWord.m_split_words[w].size();
        wordlist = ((mWord.m_split_words[w][0] == '(') && (mWord.m_split_words[w][length-1] == ')'));

        // Compare with one of the WordList words :
        if (wordlist) {
            mWord.split_wordlist();
            result = compare_wordlist( mWord.m_split_words[w], selected_index, start_word );        // any one word found
            if (answers)
                answers->push_back  ( selected_index );
            if (remove_wi)
                remove_wi->push_back( selected_index );
        } else {
            compare_result = mWord.m_split_words[sw].compare( m_split_words[w] );
            if (compare_result==0)
                result = 1;
            else
                result = 0;
        }
        if (result)
        {
            match_count++;
            if (remove_wi)
                remove_wi->push_back(w);
            sw++;
        }
        // if no match, just keep going (fill words are allowed)
    }
    //if ( match_count == mWord.size() )
    return match_count;
    //return 0;
}

/*
 Searches all words in this class with all words in mWordList

 start_word  -  word within the sentence
 Return:     0  = Not found
             x  = Number of words that match
             mWhichWordIndex - word index within the sentence of first match
 */
int SuperString::any_one_word_found_in_sentence( SuperString& mWordList, int start_word, vector<int>* remove_wi )
{
    int match_count  = 0;
    if (remove_wi)    remove_wi->clear();
    if (start_word<0) start_word=0;

    // For all words in this string :
    for (int w=start_word; w<m_split_words.size(); w++)
    {
        // for all words in the parameter : 
        for (int sw=0; sw<mWordList.m_split_words.size(); sw++)
        {
            int result = mWordList.m_split_words[sw].compare(m_split_words[w]);
            if (result==0)
            {
                match_count++;
                if (remove_wi)
                    remove_wi->push_back(w);
            }
        }
    }
    return match_count;
}


/*  All words, any order.

   Return:  0 ==>   at least one word in mWordList was not found (No match)
                    number of matches found :
                        total = sum( word_i * occurence_i ) for all i )
 */
int SuperString::all_words_found_in_sentence_any_order( SuperString& mWordList, int start_word, vector<int>* remove_wi  )
{
    int  sw               = 0;
    bool found            = false;
    int  match_count      = 0;
    if (remove_wi)    remove_wi->clear();
    if (start_word<0) start_word=0;
    
    // For all words in wordlist :
    for (sw=0; sw < mWordList.m_split_words.size(); sw++)
    {
        found = false;
        // For all words in sentence...
        for (int w=start_word; w < m_split_words.size(); w++)
        {
            int result = mWordList.m_split_words[sw].compare( m_split_words[w] );
            if (result==0)
            {
                match_count++;
                found = true;
                if (remove_wi)
                    remove_wi->push_back(w);
            }
        }
        if (found==false)
            return 0;      // a word which was not in the sentence!
    }
    return match_count;
}

/*  Here words are allowed to slip between the search words.
    Return:    0       ==>  at least one word was not found.
               [0..n]  ==>  number of matching words.
 */
int SuperString::all_words_sequentially_found_in_sentence( SuperString& mWordList, int start_word, vector<int>* remove_wi )
{
    int  sw               =  0;
    int  match_count      =  0;
    size_t wordlist_size  =  mWordList.m_split_words.size();
    if (remove_wi)    remove_wi->clear();
    if (start_word<0) start_word=0;
    
    // For all words in this sentence :
    for (int w=start_word; w<m_split_words.size(); w++)
    {
        // Compare with one of the WordList words :
        int result = mWordList.m_split_words[sw].compare( m_split_words[w] );
        if (result==0)
        {
            // Match Found :
            match_count++;
            if (remove_wi)
                remove_wi->push_back(w);
            sw++;
            if (sw >= wordlist_size)
                break;
        }
        // if no match, just keep going (fill words are allowed)
    }
    if (match_count == wordlist_size)
        return match_count;
    return 0;
}

int SuperString::all_words_exact_match( const char*  mWordList, int start_word, vector<int>* remove_wi )
{
    SuperString wl(mWordList);
    return all_words_exact_match(wl, start_word, remove_wi );
}

/*  Even here words are allowed to slip between the search words.
    Return:     -1      ==>  Not found
                [0..n]  ==>  First word index of match.
 */
int SuperString::all_words_exact_match( SuperString& mWordList, int start_word, vector<int>* remove_wi )
{
    int sw               =  0;
    int match_count      =  0;
    bool all_consequative= true;
    size_t wordlist_size = mWordList.m_split_words.size();
    if (remove_wi)    remove_wi->clear();
    if (start_word<0) start_word=0;

    // For all words in this sentence :
    for (int w=start_word; w<m_split_words.size(); w++)
    {
        // Compare with one of the WordList words:
        int result = mWordList.m_split_words[sw].compare( m_split_words[w] );
        if (result==0)
        {
            // Match Found :
            match_count++;
            if (remove_wi)
                remove_wi->push_back(w);
            sw++;
        } else {
            if (remove_wi->size() > 0)      // We started matching words and
                all_consequative = false;   //   found a non match.  This routine doesn't allow words in between.  Must be consequative.
        }
    }
    if ((match_count == wordlist_size) && (all_consequative))
        return match_count;
    return 0;
}
/************** END OF SEARCH ROUTINES **********************************************************/


// in Sentence :
int SuperString::count_occurences( SuperString& mWord )
{
    int count=0;
    for (int w=0; w < m_split_words.size(); w++)
        for (int sw=0; sw < mWord.m_split_words.size(); sw++)
        {
            if (mWord.m_split_words[sw].compare( m_split_words[w]) )
                count++;
        }
    return count;
}

// in Sentence :
int SuperString::get_nth_word_position( int mWordIndex )
{
    return m_split_words_original_index[mWordIndex];
}

void SuperString::print()
{
    printf("%s\n", this->c_str() );
}

/*  Remove words from the m_reduced_sentence :
 INPUT  :  vector<int>     indices into the m_reduced_sentence.
 RETRUN :  m_reduced_sentence is modified.
 */
void SuperString::reduce_string( vector<int> mWordIndices )
{
    vector<int>::iterator iter = mWordIndices.begin();
    while ( iter != mWordIndices.end() )
    {
        // We remove from the m_reduced_sentence string.  Will re-split the words after.
        size_t  r_size = size() + 1;
        int index      = get_nth_word_position( *iter );
        //int index2      = m_reduced_sentence.get_nth_word_position( *iter+1 );
        size_t length  = m_split_words[*iter].length();
        if (index+length <= r_size)
        {
            if (index+length <= r_size)
                if ( (*this)[index+length]==' ' )
                    length +=1;
            
            erase( index, length );
            trim_spaces();
            // Reduce the word indices after this one:
            for (int wi=0; wi<mWordIndices.size(); wi++)
            {
                if (mWordIndices[wi]>*iter)
                    mWordIndices[wi]--;
            }
        }
        split(' ');      // resplit.
        iter++;
    }
}
