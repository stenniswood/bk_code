//
//  truss.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 8/18/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "truss.h"
#include "cylinder.hpp"

glTruss::glTruss()
{
    m_separation_distance = 18.0;
    m_length              = 10*12.0;
    m_column_radius       = 2.5;
}

glTruss::~glTruss()
{
    
}

void glTruss::create_columns( )
{
    m_perpendicular_spacing = 2* m_separation_distance;
    
    // extrusion axis is 1 (y axis)
    glCylinder* column = new glCylinder(36);
    column->m_extrusion_length = m_length;
    column->m_radius = m_column_radius;
    column->setup();
    m_components.push_back(column);
    column->relocate( 0, 0, 0 );
    
    column = new glCylinder(36);
    column->m_extrusion_length = m_length;
    column->m_radius = m_column_radius;
    column->setup();
    m_components.push_back(column);
    column->relocate( m_separation_distance, 0, 0 );
    
    column = new glCylinder(36);
    column->m_extrusion_length = m_length;
    column->m_radius = m_column_radius;
    column->setup();
    m_components.push_back(column);
    column->relocate( m_separation_distance, 0, m_separation_distance );
    
    column = new glCylinder(36);
    column->m_extrusion_length = m_length;
    column->m_radius = m_column_radius;
    column->setup();
    m_components.push_back(column);
    column->relocate( 0, 0, m_separation_distance );
}

extern float degrees(float);

void glTruss::create_diagonals( )
{
    float y = 12;       // starting offset.
    glCylinder* column=NULL;
    float pipe_radius = 0.75;
    
    float d_offset  = m_perpendicular_spacing * 0.10;
    float d_spacing = m_perpendicular_spacing * 0.80;   // back off 10% on each end.
    float d_angle   = degrees( atan2( m_separation_distance, d_spacing ) );
    float d_length  = d_spacing / cos(d_angle);

    float start_x = 0;
    while ((y+d_offset+d_spacing) < m_length)
    {
        //
        column = new glCylinder(36);
        column->m_extrusion_length = d_length;
        column->m_radius = pipe_radius;
        column->setup();
        column->m_x_angle = d_angle;
        m_components.push_back(column);
        column->relocate( 0, y+d_offset, start_x );
        
        column = new glCylinder(36);
        column->m_extrusion_length = d_length;
        column->m_radius = pipe_radius;
        column->setup();
        column->m_x_angle = d_angle;
        m_components.push_back(column);
        column->relocate( m_separation_distance, y+d_offset, start_x );
        
        // Alternate :
        d_angle = -d_angle;
        if (start_x==0)
            start_x = m_separation_distance;
        else
            start_x = 0.;

        y += m_perpendicular_spacing;
    }
}

void glTruss::create_perpendiculars( )
{
    // Create 1 "ring" of 4.  All sides.
    float y = 12;       // starting offset.
    glCylinder* column=NULL;
    float pipe_radius = 0.75;
    
    while (y < m_length)
    {
        column = new glCylinder(36);
        column->m_extrusion_length = m_separation_distance;
        column->m_radius = pipe_radius;
        column->setup();
        column->m_x_angle = 90.0;
        m_components.push_back(column);
        column->relocate( 0, y, 0 );

        column = new glCylinder(36);
        column->m_extrusion_length = m_separation_distance;
        column->m_radius = pipe_radius;
        column->setup();
        column->m_x_angle = 90.0;
        m_components.push_back(column);
        column->relocate( m_separation_distance, y, 0 );
        
        column = new glCylinder(36);
        column->m_extrusion_length = m_separation_distance;
        column->m_radius = pipe_radius;
        column->setup();
        column->m_z_angle = 90.0;
        m_components.push_back(column);
        column->relocate( m_separation_distance, y, 0 );
        
        column = new glCylinder(36);
        column->m_extrusion_length = m_separation_distance;
        column->m_radius = pipe_radius;
        column->setup();
        column->m_z_angle = 90.0;
        m_components.push_back(column);
        column->relocate( m_separation_distance, y, m_separation_distance );
        
        y += m_perpendicular_spacing;
    }
}

void glTruss::create_components( )
{
    create_columns();
    create_perpendiculars();
    create_diagonals();
}
/**************************************************************************/

glTruss2D::glTruss2D()
{
    m_separation_distance   = 18.0;
    m_length                = 10*12.0;
    m_perpendicular_spacing = 2* m_separation_distance;
    m_column_radius = 2.5;
}

glTruss2D::~glTruss2D()
{
    
}

void  glTruss2D::create_columns  ( )
{
    // Extrusion axis is 1 (y axis)
    glCylinder* column = new glCylinder(36);
    column->m_extrusion_length = m_length;
    column->m_radius = m_column_radius;
    column->setup();
    m_components.push_back(column);
    column->relocate( 0, 0, 0 );
    
    column = new glCylinder(36);
    column->m_extrusion_length = m_length;
    column->m_radius = m_column_radius;
    column->setup();
    m_components.push_back(column);
    column->relocate( 0, 0, m_separation_distance );
}

void  glTruss2D::create_diagonals( )
{
    float y = 12;                                       // starting offset.
    glCylinder* column=NULL;
    float pipe_radius = 0.75;
    
    float d_offset  = m_perpendicular_spacing * 0.10;
    float d_spacing = m_perpendicular_spacing * 0.80;   // back off 10% on each end.
    float d_angle   = degrees( atan2( m_separation_distance, d_spacing ) );
    float d_length  = d_spacing / cos(d_angle);
    
    float start_x = 0;
    while ((y+d_offset+d_spacing) < m_length)
    {
        column = new glCylinder(36);
        column->m_extrusion_length = d_length;
        column->m_radius = pipe_radius;
        column->setup();
        column->m_x_angle = d_angle;
        m_components.push_back(column);
        column->relocate( 0, y+d_offset, start_x );

        // Alternate :
        d_angle = -d_angle;
        if (start_x==0)
            start_x = m_separation_distance;
        else
            start_x = 0.;
        
        y += m_perpendicular_spacing;
    }
}
void glTruss2D::create_perpendiculars( )
{
    // Create 1 "ring" of 4.  All sides.
    //float y = 12;       // starting offset.
    glCylinder* column=NULL;
    float pipe_radius = 0.75;
    
    column = new glCylinder(36);
    column->m_extrusion_length = m_separation_distance;
    column->m_radius = pipe_radius;
    column->setup();
    column->m_x_angle = 90.0;
    m_components.push_back(column);
    column->relocate( 0, m_perpendicular_spacing*0.08, 0 );
    
    column = new glCylinder(36);
    column->m_extrusion_length = m_separation_distance;
    column->m_radius = pipe_radius;
    column->setup();
    column->m_x_angle = 90.0;
    m_components.push_back(column);
    column->relocate( 0, m_length-(m_perpendicular_spacing*0.8), 0 );

}


void  glTruss2D::create_components( )
{
    create_columns();
    create_diagonals();
    create_perpendiculars();
}

/**************************************************************************/
#define LANE_WIDTH 5*12

glHighwayOverhead::glHighwayOverhead()
{
    m_road_width    = LANE_WIDTH*4;
    m_height        = 10*12;

    m_right_side_support = new glTruss2D();
    m_left_side_support = new glTruss2D();
    m_overhead = new glTruss();
}
glHighwayOverhead::glHighwayOverhead( int Number_of_lanes )
{
    m_road_width    = LANE_WIDTH*4;
    m_height        = 10*12;
    
    m_right_side_support = new glTruss2D();
    m_left_side_support = new glTruss2D();
    m_overhead = new glTruss();
    
}
glHighwayOverhead::~glHighwayOverhead()
{
    
}
void glHighwayOverhead::create_components()
{
    //m_right_side_support = new glTruss2D();
    m_right_side_support->m_length = m_height;
    m_right_side_support->setup();
    m_right_side_support->relocate(m_road_width, 0, 0);
    m_components.push_back( m_right_side_support );
    
    //m_left_side_support  = new glTruss2D();
    m_left_side_support->m_length = m_height;
    m_left_side_support->setup();
    m_left_side_support->relocate( 0, 0, 0 );
    m_components.push_back( m_left_side_support );
    
    //m_overhead = new glTruss();
    m_overhead->m_length = m_road_width;
    m_overhead->m_z_angle = -90;
    m_overhead->m_x_angle = 90;
    m_overhead->setup();
    m_components.push_back(m_overhead);
    m_overhead->relocate( 0, m_height,  m_right_side_support->m_separation_distance );
}

void glHighwayOverhead::add_sign( glFaceBox* mSign, int mLane, int mSide )
{
    const int LaneWidth = 6*12;
    mSign->set_color ( 0xFF135B00 );
    mSign->grab_bottom();
    if (m_right_side_support)
    {
        float forward_distance =  +m_right_side_support->m_separation_distance+m_right_side_support->m_column_radius*1.1 ;
        float lane_position = mLane*LaneWidth+LaneWidth/2;
        float overhang = 24;
        float height = m_height - overhang;
        if (mSide)
        {
            mSign->m_y_angle = 180;
            mSign->relocate( lane_position, height, -m_right_side_support->m_column_radius*1.1 );
            
        } else {

            float height = m_height - overhang;
            mSign->relocate( m_road_width-lane_position, height, forward_distance );
        }
        
    }
    //m_signs.push_back       ( mSign );
    m_components.push_back  ( mSign );
    
}

/**************************************************************************/

