#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"

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


glPaper::glPaper( int mSamples_per_inch )
{
	m_TBO 		   = 0;
	m_VBOTexCoords = 0;

	m_samples_per_inch = mSamples_per_inch;
	m_Xsamples = 0;
	m_Ysamples = 0;
	printf("samples_per_inch=%d\n", m_samples_per_inch );
}

void glPaper::load_texture( const char* mFilename )
{
	printf("glPaper::load_image( %s )\n", mFilename );
	m_src = imread(mFilename);
}

void glPaper::Initialize( float mWidth, float mHeight )
{
	m_Xsamples = m_samples_per_inch * mWidth;
	m_Ysamples = m_samples_per_inch * mHeight;
	float Xincrement = mWidth / m_Xsamples;
	float Yincrement = mHeight/ m_Ysamples;
	struct Vertex v;
	float  xpos,ypos;
	printf("xsamples=%d;  ysamples=%d  \n", m_Xsamples, m_Ysamples );
	printf("mWidth=%6.1f; mHeight=%6.1f\n", mWidth, mHeight );
		
	// Create a grid of vertices : 
	for (int y=0; y<m_Ysamples; y++)
	{
		ypos = y*Yincrement;
		for (int x=0; x<m_Xsamples; x++)
		{
			xpos = x * Xincrement;
			v.position[0] = xpos;
			v.position[1] = 0.0;
			v.position[2] = ypos;
			m_vertices.push_back(v);
		}
	}
	
	// Create a grid of TexCoordinates:
	struct stTexCoord tc;
	Xincrement = 1.0 / m_Xsamples;
	Yincrement = 1.0 / m_Ysamples;
	for (int y=0; y<m_Ysamples; y++)	
	{
		ypos = y * Yincrement;
		for (int x=0; x<m_Xsamples; x++)
		{
			xpos = x * Xincrement;
			tc.v = xpos;
			tc.u = ypos;
			m_TexCoords.push_back( tc );
		}
	}
}

void glPaper::generate_indices( )
{
	int number_of_quads = (m_Xsamples-1)*(m_Ysamples-1);
	int index,index2;
	m_indices.clear();
	for (int y=0; y<m_Ysamples-1; y++)
	{
		index  = y * m_Xsamples;
		index2 = index + m_Xsamples;
		for (int x=0; x<m_Xsamples; x++)
		{
			m_indices.push_back( index + x+0 );
			m_indices.push_back( index + x+1 );
			m_indices.push_back( index2+ x+1 );
			m_indices.push_back( index2+ x+0 );
		}
	}
}


GLuint glPaper::generate_TBO() 
{
	uint8_t* pixelPtr   = (uint8_t*)m_src.data;
	int channels        = m_src.channels();
	int bytes_per_pixel = m_src.elemSize();
	int format,format2,type;
	printf("glPaper: channels=%d; bytes_per_pixel=%d\n", channels, bytes_per_pixel);
	switch(channels) {
	case 1: break;
	case 2: break;
	case 3: format = 3;		format2 = GL_BGR;	type=GL_UNSIGNED_BYTE;
			break;
	case 4: format = GL_RGBA;	type = GL_UNSIGNED_INT_8_8_8_8;
			format2 = GL_RGB;
			break;
	default: break;
	}
	printf("glPaper: rows=%d; cols=%d\n", m_src.rows, m_src.cols);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &m_TBO );
	glBindTexture(GL_TEXTURE_2D, m_TBO);
	glTexImage2D(GL_TEXTURE_2D,
			 0,
			 format,
			 m_src.cols, m_src.rows,
			 0,
			 format2,	 type,
			 pixelPtr );
	printf("glPaper: m_TBO=%d\n", m_TBO);
	imshow( "Display Image", m_src );
	return m_TBO;
}

void glPaper::generate_VBOTexCoords()
{
	// Generate And Bind The Texture Coordinate Buffer 
    glGenBuffers( 1, &m_VBOTexCoords );                 	// Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, m_VBOTexCoords );        // Bind The Buffer
    // Load The Data
    glBufferData( GL_ARRAY_BUFFER, m_TexCoords.size()*sizeof(float), 
    				 m_TexCoords.data(), GL_STATIC_DRAW );
}

void glPaper::create( )
{
	generate_TBO		 ( );
	change_color( 0xFFFFFF00 );
	generate_indices	 ( );		//print_indices();	
	generate_VBO		 ( );
	generate_IBO		 ( );

	generate_VBOTexCoords( );
}

void glPaper::draw_body	( )
{
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer	   (3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer 	   (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
	glEnableClientState(GL_COLOR_ARRAY);
	
	//glDrawArrays(GL_LINE_LOOP, 0, m_vertices.size() );
	//glDrawElements(GL_QUADS, m_indices.size(), GL_UNSIGNED_INT, (GLvoid*)((char*)NULL));
	glEnable		(GL_TEXTURE_2D);
	glBindBuffer	(GL_TEXTURE_2D, m_TBO );	
	//printf(" paper TBO = %d\n", m_TBO);
	glTexEnvi		(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		
	glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindBuffer	(GL_ARRAY_BUFFER, m_VBOTexCoords );
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer  (2, GL_FLOAT,   0, NULL);

	glDrawElements(GL_QUADS, m_indices.size(), GL_UNSIGNED_INT, (GLvoid*)((char*)NULL));

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);					
//	glDisable	(GL_TEXTURE_2D);

}
