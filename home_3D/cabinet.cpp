#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "gl_container.hpp"
#include "cylinder.hpp"
#include "table.hpp"
#include "cabinet.hpp"

float radians( float Degrees)
{
	return Degrees * M_PI / 180.0;	
}

glCabinet::glCabinet(  )
:glExtrusion()
{	
	m_depth  = 12.*2.;	// Actual Measurements!
	m_width  = 18.   ;
	m_height = 12.*3.;

	Initialize();
}

void glCabinet::Relocate( float mX, float mY, float mZ )
{
	m_x = mX;
	m_y = mY;
	m_z = mZ;

	// The translation will be done in the draw() function.
	// so the objects don't need to be actually positioned.	
/*	m_body.m_x = m_x;
	m_drawer.m_x = m_x -m_table_top.width/2.;
	m_door.m_z = m_z -m_table_top.depth/2.; */
}

#define DRAWER_BELOW_TOP  1.
#define LEFT_MARGIN 	  +1.

void glCabinet::Initialize( )
{
	// (inches):
	m_body.width  = m_width;
	m_body.height = m_height;
	m_body.depth  = m_depth;
	m_body.generate_vertices();	
	m_body.m_x  = 0.;
	m_body.m_y	= m_height/2.;
	m_body.m_z	= 0.;

	m_drawer.width	= 16.;	//inches measured!
	m_drawer.height	=  6.;
	m_drawer.depth	= 0.9*m_depth;
	m_drawer.m_x	= LEFT_MARGIN;
	m_drawer.m_y  	= m_height - m_drawer.height/2. - DRAWER_BELOW_TOP;
	m_drawer.m_z	= -m_drawer.depth/2.;
	m_drawer.m_color = 0xFF00FF00;
	m_drawer.m_is_closed = false;	
	m_drawer.generate_vertices();
		
	m_door.width	= 16.;	// inches measured!
	m_door.height	= 22.;
	m_door.depth	= 1.0;
	m_door.m_x 		= -m_body.width/2.+LEFT_MARGIN;	//+m_door.width;
	m_door.m_y 		=  4.75;	// above ground.
	m_door.m_z 		=  -m_body.depth/2.-m_door.depth/2.;
	m_door.m_color 	= 0xFFFF0000;
	m_door.m_is_closed 	= true;
	m_door_angle 		= -30.0; // radians(30.0);
	m_door.generate_vertices();
	m_door.grab_bottom();
	m_door.grab_right ();
	// Need to add the orientation (degree open)
}

/* Both sides are identical.  So extrude just the 1 inch (board thickness)
	And draw twice (using glTranslatef() )
*/
void  glCabinet::generate_cabinet_side_vertices( )
{
	m_layer_one_vertices = 6;
	//m_vertices = (Vertex*)malloc( sizeof(Vertex)*(m_layer_one_vertices*2) );
	struct Vertex v;

	// Square with a notch for the feet!	
	// Start at the back bottom:
	v.position[0] =  0.0;
	v.position[1] =  0.;
	v.position[2] =  0.0;
	m_vertices.push_back( v );

	// Then up to the top (not the counter added later)
	v.position[0] =  0.0;
	v.position[1] =  m_height;
	v.position[2] =  0.0;
	m_vertices.push_back( v );
	
	// top front:
	v.position[0] =  0.0;
	v.position[1] =  m_height;
	v.position[2] =  m_depth;
	m_vertices.push_back( v );
	
	// front (top of toe):
	v.position[0] =  0.0;
	v.position[1] =  m_toe_height;
	v.position[2] =  m_depth;
	m_vertices.push_back( v );
	
	// front (back of toe):
	v.position[0] =  0.0;
	v.position[1] =  m_toe_height;
	v.position[2] =  m_depth-m_toe_inward;
	m_vertices.push_back( v );
	
	// front (back of toe):
	v.position[0] =  0.0;
	v.position[1] =  0.0;
	v.position[2] =  m_depth-m_toe_inward;
	m_vertices.push_back( v );
}

void glCabinet::generate_vertices()
{
	generate_cabinet_side_vertices();
}
void  glCabinet::generate_cabinet_side_IBO( GLubyte* iptr )
{
	iptr[0] = 0;
	iptr[1] = 1;
	iptr[2] = 2;
	iptr[3] = 3;
	iptr[4] = 4;
	iptr[5] = 5;
	iptr[6] = 0;
}

void 	glCabinet::generate_IBO( )
{
	m_body.generate_IBO();
	m_drawer.generate_IBO();
	m_door.generate_IBO();
}

void 	glCabinet::generate_VBO( )
{
	m_body.generate_VBO();
	m_drawer.generate_VBO();
	m_door.generate_VBO();
}

void	glCabinet::draw()
{
	glTranslatef(m_x, m_y, m_z);
	m_body.draw		();
	m_drawer.draw	();

	// Translate to the Hinge location (x,z)
	glPushMatrix();

	// Realize the door (like all glContainers) are going to be
	// centered at the m_x,m_z.  ie vertices are +- 1/2 width.
	glTranslatef( m_door.m_x, 0.0, m_door.m_z     );
	glRotatef   ( -m_door_angle, 0.0f, 1.0f, 0.0f );
	glTranslatef( -m_door.m_x, -m_door.m_y, -m_door.m_z );
	m_door.draw	( );
	glTranslatef( m_door.m_x, m_door.m_y, m_door.m_z );
	glPopMatrix ( );

}