//
//  table_pole.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/6/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "all_objects.h"

const int TABLE_THICKNESS = 1.0;


glTableCenterPole::glTableCenterPole( )
: m_table_top(24), m_center_pole(24)
{
    m_table_radius      = 20.;		// Length along 1 side.
    m_table_num_feet    = 4  ;		// Distance across the table.
    m_table_height      = 29.;		// how high off ground.
    Initialize();
}

void glTableCenterPole::Initialize	( )
{
      m_object_type_name = "table";
//    m_object_class  = 15;
}


void glTableCenterPole::create_components()
{
    // Table Top (inches) :
    m_table_top.m_radius = m_table_radius;
    set_relative_x( &m_table_top, 0. );
    set_relative_y( &m_table_top, m_table_height );
    set_relative_z( &m_table_top, 0. );

    // Table Height. m_y is the center of the cylinder (middle of leg)
    m_table_top.m_is_closed = true;
    m_table_top.set_la( TABLE_THICKNESS, 1 );
    m_table_top.setup( );
    //m_table_top.load_image("textures/oak_wood.jpg");
    m_components.push_back( &m_table_top );
    
    m_center_pole.m_radius = 1.5;
    set_relative_xyz( &m_center_pole, 0., 0., 0. );
    m_center_pole.set_la(m_table_height, 1);
    m_center_pole.setup ( );
    m_components.push_back( &m_center_pole );

    static glBox      one;
    one.m_y    = 0.5;
    one.width  = 1.;
    one.height = 1.;
    one.depth  = m_table_top.m_radius;
    float angle_increment_deg = 360  / m_table_num_feet;
    for (int f=0; f<m_table_num_feet; f++)
    {
        set_relative_x( &one, 0. );
        set_relative_y( &one, 0. );
        set_relative_z( &one, 0. );
        one.m_y_angle  += angle_increment_deg;
        m_feet.push_back(one);
        m_feet.back().setup();
        m_feet.back().grab_bottom();
    }
    glAtom* ptr = NULL;
    for (int f=0; f<m_feet.size(); f++)
    {
        ptr = &(m_feet[f]);
        m_components.push_back( ptr );
    }
}


