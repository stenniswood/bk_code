#include <iostream>
#include <list>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#include "nlp_extraction.hpp"
#include "alias.hpp"
#include "string_util.h"



Word::Word()
{
    m_synonyms      = new vector<string>;
    m_all_extracted = new vector<string>;
}

Word::Word(const char* mWordsString)
{
    m_synonyms      = new vector<string>;
    m_all_extracted = new vector<string>;
    
    add_words( mWordsString );
}

Word::Word(const Word& mCopy)
{
    // Can't just copy the pointers, b/c we never know the scope of the one being passed in, it may go away and delete it's vectors.

    // So we have to allocate new ones,
    m_synonyms      = new vector<string>;
    m_all_extracted = new vector<string>;
    
    // and then copy the data.
    for (int i=0; i<mCopy.m_synonyms->size(); i++)
        m_synonyms->push_back( (*mCopy.m_synonyms)[i] );

    for (int i=0; i<mCopy.m_all_extracted->size(); i++)
        m_all_extracted->push_back( (*mCopy.m_all_extracted)[i] );
    
}

Word::~Word()
{
    delete m_synonyms;
    delete m_all_extracted;
}

/* Add a whole bunch of words at once in a single string:  add_words("times, multiply, multiplied by");
 */
void Word::add_words(const char* mWordsList)      // ',' deliminated
{
    char tmp_str[1024];
    int num_words = 0;
    strcpy( tmp_str, mWordsList );
    
    char** m_words = split( tmp_str, ' ', &num_words );
    for (int w=0; w<num_words; w++)
        add_new( m_words[w]);
    
    delete m_words;
}

void	Word::add_new( char*  mNew )
{
	m_synonyms->push_back( mNew );
}

void	Word::add_new( string mNew )
{
	m_synonyms->push_back( mNew );
}

void	Word::convert_all_to_lower( )
{
	vector<string>::iterator iter = m_synonyms->begin();
	while ( iter != m_synonyms->end())
	{
		//convert_to_lower_case( char* str );
		iter++;
	}	
}

bool	Word::is_synonym( const char*  mLookupWord )
{
	if (mLookupWord==NULL) return false;	
	//string lookup(mLookupWord);
	
	vector<string>::iterator iter = m_synonyms->begin();
	while ( iter != m_synonyms->end())
	{
		if (strcmp(iter->c_str(), mLookupWord)==0)
			return true;
		iter++;
	}
	return false;
}
bool Word::is_synonym( string  mLookupWord )
{
	return is_synonym( mLookupWord.c_str() );
}

int  Word::get_num_synonyms()
{
    return (int)m_synonyms->size();
}

int	Word::number_times_found()	// within the sentence.
{
	return (int)m_all_extracted->size();
}

bool is_whole_word(string& mSentence, size_t mWordStartIndex, int mWord_length )
{
	// Found, now verify whole word:
	if (mWordStartIndex==0)	// if at start of string, must have a space after the word.
	{				
		if (is_word_break( mSentence[mWord_length] ))
			return true;
	} 
	// if at end of string, must have a space at start of word:
	else if (mWordStartIndex==(mSentence.length()-mWord_length))
	{
		if (mWordStartIndex==0)	return true;	// only word in sentence?
		if (is_word_break(mSentence[mWordStartIndex-1]))
			return true;			
	}
	else		// must have space at start and end of word.
	{
		if ( is_word_break(mSentence[mWordStartIndex-1]) && is_word_break(mSentence[mWordStartIndex+mWord_length]) )
			return true;
	}
	return false;
}

/* 
Returns - 

*/
int Word::evaluate_sentence( const char*  mSentence, bool mWholeWordOnly)
{
	string  Sentence(mSentence);
    //size_t  size = m_all_extracted->size();
    m_all_extracted->clear();
	vector<string>::iterator iter = m_synonyms->begin();
	//printf(" Sentence=%s;  wl_count=%ld;  (NOT_FOUND=%ld)\n", Sentence.c_str(), m_synonyms->size(), string::npos );

	int i=0;
	for ( ; iter != m_synonyms->end(); iter++, i++)
	{
		// Get Lenth of test word in the list:
        string tmp_str = *iter;
		size_t word_length = strlen( tmp_str.c_str() );
		
		// Find the test word:
		size_t pos = Sentence.find( (*iter).c_str() );
		
		while (pos != string::npos)
		{	
			//printf(" word_length=%d; %s\t", word_length, iter->c_str() );
			//printf(" pos=%d;   end=%d\n", pos, (Sentence.length()-word_length) );
			bool whole_word = is_whole_word( Sentence, pos, (int)word_length );
			if (mWholeWordOnly==false)			
				m_all_extracted->push_back( *iter );
			else if (whole_word)
				m_all_extracted->push_back( *iter );
				
			// next find:
			pos = Sentence.find( (*iter), pos+1 );
		}
	}

	return (int)(m_all_extracted->size());
}

/************************************************************************
	
************************************************************************/

WordGroup::WordGroup()
{
    m_Words         = new vector<Word>;
    m_all_extracted = new vector<Word>;
}

WordGroup::WordGroup(const WordGroup& mWG)
{
    m_Words         = new vector<Word>;
    m_all_extracted = new vector<Word>;
    
    // and then copy the data.
    for (int i=0; i<mWG.m_Words->size(); i++)
        m_Words->push_back( (*mWG.m_Words)[i] );
    
    for (int i=0; i<mWG.m_all_extracted->size(); i++)
        m_all_extracted->push_back( (*mWG.m_all_extracted)[i] );
    
}
WordGroup::~WordGroup()
{
    delete m_Words;
    delete m_all_extracted;
}

void	WordGroup::add_word( const char*  mNewWord )		// could be simple string.
{
    static string tmp;
    tmp = mNewWord;
    add_word( tmp );
}
void	WordGroup::add_word( string&  mNewWord )
{ 
	Word tmp;
	tmp.add_new( mNewWord );
	add_new(tmp);
    //printf("size=%d  wg_size=%ld\n", tmp.get_num_synonyms(), m_Words->size() );

    /*int size = m_Words->front().get_num_synonyms();
    Word* ptr = get_Word(0);    
    printf("Word ptr=%ld  synynom_ptr=%ld\n", ptr, ptr->m_synonyms );
    int size2 = ptr->get_num_synonyms(); */
}

void	WordGroup::add_new( Word&  mNewWord )
{ 
	m_Words->push_back( mNewWord );
}


Word*	WordGroup::get_Word( int mIndex )
{
   vector<Word>::iterator iter = m_Words->begin();
   while (iter != m_Words->end() && (mIndex))
   {
		iter++;
        mIndex--;
   }
	return &(*iter);
}

Word*	WordGroup::get_Word( string& mWordWordId )		// main word which describes the Word.
{
   vector<Word>::iterator iter = m_Words->begin();
//   while (iter != m_Words->end() && ( mWordWordId.compare(*iter) ))
   {
//		iter++;
        // BLAH!!
   }   	
	return &(*iter);
}

bool	WordGroup::has_group_member( char*  mLookupWord, bool mWholeWordOnly )
{ 
   vector<Word>::iterator iter = m_Words->begin();
   while (iter != m_Words->end())
   {
   		if (iter->is_synonym(mLookupWord))
			return true;
		iter++;
   }   	
	return false;
}

int	 WordGroup::evaluate_sentence( const char*  mSentence, bool mWholeWordOnly )
{ 
	int result=0;
    m_all_extracted->clear();        // Start Over with new Sentence!
    vector<Word>::iterator iter = m_Words->begin();
   while (iter != m_Words->end())
   {
   		result = iter->evaluate_sentence( mSentence, mWholeWordOnly );
   		if (result)
   			m_all_extracted->push_back( *iter );
		iter++;
   }
   return (int)m_all_extracted->size();
}	

int WordGroup::was_found_in_sentence( string mWordWord )
{
	vector<Word>::iterator iter = m_all_extracted->begin();
	while (iter != m_all_extracted->end())
	{
		if ( iter->is_synonym( mWordWord ) )
			return 1;
        iter++;
	}
	return 0;
}

// within the sentence.
int	WordGroup::number_times_found()
{
   return (int)m_all_extracted->size();
}


/*****************************************************************************************************
 
 ****************************************************************************************************/

Vocabulary::Vocabulary()
{
    m_wordgroups = new vector<WordGroup>;
}

Vocabulary::~Vocabulary()
{
    delete m_wordgroups;
}

void Vocabulary::add_new( WordGroup mNewWord )
{
    m_wordgroups->push_back(mNewWord);
}

