#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "gl_container.hpp"
#include "room.hpp"


#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define TOP_AND_BOTTOM 2
#define CLOSE_OUT 1

glRoom::glRoom(  )
{
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;
	m_color = 0xFFFFFFFF;
	m_is_closed = false;
}

// Sample Apartment room.  Each vertex is a dimension in inches.
void glRoom::generate_floor_vertices()
{
	m_number_of_floor_vertices = 11;
	m_total_vertices = m_number_of_floor_vertices * 2.;
	m_vertices = (Vertex*) malloc( sizeof(Vertex)*(m_number_of_floor_vertices*2.) );

	// Living room:  0, 1, 4, 5, 2, 3, 6, 10, 9, 8, 7, 0 
	// front side
	m_vertices[0].position[0] = 0.;
	m_vertices[0].position[2] = 0.;

	m_vertices[1].position[0] = 0.;			
	m_vertices[1].position[2] = 144.0;		

	m_vertices[2].position[0] = 0.;			
	m_vertices[2].position[2] = 144.+116.;	

	m_vertices[3].position[0] = 0.;			
	m_vertices[3].position[2] = 144.+116.+120.;

	// hallway
	m_vertices[4].position[0] = 160.0;		
	m_vertices[4].position[2] = 144.0;		

	m_vertices[5].position[0] = 160.0;		
	m_vertices[5].position[2] = 144.0+116.;	

	m_vertices[6].position[0] = 160.0;			
	m_vertices[6].position[2] = 144.0+116.+120.;	

	// 
	m_vertices[7].position[0] = 160.0+36.+95.;
	m_vertices[7].position[2] = 0.0;		

	m_vertices[8].position[0] = 160.0+36.+95.;	
	m_vertices[8].position[2] = 144.0;		

	m_vertices[9].position[0] = 160.0+36.;	
	m_vertices[9].position[2] = 144.0;		

	m_vertices[10].position[0] = 160.0+36.;		
	m_vertices[10].position[2] = 144.0+116.+120.;	

	// Set the up/down for all floor samples:
	for (int i=0; i<m_number_of_floor_vertices; i++)
		m_vertices[i].position[1] = 0;
}

void glRoom::generate_ceiling_vertices()
{
	const float ceiling_height = 120.;		// inches
	// CEILING (DUPLICATE & SET HEIGHT)
	// Duplicate all Floor points with ceiling height.
	for (int i=0; i<m_number_of_floor_vertices; i++)
	{
		m_vertices[i+m_number_of_floor_vertices].position[0] = m_vertices[i].position[0];
		m_vertices[i+m_number_of_floor_vertices].position[1] = ceiling_height;
		m_vertices[i+m_number_of_floor_vertices].position[2] = m_vertices[i].position[2];
	}
}

void glRoom::generate_vertices_colors()
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

void glRoom::scale_vertices( float mScale )
{
	for (int v=0; v<m_total_vertices; v++)
	{
		m_vertices[v].position[0] *= mScale;
		m_vertices[v].position[1] *= mScale;
		m_vertices[v].position[2] *= mScale;
	}
}
GLubyte floor_plan[] = { 0, 1, 4, 5, 2, 3, 6, 10, 9, 8, 7, 0 };

GLbyte glRoom::generate_floor_IBO( GLubyte* isptr, GLbyte mStartingVertexIndex )
{	
	// We're using GL_POLYGON. So no need for the center point.
	
	m_floor_indices = 12;
	int i;	
	for (i=0; i<m_floor_indices; i++)
		isptr[i] = floor_plan[i];	// These are the indices into vertices[] array:
	return m_floor_indices;
}

GLbyte 	glRoom::generate_ceiling_IBO( GLubyte* mptr, GLbyte mStart_of_top_index )
{
	// Already added to mptr before call to this:  m_floor_indices + number_side_indices 
	//GLubyte floor_plan[] = { 0, 1, 4, 5, 2, 3, 6, 10, 9, 8, 7, 0 };
	m_floor_indices = 12;
	int i;	
	for (i=0; i<m_floor_indices; i++)
		mptr[i] = floor_plan[i]+m_number_of_floor_vertices;	// These are the indices into vertices[] array:
	return m_floor_indices;
}

void glRoom::generate_side_IBO( GLubyte* isptr )
{
	int first_ceiling_index = m_number_of_floor_vertices;
	int iptr_index = 0;
	for (int i=0; i<m_floor_indices; i++)
	{
		// These are the indices into vertices[] array : 
		isptr[iptr_index++] = floor_plan[i];						// Bottom sample
		isptr[iptr_index++] = floor_plan[i]+first_ceiling_index;	// Top sample
	}
	if (CLOSE_OUT) {
		isptr[iptr_index++] = floor_plan[0];						// Close out
		isptr[iptr_index++] = floor_plan[0]; //first_ceiling_index;		//
	}
}

void glRoom::generate_IBO()
{
	m_floor_indices		= (m_number_of_floor_vertices+CLOSE_OUT);  // No *2 b/c GL_POLYGON 
	m_side_indices 		= m_floor_indices * TOP_AND_BOTTOM;
	int total_indices   = m_floor_indices * 2 + m_side_indices;

	GLubyte* iptr = new GLubyte[total_indices];

	// Create BOTTOM:
	generate_floor_IBO( iptr, 0 );
	
	// Create SIDE:
	generate_side_IBO( iptr+m_floor_indices );
	total_indices = m_floor_indices+m_side_indices;

	if (m_is_closed)
	{	
		// Create TOP:
		generate_ceiling_IBO( (GLubyte*)(iptr+m_floor_indices+m_side_indices), m_floor_indices+1  );
		total_indices += m_floor_indices;
	}

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (total_indices)*sizeof(GLubyte), 					
				 iptr, GL_STATIC_DRAW );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	delete iptr;
}

void glRoom::generate_VBO()
{	
	generate_floor_vertices  ( );
	generate_ceiling_vertices( );
	generate_vertices_colors ( );
	//scale_vertices( 1./10. );
	
	//Create a new VBO and use the variable id to store the VBO id
	glGenBuffers( 1, &m_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );	//Make the new VBO active  GL_COLOR_ARRAY

	// Generate & Upload vertex data to the video device 
	int size;
	size = m_total_vertices * sizeof(Vertex);
	glBufferData( GL_ARRAY_BUFFER, size, m_vertices, GL_STATIC_DRAW );
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// vertex positions	
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(struct Vertex,color)));	// color
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );

	//Draw Triangle from VBO - do each time window, view point or data changes
	//Establish its 3 coordinates per vertex with zero stride in this array; necessary here

	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(struct Vertex, normal)));	// normals
	glEnableVertexAttribArray( 0 );	
	glEnableVertexAttribArray( 2 );	// GLKVertexAttribColor	

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}


void glRoom::draw()
{
	glTranslatef(m_x, m_y, m_z);

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

	// Draw Floor : 
	glDrawElements(GL_LINE_LOOP, m_floor_indices, GL_UNSIGNED_BYTE, 0 );

	// Draw the sides:
	int side_indices = m_floor_indices*2;
	glDrawElements(GL_QUAD_STRIP, side_indices, GL_UNSIGNED_BYTE,
				   (GLvoid*)BUFFER_OFFSET( m_floor_indices ) );

	// Draw Ceiling:
	if (m_is_closed)
		glDrawElements(GL_POLYGON, m_floor_indices, GL_UNSIGNED_BYTE, 
						(GLvoid*)BUFFER_OFFSET(m_floor_indices+m_side_indices) );

	glTranslatef(-m_x, -m_y, -m_z);		
}
	