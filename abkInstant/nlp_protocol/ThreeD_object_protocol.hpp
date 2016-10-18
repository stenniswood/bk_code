//
//  ThreeD_object_protocol.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/20/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef ThreeD_object_protocol_hpp
#define ThreeD_object_protocol_hpp

#include <stdio.h>
#include "nlp_sentence.hpp"
#include "serverthread.hpp"

class Object3D
{
public:
    Object3D();
    ~Object3D();
    
    int     parse( string& mSentence );
    
    string  m_regexp;
    string  m_parameters;
};

void Init_ThreeD_Protocol  ( );
int  Parse_ThreeD_Statement( Sentence& mSentence, ServerHandler* mh );


#endif /* ThreeD_object_protocol_hpp */


