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

//#include "all_objects.h"
#include "ibeam.hpp"


#define IBEAM_WIDTH   5.0
#define IBEAM_TUCK_IN 2.0
#define IBEAM_HEIGHT  7.5
#define IBEAM_THICKNESS  1.0

#define  TOP_AND_BOTTOM 2



glIbeam::glIbeam( float mExtrusionLength )
:glExtrusion()
{
    m_object_type_name = "i beam";
    m_object_class  = 6;
	m_layer_one_indices	= 0;    
    set_la( mExtrusionLength, 2);
}

void	glIbeam::generate_vertices( )
{
	struct Vertex v;
    // We set it to m_color (which presumably has already been set!):
    set_vertex_color(v);
	
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
	//printf("Ibeam vertices=%lu\n", m_vertices.size() );
}

size_t 	glIbeam::generate_disc_indices( GLuint mStartVertexIndex )
{
	m_indices.push_back(0 +mStartVertexIndex);	m_indices.push_back(1+mStartVertexIndex);
	m_indices.push_back(11+mStartVertexIndex);	m_indices.push_back(2+mStartVertexIndex);	
	m_indices.push_back(10+mStartVertexIndex);	m_indices.push_back(3+mStartVertexIndex);
	m_indices.push_back(9 +mStartVertexIndex);	m_indices.push_back(4+mStartVertexIndex);
	m_indices.push_back(8 +mStartVertexIndex);	m_indices.push_back(5+mStartVertexIndex);	
	m_indices.push_back(7 +mStartVertexIndex);	m_indices.push_back(6+mStartVertexIndex);
	m_layer_one_indices	= m_indices.size();
	return m_layer_one_indices;
}

void glIbeam::draw_body( )
{
	glExtrusion::draw_body();
}



