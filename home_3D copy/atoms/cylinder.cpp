#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "gl_container.hpp"
#include "cylinder.hpp"


#define  CLOSE_OUT 		1
#define  TOP_AND_BOTTOM 2
#define  BUFFER_OFFSET(i) ((char *)NULL + (i))

glCylinder::glCylinder( int mNumberSamples )
{
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;
	m_color 	 = 0xFFFFFFFF;
	m_ends_color = 0xFF00FFFF;
	m_is_closed  = true;
	m_number_of_samples  = mNumberSamples;
	m_number_of_vertices = m_number_of_samples*2;
}

void glCylinder::generate_vertices()
{
	vertices = (Vertex*)malloc( sizeof(Vertex)*(m_number_of_vertices) );
	
	float TwoPi     = 2.*M_PI;
	float increment = (TwoPi / ((float)m_number_of_samples));
	int   i;
	float a=0.;
	for (i=0; i<(m_number_of_samples); a+=increment, i++)
	{
		vertices[i].position[0] =  m_radius * sin(a);
		vertices[i].position[1] = -m_height/2.0;
		vertices[i].position[2] =  m_radius * cos(a);
	}
	
	int final = 0;
	a=0.;
	for (; i<(m_number_of_samples)*2; a+=increment, i++)
	{
		vertices[i].position[0] = m_radius * sin(a);
		vertices[i].position[1] = m_height/2.0;
		vertices[i].position[2] = m_radius * cos(a);
		final = i;
	}
}

void glCylinder::generate_vertices_colors()
{
	for (int i=0; i<m_number_of_vertices; i++)
	{
		if (i<m_number_of_samples)	// Bottom Blue
		{
			vertices[i].color[0] = 0x00;
			vertices[i].color[1] = 0x20;
			vertices[i].color[2] = 0xFF;
			vertices[i].color[3] = 0xFF;
		} else {					// Top Red 
			vertices[i].color[0] = 0xFF;
			vertices[i].color[1] = 0x00;
			vertices[i].color[2] = 0x00;
			vertices[i].color[3] = 0xFF;
		}	// Sides Green 
	}
}

void glCylinder::generate_side_IBO( GLubyte* isptr )
{		
	int iptr_index = 0;
	for (int i=0; i<m_number_of_samples; i++)
	{
		// These are the indices into vertices[] array : 
		isptr[iptr_index++] = i;						// Bottom sample
		isptr[iptr_index++] = i+m_number_of_samples;	// Top sample
	}
	if (CLOSE_OUT) {
		isptr[iptr_index++] = 0;						// Close out
		isptr[iptr_index++] = m_number_of_samples;		//
	}
}

GLbyte glCylinder::generate_disc_indices(GLubyte* mptr, GLbyte mStartingVertexIndex )
{
	// We're using GL_POLYGON. So no need for the center point.
	int vi;
	for (vi=0; vi<(m_number_of_samples); vi++)
	{
		mptr[vi] = vi + mStartingVertexIndex;
	}
	if (CLOSE_OUT)
		mptr[vi] = mStartingVertexIndex;
	return m_number_of_samples+CLOSE_OUT;
}

void glCylinder::generate_IBO()
{	
	m_disc_indices		= (m_number_of_samples+CLOSE_OUT);  // No *2 b/c GL_POLYGON
	m_number_side_indices = m_disc_indices * TOP_AND_BOTTOM;
	int total_indices   = m_disc_indices*2 + m_number_side_indices;

	GLubyte* iptr = new GLubyte[total_indices];

	// Create BOTTOM : 
	generate_disc_indices( iptr, 0 );

	// Create SIDE : 
	generate_side_IBO( iptr+m_disc_indices );
	total_indices = m_disc_indices+m_number_side_indices;

	if (m_is_closed)
	{
		// Create TOP:
		generate_disc_indices( (GLubyte*)(iptr+total_indices), m_number_of_samples  );
		total_indices += m_disc_indices;
	}

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (total_indices)*sizeof(GLubyte), 					
				 iptr, GL_STATIC_DRAW );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	delete iptr;
}

void glCylinder::generate_VBO()
{	
	generate_vertices();
	generate_vertices_colors();

	//Create a new VBO and use the variable id to store the VBO id
	glGenBuffers( 1, &m_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );	//Make the new VBO active  GL_COLOR_ARRAY

	// Generate & Upload vertex data to the video device 
	int size;
	size = m_number_of_vertices * sizeof(Vertex);
	glBufferData( GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW );
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// vertex positions	
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(struct Vertex,color)));	// color
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );

	//Draw Triangle from VBO - do each time window, view point or data changes
	//Establish its 3 coordinates per vertex with zero stride in this array; necessary here

	//glVertexAttribPointer(1, 3, GL_FLOAT, 		GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(struct Vertex, normal)));	// normals
	glEnableVertexAttribArray( 0 );	
	glEnableVertexAttribArray( 2 );	// GLKVertexAttribColor	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

void glCylinder::print_info()
{
	printf("glCylinder()  <x,y,z> = <%6.3f %6.3f %6.3f> \n", 
			m_x,m_y,m_z );
	printf(" radius=%6.3f;  height=%6.3f \n", 
			m_radius, m_height );			
}

void glCylinder::draw()
{
	glTranslatef(m_x, m_y, m_z);

	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	//Establish array contains vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY );

	// We draw the top and bottoms as GL_POLYGON and the sides as GL_QUAD_STRIP.
	// Both sets of indices stored in GL_INDEX_ARRAY.  With a known offset for each 
	// of the 3 sets.

	// Draw Bottom : 
	glDrawElements(GL_POLYGON, m_disc_indices, GL_UNSIGNED_BYTE, 0 );
	// Draw the sides:
	glDrawElements(GL_QUAD_STRIP, m_number_side_indices, GL_UNSIGNED_BYTE,
				   (GLvoid*)BUFFER_OFFSET( m_disc_indices ) );
	// Draw Top : 
	if (m_is_closed)
		glDrawElements(GL_POLYGON, m_disc_indices, GL_UNSIGNED_BYTE, 
					   (GLvoid*)BUFFER_OFFSET(m_disc_indices+m_number_side_indices) ); 

	glTranslatef(-m_x, -m_y, -m_z);		
}
	