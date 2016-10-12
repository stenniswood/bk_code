//
//  scene_nlp.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/14/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef scene_nlp_hpp
#define scene_nlp_hpp

#include <stdio.h>
#include <string>
#include "nlp_sentence.hpp"

void Init_Scene_Protocol  ( );

int  Parse_Scene_Statement( Sentence& mSentence, string* mh );



#endif /* scene_nlp_hpp */
