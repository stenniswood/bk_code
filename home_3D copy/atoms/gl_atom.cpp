#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"


#define Debug 0


glAtom::glAtom( ) 
{ 
	m_IBO = 0;
	m_VBO = 0;
}

void	glAtom::create			( )
{
	generate_VBO();
	generate_IBO();
}
	
void	glAtom::change_color	( long mColor ) 
{
	if (Debug) printf("change_color( %4x ) %d\n", mColor, m_vertices.size() );
	for (int i=0; i<m_vertices.size(); i++)
	{
		m_vertices[i].color[0] = ((mColor & 0x00FF0000)>>16);
		m_vertices[i].color[1] = ((mColor & 0x0000FF00)>>8);
		m_vertices[i].color[2] = ((mColor & 0x000000FF));		
		m_vertices[i].color[3] = ((mColor & 0xFF000000)>>24);
	} 
}

void	glAtom::draw_body		( ) 
{ 

}	// Override this with open gl commands.


void 	glAtom::generate_VBO	( ) 
{ 
	//Create a new VBO and use the variable id to store the VBO id
	glGenBuffers( 1, &m_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );	// Make the new VBO active  GL_COLOR_ARRAY

	// Generate & Upload vertex data to the video device 
	int size = m_vertices.size() * sizeof(Vertex);
	glBufferData		 ( GL_ARRAY_BUFFER, size, m_vertices.data(), GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// vertex positions	
	glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(struct Vertex,color)));	// color
	glBindBuffer		 ( GL_ARRAY_BUFFER, m_VBO );

	//Draw Triangle from VBO - do each time window, view point or data changes
	//Establish its 3 coordinates per vertex with zero stride in this array; necessary here

	//glVertexAttribPointer(1, 3, GL_FLOAT, 		GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(struct Vertex, normal)));	// normals
	glEnableVertexAttribArray( 0 );	
	glEnableVertexAttribArray( 2 );	// GLKVertexAttribColor	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

// IBO:
void	glAtom::generate_IBO	( ) 
{ 
	int     size = m_indices.size();
	GLubyte* tmp = m_indices.data();

	glGenBuffers( 1, &m_IBO );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (size)*sizeof(GLubyte), 
				 tmp, GL_STATIC_DRAW );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
}

void	glAtom::print_indices	( ) 
{ 
	printf("print_indices()\n");
	for (int i=0; i<m_indices.size(); i++)
	{
		printf("%d : %d\n", i, m_indices[i] );
	}
}

void	glAtom::print_vertices	( bool mShowColors) 
{ 
	printf("print_vertices()  Number of Vertices=%d\n", m_vertices.size() );
	for (int i=0; i<m_vertices.size(); i++)
	{
		printf("%d : %5.1f %5.1f %5.1f\t", i, 
					m_vertices[i].position[0],
					m_vertices[i].position[1],
					m_vertices[i].position[2] );
		if (mShowColors)
			printf(": %d\t%d\t%d\t%d\n",
					m_vertices[i].color[0],
					m_vertices[i].color[1],
					m_vertices[i].color[2],
					m_vertices[i].color[3] );		
	}
}

