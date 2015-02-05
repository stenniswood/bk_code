#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"


glDoor::glDoor(  ) 
{ 
	m_max_angle = 90.;
} 

void	glDoor::Initialize	( float mWidth, float mHeight, float mThick ) 
{
	m_door_body.width  = mWidth;
	m_door_body.height = mHeight;
	m_door_body.depth  = mThick;
}

void	glDoor::open		( float mFraction ) 
{
	if (mFraction>1.0) mFraction = 1.0;
	if (mFraction<0.0) mFraction = 0.0;
	m_fraction_open = mFraction;
	m_y_angle = m_fraction_open * m_max_angle;
} 	

void	glDoor::close		( float mFraction ) 
{
	if (mFraction > 1.0) mFraction = 1.0;
	if (mFraction < 0.0) mFraction = 0.0;
	m_fraction_open = 1.0 - mFraction; 
	m_y_angle = m_fraction_open * m_max_angle;
} 	

void	glDoor::create		( ) 
{ 
/*	m_door_body.generate_vertices();
	m_door_body.change_color( 0xFF5F3F20 );
	m_door_body.grab_bottom ();
	m_door_body.grab_left();  */

	m_door_body.generate_VBO();
	m_door_body.generate_IBO();
	m_door_body.Relocate( 0.0, 0., 0.0);
} 

void glDoor::draw_body( )
{
	m_door_body.draw();
}



