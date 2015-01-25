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
#include "ibeam.hpp"


#define IBEAM_WIDTH   5.0
#define IBEAM_TUCK_IN 2.0
#define IBEAM_HEIGHT  7.5
#define IBEAM_THICKNESS  1.0

#define  TOP_AND_BOTTOM 2
#define  BUFFER_OFFSET(i) ((char *)NULL + (i))


glIbeam::glIbeam( float mExtrusionLength )
:glExtrusion()
{
	m_extrusion_length = mExtrusionLength;
	m_layer_one_indices	= 12;
}

void	glIbeam::generate_layer_vertices( )
{
	m_layer_one_vertices = 13;
	m_vertices = (Vertex*)malloc( sizeof(Vertex)*(m_layer_one_vertices*2) );
	
	// Go along bottom first:
	m_vertices[0].position[0] =  0.0;
	m_vertices[0].position[1] =  0.0;
	m_vertices[0].position[2] =  0.0;

	m_vertices[1].position[0] =  IBEAM_WIDTH;
	m_vertices[1].position[1] =  0.;
	m_vertices[1].position[2] =  0.0;

	m_vertices[2].position[0] =  IBEAM_WIDTH;
	m_vertices[2].position[1] =  IBEAM_THICKNESS;
	m_vertices[2].position[2] =  0.;

	m_vertices[3].position[0] =  IBEAM_WIDTH-IBEAM_TUCK_IN;
	m_vertices[3].position[1] =  IBEAM_THICKNESS;
	m_vertices[3].position[2] =  0.;

	m_vertices[4].position[0] =  IBEAM_WIDTH-IBEAM_TUCK_IN;
	m_vertices[4].position[1] =  IBEAM_HEIGHT;
	m_vertices[4].position[2] =  0.;

	m_vertices[5].position[0] =  IBEAM_WIDTH;
	m_vertices[5].position[1] =  IBEAM_HEIGHT;
	m_vertices[5].position[2] =  0.;

	m_vertices[6].position[0] =  IBEAM_WIDTH;
	m_vertices[6].position[1] =  IBEAM_HEIGHT+IBEAM_THICKNESS;
	m_vertices[6].position[2] =  0.;

	m_vertices[7].position[0] =  0.0;
	m_vertices[7].position[1] =  IBEAM_HEIGHT+IBEAM_THICKNESS;
	m_vertices[7].position[2] =  0.;

	m_vertices[8].position[0] =  0.0;
	m_vertices[8].position[1] =  IBEAM_HEIGHT;
	m_vertices[8].position[2] =  0.;

	m_vertices[9].position[0] =  IBEAM_TUCK_IN;
	m_vertices[9].position[1] =  IBEAM_HEIGHT;
	m_vertices[9].position[2] =  0.;

	m_vertices[10].position[0] =  IBEAM_TUCK_IN;
	m_vertices[10].position[1] =  IBEAM_THICKNESS;
	m_vertices[10].position[2] =  0.;

	m_vertices[11].position[0] =  0.0;
	m_vertices[11].position[1] =  IBEAM_THICKNESS;
	m_vertices[11].position[2] =  0.;

	m_vertices[12].position[0] =  0.0;
	m_vertices[12].position[1] =  0.0;
	m_vertices[12].position[2] =  0.0;
}

GLbyte 	glIbeam::generate_disc_indices( GLbyte mStartVertexIndex )
{
	m_layer_one_indices	= 12; 	
	m_indices.push_back(0 +mStartVertexIndex);	m_indices.push_back(1+mStartVertexIndex);
	m_indices.push_back(11+mStartVertexIndex);	m_indices.push_back(2+mStartVertexIndex);	
	m_indices.push_back(10+mStartVertexIndex);	m_indices.push_back(3+mStartVertexIndex);
	m_indices.push_back(9 +mStartVertexIndex);	m_indices.push_back(4+mStartVertexIndex);
	m_indices.push_back(8 +mStartVertexIndex);	m_indices.push_back(5+mStartVertexIndex);	
	m_indices.push_back(7 +mStartVertexIndex);	m_indices.push_back(6+mStartVertexIndex);

	return m_layer_one_indices;	
}






