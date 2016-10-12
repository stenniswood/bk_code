//
//  hinge.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "hinge.h"
#include "all_objects.h"

glHinge::glHinge()
{
    m_object_type_name = "hinge";
    m_height = 3.0;
}

glHinge::~glHinge()
{
    
}

void  glHinge::create_components()
{
    glBox tmp;
    tmp.width  = 1.5;
    tmp.height = m_height;
    tmp.depth  = 0.25;
    tmp.setup();
    m_components.push_back( &tmp );

    tmp.width  = 1.5;
    tmp.height = m_height;
    tmp.depth  = 0.25;
    tmp.setup();
    m_components.push_back( &tmp );

    glCylinder joint(36);
    joint.m_radius = 1.5;
    joint.set_height      ( m_height );
    joint.setup           (        );
    m_components.push_back( &joint );
}

