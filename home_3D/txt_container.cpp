#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "gl_container.hpp"
#include "txt_container.hpp"

using namespace std;


txtContainer::txtContainer(  )
:glContainer()
{
	m_image			= NULL;
	m_TBO			= 0;
	m_side_applied	= TOP_SIDE_ID;
}

void txtContainer::load_image( string mFilename )
{
	m_image = loadBMP( mFilename.c_str() );
	printf("load_image=%x\n", m_image );
}

GLuint txtContainer::generate_TBO() 
{
	if (m_image)
	{
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
	}
	return m_TBO;
}

GLuint txtContainer::generate_texture_coords( )
{
	// Texture coords [0.0..1.0]   1.0 represents the far edge of the image.
	// Pretty sure we need 1 coordinate for each vertex.
	//   So if we're just doing 1 face.  It should mean just 4 text coords.
	m_NumTexCoords = 8;
	m_pTexCoords = new float[m_NumTexCoords*2];
	m_pTexCoords[0] = 0.0;			m_pTexCoords[1] = 0.0;
	m_pTexCoords[2] = 1.0;			m_pTexCoords[3] = 0.0;	
	m_pTexCoords[4] = 1.0;			m_pTexCoords[5] = 1.0;
	m_pTexCoords[6] = 0.0;			m_pTexCoords[7] = 1.0;

	m_pTexCoords[ 8] = 0.0;			m_pTexCoords[9] = 0.0;
	m_pTexCoords[10] = 1.0;			m_pTexCoords[11] = 0.0;	
	m_pTexCoords[12] = 1.0;			m_pTexCoords[13] = 1.0;
	m_pTexCoords[14] = 0.0;			m_pTexCoords[15] = 1.0;

	return m_NumTexCoords;
}

void txtContainer::generate_VBOTexCoords()
{
	// Generate And Bind The Texture Coordinate Buffer 
    glGenBuffers( 1, &m_VBOTexCoords );                 // Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, m_VBOTexCoords );        // Bind The Buffer
    // Load The Data
    glBufferData( GL_ARRAY_BUFFER, m_NumTexCoords*2*sizeof(float), 
    				 m_pTexCoords, GL_STATIC_DRAW );
}

void txtContainer::draw()
{
//	glContainer::draw();
	glTranslatef(m_x, m_y, m_z);
		
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO	);		
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );
	
	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	if (m_is_closed)	// GL_QUADS
	{
		glEnable		(GL_TEXTURE_2D);
		glBindBuffer	(GL_TEXTURE_2D, m_TBO );	
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindBuffer	(GL_ARRAY_BUFFER, m_VBOTexCoords );	
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 	0, NULL); 

//		glDrawElements(GL_QUAD_STRIP, NUMBER_OF_CUBE_INDICES, GL_UNSIGNED_BYTE, 
//				(GLvoid*)((char*)NULL));
		glDrawElements(GL_QUADS, NUMBER_OF_QUAD_INDICES, GL_UNSIGNED_BYTE, 
					(GLvoid*)((char*)NULL));
		
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);					
		glDisable	(GL_TEXTURE_2D);	
	}
	//else 	//GL_QUAD_STRIP


	// glDrawArrays(GL_TRIANGLE_STRIP, 0, m_number_of_vertices );
	// GL_POLYGON, GL_LINE_LOOP
	glTranslatef(-m_x, -m_y, -m_z);		 

}

void txtContainer::print_info()
{
	printf("txtContainer()  <x,y,z> = <%6.3f %6.3f %6.3f> \n", 
			m_x,m_y,m_z );
	printf(" width=%6.3f;  height=%6.3f; depth=%6.3f> \n", 
			width, height, depth );
}

