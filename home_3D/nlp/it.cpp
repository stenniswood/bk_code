//
//  it.cpp      Designed to hold the reference object.
//              When user says, "Move it to the bedroom".  "Delete it." etc.
//              These
//  home3D
//
//  Created by Stephen Tenniswood on 7/14/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <vector>
#include <string>

#include "it.hpp"


/*  */
ReferenceIt last_entity;

/*
    Keep a queue of entities which were spoken.  
    For things like "go back to the previous one.  No the one before that."
 
 */
vector<VerbalObject*> referers;


ReferenceIt::ReferenceIt()
{
    m_it = NULL;
}

ReferenceIt::~ReferenceIt()
{
    
}

VerbalObject*   ReferenceIt::get  ()
{
    return NULL;
}

VerbalObject*   ReferenceIt::store()
{
    return NULL;
}

std::string      ReferenceIt::what_is_it()
{
    return "";
}

std::string      ReferenceIt::which_one_is_it()
{
    return "";
}


