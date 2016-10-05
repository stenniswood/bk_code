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

#include "all_objects.h"
#include "txt_cube.hpp"

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


txtContainer::txtContainer(  )
:glContainer()
{
	//m_image		= NULL;
	m_TBO			= 0;
	m_side_applied	= TOP_SIDE_ID;
}

void txtContainer::load_image( string mFilename )
{
	printf("txtContainer::load_image( %s )\n", mFilename.c_str() );
	m_src = imread(mFilename);

	//cvtColor( src, m_src, CV_BGR2RGB );
	// Data goes:  BGR 
	imshow( "Display Image", m_src );	
}

void txtContainer::setup  		(   )
{
	glContainer::setup();
}

void txtContainer::gl_register(  )
{
	generate_VBO();
	generate_IBO();
	generate_TBO();
	generate_texture_coords( );
	generate_VBOTexCoords  ( );
}

GLuint txtContainer::generate_TBO() 
{
		uint8_t* pixelPtr   = (uint8_t*)m_src.data;
		int channels        = m_src.channels();
		int bytes_per_pixel = m_src.elemSize();
		//int format,format2,type;
		switch(channels) {
		case 1: break;
		case 2: break;
		case 3: format = 3;	type =  GL_UNSIGNED_BYTE;
				format2 = GL_BGR;
				break;
		case 4: format = GL_RGBA;	type = GL_UNSIGNED_INT_8_8_8_8;
				format2 = GL_RGB;
				break;
		default: break;
		}
		printf("txtContainer: pixPtr=%4x; channels=%d; bytes_per_pixel=%d\n", pixelPtr, channels, bytes_per_pixel);
		printf("txtContainer: rows=%d; cols=%d\n", m_src.rows, m_src.cols);

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &m_TBO );
		glBindTexture(GL_TEXTURE_2D, m_TBO);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);		
		glTexImage2D(GL_TEXTURE_2D,
				 0,
				 format,
				 m_src.cols, m_src.rows,
				 0,
				 format2,	 type,
			 	 pixelPtr );  

		printf("txtContainer: m_TBO=%d\n", m_TBO);

	//  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// gluBuild2DMipmaps(GL_TEXTURE_2D, channels, m_src.cols, m_src.rows, format2, GL_UNSIGNED_BYTE, pixelPtr);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glGenerateMipmap(GL_TEXTURE_2D);
	  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		glDisable(GL_TEXTURE_2D);
	
	return m_TBO;
}

/*
	-1.0, -1.0,  1.0,	 // 0
	 1.0, -1.0,  1.0,	 // 1
	 1.0, -1.0, -1.0,	 // 2
	-1.0, -1.0, -1.0,	 // 3

	-1.0, 1.0,  1.0,	 // 4
	 1.0, 1.0,  1.0,     // 5
	 1.0, 1.0, -1.0,     // 6
	-1.0, 1.0, -1.0,	 // 7
*/
GLuint txtContainer::generate_texture_coords( )
{
	// Texture coords [0.0..1.0]   1.0 represents the far edge of the image.
	// Pretty sure we need 1 coordinate for each vertex.
	//   So if we're just doing 1 face.  It should mean just 4 text coords.
	//m_NumTexCoords = 8;
	const float fs = 1.0/4.;
	
	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( 0.0 );
	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( fs  );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( fs  );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( 0.0 );

	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( 0.0 );
	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( fs );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( fs );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( 0.0 );

/*
	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( 0.0 );
	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( fs );	
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( fs );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( 0.0 );

	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( 0.0 );
	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( fs );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( fs );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( 0.0 );
*/
	return m_TexCoords.size()/2;
}

void txtContainer::generate_VBOTexCoords()
{
	// Generate And Bind The Texture Coordinate Buffer 
    glGenBuffers( 1, &m_VBOTexCoords );                 	// Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, m_VBOTexCoords );        // Bind The Buffer
    glBufferData( GL_ARRAY_BUFFER, m_TexCoords.size()*sizeof(float), 
    				 m_TexCoords.data(), GL_STATIC_DRAW );
}

void txtContainer::draw_body()
{
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	if (m_is_closed)	// GL_QUADS 
	{
		glEnable		   (GL_TEXTURE_2D);
		glBindTexture	   (GL_TEXTURE_2D,   m_TBO );
		glBindBuffer	   (GL_ARRAY_BUFFER, m_VBOTexCoords );	
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer  (2, GL_FLOAT,   0, NULL);  

		glDrawElements(GL_QUADS,  m_indices.size(), GL_UNSIGNED_INT, (GLvoid*)((char*)NULL));

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);					
		glDisable	(GL_TEXTURE_2D);	
	}
}

void txtContainer::print_info()
{
	printf("txtContainer()  <x,y,z> = <%6.3f %6.3f %6.3f> \n", 
			m_x,m_y,m_z );
	printf(" width=%6.3f;  height=%6.3f; depth=%6.3f> \n", 
			width, height, depth );
}

