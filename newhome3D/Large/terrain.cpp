//
//  terrain.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/9/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "terrain.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <vector>
#include <stdio.h>
#include "all_objects.h"

// Include OpenCV here for the imread function!
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
// Include OpenCV here for the imread function!
using namespace cv ;
using namespace std;


glTerrain::glTerrain()
{
    m_stretch_x = 5.0;
    m_stretch_y = 5.0;
    m_y_scale = 0.5;
}
glTerrain::~glTerrain()
{
    
}

void glTerrain::set_stretch( float mStretchX, float mStretchY )
{
    m_stretch_x = mStretchX;
    m_stretch_y = mStretchY;
}

void glTerrain::load_image( string mFilename )
{
    printf("TerrainMap::load_image( %s )\n", mFilename.c_str() );
    m_image = imread(mFilename, IMREAD_GRAYSCALE);      // Note imread can't use "~/" for a pathname!  results in 0 rows,cols.
    
   // Mat dst;
   // cvtColor( m_image, dst, COLOR_BGR2GRAY );
}

/* An Omission is a region where the contour is made to skip where the building 
 (presumably the excavators have bulldozed/dug the basement/terrain to a constant level.  
 therefore it is set to .1 inch below the floor.
 */
void glTerrain::add_omission( glDwellingLevel* mOmitBuilding )
{
    struct omission_ranges omr;
    mOmitBuilding->compute_max();
    mOmitBuilding->compute_min();

    omr.sx = mOmitBuilding->m_x + mOmitBuilding->m_min.position[0];
    omr.sz = mOmitBuilding->m_z + mOmitBuilding->m_min.position[2];
    omr.ex = mOmitBuilding->m_x + mOmitBuilding->m_max.position[0];
    omr.ez = mOmitBuilding->m_z + mOmitBuilding->m_max.position[2];
    
//    omr.ex = mOmitBuilding->get_max_x();
//    omr.ez = mOmitBuilding->get_max_z();
    omr.floor = mOmitBuilding->m_y-0.1;
    m_omissions.push_back( omr );
}

void glTerrain::add_omission( glBox* mOmitBox )
{
    struct omission_ranges omr;
    omr.sx = mOmitBox->m_x;
    omr.sz = mOmitBox->m_z;
    omr.ex = mOmitBox->m_x + mOmitBox->width;
    omr.ez = mOmitBox->m_z + mOmitBox->depth;
    omr.floor = mOmitBox->m_y-0.1;
    m_omissions.push_back( omr );
}

void glTerrain::compute_normal( )
{
    
}
void glTerrain::compute_normals( )
{
    
}
void glTerrain::generate_vertices()
{
    long x_size = m_image.cols;
    long y_size = m_image.rows;
    long index;
    long size=0;
    struct Vertex v;
    set_vertex_color(v );
    float x_offset=x_size/2.*m_stretch_x;
    float y_offset=y_size/2.*m_stretch_y;
    
    for (int x=0; x<x_size; x++)            // cols
        for (int y=0; y<y_size; y++)        // rows
        {
            index = y*m_image.cols + x;
            v.color[0] = 0xFF;
            v.color[1] = m_image.data[index] & 0xFF;
            v.color[2] = m_image.data[index] & 0xFF;
            v.color[3] = 0xFF;
            
            v.position[0] = x*m_stretch_x - x_offset;
            v.position[2] = y*m_stretch_y - y_offset;
            v.position[1] = m_image.data[index] * m_y_scale;

            size = m_omissions.size();
            for (int o=0; o<size; o++)
            {
               if   ((v.position[0] > m_omissions[o].sx) && (v.position[0] < m_omissions[o].ex))
                 if ((v.position[2] > m_omissions[o].sz) && (v.position[2] < m_omissions[o].ez))
                    v.position[1] = m_omissions[o].floor;
            }
            m_vertices.push_back( v );
        }
}

void glTerrain::generate_indices()
{
    GLuint x_size = m_image.cols;
    GLuint y_size = m_image.rows;
    GLuint col1_start = 0;
    GLuint col2_start = y_size;

    struct Vertex v;
    set_vertex_color( v );
    
    for (int x=0; x<(x_size/2-2); x++)          // cols
    {
        for (int y=0; y<(y_size-2); y++)        // rows
        {
            m_indices.push_back( col1_start+y );
            m_indices.push_back( col2_start+y );
        }
        col1_start += y_size;     // leap frog
        col2_start += y_size;     //

        for (int y=y_size-2; y>0; y--)        // rows
        {
            m_indices.push_back( col1_start+y );
            m_indices.push_back( col2_start+y );
        }
        
        // how many vertices does it jump form 1 col to the next?  answer: y_size
        col1_start += y_size;     // leap frog
        col2_start += y_size;     //
    }
}

void glTerrain::draw_primitive( )
{
    glDrawElements(GL_QUAD_STRIP, (int)m_indices.size(), GL_UNSIGNED_INT, 0 );
}

