//
//  ring.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/30/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "ring.h"
#include "all_objects.h"



glRing::glRing()
{
}

glRing::~glRing()
{
    
}

void glRing::set_finger_params( float mRadius, int mSlices, float mStartAngle, float mEndAngle )
{
    m_finger_radius  = mRadius;
    m_number_of_rings= mSlices;
    m_start_degrees  = mStartAngle;
    m_end_degrees    = mEndAngle;
}

void glRing::set_circle_params( float mRadius, int mSamples )
{
    m_circle_radius     = mRadius;
    m_number_of_samples = mSamples;
}




void glRing::generate_vertices( )
{
    // TRANSLATE & ROTATE AROUND A LARGER DIAMETER.  Rotation Matrix to rotate each circle around the cirlce.
    float TwoPi     = 2.*M_PI;
    float y_angle   = m_start_degrees;
    float slice_increment =  ((m_end_degrees-m_start_degrees) / (float)(m_number_of_rings-2)) * M_PI/180.;
    float increment = (TwoPi/((float)m_number_of_samples));

    glm::vec3 Translate( m_finger_radius, 0, 0 );
    glm::mat4 transmat = glm::translate( glm::mat4(1.0f), Translate );
    glm::mat4 spin     = glm::rotate( transmat, y_angle, glm::vec3(0.0f, 1.0f, 0.0f) );

    struct Vertex v;
    set_vertex_color(v);
    glm::vec4 pt;
    pt[3]  = 1.0;
    glm::vec4 mapped_pt;

    int   i;
    float a=0.;
    for (int ring=0; ring<m_number_of_rings; ring++)
    {
        spin    = glm::rotate   ( glm::mat4(1.0f), y_angle, glm::vec3(0.0f, 1.0f, 0.0f) );
        spin    = glm::translate( spin, Translate );
        y_angle += slice_increment;

        // WE GENERATE A CIRCLE:
        for (i=0,a=0.; i<m_number_of_samples; a+=increment, i++)
        {
            pt[0] = m_circle_radius * sin(a);
            pt[1] = m_circle_radius * cos(a);
            pt[2] = 0.;
            mapped_pt = spin * pt;
            v.position[0] = mapped_pt[0];
            v.position[1] = mapped_pt[1];
            v.position[2] = mapped_pt[2];
            m_vertices.push_back( v );
        }
    }
}

void glRing::generate_indices2( )
{
    //    m_indices.resize(m_rings * m_sectors * 4);
    int r, s;
    for(r=0; r<m_number_of_rings-1; r++)
        for(s=0; s < m_number_of_samples-1; s++)
            m_indices.push_back(  r * m_number_of_samples + s );
}

void glRing::generate_indices( )
{
    int r, s;
    for(r=0; r<m_number_of_rings-1; r++)
        for(s=0; s < m_number_of_samples-1; s++)
        {
            m_indices.push_back(  r    * m_number_of_samples + s      );
            m_indices.push_back(  r    * m_number_of_samples + (s+1)  );
            m_indices.push_back( (r+1) * m_number_of_samples + (s+1)  );
            m_indices.push_back( (r+1) * m_number_of_samples + s      );
        }
}

void glRing::draw_primitive( )
{
    glDrawElements(GL_QUADS, (int)m_indices.size(), GL_UNSIGNED_INT, 0 );
}


