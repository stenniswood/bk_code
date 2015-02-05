#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "gl_container.hpp"
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
	//imshow( "Display Image", m_src );	
}

GLuint txtContainer::generate_TBO() 
{
//	if (m_image)
	{
		uint8_t* pixelPtr   = (uint8_t*)m_src.data;
		int channels        = m_src.channels();
		int bytes_per_pixel = m_src.elemSize();
		int format,format2,type;
		printf("txtContainer: channels=%d; bytes_per_pixel=%d\n", channels, bytes_per_pixel);
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
		printf("txtContainer: rows=%d; cols=%d\n", m_src.rows, m_src.cols);
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &m_TBO );
		glBindTexture(GL_TEXTURE_2D, m_TBO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);		
		glTexImage2D(GL_TEXTURE_2D,
				 0,
				 format,
				 m_src.cols, m_src.rows,
				 0,
				 format2,	 type,
			 	 pixelPtr );
		printf("txtContainer: m_TBO=%d\n", m_TBO);
	}
	return m_TBO;
}


GLuint txtContainer::generate_texture_coords( )
{
	// Texture coords [0.0..1.0]   1.0 represents the far edge of the image.
	// Pretty sure we need 1 coordinate for each vertex.
	//   So if we're just doing 1 face.  It should mean just 4 text coords.
	//m_NumTexCoords = 8;
	const float fs = 1.0;
	
	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( 0.0 );
	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( fs );	
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( fs );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( 0.0 );

	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( 0.0 );
	m_TexCoords.push_back( 0.0 );			m_TexCoords.push_back( fs );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( fs );
	m_TexCoords.push_back( fs );			m_TexCoords.push_back( 0.0 );

	return m_TexCoords.size()/2;
}

void txtContainer::generate_VBOTexCoords()
{
	// Generate And Bind The Texture Coordinate Buffer 
    glGenBuffers( 1, &m_VBOTexCoords );                 	// Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, m_VBOTexCoords );        // Bind The Buffer
    // Load The Data
    glBufferData( GL_ARRAY_BUFFER, m_TexCoords.size()*sizeof(float), 
    				 m_TexCoords.data(), GL_STATIC_DRAW );
}

void txtContainer::draw()
{
	glTranslatef(m_x, m_y, m_z);
	//glColor3f (1.0f, 1.0f, 1.0f);
		
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	if (m_is_closed)	// GL_QUADS
	{
		glEnable		(GL_TEXTURE_2D);
		glActiveTexture(m_TBO);
		glBindBuffer	(GL_TEXTURE_2D, m_TBO );	
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindBuffer	(GL_ARRAY_BUFFER, m_VBOTexCoords );	
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer  (2, GL_FLOAT,   0, NULL); 

//		glDrawElements(GL_QUAD_STRIP, NUMBER_OF_CUBE_INDICES, GL_UNSIGNED_BYTE, 
//				(GLvoid*)((char*)NULL));
		glDrawElements(GL_QUADS, 8, GL_UNSIGNED_BYTE, (GLvoid*)((char*)NULL));

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);					
		glDisable	(GL_TEXTURE_2D);	
	}

	// glDrawArrays(GL_TRIANGLE_STRIP, 0, m_number_of_vertices );
	glTranslatef(-m_x, -m_y, -m_z);
}

void txtContainer::print_info()
{
	printf("txtContainer()  <x,y,z> = <%6.3f %6.3f %6.3f> \n", 
			m_x,m_y,m_z );
	printf(" width=%6.3f;  height=%6.3f; depth=%6.3f> \n", 
			width, height, depth );
}


/*GLuint txtContainer::generate_TBOi() 
{
	if (m_image==NULL) return 0;
	
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &m_TBO );
		glBindTexture(GL_TEXTURE_2D, m_TBO);
		glTexImage2D(GL_TEXTURE_2D,
				 0,
				 m_image->format,
				 m_image->width, m_image->height,
				 0,
				 m_image->format,
				 m_image->type,
				 m_image->pixels );
	return m_TBO;
}*/
