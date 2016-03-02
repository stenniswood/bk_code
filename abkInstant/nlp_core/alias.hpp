


#ifndef _Word_
#define _Word_
 
//#include <string>
#include <list>
#include <vector>

using namespace std;


/* This was the original simplistic way of making a vocabulary:

  However, there are many words with the same meaning.  Synonyms.
  Therefore this class will encapsulate a synonym group - Word.
  Every word added should have the same meaning.  No differences will be distinguished.
  
*/

class Word
{
public:
	Word();
	Word(const char* mWordsString);
    Word(const Word& mCopy);      // With no copy constructor, the default one wont copy our vectors!  And since the var was on the stack, the pointers to m_synynoms would be garbage.
	~Word();

    void            add_words(const char* mWordsList);      // ',' deliminated
	void			add_new( char*  mNew );
	void			add_new( string mNew );
	void			convert_all_to_lower( );
    int             get_num_synonyms();
	
	bool			is_synonym( const char*  mLookupWord );
	bool			is_synonym( string  mLookupWord );

	int				evaluate_sentence( char*  mSentence, bool mWholeWordOnly=true );	
	//string*			extract_member ( char*  mSentence, bool mWholeWordOnly=true );	
	int				number_times_found();	// within the sentence.
	

	vector<string>*  	m_synonyms;
	vector<string>*  	m_all_extracted;		// all matching words in the sentence.
	
};


/* Now we can put together a list of Wordes.  
	For example we want all the words that go with cameras:
			Word #1:  camera, web cam, eyes, etc.
			Word #2:  tilt, angle up
			Word #3:  set, put, make, raise to,
			Word #4:  , angle down, etc.
			Word #3:  pan, left, right, etc
			
	Something to this effect.  So 1 Word Group would be all the subjects.
				Another WordGroup would be all the verbs, 
				Another WordGroup would be all the objects,
				Another WordGroup would be all the adjectives, 
				Another WordGroup would be all the prepositions, etc.
	This is the thinking at least.
*/
class WordGroup
{
public:
	WordGroup ();
    WordGroup ( const WordGroup& mWG );
    ~WordGroup();

	void			add_word( string&  mNewWord );		// could be simple string.
    void			add_word( const char*  mNewWord );		// could be simple string.
    
	void			add_new ( Word&  mNewWord );
	
	bool			has_group_member  ( char*  mLookupWord,	bool mWholeWordOnly=true );
	int				evaluate_sentence( char*  mSentence,	bool mWholeWordOnly=true );
	int				was_found_in_sentence( string mWordWord );
	
	int				number_times_found();	// within the sentence.

    Word*			get_Word        ( string& mWordWordId );		// main word which describes the Word.
	Word*			get_Word		( int mIndex );
	Word*			get_extracted	( int mIndex );	

	vector<Word>*  	m_Words;
	vector<Word>*  m_all_extracted;		// all matching words in the sentence.
};


class Vocabulary
{
public:
    Vocabulary();
    ~Vocabulary();
    
    void			add_new( WordGroup mNewWord );

    
    vector<WordGroup>*  m_wordgroups;		// all matching words in the sentence.
    
};
 
#endif
