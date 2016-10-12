//
//  roof.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 8/19/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "roof.h"
#include "face_box.h"



glRoof::glRoof()
{
    m_rafter_spacing = 8*12;
}

glRoof::glRoof(MathVector start, MathVector end)   // close and far corner (square house)
{
    MathVector delta = end - start;
    m_house_length = delta[0];
    m_house_width  = delta[2];
}

glRoof::~glRoof()
{
    
}

void glRoof::create_rafters()
{
    int number_rafters = m_house_length / m_rafter_spacing;
    float z_start = 0;
    
    for (int i=0; i<number_rafters; i++)
    {
        glRafter* raft = new glRafter();
        raft->m_wall_to_wall_width = m_house_width;
        raft->m_peak_height = 0.25 * m_house_width;
        raft->m_overhang_width = 12;
        raft->setup();
        raft->relocate( z_start, 0, 0 );

        m_rafters.push_back(raft);
        m_components.push_back(raft);
        z_start += m_rafter_spacing;
    }
}
void glRoof::select_shingles(int mShingleIndex)
{
    switch( mShingleIndex )
    {
        case 1: //load_image("textures/golden_brown_asphalt_shingles_seamless_background_texture.jpg");
            break;
        case 2: // load_image("textures/roof-shingle-texture.jpg");
            break;
        case 3:
            break;
        case 4:
            break;
        default:
            break;
    }
}

void glRoof::create_roof()
{
    glFaceBox* fbox = new glFaceBox();
    fbox->load_image("textures/golden_brown_asphalt_shingles_seamless_background_texture.jpg");
    fbox->width  = m_house_length;
    // 1/2 house_width squared  +  peak squared
    fbox->height = sqrt((m_house_width/2)*(m_house_width/2)+ (0.25*m_house_width)*(0.25*m_house_width));
    fbox->depth  = 0.25;
    fbox->m_x_angle = m_rafters[0]->m_angle+180;
    fbox->relocate( -m_rafter_spacing/2, RAFTER_BOARD_THICK, -m_rafters[0]->m_overhang_width-RAFTER_BOARD_THICK );
    fbox->setup();
    fbox->grab_right();
    fbox->grab_top();
    m_components.push_back(fbox);
    
    fbox = new glFaceBox();
    fbox->load_image("textures/golden_brown_asphalt_shingles_seamless_background_texture.jpg");
    fbox->width  = m_house_length;
    // 1/2 house_width squared  +  peak squared
    fbox->height = sqrt((m_house_width/2)*(m_house_width/2)+ (0.25*m_house_width)*(0.25*m_house_width));
    fbox->depth  = 0.25;
    fbox->setup();
    fbox->grab_right();
    fbox->grab_bottom();
    fbox->m_x_angle = -m_rafters[0]->m_angle;
    fbox->relocate( -m_rafter_spacing/2, RAFTER_BOARD_THICK, m_house_width+m_rafters[0]->m_overhang_width+RAFTER_BOARD_THICK );
    m_components.push_back(fbox);
    
}

void glRoof::create_components()
{
    create_rafters();
    create_roof();
}