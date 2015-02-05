#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "gl_container.hpp"
#include "cylinder.hpp"
#include "extrusion.hpp"


#define  CLOSE_OUT 		1
#define  TOP_AND_BOTTOM 2
#define  BUFFER_OFFSET(i) ((char *)NULL + (i))

#define Debug 0


glExtrusion::glExtrusion( )
{
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;
	m_color 	 = 0xFFFFFFFF;
	m_ends_color = 0xFF0020FF;
	m_is_closed  = true;
	m_VBO 	= 0;
	m_IBO 	= 0;
	m_layer_one_vertices = 0;
	m_layer_one_indices  = 0;
	m_number_side_indices= 0;
	m_extrusion_length = 10.0;
}

void glExtrusion::Relocate( float mX, float mY, float mZ )
{
	m_x = mX;
	m_y = mY;
	m_z = mZ;
}

/* Override this function to generate the polygon. */
void glExtrusion::generate_layer_vertices()
{
	struct Vertex v;
	v.position[0] =  0.0;
	v.position[1] =  0.0;
	v.position[2] =  0.0;
	m_vertices.push_back( v );

	v.position[0] =  1.0;
	v.position[1] =  0.0;
	v.position[2] =  1.0;
	m_vertices.push_back( v );
	
	v.position[0] =  1.0;
	v.position[1] =  0.0;
	v.position[2] =  0.0;
	m_vertices.push_back( v );

	m_layer_one_vertices = 3;
}

// creates a copy of the polygon, lofted by distance.
void glExtrusion::extrude_vertices( float mExtrusionLength, int mLoftAxis )
{	
	if (mLoftAxis>2) return;
	if (mLoftAxis<0) return;

	struct Vertex v;
	
	// CEILING (DUPLICATE & SET HEIGHT)
	// Duplicate all Floor points with ceiling height.
	int d=0;
	// was ; i<m_layer_one_vertices;
	m_layer_one_vertices = m_vertices.size();
	for (int i=0; i<m_layer_one_vertices; i++)
	{
		memcpy ( &v, &(m_vertices[i]), sizeof (struct Vertex) );
//		v.position[0] = m_vertices[i].position[0];
//		v.position[1] = m_vertices[i].position[1];
//		v.position[2] = m_vertices[i].position[2];		
		v.position[mLoftAxis] = mExtrusionLength;
		m_vertices.push_back( v );
	}
}

/* NOTE: This must be called before the VBO is generated!!
	Has no effect after generate_VBO()
*/
void glExtrusion::change_color( long mColor )
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

void glExtrusion::generate_vertices_colors()
{
	for (int i=0; i<m_vertices.size(); i++)
	{
		if (i<m_layer_one_vertices)	// Bottom Blue
		{
			m_vertices[i].color[0] = (m_ends_color & 0x00FF0000) >> 16;
			m_vertices[i].color[1] = (m_ends_color & 0x0000FF00) >>  8;
			m_vertices[i].color[2] = (m_ends_color & 0x000000FF);
			m_vertices[i].color[3] = (m_ends_color & 0xFF000000) >> 24;
		} else {	// Top Red 
			m_vertices[i].color[0] = (m_color & 0x00FF0000) >> 16;
			m_vertices[i].color[1] = (m_color & 0x0000FF00) >>  8;
			m_vertices[i].color[2] = (m_color & 0x000000FF);
			m_vertices[i].color[3] = (m_color & 0xFF000000) >> 24;
		}	// Sides Green 
	}
}

void glExtrusion::generate_vertices()
{
	generate_layer_vertices();
	extrude_vertices( m_extrusion_length, 2 );	
	generate_vertices_colors();	
}

void glExtrusion::generate_VBO()
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

//======================= INDICES =======================================

/* This must be done before the side indices can be created!!  */
void glExtrusion::generate_otherside_indices()
{
	GLubyte	matching_index;
	m_layer_one_indices = m_indices.size();
	//printf("generate_otherside_indices()  m_layer_one_indices=%d\n", m_layer_one_indices );
	
	for (int vi=0; vi<m_layer_one_indices; vi++)
	{
		matching_index = m_indices[vi] + m_layer_one_vertices;
		m_indices.push_back( matching_index );
	}
}	
GLbyte glExtrusion::generate_disc_indices( GLbyte mStartingVertexIndex )
{
	// FOR A CONVEX POLYGON : 
	int vi;
	for (vi=0; vi<(m_layer_one_indices-CLOSE_OUT); vi++)
	{
		m_indices.push_back( vi + mStartingVertexIndex );
	}
	if (CLOSE_OUT)
	{
		m_indices.push_back( mStartingVertexIndex );
	}
	return m_indices.size();
}

void glExtrusion::generate_side_indices(  )
{
	m_number_side_indices = (m_layer_one_vertices+CLOSE_OUT) * TOP_AND_BOTTOM;
	int iptr_index = 0;
	for (int i=0; i<m_layer_one_vertices; i++)
	{
		// These are the indices into vertices[] array : 
		m_indices.push_back( i );		
		m_indices.push_back( i+m_layer_one_vertices );		
	}
	if (CLOSE_OUT) {
		m_indices.push_back( 0 );
		m_indices.push_back( m_layer_one_vertices );
	}
}

void glExtrusion::generate_indices( )
{
	m_indices.clear();
	generate_disc_indices( 0 );
	generate_side_indices(   );
	if (m_is_closed)
		generate_disc_indices( m_layer_one_vertices );
}

void glExtrusion::generate_IBO()
{
	int     size = m_indices.size();
	GLubyte* tmp = m_indices.data();

	glGenBuffers( 1, &m_IBO );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (size)*sizeof(GLubyte), 
				 tmp, GL_STATIC_DRAW );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
}
//======================= END OF INDICES ================================

void glExtrusion::create(float mLength)
{
	if (mLength>0) 
		m_extrusion_length = mLength;

	generate_vertices();	
	generate_indices();	

	generate_VBO();
	generate_IBO();

	//print_indices();
	//print_vertices();
}

void glExtrusion::print_indices( )
{
	printf("print_indices()\n");
	for (int i=0; i<m_indices.size(); i++)
	{
		if ((i % m_layer_one_indices)==0)
			printf("-------------\n");
		printf("%d : %d\n", i, m_indices[i] );
	}
}

void glExtrusion::print_vertices( bool mShowColors )
{
	printf("print_vertices()  Number of Vertices=%d\n", m_vertices.size() );
	for (int i=0; i<m_vertices.size(); i++)
	{
		//if ((i%m_layer_one_vertices)==0)
		//	printf("-------------\n");
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

void glExtrusion::draw()
{
	glTranslatef(m_x, m_y, m_z);

	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
	
	//Establish array contains m_vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY );

	// Draw Bottom :  
	glDrawElements(GL_QUAD_STRIP, m_layer_one_indices, GL_UNSIGNED_BYTE, 0 );

	// Draw the sides:
	glDrawElements(GL_QUAD_STRIP, m_number_side_indices, GL_UNSIGNED_BYTE,
				   (GLvoid*)BUFFER_OFFSET( m_layer_one_indices ) );

	// Draw Top: 
	if (m_is_closed)
	{
		glDrawElements(GL_QUAD_STRIP, m_layer_one_indices, GL_UNSIGNED_BYTE, 
	                  (GLvoid*)BUFFER_OFFSET(m_layer_one_indices + m_number_side_indices) );
	}
	glTranslatef(-m_x, -m_y, -m_z);		
}
	