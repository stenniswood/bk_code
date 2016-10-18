//
//  face_box.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 8/1/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

// Include OpenCV here for the imread function!
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
// Include OpenCV here for the imread function!

#include "face_box.h"
#include "all_objects.h"

using namespace cv;
using namespace std;

#define Debug 0

CuboidTexture::CuboidTexture()
{
    for (int s=0; s<6; s++) {
        m_orientations[s] = 30;
        m_TBOs[s] = 0;
    }
}
CuboidTexture::~CuboidTexture()
{
    
}
Mat* CuboidTexture::load_image(string mFilename, int side, char mOrientation)
{
    m_images[side] = imread(mFilename);
    m_orientations[side] = mOrientation;
    return &(m_images[side]);
}

/* Allocate a TBO id and upload the image to video memory */
GLuint	CuboidTexture::generate_TBO	( int mSide )
{
    if (m_orientations[mSide]==30) return 0;
    
    uint8_t* pixelPtr   = (uint8_t*)m_images[mSide].data;
    int channels        = m_images[mSide].channels();
    int type;
    switch(channels) {
        case 1: break;
        case 2: break;
        case 3: format  = GL_RGB;	type = GL_UNSIGNED_BYTE;
            format2 = GL_BGR;
            break;
        case 4: format  = GL_RGBA;	type = GL_UNSIGNED_BYTE; //type = GL_UNSIGNED_INT_8_8_8_8;
            format2 = GL_RGBA;
            break;
        default: break;
    }
    dprintf("Texture::generate_TBO: rows=%d; cols=%d\n", m_images[mSide].rows, m_images[mSide].cols);
    glEnable     (GL_TEXTURE_2D );
    glGenTextures(1, &m_TBOs[mSide]     );
    glBindTexture(GL_TEXTURE_2D, m_TBOs[mSide]);
    //dprintf("Texture::generate_TBO: m_TBO=%d\n", m_TBOs[mSide]);
    
    /* may need these for odd sized images: */
    glPixelStorei(GL_UNPACK_ALIGNMENT,   1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,  0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,   0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 format,
                 m_images[mSide].cols, m_images[mSide].rows,
                 0,
                 format2,	 type,
                 pixelPtr );
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    return m_TBOs[mSide];
}
void  CuboidTexture::generate_TBO_allsides  ( )
{
    for (int s=0; s<6; s++)
        generate_TBO( s );
}

void CuboidTexture::generate_texture_coords_allsides	( )		// handle
{
    for (int s=0; s<6; s++)
        generate_texture_coords(m_orientations[s]);
    generate_VBOTexCoords();
}

void CuboidTexture::draw(int mSide)
{
    glEnable            (GL_TEXTURE_2D              );
    glBindTexture       (GL_TEXTURE_2D,   m_TBOs[mSide]   );
    glBindBuffer        (GL_ARRAY_BUFFER, m_TBOTexCoords );
    glEnableClientState (GL_TEXTURE_COORD_ARRAY     );
    glTexCoordPointer   (2, GL_FLOAT,   0, NULL     );
}

void CuboidTexture::apply_top( Mat* mImage, char mOrientation )     // Show the texture just on 1 side ()
{
    m_orientations[FACE_TOP_ID] = mOrientation;
    m_images[FACE_TOP_ID] = *mImage;
}
void CuboidTexture::apply_bottom( Mat* mImage, char mOrientation )
{
    m_orientations[FACE_BOTTOM_ID] = mOrientation;
    m_images[FACE_BOTTOM_ID] = *mImage;
}
void CuboidTexture::apply_front( Mat* mImage, char mOrientation )
{
    m_orientations[FACE_FRONT_ID] = mOrientation;
    m_images[FACE_FRONT_ID] = *mImage;
}
void CuboidTexture::apply_back( Mat* mImage, char mOrientation )
{
    m_orientations[FACE_BACK_ID] = mOrientation;
    m_images[FACE_BACK_ID] = *mImage;
}
void CuboidTexture::apply_left( Mat* mImage, char mOrientation )
{
    m_orientations[FACE_LEFT_ID] = mOrientation;
    m_images[FACE_LEFT_ID] = *mImage;
}
void CuboidTexture::apply_right( Mat* mImage, char mOrientation )
{
    m_orientations[FACE_RIGHT_ID] = mOrientation;
    m_images[FACE_RIGHT_ID] = *mImage;
}


/***************************** glFaceBox **************************************/
glFaceBox::glFaceBox(  )
{
    m_uses_normals = true;
    for (int i=0; i<6; i++) {
        m_orientations[i] = 0;
    }
    
    m_repetitions_x = 1.0;
    m_repetitions_y = 1.0;
    m_object_type_name = "textured box";
    m_object_class  = 9;
    m_side_applied	= TOP_SIDE_ID;
    m_texture = new CuboidTexture();
}

/* 4*6=24 vertices total. */
void glFaceBox::generate_vertices	( )
{
    struct Vertex_pnc v;
    set_vertex_color( v );

    // FACE #1: (back)
    v.normal[0]   = 0.0;
    v.normal[1]   = 0.0;
    v.normal[2]   = -1.0;
    v.position[0] = -width/2;
    v.position[1] = -height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = -height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = +height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    v.position[0] = -width/2;
    v.position[1] = +height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    
    // FACE #2: (front)
    v.normal[0]   = 0.0;
    v.normal[1]   = 0.0;
    v.normal[2]   = +1.0;
    v.position[0] = -width/2;
    v.position[1] = -height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = -height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = +height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = -width/2;
    v.position[1] = +height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );

    // FACE #3: (left)
    v.normal[0]   = -1.0;
    v.normal[1]   = 0.0;
    v.normal[2]   = 0.0;
    v.position[0] = -width/2;
    v.position[1] = -height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    v.position[0] = -width/2;
    v.position[1] = -height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = -width/2;
    v.position[1] = +height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = -width/2;
    v.position[1] = +height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );

    // FACE #4: (right)
    v.normal[0]   = +1.0;
    v.normal[1]   = 0.0;
    v.normal[2]   = 0.0;
    v.position[0] = +width/2;
    v.position[1] = -height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = -height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = +height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = +height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );

    // FACE #4: (top)
    v.normal[0]   = 0.0;
    v.normal[1]   = +1.0;
    v.normal[2]   = 0.0;
    v.position[0] = -width/2;
    v.position[1] = +height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = +height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = +height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    v.position[0] = -width/2;
    v.position[1] = +height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );

    // FACE #4: (bottom)
    v.normal[0]   = 0.0;
    v.normal[1]   = -1.0;
    v.normal[2]   = 0.0;
    v.position[0] = -width/2;
    v.position[1] = -height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = -height/2;
    v.position[2] = +depth/2;
    m_vertices.push_back( v );
    v.position[0] = +width/2;
    v.position[1] = -height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    v.position[0] = -width/2;
    v.position[1] = -height/2;
    v.position[2] = -depth/2;
    m_vertices.push_back( v );
    compute_min_max();
}

void glFaceBox::generate_indices ( )
{
    for (int i=0; i<24; i++)
        m_indices.push_back(i);
}

Mat* glFaceBox::load_image( string mFilename, int mSide, int mOrientation )
{
//    m_side[mSide] = new Texture();
//    m_side[mSide]->load_image(mFilename);
    ((CuboidTexture*)m_texture)->load_image(mFilename, mSide, mOrientation);
    return &(((CuboidTexture*)m_texture)->m_images[mSide]);
}

// Show the texture just on 1 side ()
/*void glFaceBox::wrap_3_sides( )
{
    
}
void glFaceBox::wrap_4_sides( )
{
    
}
void glFaceBox::wrap_5_sides( )
{
    
}
void glFaceBox::wrap_6_sides()
{
    
}*/

void glFaceBox::gl_register(  )
{
    glAtom::gl_register();       // Base class
    
    if (m_texture==NULL) return;
    ((CuboidTexture*)m_texture)->generate_TBO_allsides();
    ((CuboidTexture*)m_texture)->generate_texture_coords_allsides();
    ((CuboidTexture*)m_texture)->generate_VBOTexCoords();
    
/*    for (int s=0; s<6; s++)
        if (((CuboidTexture*)m_texture)->m_images[s])
        {
            m_side[s]->generate_TBO();
            m_side[s]->generate_texture_coords(m_orientations[s]);
            m_side[s]->generate_VBOTexCoords();
        } */
    //generate_texture_coords_4_side_stretch();
}

/*
    Top, front, bottom, back sides.
 */
GLuint glFaceBox::generate_texture_coords_4_side_stretch( )
{
    float hdh = 1.0/(height+depth+height+depth);
    float hwh = 1.0/(width);
    const float fs = 1.0;
    struct stTextCoord tc;
    
    // U                                            V
    tc.u = hwh*height;          tc.v = hdh*height;            m_texture->m_TexCoords.push_back( tc );
    tc.u = hwh*height;          tc.v = hdh*(height+depth);    m_texture->m_TexCoords.push_back( tc );
    tc.u = hwh*(height+width);  tc.v = hdh*(height+depth);    m_texture->m_TexCoords.push_back( tc );
    tc.u = hwh*(height+width);  tc.v = hdh*height ;           m_texture->m_TexCoords.push_back( tc );
    
    tc.u = 0.0;                 tc.v = 0.0;         m_texture->m_TexCoords.push_back( tc );
    tc.u = 0.0;                 tc.v = fs;          m_texture->m_TexCoords.push_back( tc );
    tc.u = fs;                  tc.v = fs;          m_texture->m_TexCoords.push_back( tc );
    tc.u = fs;                  tc.v = 0.0;         m_texture->m_TexCoords.push_back( tc );
    return m_texture->m_TexCoords.size()/2.;
}


void glFaceBox::draw_body()
{
    //Make the new VBO active. Repeat here incase changed since initialisation
    glBindBuffer    (GL_ARRAY_BUFFER, 		  m_VBO	);
    glBindBuffer    (GL_ELEMENT_ARRAY_BUFFER, m_IBO );
    
    glVertexPointer (3, GL_FLOAT, 		  sizeof(struct Vertex), NULL );
    glColorPointer  (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
    glNormalPointer (GL_FLOAT,            sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)) );
    
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY  );

    int offset = 0;
    for (int s=0; s<6; s++)
    {
        
        bool is_loaded = false;
        if (m_texture!=NULL)
            is_loaded = (((CuboidTexture*)m_texture)->m_images[s].rows>0);
        if (is_loaded)
        {
            ((CuboidTexture*)m_texture)->draw(s);
            glDrawElements(GL_QUADS,(int)4, GL_UNSIGNED_INT,
                           (GLvoid*)(offset*sizeof(GL_UNSIGNED_INT)));
            ((CuboidTexture*)m_texture)->after_draw();
        }
        else
            glDrawElements(GL_QUADS, (int)4, GL_UNSIGNED_INT,
                           (GLvoid*)(offset*sizeof(GL_UNSIGNED_INT)));
        offset += 4;
    }
}

/* The problem with putting different images on each face:
 a) Every vertex has to have a corresponding TexCoord.
 
 the order of vertices must match the order of the TexCoords.
 
 However, the order of the vertices wont match the order needed by GL_QUADS.
 the index buffer fixes this.
 
 When doing the front (for instance),  we are using vertex (0,1,5,4)
 And the TexCoords are (0,1,2,3, 4,5,6,7)
 
 so 0=<0,0>;   1=<1,0>;  2=<1,1>;  3=<1,0>
 
 How do we fix this, b/c loading the texture, it would not know the set (0,1,5,4).
 
 Best way really is to store duplicates of the vertices for all 6 faces.
 This would allow normals for all faces too.
 
 A whole new class needed?  FaceBox created.
 We need 24 TexCoords too.
 
 */
void glFaceBox::print_info()
{
    printf("glFaceBox()  <x,y,z> = <%6.3f %6.3f %6.3f> \n",
           m_x,m_y,m_z );
    printf(" width=%6.3f;  height=%6.3f; depth=%6.3f> \n",
           width, height, depth );
}


/* this does a */
GLuint glFaceBox::generate_texture_coords( int mSide )
{
    // Texture coords [0.0..1.0]   1.0 represents the far edge of the image.
    // Pretty sure we need 1 coordinate for each vertex.
    //   So if we're just doing 1 face.  It should mean just 4 text coords.
    //m_NumTexCoords = 8;
    /* the texCoords have to be all in 1 array.  
        There has to be 24 of them to match the vertices!
        so we combine them into m_TexCoords
     */
    
    ((CuboidTexture*)m_texture)->generate_texture_coords_allsides();
    
/*  struct stTextCoord tc;
 for (int s=0; s<6; s++)
    {
        if (m_side[s]) {
            m_side[s]->generate_texture_coords();
            m_TexCoords.push_back( m_side[s]->m_TexCoords[0]);
            m_TexCoords.push_back( m_side[s]->m_TexCoords[1]);
            m_TexCoords.push_back( m_side[s]->m_TexCoords[2]);
            m_TexCoords.push_back( m_side[s]->m_TexCoords[3]);
        }
        else
        {
            tc.u = 0.0;   tc.v = 0.0;    m_TexCoords.push_back ( tc );     // 4 vertices Bottom
            tc.u = 0.0;   tc.v = 1.0;    m_TexCoords.push_back ( tc );
            tc.u = 1.0;   tc.v = 1.0;    m_TexCoords.push_back ( tc );
            tc.u = 1.0;   tc.v = 0.0;    m_TexCoords.push_back ( tc );
        }
    } */
    return 0;
}


