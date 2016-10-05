//
//  txt_cylinder.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/26/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "txt_cylinder.h"
#include "all_objects.h"


glTextureCylinder::glTextureCylinder( int mNumberSamples )
:glCylinder(mNumberSamples)
{
    m_object_type_name = "textured cylinder";
    m_texture = new Texture();
}

void glTextureCylinder::setup ()
{
//    glExtrusion::setup(m_extrusion_length, m_extrusion_axis);
    glExtrusion::setup( );
    
    m_texture->load_image( "textures/me_in_car.bmp" );
    generate_text_coords();
    m_texture->generate_TBO           ();
    m_texture->generate_VBOTexCoords  ();
}

void glTextureCylinder::generate_text_coords()
{
    struct stTextCoord tc;
    float increment = ((float)m_texture->m_repetitions_x / ((float)m_number_of_samples));
    int   i;
    float a=0.;
    for (i=0; i<m_number_of_samples; a+=increment, i++)
    {
        tc.u =  a;
        tc.v =  m_texture->m_repetitions_x;
        m_texture->m_TexCoords.push_back(tc);
    }

    a=0.;
    for (i=0; i<m_number_of_samples; a+=increment, i++)
    {
        tc.u =  a;
        tc.v =  0.0;
        m_texture->m_TexCoords.push_back(tc);
    }
}

void	glTextureCylinder::draw_body( )
{
    glAtom::draw_body();
}

void	glTextureCylinder::draw_primitive( )
{
    // Draw Bottom :
    glDrawElements(GL_POLYGON, (int)m_layer_one_indices, GL_UNSIGNED_INT, 0 );
    
    // Draw the sides: m_number_side_indices
    glDrawElements(GL_QUAD_STRIP, m_layer_one_vertices*2., GL_UNSIGNED_INT,
                   (GLvoid*)BUFFER_OFFSET( m_layer_one_indices ) );
    // Draw Top :
    if (m_is_closed)
        glDrawElements(GL_POLYGON, (int)m_layer_one_indices, GL_UNSIGNED_INT,
                       (GLvoid*)BUFFER_OFFSET(m_layer_one_indices+m_number_side_indices) );
}

