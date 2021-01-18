#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "extrusion.hpp"
//#include "all_objects.h"



#define  CLOSE_OUT 		1
#define  TOP_AND_BOTTOM 2
#define  BUFFER_OFFSET(i) ((GLuint*)NULL + (i))

#define Debug 0


glExtrusion::glExtrusion( )
{
    m_object_type_name = "extrusion";    
    m_object_class  = 3;
	m_color 	 	= 0xFFFFFFFF;
	m_ends_color 	= 0xFF0020FF;
	m_is_closed  	= true;
	m_layer_one_vertices = 0;
	m_layer_one_indices  = 0;
	m_number_side_indices= 0;
	m_extrusion_length = 10.0;
	m_extrusion_axis   = -1;
}

/* Override this function to generate the polygon. */
void glExtrusion::generate_vertices()
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
	if ((mLoftAxis>=0) && (mLoftAxis<=2))
		m_extrusion_axis = mLoftAxis;
	if (m_extrusion_axis==-1)
    {
        printf("BAD EXTRUSION: glExtrusion::extrude_vertices() - no axis specified!");
        return;
    }
	struct Vertex_pnc v;

	// CEILING (DUPLICATE & SET HEIGHT)
	// Duplicate all Floor points with ceiling height.	
	//m_layer_one_vertices = m_vertices.size();
	for (int i=0; i<m_layer_one_vertices; i++)
	{
		memcpy ( &v, &(m_vertices[i]), sizeof (struct Vertex_pnc) );
		v.position[m_extrusion_axis] = mExtrusionLength;
		m_vertices.push_back( v );
	}
}

void glExtrusion::change_vertices_colors()
{
	for (int i=0; i<m_vertices.size(); i++)
	{
		if (i<m_layer_one_vertices)		// Bottom Blue
		{
			m_vertices[i].color[0] = (m_ends_color & 0x00FF0000) >> 16;
			m_vertices[i].color[1] = (m_ends_color & 0x0000FF00) >>  8;
			m_vertices[i].color[2] = (m_ends_color & 0x000000FF) ;
			m_vertices[i].color[3] = (m_ends_color & 0xFF000000) >> 24;
		} else {	// Top Red 
			m_vertices[i].color[0] = (m_color & 0x00FF0000) >> 16;
			m_vertices[i].color[1] = (m_color & 0x0000FF00) >>  8;
			m_vertices[i].color[2] = (m_color & 0x000000FF) ;
			m_vertices[i].color[3] = (m_color & 0xFF000000) >> 24;
		}			// Sides Green 
	}
}


//======================= INDICES =======================================
size_t glExtrusion::generate_disc_indices( GLuint mStartingVertexIndex )
{
    //assert(m_layer_one_indices>0);
    
	// FOR A CONVEX POLYGON: 
	int vi;
	for (vi=0; vi<(m_layer_one_vertices-CLOSE_OUT); vi++)
	{
		m_indices.push_back( vi+mStartingVertexIndex );
	}
	if (CLOSE_OUT)
	{
		m_indices.push_back( mStartingVertexIndex );
	}
    m_layer_one_indices = m_indices.size();
	return m_layer_one_indices;
}

void glExtrusion::generate_side_indices(  )
{
	m_number_side_indices = (m_layer_one_vertices+CLOSE_OUT) * TOP_AND_BOTTOM;	
	for (int i=0; i<m_layer_one_vertices; i++)
	{
		// These are the index into vertices[] array : 
		m_indices.push_back( i );
		m_indices.push_back( (int)(i+m_layer_one_vertices) );
	}
	if (CLOSE_OUT) {
		m_indices.push_back( 0 );
		m_indices.push_back( (int)m_layer_one_vertices );
	}
	//printf("# side indices=%d \n", m_number_side_indices );
}

/* This must be done before the side indices can be created!!  */
void glExtrusion::generate_otherside_indices()
{
	//printf("extrusion::create() m_is_closed = true.  generating otherside.\n");
	//printf("starting at %d;  layer_one_indices= %d\n",m_indices.size(), m_layer_one_indices );
	GLubyte	matching_index;
	for (int vi=0; vi<m_layer_one_indices; vi++)
	{
		matching_index = m_indices[vi] + m_layer_one_vertices;
		m_indices.push_back( matching_index );
	}
}	

void glExtrusion::generate_indices( )
{
	m_indices.clear();
	generate_disc_indices( 0 );
	generate_side_indices(   ); 
	if (m_is_closed)	
		generate_otherside_indices( );
	//printf("extrusion indices = %d\n", m_indices.size() );
}

//======================= END OF INDICES ================================
void glExtrusion::set_la(float mLength, int mAxis)
{
    if (mLength>0)
        m_extrusion_length = mLength;
    if (mAxis>0)
        m_extrusion_axis   = mAxis;
}
void glExtrusion::setup( )
{
    generate_vertices( );
    extrude_vertices ( m_extrusion_length, m_extrusion_axis );
    //change_vertices_colors( );
    generate_indices ( );
    glObject::setup();
}

void glExtrusion::draw_body()
{
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer( GL_ARRAY_BUFFER, 		   m_VBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	//Establish array contains m_vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY );

	// Draw Bottom : 
	glDrawElements(GL_QUAD_STRIP, (int)m_layer_one_indices, GL_UNSIGNED_INT, 0 );

	// Draw the sides:
	glDrawElements(GL_QUAD_STRIP, (int)m_number_side_indices, GL_UNSIGNED_INT,
				   (GLvoid*)BUFFER_OFFSET( m_layer_one_indices ) );

	// Draw Top: 
	if (m_is_closed)
	{  
		glDrawElements(GL_QUAD_STRIP, (int)m_layer_one_indices, GL_UNSIGNED_INT,
	                  (GLvoid*)BUFFER_OFFSET(m_layer_one_indices + m_number_side_indices) );
	}
}



