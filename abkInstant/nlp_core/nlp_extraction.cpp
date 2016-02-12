#include <iostream>
#include <list>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <time.h> 
#include "protocol.h"
#include "devices.h"
#include "package_commands.h"
#include <termios.h>
#include <unistd.h>
#include <pthread.h>


using namespace std;



void diagram_sentence(	std::string* subject,
						std::string* verb, 		
						std::string* adjective,
						std::string* object,
						std::string* preposition)
{
	if (subject)
		printf("subject=%s;\t", 	subject->c_str() );
	if (verb)
		printf("verb=%s;\t", 		verb->c_str() );
	if (adjective)
		printf("adjective=%s;\t", 	adjective->c_str() );
	if (object)
		printf("object=%s;\t", 	 	object->c_str() );
	if (preposition)
		printf("preposition=%s;\t", preposition->c_str() );
	printf("\n");
}

bool is_word_break( char mTest )
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

/*****************************************************************
	extract_word()
*****************************************************************/
std::string* extract_word(char* mSentence, std::list<std::string> *m_word_list )
{
	std::string  Sentence(mSentence);
	std::list<std::string>::iterator iter = m_word_list->begin();
	//printf(" Sentence=%s;  wl_count=%d;  (NOT_FOUND=%d)\n", Sentence.c_str(), m_word_list->size(), std::string::npos );

	// want to make this whole words only.  
	int i=0;
	for ( ; iter != m_word_list->end(); iter++, i++)
	{
		// Get Lenth of test word in the list:
		int word_length = strlen( iter->c_str() );	
		
		// Find the test word:
		std::size_t pos = Sentence.find( (*iter).c_str() );
		while (pos != std::string::npos)
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
	//printf(" none found!\n" );
	return (NULL);
}


void extract_nlp_words( char* mFilteredSentence, 
						std::list<std::string>* msubject_list,
						std::list<std::string>* mverb_list,
						std::list<std::string>* mobject_list,
						std::list<std::string>* madjective_list,
						std::string** mResults, int mNumResults )
{
    if (mFilteredSentence==NULL)  return;
 	
/*    subject   = extract_word( mFilteredSentence, &msubject_list );
    verb 	  = extract_word( mFilteredSentence, &mverb_list );    
    object    = extract_word( mFilteredSentence, &mobject_list  );
    adjective = extract_word( mFilteredSentence, &madjective_list  );	
    diagram_sentence		( subject, verb, adjective, object );*/
}


int compare_word( std::string* mSubject, char* mWord )
{
	int result = -1;
	if ((mSubject) && (mWord))
		result = strcmp(mSubject->c_str(), mWord );
	return result;
}


