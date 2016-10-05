#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "all_objects.h"


const float TOE_HEIGHT = 4.;


glCabinetEndPiece::glCabinetEndPiece( float mDepth, float mWidth, float mHeight  )
{
    m_object_class  = 20;
    
	m_depth  = mDepth;
	m_Thick  = mWidth;
	m_height = mHeight;
		
	m_toe_height = TOE_HEIGHT;
	m_toe_inward = TOE_HEIGHT;		
}

void glCabinetEndPiece::generate_vertices()
{
	struct Vertex v;
	v.color[0] = 0xFF;
	v.color[1] = 0xFF;	
	v.color[2] = 0x00;	
	v.color[3] = 0x00;	
			
	// Square with a notch for the feet!	
	// Start at the BACK BOTTOM : 		// 0
	v.position[0] =  0.0;
	v.position[1] =  0.0;
	v.position[2] =  0.0;
	m_vertices.push_back( v );

	// Then up to the BACK TOP (not the counter added later)
	v.position[0] =  0.0;				// 1
	v.position[1] =  m_height;
	v.position[2] =  0.0;
	v.color[2] = 0xFF;  // yellow
	m_vertices.push_back( v );
	
	// top front:
	v.position[0] =  0.0;				// 2
	v.position[1] =  m_height;
	v.position[2] =  m_depth;		// position will be forward.
	v.color[1] = 0x00;		// green
	m_vertices.push_back( v );
	
	// front (top of toe):
	v.position[0] =  0.0;				// 3
	v.position[1] =  m_toe_height;
	v.position[2] =  m_depth;
	v.color[2] = 0x7F;		// cyanish
	m_vertices.push_back( v );
	
	// front (back of toe):
	v.position[0] =  0.0;				// 4
	v.position[1] =  m_toe_height;
	v.position[2] =  m_depth-m_toe_inward;
	v.color[3] = 0x7F;		// cyanish
	m_vertices.push_back( v );
	
	// front (back of toe):
	v.position[0] =  0.0;				// 5
	v.position[1] =  0.0;
	v.position[2] =  m_depth-m_toe_inward;
	v.color[1] = 0x7F;
	m_vertices.push_back( v );

	// Then up to the BACK TOE TOP (not the counter added later)
	v.position[0] =  0.0;				// 6
	v.position[1] =  m_toe_height;
	v.position[2] =  0.0;
	v.color[1] = 0x7F;
	m_vertices.push_back( v );

/* Cannot draw a convex shape directly.  So we have to break it into 2 Quads.	
	First quad is ground to toe-height.
	Actually drawn as GL_QUAD_STRIP!
	2nd is toe-height to top.	
   Order of indices:  0,5, 6,4, 6,3, 1,2 
*/
	m_layer_one_vertices = m_vertices.size();
}

size_t 	glCabinetEndPiece::generate_disc_indices( GLuint mStartingVertexIndex )
{
	// GL_QUAD_STRIP way : 
	m_indices.push_back( 0 );		m_indices.push_back( 5 );	
	m_indices.push_back( 6 );		m_indices.push_back( 4 );	
	m_indices.push_back( 6 );		m_indices.push_back( 3 );	
	m_indices.push_back( 1 );		m_indices.push_back( 2 );
	m_layer_one_indices = m_indices.size();
	return m_layer_one_indices;
}

/*************************************************************/
#define DRAWER_BELOW_TOP  1.
#define LEFT_MARGIN 	  +1.
const float WOOD_THICKNESS = 0.75;


glCabinet::glCabinet(  )
:glMolecule(),
m_end1( 12.*2., 18., 12*3),
m_end2( 12.*2., 18., 12*3)
{
}

void glCabinet::Initialize( )
{
    m_object_type_name = "cabinet";    
	m_depth  = 12.*2.;	// Actual Measurements!
	m_width  = 18.   ;
	m_height = 12.*3.;

	// origin is right side, back.
	// Door origin is bottom left corner.
	m_door.m_x 		= LEFT_MARGIN; 
	m_door.m_y 		=  4.75;	// above ground.
	m_door.m_z 		=  m_depth;	
	m_door.m_color 	= 0xFFAFAFAF;
	m_door.m_direction_positive = false;

// ******** Now the adjoining pieces (end to end)  **********
	float adjoining_width  = m_width - 2*WOOD_THICKNESS;
	float separator_height = 1.5;	// inch
    
    // (Inches)
    float face_plate_height = 5.75;
    //float offset 	= (m_width - m_drawer.get_width())/2. ;
    m_drawer.m_color = 0xFF00FF00;
    m_drawer.Initialize( 0.9*m_depth, 16., 4. );
    m_drawer.initialize_faceplate( 16.,face_plate_height, 0.75 );
    m_drawer.m_x	= 0.0;
    m_drawer.m_y  	= m_height - face_plate_height+1 - DRAWER_BELOW_TOP;
    m_drawer.m_z	= 0.1*m_depth;
    
	m_drawer_top_bar.width  = adjoining_width;
	m_drawer_top_bar.height = separator_height;
	m_drawer_top_bar.depth  = WOOD_THICKNESS;
	m_drawer_top_bar.m_x    = WOOD_THICKNESS;
	m_drawer_top_bar.m_y    = m_height;
	m_drawer_top_bar.m_z    = m_depth;

	m_separator.width  = adjoining_width;
	m_separator.height = separator_height;
	m_separator.depth  = WOOD_THICKNESS;
	m_separator.m_x 	= WOOD_THICKNESS;		
	m_separator.m_y 	= 26.5;		
	m_separator.m_z 	= m_depth;	
	
	m_shelf.width  = adjoining_width;
	m_shelf.height = WOOD_THICKNESS;
	m_shelf.depth  = 12.0;	
	m_shelf.m_x 	= WOOD_THICKNESS;
	m_shelf.m_y 	= 17.0;
	m_shelf.m_z 	= 0;

	m_below_door.width  = adjoining_width;
	m_below_door.height = separator_height;
	m_below_door.depth  = WOOD_THICKNESS;
	m_below_door.m_x 	= WOOD_THICKNESS;
	m_below_door.m_y 	= TOE_HEIGHT;
	m_below_door.m_z 	= m_depth;
    
    m_end1.m_extrusion_length = WOOD_THICKNESS;
    m_end1.m_extrusion_axis = 0;
    m_end1.m_x = 0.;
    m_end1.m_y = 0.;
    m_end1.m_z = 0.;
    
    m_end2.m_extrusion_length = WOOD_THICKNESS;
    m_end2.m_extrusion_axis = 0;
    m_end2.m_x = m_width-WOOD_THICKNESS;
    m_end2.m_y = 0.;
    m_end2.m_z = 0.;
}

void glCabinet::colorize()
{
	long int GREY = 0xFFCFCFCF;
	m_drawer_top_bar.change_color( GREY );
	m_separator.change_color	 ( GREY );
	m_shelf.change_color		 ( GREY );
	m_below_door.change_color	 ( GREY );
}

void glCabinet::setup()
{
    m_door.set_params( 16., 22., 1. );
    m_door.Initialize( );
    m_door.setup  ();
    m_door.open( 0.0 );
    
    m_drawer.setup();
    m_drawer.grab_right();
    m_drawer.grab_back();
    
	m_drawer_top_bar.generate_vertices();
	m_drawer_top_bar.grab_top();
	m_drawer_top_bar.grab_right();	
	m_drawer_top_bar.grab_front();
    m_components.push_back( &m_drawer_top_bar );

//	m_separator.generate_vertices();
    m_separator.setup();
	m_separator.grab_bottom();
	m_separator.grab_right();		
	m_separator.grab_front();
    m_components.push_back( &m_separator );

	//m_shelf.generate_vertices();
    m_shelf.setup();
	m_shelf.grab_bottom();
	m_shelf.grab_right();		
	m_shelf.grab_back();	
    m_components.push_back( &m_shelf );

	m_below_door.generate_vertices();
	m_below_door.grab_bottom();
	m_below_door.grab_right();
	m_below_door.grab_front();
    m_components.push_back( &m_below_door );
    
    //m_end1.create();
    //m_end2.create();
    m_end1.setup();
    m_end2.setup();
    
    m_components.push_back( &m_end1 );
    m_components.push_back( &m_end2 );
    m_components.push_back( &m_body );
    m_components.push_back( &m_drawer);
    m_components.push_back( &m_door );
}

void glCabinet::create( )
{
	Initialize();
	setup();
	colorize();
	

    gl_register();    
}

/*
void	glCabinet::draw()
{
	glPushMatrix();
	glTranslatef(m_x, m_y, m_z);
	// When want to change gl_Extrusion so that it is derived from glAtom.
	// then this will not have to be duplicated here.
	glRotatef   (m_y_angel, 0.0, 1.0, 0.0 );

	m_drawer.draw();
	m_door.draw	( );
	m_end1.draw();
	m_end2.draw();
	m_separator.draw	 ();
	m_drawer_top_bar.draw();	
	m_below_door.draw	 ();
	m_shelf.draw		 ();

	glPopMatrix ( );
}*/

