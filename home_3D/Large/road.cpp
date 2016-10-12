//
//  road.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 8/14/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "road.h"


glRoad::glRoad()
{
    m_road_width = 5*12;
}
glRoad::~glRoad()
{
    
}

void   glRoad::select_texture()
{
    m_texture = new Texture();
    m_texture->load_image( "textures/seemless_asphalt_drenante.jpg");
    //m_texture->load_image( "textures/asphalt");
    
}

void glRoad::generate_vertices()
{
    MathVector perp(3);
    MathVector point(3);
    
    struct Vertex vert;
    m_color=0xFFFFFFFF;
    set_vertex_color(vert);
    
    long size = m_path.m_vertices.size();
    for (int i=0; i<size; i++)
    {
        // Right Side shoulder
        perp = m_path.get_perpendicular_vector(i);
        vert = m_path.m_vertices[i];    // obtain the center line xyz.
        point[0] = vert.position[0];
        point[1] = vert.position[1];
        point[2] = vert.position[2];
        point += perp * m_road_width/2;     // go 1/2 road width to left side.
        vert.position[0] = point[0];
        vert.position[1] = point[1];
        vert.position[2] = point[2];
        m_vertices.push_back(vert);
        
        // other side of the road
        vert = m_path.m_vertices[i];    // obtain the center line xyz.
        point[0] = vert.position[0];
        point[1] = vert.position[1];
        point[2] = vert.position[2];
        point -= perp * m_road_width/2;     // go 1/2 road width to left side.
        vert.position[0] = point[0];
        vert.position[1] = point[1];
        vert.position[2] = point[2];
        m_vertices.push_back(vert);
    }
    
}

void glRoad::generate_indices ( )
{
    long size = m_vertices.size();
    for (int i=0; i<size; i++)
        m_indices.push_back(i);
    
}

void glRoad::draw_primitive( )
{
    glDrawElements(GL_QUAD_STRIP, (int)m_indices.size(), GL_UNSIGNED_INT, 0 );
}


