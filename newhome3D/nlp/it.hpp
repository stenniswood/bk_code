//
//  it.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/14/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef it_hpp
#define it_hpp

#include <stdio.h>
#include <string>
#include "verbal_object.h"


class ReferenceIt
{
public:
    ReferenceIt ();
    ~ReferenceIt();
    
    VerbalObject*   get  ();
    VerbalObject*   store();
    
    std::string      what_is_it();
    std::string      which_one_is_it();
    
    VerbalObject* m_it;
};


extern ReferenceIt         last_entity;
extern vector<VerbalObject*> referers;


#endif /* it_hpp */
