#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"


float radians( float Degrees)
{
	return Degrees * M_PI / 180.0;	
}

glCabinetEndPiece::glCabinetEndPiece( float mDepth, float mWidth, float mHeight  )
{
	m_depth  = mDepth;
	m_Thick  = mWidth;
	m_height = mHeight;
		
	m_toe_height = 4.;
	m_toe_inward = 4.;		
}
void glCabinetEndPiece::Initialize( )
{

}
void glCabinetEndPiece::generate_vertices()
{

	struct Vertex v;

	// Square with a notch for the feet!	
	// Start at the back bottom:
	v.position[0] =  0.0;
	v.position[1] =  0.0;
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
	v.position[2] =  m_depth;		// position will be forward.
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

	m_layer_one_vertices = m_vertices.size();
}


glCabinet::glCabinet(  )
:glExtrusion(),
m_end1( 12.*2., 18., 12*3),
m_end2( 12.*2., 18., 12*3)
{	
	m_depth  = 12.*2.;	// Actual Measurements!
	m_width  = 18.   ;
	m_height = 12.*3.;
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
	m_body.grab_bottom();
	m_body.grab_right();	
	m_body.Relocate( 0., 0., 0.);
	printf("Body complete\n");
	
	//m_drawer.m_width	= 16.;	//inches measured!
	//m_drawer.m_height	=  6.;
	//m_drawer.m_depth	= 0.9*m_depth;
 	float face_plate_height = 5.75;
	m_drawer.m_color = 0xFF00FF00;
	m_drawer.Initialize( 0.9*m_depth, 16., 6. );
 	m_drawer.initialize_faceplate( 16.,face_plate_height, 0.75 );
	m_drawer.setup();
 	m_drawer.grab_right();
	m_drawer.create();
	m_drawer.m_y_angle = 180.;
	float offset = m_body.width-(m_body.width - m_drawer.get_width())/2. ;
	m_drawer.m_x	= offset;
	m_drawer.m_y  	= m_height - face_plate_height+1 - DRAWER_BELOW_TOP;
	m_drawer.m_z	= -2.;
	printf("Body complete\n");
	
	// origin is right side, back.
/*	m_drawer.m_x	= LEFT_MARGIN;
	m_drawer.m_y  	= m_height - m_drawer.m_height/2. - DRAWER_BELOW_TOP;
	m_drawer.m_z	= -m_drawer.m_depth/2.; */
/* was	*/

	// Door origin is bottom left corner.
	m_door.Initialize		( 16., 22., 1. );
	m_door.m_door_body.generate_vertices( );
	m_door.m_door_body.change_color( 0xFF5F3F20 );
	m_door.m_door_body.grab_bottom ( );
	m_door.m_door_body.grab_right  ( ); 	// grab the hinge side

	m_door.m_x 		= LEFT_MARGIN; //-m_body.width/2.+;	//+m_door.width;
	m_door.m_y 		=  4.75;	// above ground.
	m_door.m_z 		=  -m_body.depth/2.-m_door.m_door_body.depth/2.;	
	m_door.m_color 	= 0xFFAFAFAF;	
	m_door.create();
	m_door.open( 0.0 ); 
	
	printf("Body complete\n");
	
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

void glCabinet::create( )
{
	generate_VBO();
	generate_IBO();
}
void 	glCabinet::generate_IBO( )
{
	m_body.generate_IBO();
	//m_drawer.generate_IBO();
	//m_door.generate_IBO();
}

void 	glCabinet::generate_VBO( )
{
	m_body.generate_VBO();
	//m_drawer.generate_VBO();
	//m_door.generate_VBO();
}

void	glCabinet::draw()
{
	glPushMatrix();
	glTranslatef(m_x, m_y, m_z);
	m_body.draw		();
	m_drawer.draw	();

	// Translate to the Hinge location (x,z)

	// Realize the door (like all glContainers) are going to be
	// centered at the m_x,m_z.  ie vertices are +- 1/2 width.
//	glTranslatef( m_door.m_x, 0.0, m_door.m_z     );
//	glRotatef   ( -m_door.m_door_angle, 0.0f, 1.0f, 0.0f );
//	glTranslatef( -m_door.m_x, -m_door.m_y, -m_door.m_z );
	m_door.draw	( );
//	glTranslatef( m_door.m_x, m_door.m_y, m_door.m_z );
	glPopMatrix ( );

}