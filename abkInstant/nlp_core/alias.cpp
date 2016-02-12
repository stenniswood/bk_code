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





Alias::Alias()
{

}

Alias::~Alias()
{

}

void	Alias::add_new( char*  mNew )
{
	m_synonyms.push_back( mNew );
}

void	Alias::add_new( string mNew )
{
	m_synonyms.push_back( mNew );
}

void	Alias::convert_all_to_lower( )
{
	list<string>::iterator iter = m_synonyms.begin();
	while ( iter != m_synonyms.end())
	{
		//convert_to_lower_case( char* str );
		iter++;
	}	
}

bool	Alias::is_group_member( char*  mLookupWord )
{
	if (mLookupWord==NULL) return false;	
	//string lookup(mLookupWord);
	
	list<string>::iterator iter = m_synonyms.begin();
	while ( iter != m_synonyms.end())
	{
		if (strcmp(iter->c_str(), mLookupWord)==0)
			return true;
		iter++;
	}
	return false;
}

/* Returns - 

*/
string* Alias::extract_member( char*  mSentence, bool mWholeWordOnly)
{
	string  Sentence(mSentence);
	list<string>::iterator iter = m_synonyms.begin();
	//printf(" Sentence=%s;  wl_count=%d;  (NOT_FOUND=%d)\n", Sentence.c_str(), m_word_list->size(), string::npos );

	// want to make this whole words only.  
	int i=0;
	for ( ; iter != m_synonyms.end(); iter++, i++)
	{
		// Get Lenth of test word in the list:
		int word_length = strlen( iter->c_str() );	
		
		// Find the test word:
		size_t pos = Sentence.find( (*iter).c_str() );
		while (pos != string::npos)
		{	
			//printf(" word_length=%d; %s\t", word_length, iter->c_str() );
			//printf(" pos=%d;   end=%d\n", pos, (Sentence.length()-word_length) );

			// Found, now verify whole word:
			if (pos==0)	// if at start of string, must have a space after the word.
			{				
				if (is_word_break( Sentence[word_length] ))
					return &(*iter);
					//printf("rejected 1\n");
			} 
			else if (pos==(Sentence.length()-word_length))		// if at end of string, must have aspace at start of word.
			{
				if (pos==0)	return &(*iter);	
				if (is_word_break(Sentence[pos-1]))
					return &(*iter);			
					//printf("rejected 2\n");
			}
			else		// must have space at start and end of word.
			{
				if (pos==0)	return &(*iter);	
				if ( is_word_break(Sentence[pos-1]) && is_word_break(Sentence[pos+word_length]) )
					return &(*iter);					
					//printf("rejected 3\n");					
			}
			// next find:
			pos = Sentence.find( (*iter), pos+1 );
		}
	}

	return (NULL);
}

/************************************************************************
	
************************************************************************/
WordGroup::WordGroup()
{
 
}
WordGroup::~WordGroup()
{
 
}
void	WordGroup::add_word( string&  mNewAlias )
{ 
	Alias tmp;
	tmp.add_new( mNewAlias );
	add_new(tmp);	
}		// could be simple string.

void	WordGroup::add_new( Alias&  mNewAlias )
{ 
	m_alias_groups.push_back( mNewAlias );
}

Alias*	WordGroup::get_alias( int mIndex )
{
   list<Alias>::iterator iter = m_alias_groups.begin();
   while (iter != m_alias_groups.end())
   {
		iter++;   // blahs
   }   	
	return &(*iter);
}

Alias*	WordGroup::get_alias( string& mAliasWordId )		// main word which describes the alias.
{
   list<Alias>::iterator iter = m_alias_groups.begin();
   while (iter != m_alias_groups.end())
   {
		iter++;   
   }   	
	return &(*iter);
}

bool	WordGroup::is_group_member( char*  mLookupWord, bool mWholeWordOnly )
{ 
   list<Alias>::iterator iter = m_alias_groups.begin();
   while (iter != m_alias_groups.end())
   {
   		if (iter->is_group_member(mLookupWord))
			return true;
		iter++;
   }   	
	return false;
}

string*	 WordGroup::extract_member( char*  mSentence, bool mWholeWordOnly )
{ 
   list<Alias>::iterator iter = m_alias_groups.begin();   
   while (iter != m_alias_groups.end())
   {
		iter++;   
   }   
}	

