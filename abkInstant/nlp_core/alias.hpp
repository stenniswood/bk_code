#ifndef _ALIAS_
#define _ALAIS_

//#include <string>
using namespace std;


/* This was the original simplistic way of making a vocabulary:

  However, there are many words with the same meaning.  Synonyms.
  Therefore this class will encapsulate a synonym group - alias.
  Every word added should have the same meaning.  No differences will be distinguished.
  
*/

class Alias 
{
public:
	Alias();
	~Alias();

	void			add_new( char*  mNew );
	void			add_new( string mNew );
	void			convert_all_to_lower( );
	
	bool			is_group_member( char*  mLookupWord );
	void			extract_member ( char*  mSentence, bool mWholeWordOnly=true );	
	
private:
   list<string>  	m_synonyms;

};


/* Now we can put together a list of aliases.  
	For example we want all the words that go with cameras:
			alias #1:  camera, web cam, eyes, etc.
			alias #2:  tilt, angle up
			alias #3:  set, put, make, raise to,
			alias #4:  , angle down, etc.
			alias #3:  pan, left, right, etc
			
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
	WordGroup();
	~WordGroup();

	void			add_word( string&  mNewAlias );		// could be simple string.
	void			add_new ( Alias&  mNewAlias );
	
	void			is_group_member( char*  mLookupWord, bool mWholeWordOnly=true );
	void			extract_member( char*  mSentence, bool mWholeWordOnly=true );	


private:
   list<Alias>  	m_alias_groups;

};


 
#endif
