//
//  chair.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/7/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "all_objects.h"



glChair::glChair(  )
{
    m_object_type_name = "chair";
    Initialize();
}

void glChair::Initialize	( )
{
    m_object_class  = 14;
    m_seat_length = 24;
    m_seat_width  = 24;
    m_seat_height = 24;
    
}

void glChair::create_components()
{
    // Create Seat:
    glBox* box  = new glBox();
    box->height = 1.0;
    box->width  = m_seat_width;
    box->depth  = m_seat_length;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(0xFF8F5F2F);
    set_relative_y( box, m_seat_height );
    set_relative_x( box, 0. );
    set_relative_z( box, 0. );
    m_components.push_back(box);

    // Create Legs:
    box = new glBox();
    box->height = m_seat_height;
    box->width  = 1.;
    box->depth  = 1.;
    set_relative_x( box, 0.);
    set_relative_y( box, 0.);
    set_relative_z( box, 0.);
    box->setup();
    box->grab_bottom();
    m_components.push_back(box);
    
    box = new glBox();
    box->height = m_seat_height;
    box->width  = 1.;
    box->depth  = 1.;
    set_relative_x( box, m_seat_width-1 );
    set_relative_y( box, 0. );
    set_relative_z( box, 0. );
    box->setup();
    box->grab_bottom();
    m_components.push_back(box);

    box = new glBox();
    box->height = m_seat_height;
    box->width  = 1.;
    box->depth  = 1.;
    set_relative_x( box, 0. );
    set_relative_y( box, 0. );
    set_relative_z( box, m_seat_length-1 );
    box->setup();
    box->grab_bottom();
    m_components.push_back(box);

    box = new glBox();
    box->height = m_seat_height;
    box->width  = 1.;
    box->depth  = 1.;
    set_relative_x( box, m_seat_width-1 );
    set_relative_y( box, 0. );
    set_relative_z( box, m_seat_length-1 );
    box->setup();
    box->grab_bottom();
    m_components.push_back(box);
    
    // Create Back:
    box = new glBox();
    box->height = m_seat_width;
    box->width  = m_seat_width;
    box->depth  = 1.;
    set_relative_x( box, 0. );
    set_relative_y( box, m_seat_height );
    set_relative_z( box, 0. );
    box->setup();
    box->change_color(0xFF7F5F2F);
    box->grab_bottom();
    box->grab_right();
    m_components.push_back(box);
}

void glChair::setup()
{
    create_components();
}

void glChair::create()
{
    setup();
    gl_register();
}


