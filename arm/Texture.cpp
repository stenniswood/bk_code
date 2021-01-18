#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>


#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <vector>
#include <stdio.h>
//#include "all_objects.h"
#include "Texture.hpp"
#include <SOIL/SOIL.h>


// Include OpenCV here for the imread function!
/*#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp" */
// Include OpenCV here for the imread function!
//#include "global_funcs.h"


//using namespace cv ;
using namespace std;

#define Debug 0

#define Dprintf if(Debug)printf


Texture::Texture()
{    
    m_TBO   =   0;
    m_repetitions_x = 1;        // 1 image per object default
    m_repetitions_y = 1;
    m_width_in_inches  = 12;
    m_height_in_inches = 12;
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_TBO);
}

void Texture::load_image( string mFilename )
{ 
	Dprintf("Texture::load_image( %s )\n", mFilename.c_str() );
	

	m_image = SOIL_load_image("../home_3D/soccer_ball_3.jpg", &m_cols, &m_rows, 0, SOIL_LOAD_RGB);
	//glTexImage2D	( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_image );

              
//PUT BACK!!	m_image = imread(mFilename);

    //imshow( "Display Image2", m_image );
	//cvtColor( dst, cdst, COLOR_GRAY2BGR );	
}

/* Allocate a TBO id and upload the image to video memory */
GLuint	Texture::generate_TBO	( )
{
    //int bytes_per_pixel = m_image.elemSize();
    uint8_t* pixelPtr   = (uint8_t*)m_image;
    int channels        = 3; //SOIL default.  m_image.channels();
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
    Dprintf("Texture::generate_TBO: rows=%d; cols=%d\n", m_rows, m_cols);
    glEnable     (GL_TEXTURE_2D );
    glGenTextures(1, &m_TBO     );
    glBindTexture(GL_TEXTURE_2D, m_TBO);
    //Dprintf("Texture::generate_TBO: m_TBO=%d\n", m_TBO);

    /* may need these for odd sized images: */
//    glPixelStorei(GL_UNPACK_ALIGNMENT,   1);
//    glPixelStorei(GL_UNPACK_ROW_LENGTH,  0);
//    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
//    glPixelStorei(GL_UNPACK_SKIP_ROWS,   0);
    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
             0,
             format,
             m_cols, m_rows,
             0,
             format2,	 type,
             pixelPtr );
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	return m_TBO;
}

// data to match vertexes.  Unique to each object!
GLuint	Texture::generate_grid_coords( int mWidth, int mHeight )
{
    struct stTextCoord tc;
    float x_incr = (float)m_repetitions_x / mWidth;
    float y_incr = (float)m_repetitions_y / mHeight;
    for (int y=0; y<mHeight; y++)
    {
        tc.v = y*y_incr;
        for (int x=0; x<mWidth; x++)
        {
            tc.u = x*x_incr;
            m_TexCoords.push_back(tc);
        }
    }
    return (GLuint)m_TexCoords.size();
}


GLuint	Texture::generate_texture_coords( int mRotation )
{
    /* Fill in in derived class.  
       Here only a simple square coordinates.
     */
    float fsx = m_repetitions_x;
    float fsy = m_repetitions_y;
    struct stTextCoord tc[4];
/*
4  5
7  6

5  6
4  7
*/
    // U								V
    tc[0].u = 0.0;      tc[0].v = 0.0;
    tc[1].u = 0.0;      tc[1].v = fsy;
    tc[2].u = fsx;      tc[2].v = fsy;
    tc[3].u = fsx;      tc[3].v = 0.0;
    switch (mRotation)
    {
        case 0: m_TexCoords.push_back( tc[0] );
            m_TexCoords.push_back( tc[1] );
            m_TexCoords.push_back( tc[2] );
            m_TexCoords.push_back( tc[3] );
            break;
        case 1:
            m_TexCoords.push_back( tc[1] );
            m_TexCoords.push_back( tc[2] );
            m_TexCoords.push_back( tc[3] );
            m_TexCoords.push_back( tc[0] );
            break;
        case 2:
            m_TexCoords.push_back( tc[2] );
            m_TexCoords.push_back( tc[3] );
            m_TexCoords.push_back( tc[0] );
            m_TexCoords.push_back( tc[1] );
            break;
        case 3:
            m_TexCoords.push_back( tc[3] );
            m_TexCoords.push_back( tc[0] );
            m_TexCoords.push_back( tc[1] );
            m_TexCoords.push_back( tc[2] );
            break;
        default:m_TexCoords.push_back( tc[0] );
            m_TexCoords.push_back( tc[1] );
            m_TexCoords.push_back( tc[2] );
            m_TexCoords.push_back( tc[3] );
            break;
    }
    return (GLuint)m_TexCoords.size();
}

void Texture::generate_VBOTexCoords()
{
	// Generate And Bind The Texture Coordinate Buffer
    glGenBuffers( 1, &m_TBOTexCoords ); 	                // Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, m_TBOTexCoords );        // Bind The Buffer
    glBufferData( GL_ARRAY_BUFFER, m_TexCoords.size()*sizeof(float)*2,
    				 m_TexCoords.data(), GL_STATIC_DRAW );
}

void Texture::pre_draw()
{
    glEnable            (GL_TEXTURE_2D              );
    glBindTexture       (GL_TEXTURE_2D, m_TBO       );
    glBindBuffer        (GL_ARRAY_BUFFER, m_TBOTexCoords );
    glEnableClientState (GL_TEXTURE_COORD_ARRAY     );
    glTexCoordPointer   (2, GL_FLOAT,   0, NULL     );

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::after_draw()
{
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable           (GL_TEXTURE_2D);
}

void Texture::print_info()
{ 

}

