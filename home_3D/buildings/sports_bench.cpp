//
//  sports_bench.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 8/3/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "sports_bench.h"
#include "gl_box.hpp"
#include "cylinder.hpp"
#include "face_box.h"


const float LEG_SPACING = 4*12;

glBench::glBench ()
{
    
}
glBench::~glBench()
{
    
}

void glBench::create_components( )
{
    // CREATE THE SEAT:
    glFaceBox* tmp = new glFaceBox();
    tmp->width  = m_length;     // length of bench
    tmp->height = 1.0;          // 1 inch deep
    tmp->depth  = 18;           // 18 inches wide
    tmp->set_color(m_color);
    tmp->setup();
    //tmp->load_image("textures/corrogated_steel_metal.jpg");
    tmp->relocate  ( 0, m_height-tmp->height/2, 0);
    tmp->grab_right( );
    m_components.push_back(tmp);
    
    // PUT LEGS UNDER IT EVERY SO OFTEN:
    glCylinder* leg = NULL;
    int num_legs = m_length / (LEG_SPACING);           // every 5 feet.
    float offset = LEG_SPACING/2;
    for (int i=0; i<num_legs; i++)
    {
        leg = new glCylinder(24);
        leg->m_radius = 1;
        leg->m_extrusion_axis = 1;
        leg->m_extrusion_length = m_height-1.0;
        leg->setup();
        leg->relocate( offset, 0, 0);
        m_components.push_back( leg );
        offset += LEG_SPACING;
    }
}


