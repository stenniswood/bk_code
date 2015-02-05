#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"


#define IBEAM_WIDTH   5.0
#define IBEAM_TUCK_IN 2.0
#define IBEAM_HEIGHT  7.5
#define IBEAM_THICKNESS  1.0

#define  TOP_AND_BOTTOM 2



glIbeam::glIbeam( float mExtrusionLength )
:glExtrusion()
{
	m_extrusion_length = mExtrusionLength;
	m_layer_one_indices	= 0;
}

void	glIbeam::generate_layer_vertices( )
{
	struct Vertex v;
	
	// Go along bottom first:
	v.position[0] =  0.0;
	v.position[1] =  0.0;
	v.position[2] =  0.0;
	m_vertices.push_back( v );
	
	v.position[0] =  IBEAM_WIDTH;
	v.position[1] =  0.;
	v.position[2] =  0.0;
	m_vertices.push_back( v );
	
	v.position[0] =  IBEAM_WIDTH;
	v.position[1] =  IBEAM_THICKNESS;
	v.position[2] =  0.;
	m_vertices.push_back( v );
	
	v.position[0] =  IBEAM_WIDTH-IBEAM_TUCK_IN;
	v.position[1] =  IBEAM_THICKNESS;
	v.position[2] =  0.;
	m_vertices.push_back( v );
	
	v.position[0] =  IBEAM_WIDTH-IBEAM_TUCK_IN;
	v.position[1] =  IBEAM_HEIGHT;
	v.position[2] =  0.;
	m_vertices.push_back( v );
	
	v.position[0] =  IBEAM_WIDTH;
	v.position[1] =  IBEAM_HEIGHT;
	v.position[2] =  0.;
	m_vertices.push_back( v );

	v.position[0] =  IBEAM_WIDTH;
	v.position[1] =  IBEAM_HEIGHT+IBEAM_THICKNESS;
	v.position[2] =  0.;
	m_vertices.push_back( v );

	v.position[0] =  0.0;
	v.position[1] =  IBEAM_HEIGHT+IBEAM_THICKNESS;
	v.position[2] =  0.;
	m_vertices.push_back( v );

	v.position[0] =  0.0;
	v.position[1] =  IBEAM_HEIGHT;
	v.position[2] =  0.;
	m_vertices.push_back( v );

	v.position[0] =  IBEAM_TUCK_IN;
	v.position[1] =  IBEAM_HEIGHT;
	v.position[2] =  0.;
	m_vertices.push_back( v );

	v.position[0] =  IBEAM_TUCK_IN;
	v.position[1] =  IBEAM_THICKNESS;
	v.position[2] =  0.;
	m_vertices.push_back( v );
	
	v.position[0] =  0.0;
	v.position[1] =  IBEAM_THICKNESS;
	v.position[2] =  0.;
	m_vertices.push_back( v );

	v.position[0] =  0.0;
	v.position[1] =  0.0;
	v.position[2] =  0.0;
	m_vertices.push_back( v );
	
	m_layer_one_vertices = m_vertices.size();	
	printf("Ibeam vertices=%lu\n", m_vertices.size() );
}

GLuint 	glIbeam::generate_disc_indices( GLuint mStartVertexIndex )
{
	m_indices.push_back(0 +mStartVertexIndex);	m_indices.push_back(1+mStartVertexIndex);
	m_indices.push_back(11+mStartVertexIndex);	m_indices.push_back(2+mStartVertexIndex);	
	m_indices.push_back(10+mStartVertexIndex);	m_indices.push_back(3+mStartVertexIndex);
	m_indices.push_back(9 +mStartVertexIndex);	m_indices.push_back(4+mStartVertexIndex);
	m_indices.push_back(8 +mStartVertexIndex);	m_indices.push_back(5+mStartVertexIndex);	
	m_indices.push_back(7 +mStartVertexIndex);	m_indices.push_back(6+mStartVertexIndex);
	m_layer_one_indices	= m_indices.size();
	printf("Ibeam indices=%lu\n", m_indices.size() );
	return m_layer_one_indices;
}

void glIbeam::draw_body( )
{
//	printf("iBEAM draw_body()\n");
	glExtrusion::draw_body();
}



