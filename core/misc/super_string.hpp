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
using namespace std;


class SuperString : public string
{
    
public:
    SuperString();
    SuperString(char* mOriginal);
    
        SuperString& operator=(const SuperString& mOp);
        SuperString& operator=(const char* mOp);
    
        // HELPER FUNCTIONS:
        void trim_trail_space       (  );
        void trim_leading_space     (  );
        void trim_duplicate_spaces  (  );      // between words of the sentence.
        void trim_spaces            (  );      // leading,trailing, and no duplicates!
        void trim_spaces_in_splits  (  );      // leading,trailing, and no duplicates!
    
        void convert_to_lower_case  ( );        // does it in place, ie no recovery!
        bool is_word_break          ( char mTest );
        bool is_punctuation_mark    ( char mTest );     // ,.?;:!
        void remove_all_punctuations();
    bool    char_is_a_number        (char C);
    bool    char_is_a_number_nd     (char c);
    bool    is_nth_word_a_number    ( int mIndex );
    bool    is_nth_word_a_number_nd ( int mIndex );

    
        int	 split                  ( const char deliminator);      // string remains intact.  non destructive!
    
        int  any_one_word_found_in_sentence          ( SuperString& mWordList, int& mWhichWordIndex, int start_word=0 );
        int  all_words_found_in_sentence             ( SuperString& mWordList );
        int  all_words_sequentially_found_in_sentence( SuperString& mWordList );
    
    //  void format                 ( char* mFormatter, ...);
    
    vector<SuperString>  m_split_words;
};



#endif /* super_string_hpp */
