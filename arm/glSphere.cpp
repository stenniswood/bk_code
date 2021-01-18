//
//  glSphere.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "glSphere.h"



glSphere::glSphere(float radius, unsigned int rings, unsigned int sectors)
{
    m_object_type_name = "sphere";    
    m_radius  = radius;
    m_rings   = rings;
    m_sectors = sectors;
}

glSphere::~glSphere()
{    
}

void glSphere::load_texture( string mImageFilename )
{
    m_texture = new Texture();
    m_texture->load_image(mImageFilename);
}

void glSphere::set_inertia( float mOneAxis )  // applies same to all 3 axes.
{
    Inertia[0] = mOneAxis;
    Inertia[1] = mOneAxis;
    Inertia[2] = mOneAxis;
}

void glSphere::generate_vertices( )
{
    float const R = 1./(float)(m_rings-1);
    float const S = 1./(float)(m_sectors-1);
    int r, s;
    struct Vertex tmp;
    struct stTextCoord ttmp;
    
    tmp.color[0] = (m_color & 0x00FF0000) >> 16;
    tmp.color[1] = (m_color & 0x0000FF00) >>  8;
    tmp.color[2] = (m_color & 0x000000FF) ;
    tmp.color[3] = (m_color & 0xFF000000) >> 24;

    //m_vertices.resize(m_rings * m_sectors * 3);
    //m_normals.resize (m_rings * m_sectors * 3);
    for(r = 0; r < m_rings; r++) 
      for(s = 0; s < m_sectors; s++) 
      {
		float const y = r * R*2;
        //float const y = sin( -M_PI_2 + M_PI * r * R );
        float const x = cos(2*M_PI * s * S) ;//* sin( M_PI * r * R );
        float const z = sin(2*M_PI * s * S) ;//* sin( M_PI * r * R );
        if (m_texture)
        {
//            t_radius = r*R;
//            ttmp.v = image_center_x + 0.5*t_radius*cos(2*M_PI * s * S);
//            ttmp.u = image_center_z + 0.5*t_radius*sin(2*M_PI * s * S);
            ttmp.u = s*S;
            ttmp.v = r*R;
            m_texture->m_TexCoords.push_back( ttmp );
        }
        
        tmp.position[0] = x * m_radius;
        tmp.position[1] = y * m_radius;
        tmp.position[2] = z * m_radius;
        //printf("SphereVertices: %6.1f, %6.1f, %6.1f\n", tmp.position[0], tmp.position[1], tmp.position[2] );        
        m_vertices.push_back( tmp );
        
        m_normals.push_back( x );
        m_normals.push_back( y );
        m_normals.push_back( z );
    }
}

void glSphere::draw_primitive( )
{
    glDrawElements(GL_QUADS, (int)m_indices.size(), GL_UNSIGNED_INT, 0 );
}

void glSphere::generate_indices( )
{
    m_indices.resize(m_rings * m_sectors * 4);
    int r, s;
    for(r=0; r<m_rings-1; r++)
        for(s=0; s < m_sectors-1; s++) {
            m_indices.push_back(  r * m_sectors + s         );
            m_indices.push_back(  r * m_sectors + (s+1)     );
            m_indices.push_back( (r+1) * m_sectors + (s+1)  );
            m_indices.push_back( (r+1) * m_sectors + s      );
    }
}

void glSphere::gl_register(  )
{
    glAtom::gl_register();       // Base class

    if (m_texture) {
        m_texture->generate_TBO();
        m_texture->generate_VBOTexCoords();
    }
}

bool glSphere::evaluate_collision( glMovingObject* mOther )
{
    if (m_y<m_radius)
        return true;
    return false;
}

/***************************************************************/

glBasketball::glBasketball ()
: glSphere(NBA_BASKET_BALL_RADIUS, 10, 64)
{
    is_participating = true;
    coef_of_restitution = COEFF_RESTITUTION_BASKETBALL;
    set_color( 0xFF7F7F00);
    load_texture("textures/basketball_texture.jpg" );
}

glSoccerball4::glSoccerball4 ()
:glSphere(FIFA_SOCCER_BALL_RADIUS_SIZE5, 10, 64)
{
    is_participating = true;
    coef_of_restitution = COEFF_RESTITUTION_SOCCERBALL;
    set_color( 0xFF5F7F2F);
    load_texture("textures/soccer_ball_3.jpg" );    
}

glSoccerball5::glSoccerball5 ()
:glSphere(FIFA_SOCCER_BALL_RADIUS_SIZE5, 10, 64)
{
    is_participating = true;
    coef_of_restitution = COEFF_RESTITUTION_SOCCERBALL;
    set_color( 0xFFFF0000);
    load_texture("textures/soccer_ball_world_cup.jpg" );
}


glBaseball::glBaseball    ()
:glSphere( BASEBALL_RADIUS, 10, 64 )
{
    is_participating = true;
    coef_of_restitution = COEFF_RESTITUTION_BASEBALL;
    set_color( 0xFF0F0000);
    load_texture("textures/baseball_texture.jpg" );
}

