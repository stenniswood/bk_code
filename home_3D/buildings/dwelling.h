//
//  dwelling.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/7/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#ifndef __home3D__dwelling__
#define __home3D__dwelling__

#include <stdio.h>
#include "dwelling_level.h"
#include "stairway.hpp"
#include "roof.h"

class glDwelling : public glMolecule
{
public:
    glDwelling ();
    ~glDwelling();

    void    add_level( glDwellingLevel* mLevel );
    void    add_roof ( );

    // Find door, window,
    glRoof*                  m_roof;
    float                    m_level_height;
    vector<glStairway*>      m_stairs;
    vector<glDwellingLevel*> m_levels;
};


#endif /* defined(__home3D__dwelling__) */

