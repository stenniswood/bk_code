//
//  dwelling.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/7/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "dwelling.h"
#include "all_objects.h"
#include "roof.h"


glDwelling::glDwelling()
{
    m_object_type_name = "dwelling";
    m_level_height = 0*12;        // start with a basement.
}

glDwelling::~glDwelling()
{
    
}

void glDwelling::add_level( glDwellingLevel* mLevel )
{
    mLevel->m_y = m_level_height;
    mLevel->update_body_matrix();
    m_components.push_back( mLevel );
    m_levels.push_back(mLevel);

    m_level_height += 10*12;
}

void glDwelling::add_roof()
{
    MathVector corner1(3);
    MathVector corner2(3);
    
    m_roof = new glRoof(corner1, corner2);
    m_roof->setup();
    m_roof->relocate( 0, 0, 0 );
    m_components.push_back(m_roof);
}


/*glDoor* glDwelling::get_door( string mName   )
{
    glDoor* ptr= NULL;
    for (int w=0; w<m_levels.size(); w++)
    {
        ptr = m_levels[w].get_door( mName );
        if (ptr)
            return ptr;
    }
    return NULL;
} */

