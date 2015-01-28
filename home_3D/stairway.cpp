#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "gl_container.hpp"
#include "room.hpp"
#include "stairway.hpp"


#define Debug 0

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define TOP_AND_BOTTOM 2
#define CLOSE_OUT 1

glStairway::glStairway(  )
{
	m_x = 0.0;
	m_y = 0.0;
	m_z = 50.0;
	m_rise	=  8.;		// inches standard height 
	m_run	= 10.;		// rise + run == 18.
	m_width =  3.*12;		// 

	m_number_of_steps = 15;
	m_color = 0xFFFFFFFF;
	m_is_closed = true;		// always for a stairway!
}

#define VERTICES_PER_STEP 2

// Sample stairway.  Each vertex is a dimension in inches.
/*
____
	|___
		|___
*/
void glStairway::generate_side_vertices(int mNumberOfSteps)
{
	// 2 vertices per step : 
	m_number_of_floor_vertices = mNumberOfSteps * VERTICES_PER_STEP + 1;
	m_total_vertices = m_number_of_floor_vertices * TOP_AND_BOTTOM;
	m_vertices = (Vertex*) malloc( sizeof(Vertex)*(m_total_vertices) );

	// Zero will be the bottom of the lowest step.
	int vi=0;
	for (int s=0; s<mNumberOfSteps; s++)
	{
		m_vertices[vi].position[0] = -m_run*s ;
		m_vertices[vi].position[1] =  m_rise*s;
		if (Debug) printf("stair:vi=%d;  x,y,z = %6.3f, %6.3f, %6.3f \n", vi,
			m_vertices[vi].position[0],
			m_vertices[vi].position[1],
			m_vertices[vi].position[2] );

		vi++;
		m_vertices[vi].position[0] = -m_run*(s+1);
		m_vertices[vi].position[1] =  m_rise*(s) ;
		if (Debug) printf("stair:vi=%d;  x,y,z = %6.3f, %6.3f, %6.3f \n", vi,
			m_vertices[vi].position[0],
			m_vertices[vi].position[1],
			m_vertices[vi].position[2] );

		vi++;
	}
	// One more at base of stairs:
	m_vertices[vi].position[0] = -m_run*(mNumberOfSteps);
	m_vertices[vi].position[1] =  0.;

	// Set the z for left stairway side:
	for (int i=0; i<m_number_of_floor_vertices; i++)
		m_vertices[i].position[2] = 0;
}

void glStairway::generate_side2_vertices()
{
	// CEILING (DUPLICATE & SET HEIGHT)
	// Duplicate all Floor points with ceiling height.
	for (int i=0; i<m_number_of_floor_vertices; i++)
	{
		m_vertices[i+m_number_of_floor_vertices].position[0] = m_vertices[i].position[0];
		m_vertices[i+m_number_of_floor_vertices].position[1] = m_vertices[i].position[1];
		m_vertices[i+m_number_of_floor_vertices].position[2] = m_width;
	}
}

void glStairway::generate_vertices_colors()
{
	for (int i=0; i<m_total_vertices; i++)
	{
		if (i<=m_number_of_floor_vertices)	// Bottom Blue
		{
			m_vertices[i].color[0] = 0x00;
			m_vertices[i].color[1] = 0x20;
			m_vertices[i].color[2] = 0xFF;
			m_vertices[i].color[3] = 0xFF;
		} else {				// Top Red
			m_vertices[i].color[0] = 0xFF;
			m_vertices[i].color[1] = 0x00;
			m_vertices[i].color[2] = 0x00;
			m_vertices[i].color[3] = 0xFF;
		}	// Sides Green
	}
}

GLbyte glStairway::generate_side_IBO( GLubyte* isptr, GLbyte mStartingVertexIndex )
{	
	// We're using GL_POLYGON. So no need for the center point.
	int i;
	for (i=0; i<m_number_of_floor_vertices; i++)
		isptr[i] = i + mStartingVertexIndex;	// These are the indices into vertices[] array:
	if (CLOSE_OUT) {
		isptr[i] = isptr[0];					// Close out
	}
	return m_number_of_floor_vertices+CLOSE_OUT;
}

GLbyte 	glStairway::generate_side2_IBO( GLubyte* mptr, GLbyte mStartingVertexIndex )
{
	int first_ceiling_index = m_number_of_floor_vertices;	
	generate_side_IBO(mptr, first_ceiling_index );
	return m_number_of_floor_vertices;
}

void glStairway::generate_stair_IBO( GLubyte* isptr )
{
	int first_ceiling_index = m_number_of_floor_vertices;
	int iptr_index = 0;
	for (int i=0; i<m_number_of_floor_vertices; i++)
	{
		// These are the indices into vertices[] array : 
		isptr[iptr_index++] = i;						// Bottom sample
		isptr[iptr_index++] = i+first_ceiling_index;	// Top sample
	}
	if (CLOSE_OUT) {
		isptr[iptr_index++] = 0;						// Close out
		isptr[iptr_index++] = first_ceiling_index;		//
	}
}

void glStairway::generate_IBO()
{	
	m_floor_indices			= (m_number_of_floor_vertices+CLOSE_OUT);  // No *2 b/c GL_POLYGON
	int number_side_indices = m_floor_indices * TOP_AND_BOTTOM;
	int total_indices       = m_floor_indices*2 + number_side_indices;

	GLubyte* iptr = new GLubyte[total_indices];

	// Create BOTTOM : 
	generate_side_IBO( iptr, 0 );

	// Create SIDE:
	generate_stair_IBO( iptr+m_floor_indices );
	total_indices = m_floor_indices+number_side_indices;

	if (m_is_closed)
	{	
		// Create TOP:
		generate_side2_IBO( (GLubyte*)(iptr+m_floor_indices+number_side_indices), m_floor_indices+1  );
		total_indices += m_floor_indices;
	}

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (total_indices)*sizeof(GLubyte), 					
				 iptr, GL_STATIC_DRAW );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
}

void glStairway::generate_VBO()
{		
	generate_side_vertices (m_number_of_steps);
	generate_side2_vertices(  );	
	generate_vertices_colors( );

	// Generate & Upload vertex data to the video device 
	int size;
	size = m_total_vertices * sizeof(Vertex);

	//Create a new VBO and use the variable id to store the VBO id
	glGenBuffers( 1, &m_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );	//Make the new VBO active  GL_COLOR_ARRAY
	glBufferData( GL_ARRAY_BUFFER, size, m_vertices, GL_STATIC_DRAW );
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// vertex positions	
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(struct Vertex,color)));	// color
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );

	glEnableVertexAttribArray( 0 );	
	glEnableVertexAttribArray( 2 );	// GLKVertexAttribColor	
	//glVertexAttribPointer  ( 1, 3, GL_FLOAT, 		GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(struct Vertex, normal)));	// normals	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

float glStairway::get_height()
{
	return (m_rise * m_number_of_steps);
}

void glStairway::draw()
{
	glPushMatrix();

	glTranslatef(m_x, m_y, m_z);
	glRotatef 	(m_angle, 0.0f, 1.0f, 0.0f );
	
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
	
	//Establish array contains vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// We draw the top and bottoms as GL_POLYGON and the sides as GL_QUAD_STRIP.
	// Both sets of indices stored in GL_INDEX_ARRAY.  With a known offset for each 
	// of the 3 sets.

	// Draw Floor:
	glDrawElements(GL_LINE_LOOP, m_floor_indices, GL_UNSIGNED_BYTE, 0 );

	// Draw the sides:
	int side_indices = m_floor_indices*2;
	glDrawElements(GL_QUAD_STRIP, side_indices, GL_UNSIGNED_BYTE,
						(GLvoid*)BUFFER_OFFSET( m_floor_indices ) );

	// Draw Ceiling:
	if (m_is_closed)
		glDrawElements(GL_LINE_LOOP, m_floor_indices, GL_UNSIGNED_BYTE, 
						(GLvoid*)BUFFER_OFFSET(m_floor_indices+side_indices) );

	glPopMatrix();
}
	