//
//  bookcase.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/12/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "all_objects.h"
#include "bookcase.h"


glBookcase::glBookcase(  )
{
    Initialize();
}

void  glBookcase::Initialize	( )
{
    m_object_type_name = "bookcase";    
    m_object_class  = 22;
    m_depth   = 24;
    m_width   = 46;
    m_height  = 72;
    m_shelves = 4;
}

void glBookcase::setup  		( )
{
	glObject::setup();
    create_components();
}

void glBookcase::create 		( )
{
    setup();
    gl_register();
}

void glBookcase::create_components()
{
    // Create Sides:
    glBox* box  = new glBox();
    box->height = m_height;
    box->width  = 1.0;
    box->depth  = m_depth;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_front();
    box->change_color(0xFF8F5F2F);
    box->m_x = 0.;
    box->m_y = 0.;
    box->m_z = 0.;
    m_components.push_back(box);

    box  = new glBox();
    box->height = m_height;
    box->width  = 1.0;
    box->depth  = m_depth;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_front();
    box->change_color(0xFF8F5F2F);
    box->m_x = m_width;
    box->m_y = 0.;
    box->m_z = 0.;
    m_components.push_back(box);
    
    // Create back:
    box = new glBox();
    box->height = m_height;
    box->width  = m_width;
    box->depth  = 1.;
    box->m_y_angle = 0;
    box->m_x    = 0.;
    box->m_y    = 0.;
    box->m_z    = 0.;
    box->setup();
    box->change_color(0xFF6F6F6F);
    box->grab_bottom();
    box->grab_right();
    box->grab_front();
    m_components.push_back(box);

    // Create shelves:
    float height_increment = m_height / m_shelves;
    float height = 1.0;     // 1" off ground.
    for (int i=0; i<m_shelves; i++)
    {
        box = new glBox();
        box->height = 0.5;
        box->width  = m_width;
        box->depth  = m_depth;
        box->m_x    = 0.;
        box->m_y    = height;
        box->m_z    = 0;
        box->setup();
        box->grab_bottom();
        box->grab_right ();
        box->grab_front ();
        m_components.push_back(box);
        height += height_increment;
    }
    
    // Create Top:
    box = new glBox();
    box->height = 0.5;
    box->width  = m_width;
    box->depth  = m_depth;
    box->m_x    = 0.;
    box->m_y    = m_height;
    box->m_z    = 0;
    box->setup();
    box->change_color(0xFFBF5F5F);
    box->grab_bottom();
    box->grab_right  ();
    box->grab_front ();
    m_components.push_back(box);
    
}



