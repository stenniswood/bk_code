#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"

#include "txt_container.hpp"

// Include OpenCV here for the imread function!  
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
// Include OpenCV here for the imread function!

using namespace cv;
using namespace std;


Texture::Texture()
{ 
}

void	Texture::load_image( string mFilename )
{ 
	printf("Texture::load_image( %s )\n", mFilename.c_str() );	
	m_src = imread(mFilename);
	imshow( "Display Image2", m_src );	
	//cvtColor( dst, cdst, COLOR_GRAY2BGR );	
}

GLuint	Texture::generate_TBO			( )
{ 
		uint8_t* pixelPtr   = (uint8_t*)m_src.data;
		int channels        = m_src.channels();
		int bytes_per_pixel = m_src.elemSize();
		int format,type;
		switch(channels) {
			case 1: break;
			case 2: break;
			case 3: format = GL_RGB;	type = GL_UNSIGNED_BYTE;
					break;
			case 4: format = GL_RGBA;	type = GL_UNSIGNED_INT_8_8_8_8;
					break;
			default: break;
		}
		printf("Texture::generate_TBO: rows=%d; cols=%d\n", m_src.rows, m_src.cols);
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &m_TBO );
		glBindTexture(GL_TEXTURE_2D, m_TBO);
		printf("Texture::generate_TBO: m_TBO=%d\n", m_TBO);		
		glTexImage2D(GL_TEXTURE_2D,
				 0,
				 format,
				 m_src.cols, m_src.rows,
				 0,
				 format,	 type,
			 	 pixelPtr );
	return m_TBO;
}

void Texture::generate_VBOTexCoords()
{
	// Generate And Bind The Texture Coordinate Buffer 
    glGenBuffers( 1, &m_TBOTexCoords ); 	                // Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, m_TBOTexCoords );        // Bind The Buffer
    // Load The Data
    glBufferData( GL_ARRAY_BUFFER, m_TexCoords.size()*sizeof(float), 
    				 m_TexCoords.data(), GL_STATIC_DRAW );
}

GLuint	Texture::generate_texture_coords	( )
{ 
	return 0;
}

void Texture::draw()
{ 
		glEnable		(GL_TEXTURE_2D);
		glBindBuffer	(GL_TEXTURE_2D, m_TBO );	
		//printf("Texture:draw() m_TBO=%d\n", m_TBO);		
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	// or GL_MODULATE.
//		glTexEnvi		(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindBuffer	(GL_ARRAY_BUFFER, m_TBOTexCoords );	
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer  (2, GL_FLOAT,   0, NULL);
} 

void	Texture::after_draw()
{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);					
		glDisable	(GL_TEXTURE_2D);	
}

void Texture::print_info()
{ 

}

