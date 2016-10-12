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
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#include "all_objects.h"


glDrawer::glDrawer( ) 
{ 
	m_side_thickness	= DEFAULT_SIDE_THICKNESS;
	m_bottom_thickness	= DEFAULT_BOTTOM_THICKNESS;

    // DEFAULT VALUES:
    m_depth  = 15;
    m_width  = 14;
    m_height = 6;
    float Thick  = 1;
    Initialize          ( m_depth, m_width, m_height );
    initialize_faceplate( m_width+1, m_height+1, Thick   );
}

void glDrawer::Initialize	(float mDepth, float mWidth, float mHeight ) 
{
    m_object_type_name = "drawer";    
    m_object_class  = 18;
    
	m_bottom.width  	= mWidth-2*m_side_thickness;
	m_bottom.height 	= m_bottom_thickness;
	m_bottom.depth  	= mDepth-m_side_thickness;
	
	m_left_side.width  	= m_side_thickness;
	m_left_side.height 	= mHeight;
	m_left_side.depth  	= mDepth;

	m_right_side.width	= m_side_thickness;
	m_right_side.height	= mHeight;
	m_right_side.depth	= mDepth;

	m_back.width		= mWidth; 
	m_back.height		= mHeight;
	m_back.depth		= m_side_thickness;

	m_travel_distance = 0.9*mDepth;
}

void glDrawer::initialize_faceplate( float mWidth, float mHeight, float mThick )
{
	m_facePlate.width	= mWidth;
	m_facePlate.height	= mHeight;
	m_facePlate.depth	= mThick;
}

void glDrawer::open		( float mFraction )
{
	if (mFraction> 1.0) mFraction = 1.0;
	if (mFraction< 0.0) mFraction = 0.0;
	
	m_fraction_open = mFraction;
	m_distance_open = (m_fraction_open * m_travel_distance);
}

void glDrawer::close	( float mFraction )
{
	if (mFraction> 1.0) mFraction = 1.0;
	if (mFraction< 0.0) mFraction = 0.0;

	m_fraction_open = 1.0 - mFraction;
	open( m_fraction_open );
}

void glDrawer::grab_left( )
{
	m_bottom.grab_left	   ( );
	m_back.grab_left	   ( );
	m_left_side.grab_left  ( );
	m_right_side.grab_left ( );	// the thickness of this over hang.
	m_facePlate.grab_left  ( );
}

float glDrawer::get_width ( )
{
	return (m_side_thickness + m_bottom.width + m_side_thickness);	
}

void glDrawer::grab_back( )
{
	m_bottom.grab_back	   ( );
	m_back.grab_back	   ( );
	m_left_side.grab_back  ( );
	m_right_side.grab_back ( );	// the thickness of this over hang.	
	m_facePlate.grab_back  ( );

	// Relative Placement (origin: back,right,bottom) !
	m_back.relocate			( 0.0, 					0.0, 		0.0				);
	m_bottom.relocate		( m_side_thickness, 	0.0, 		m_back.depth	);
	m_right_side.relocate	( 0.0, 					0.0, 		0.0				);	
	m_left_side.relocate    ( m_bottom.width+m_right_side.width, 0.0, 	0.0		);

	float overlap = (m_facePlate.width - (m_bottom.width+m_side_thickness*2.))/2.;	
	m_facePlate.relocate	( overlap, 	-1.0, m_bottom.depth+m_back.depth );
}

void glDrawer::grab_right( )
{
	m_bottom.grab_bottom	 ( );
	m_back.grab_bottom		 ( );
	m_left_side.grab_bottom  ( );
	m_right_side.grab_bottom ( );	// the thickness of this over hang.	
	m_facePlate.grab_bottom  ( );

	m_bottom.grab_right		( );
	m_back.grab_right		( );
	m_left_side.grab_right  ( );
	m_right_side.grab_right	( );	// the thickness of this over hang.	
	m_facePlate.grab_right  ( );

	// Relative Placement!
	m_bottom.relocate		( m_side_thickness, 	0.0, 		0.0		);
	m_back.relocate			( 0.0, 					0.0, -m_bottom.depth/2.);  
	m_left_side.relocate    ( m_bottom.width+m_side_thickness, 0.0, 	0.	);
	m_right_side.relocate	( 0.0, 					0.0, 		0.		);

	float overlap = m_facePlate.width - (m_bottom.width+m_side_thickness*2.);	
	m_facePlate.relocate	( -overlap, 			-1.0, m_bottom.depth/2.);
}

void glDrawer::setup( )
{
    m_bottom.set_color(0xFFAF2020);
    m_bottom.setup();

	m_back.change_color( 0xFF5050BF );
    m_back.setup();
    
	m_left_side.generate_vertices();
	m_left_side.change_color( 0xFF5050BF );

	m_right_side.generate_vertices();
	m_right_side.change_color( 0xFF5050BF );

	m_facePlate.generate_vertices();
	m_facePlate.change_color( 0xFFA0A0BF );

    m_bottom.grab_top();
    m_bottom.relocate		( 0.0,              -m_height/2.+m_bottom.m_y, 		0.0	);
    m_left_side.relocate    (  m_bottom.width/2, 0.0,              0.	);
    m_right_side.relocate	( -m_bottom.width/2, 0.0,              0.	);
    m_back.relocate			( 0.0,               0.0,            -m_depth/2. );
    float overlap = (m_facePlate.height - m_left_side.height)/2.;
    m_facePlate.relocate	( 0.0,              overlap,              m_depth/2.);

    
    m_components.push_back(&m_bottom);
    m_components.push_back(&m_back  );
    m_components.push_back(&m_left_side);
    m_components.push_back(&m_right_side);
    m_components.push_back(&m_facePlate);
}

void glDrawer::create( )
{
    setup();
	m_bottom.generate_VBO( );
	m_bottom.generate_IBO( );

	m_back.generate_VBO( );
	m_back.generate_IBO( );

	m_left_side.generate_VBO( );
	m_left_side.generate_IBO( );

	m_right_side.generate_VBO();
	m_right_side.generate_IBO();
	
	m_facePlate.generate_VBO();
	m_facePlate.generate_IBO();
    gl_register();
}

void glDrawer::draw_body( )
{ 
	glPushMatrix();
	// The translate m_x,m_y,m_z are all done previously in glObject::draw()
	glTranslatef(0, 0, m_distance_open );
    glMolecule::draw_body( );
	glPopMatrix();
}

