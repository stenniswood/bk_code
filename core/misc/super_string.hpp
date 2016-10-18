//
//  super_string.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/17/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef super_string_hpp
#define super_string_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <regex>

using namespace std;


class SuperString : public string
{
public:
    SuperString		( );
    SuperString		( string mOriginal);
    SuperString		( const char* mOriginal);

    SuperString& operator=         (const SuperString& mOp);
    SuperString& operator=         (const char* mOp);
    SuperString& append_float      (float mOp   );
    void    replace_all            ( char mChar, char mDest );


    // HELPER FUNCTIONS:
    void    trim_trail_space       (  );
    void    trim_leading_space     (  );
    void    trim_duplicate_spaces  (  );      // between words of the sentence.
    void    trim_spaces            (  );      // leading,trailing, and no duplicates!
    void    trim_spaces_in_splits  (  );      // leading,trailing, and no duplicates!

    void             convert_to_lower_case( );      // does it in place, ie no recovery!
    vector<float>    convert_to_float     ( );      // does it in place, ie no recovery!

    bool    is_word_break               ( char mTest );
    bool    is_punctuation_mark         ( char mTest ); // ,.?;:!
    void    remove_all_punctuations     (            );

    bool    char_is_a_number            ( char C     ); // Compares to [0..9-.]
    bool    char_is_a_number_nd         ( char c     ); // Compares [n,d,r,s,t,h] (ie for 1st,2nd,3rd,4th) Just the endings,not numbers (0..9)!
    bool    is_nth_word_a_number        ( int mIndex );
    bool    is_nth_word_a_number_nd     ( int mIndex );

    int     count_occurences            ( SuperString& mWord    );  // in Sentence
    int     get_nth_word_position       ( int mIndex            );  // in Sentence

    void    split_wordlist              ( );  // string remains intact.  non destructive!
    int     split                       ( const char deliminator);  // string remains intact.  non destructive!
    void    reduce_string               ( vector<int> mWordIndices );
    int     regex_reduce                ( );
    
    // Return for these is:         
    int  compare_wordlist        ( SuperString  mWord, int& mSelectedWord, int start_word=0,  vector<int>* remove_wi=NULL     );
    int  is_found_in_sentence    ( const char*  mWord, int start_word=0, vector<int>* remove_wi=NULL, bool mOrItsPlural=false );

    // REGULAR EXPRESSIONS :
    int  regex_find              ( string&  mRegexpression, vector<int>* answers=NULL, vector<int>* remove_wi=NULL  );
    int  extract_wordlist_results(  );
    SuperString extract_results  (  );
    int  count_wordlists         (  );

    
    int  regex_found_in_sentence( SuperString  mWord, int start_word=0, vector<int>* answers=NULL, vector<int>* remove_wi=NULL );
    int  any_one_word_found_in_sentence          ( SuperString& mWordList, int start_word=0, vector<int>* remove_wi=NULL );
    // all words, any order.
    int  all_words_found_in_sentence_any_order   ( SuperString& mWordList, int start_word=0, vector<int>* remove_wi=NULL );
    // all words, in-order, fill words(in sentence) allowed
    int  all_words_sequentially_found_in_sentence( SuperString& mWordList, int start_word=0, vector<int>* remove_wi=NULL );
    // all words, in-order, no-fill words allowed.
    int  all_words_exact_match                   ( const char*  mWordList, int start_word=0, vector<int>* remove_wi=NULL );
    int  all_words_exact_match                   ( SuperString& mWordList, int start_word=0, vector<int>* remove_wi=NULL );

    void    print();

    string              m_regex;
    std::smatch         regex_matches;
    vector<int>         m_wordlist_choices;         // For any & all word lists.
    
    //void format       ( char* mFormatter, ...);
    vector<int>          m_split_words_original_index;      // starting char index
    vector<SuperString>  m_split_words;
};



#endif /* super_string_hpp */
