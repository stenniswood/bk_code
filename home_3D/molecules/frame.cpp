//
//  frame.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/26/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "frame.h"
#include "all_objects.h"




glFrame::glFrame()
{
    m_object_type_name = "frame";
    m_width = 0;
    m_height = 0;
    m_border_width = 2.5;
    m_frame_thickness = 0.375;
    m_has_bottom = true;
}
glFrame::~glFrame()
{
    
}
void glFrame::set_params( float mWidth, float mHeight,float mBorderWidth, float mThick )
{
    m_width = mWidth;
    m_height = mHeight;
    m_border_width = mBorderWidth;
    m_frame_thickness = mThick;
}
void glFrame::create_components()
{
    m_left_side = new glBox();
    m_left_side->width  = m_border_width;               // x
    m_left_side->height = m_height;                     // y
    m_left_side->depth  = m_frame_thickness;                          // z
    m_left_side->set_color(0xFFFF2f00);
    m_left_side->setup      ();
    m_left_side->grab_bottom();
    m_left_side->grab_left ();
    m_left_side->relocate ( -0, 0, 0);
    m_components.push_back(m_left_side);
    
    m_right_side = new glBox();
    m_right_side->width  = m_border_width;
    m_right_side->height = m_height;
    m_right_side->depth  = m_frame_thickness;
    m_right_side->set_color(0xFFFF2f00);
    m_right_side->setup      ();
    m_right_side->grab_bottom();
    m_right_side->grab_right  ();
    m_right_side->relocate( m_width, 0, 0);
    m_components.push_back(m_right_side);
     
    m_top_side = new glBox();
    m_top_side->width  = m_width + 2*m_border_width;
    m_top_side->height = m_border_width;
    m_top_side->depth  = m_frame_thickness;
    m_top_side->set_color(0xFFFF2f00);
    m_top_side->setup();
    m_top_side->grab_bottom();
    m_top_side->grab_right();
    m_top_side->relocate( -m_border_width, m_height-m_border_width, 0);
    m_components.push_back(m_top_side);
    
    if (m_has_bottom)
    {
        m_bottom_side = new glBox();
        m_bottom_side->width  = m_width + 2*m_border_width;
        m_bottom_side->height = m_border_width;
        m_bottom_side->depth  = m_frame_thickness;
        m_bottom_side->setup();
        m_bottom_side->grab_right();
        m_bottom_side->grab_bottom();
        m_bottom_side->relocate( -m_border_width, -m_border_width, 0 );
        m_components.push_back(m_bottom_side);
    }
}

