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
#include "robot_leg_piece.hpp"


#define  TOP_AND_BOTTOM 2
#define  BUFFER_OFFSET(i) ((char *)NULL + (i))


/* 
Outer and Inner could be the same 2D shape!  
just turn them around from each other and make the inner thicker.
*/
glInner::glInner()
{
	m_extrusion_length  = 3.;		// inches
	m_layer_one_indices	= 12;
}

const float shank_radius = 3./16.;
const float round_radius = 3./16.;
const float LEG_THICKNESS= 2.5;

void glInner::generate_shank_vertices( )		// 2D
{
	const int SAMPLES = 36.;
	float angle       = 0.0;
	float increment   = 2.*M_PI/SAMPLES;

	for (int i=0; i<SAMPLES; i++)	
	{		
		m_vertices[i].position[0] = shank_radius*sin(angle);
		m_vertices[i].position[1] = 0.0;
		m_vertices[i].position[2] = shank_radius*cos(angle);
		angle += increment;
	}
	m_number_of_vertices += SAMPLES;
}
void glInner::generate_round_vertices( )		// 2D
{
	const int SAMPLES = 18;
	// not sure which quadrant we'll need.  Let's try going from [-PI/2 to +PI/2]
	float angle       = -2.*M_PI/4.;			
	float increment   = M_PI/SAMPLES;	// half circle only!

	for (int i=0; i<SAMPLES; i++)	
	{		
		m_vertices[i].position[0] = round_radius*sin(angle);
		m_vertices[i].position[1] = 0.0;
		m_vertices[i].position[2] = round_radius*cos(angle);
		angle += increment;
	}
	m_number_of_vertices += SAMPLES;	
}


void	glInner::generate_layer_vertices( )
{
	m_layer_one_vertices = 13;
	m_vertices = (Vertex*)malloc( sizeof(Vertex)*(m_layer_one_vertices*2) );

	// Go along bottom first:
	m_vertices[0].position[0] =  0.0;
	m_vertices[0].position[1] =  0.0;
	m_vertices[0].position[2] =  0.0;

	m_vertices[1].position[0] =  m_leg_length;
	m_vertices[1].position[1] =  0.;
	m_vertices[1].position[2] =  0.0;

	m_vertices[2].position[0] =  m_leg_length;
	m_vertices[2].position[1] =  0.;
	m_vertices[2].position[2] =  LEG_THICKNESS;

	m_vertices[3].position[0] =  0.0;
	m_vertices[3].position[1] =  0.0;
	m_vertices[3].position[2] =  LEG_THICKNESS;

}

GLbyte 	glInner::generate_disc_indices( GLbyte mStartingVertexIndex )
{
	return 0;
}


glLegSegment::glLegSegment(  )
{

}
void glLegSegment::Initialize	( )
{

}
const float OUTER_THICKNESS = 0.5;
const float INNER_THICKNESS = 2.0;

void glLegSegment::Relocate	( float mX, float mY, float mZ )
{
	m_outside.Relocate(mX, mY, mZ);
	m_center.Relocate  (mX, mY-OUTER_THICKNESS, mZ);		// which axis is extruded? Y
	m_inside.Relocate (mX, mY-OUTER_THICKNESS-INNER_THICKNESS, mZ);
}

void glLegSegment::generate_vertices()
{
	m_outside.generate_vertices();
	m_center.generate_vertices  ();
	m_inside.generate_vertices ();

	m_outside.extrude_vertices(  OUTER_THICKNESS, 1 );		// Y_AXIS
	m_center.extrude_vertices  (  INNER_THICKNESS, 1 );		// Y_AXIS
	m_inside.extrude_vertices (  OUTER_THICKNESS, 1 );		// Y_AXIS
}

void glLegSegment::generate_IBO( )
{
	m_outside.generate_IBO();
	m_center.generate_IBO();
	m_inside.generate_IBO();
}
void glLegSegment::generate_VBO( )
{
	m_outside.generate_IBO();
	m_center.generate_IBO();
	m_inside.generate_IBO();
}

void glLegSegment::draw()
{
	m_outside.draw();
	m_center.draw();
	m_inside.draw();
}
