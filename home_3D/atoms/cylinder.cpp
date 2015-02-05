#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"



#define  CLOSE_OUT 		1
#define  TOP_AND_BOTTOM 2
#define  BUFFER_OFFSET(i) ((GLuint*)NULL + (i))

glCylinder::glCylinder( int mNumberSamples )
:glExtrusion()
{
	m_color 	 = 0xFFFFFFFF;
	m_ends_color = 0xFF00FFFF;
	m_is_closed  = true;
	m_number_of_samples  = mNumberSamples;
	m_extrusion_axis = 1;
}

void glCylinder::generate_layer_vertices( )
{	
	struct Vertex v;
	float TwoPi     = 2.*M_PI;
	float increment = (TwoPi / ((float)m_number_of_samples));
	int   i;
	float a=0.;
	for (i=0; i<m_number_of_samples; a+=increment, i++)
	{
		v.position[0] =  m_radius * sin(a);
		v.position[1] =  0.;
		v.position[2] =  m_radius * cos(a);
		m_vertices.push_back( v );
	}
	m_layer_one_vertices = m_vertices.size();
	printf("glCylinder::generate_layer_vertices()  %d\n", m_layer_one_vertices );
}

GLuint glCylinder::generate_disc_indices( GLuint mStartingVertexIndex )
{
	// We're using GL_POLYGON. So no need for the center point.
	int vi;
	for (vi=0; vi<m_layer_one_vertices; vi++)
	{
		m_indices.push_back( vi+mStartingVertexIndex );
	}
	//if (CLOSE_OUT)		Should not be needed because GL_POLYGON automatically closes out!
	//	m_indices.push_back(mStartingVertexIndex );
	m_layer_one_indices = m_indices.size();
	return m_indices.size();
}


void glCylinder::draw_body()
{
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
	glDrawElements(GL_POLYGON, m_layer_one_indices, GL_UNSIGNED_INT, 0 );
 
	// Draw the sides: m_number_side_indices
	glDrawElements(GL_QUAD_STRIP, m_layer_one_vertices*2., GL_UNSIGNED_INT,
				   (GLvoid*)BUFFER_OFFSET( m_layer_one_indices ) );
	// Draw Top : 
	if (m_is_closed)
		glDrawElements(GL_POLYGON, m_layer_one_indices, GL_UNSIGNED_INT, 
					   (GLvoid*)BUFFER_OFFSET(m_layer_one_indices+m_number_side_indices) );
}
	
