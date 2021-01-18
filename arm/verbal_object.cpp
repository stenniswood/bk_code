//
//  verbal_atom.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/22/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "verbal_object.h"
//#include "all_objects.h"


VerbalObject::VerbalObject()
{
    m_max.position[0] = -10000;
    m_max.position[1] = -10000;
    m_max.position[2] = -10000;
    m_min.position[0] = +10000;
    m_min.position[1] = +10000;
    m_min.position[2] = +10000;
}

VerbalObject::~VerbalObject()
{
 // glObject::~glObject();
}

bool VerbalObject::is_a_molecule()
{
    return false;
}

bool  VerbalObject::name_matches( string mRequestedName )
{
    return m_name.compare( mRequestedName )==0;
}

void VerbalObject::compute_min()
{
    // compute it here!
}
void VerbalObject::compute_max()
{
    
}

