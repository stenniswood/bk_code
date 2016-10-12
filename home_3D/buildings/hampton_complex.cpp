//
//  hampton_complex.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/15/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "hampton_complex.h"
#include "all_objects.h"



glOneBuilding::glOneBuilding ( )
{
    m_object_type_name = "building";
}

glOneBuilding::~glOneBuilding( )
{
    long size = m_components.size();
    for (int a=0; a<size; a++)
    {
        delete m_components[a];
    }
}

void glOneBuilding::create_one_level( )
{
    glApartment* apt;
    glApartment* prev_apt;          // mine
    apt = new glApartment();
    apt->setup();
    apt->update_line_info();
    m_components.push_back( apt );
    
    prev_apt = apt;                 // back
    apt = new glApartment();
    apt->setup();
    apt->mirror_image_x();
    apt->relocate( 0, 0, prev_apt->get_max_z() );
    apt->update_line_info();
    m_components.push_back( apt );
    
    prev_apt = apt;                 // front
    apt = new glApartment();
    apt->setup();
    apt->mirror_image_z();
    apt->update_line_info();
    apt->relocate( -5*12 - prev_apt->get_max_x(), 0, 0 );
    m_components.push_back( apt );
    
    prev_apt = apt;                 // back
    apt = new glApartment();
    apt->setup();
    apt->mirror_image_x();
    apt->mirror_image_z();
    apt->relocate( -5*12 - prev_apt->get_max_x(), 0, prev_apt->get_max_z() );
    apt->update_line_info();
    m_components.push_back( apt );
    
/*    glMezannineUpstairs* mup = new glMezannineUpstairs();
    mup->create();
    mup->relocate(0,0,0);
    mup->m_y_angle = 90;
    m_components.push_back(mup); */
    
//    m_levels.push_back( &m_front_bottom );
}

void glOneBuilding::create_components( )
{
    create_one_level();
//    create_one_level();
}



