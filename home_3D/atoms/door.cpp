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
	m_min_angle = 0.;
	m_max_angle = 90.;		// this must be getting clobbered before Initialize! yikes watch out for other problems.
	m_direction_positive = true;
} 

void	glDoor::Initialize	( float mWidth, float mHeight, float mThick )
{
	m_direction_positive = true;
	m_door_body.width  = mWidth;
	m_door_body.height = mHeight;
 	m_door_body.depth  = mThick;
	m_min_angle = 0.;
	m_max_angle = 90.;
}

void	glDoor::open		( float mFraction )
{
	if (mFraction > 1.0) mFraction = 1.0;
	if (mFraction < 0.0) mFraction = 0.0;
	m_fraction_open = mFraction;
	m_y_angle = get_angle();
	//printf("door y_angle = %6.3f\n", m_y_angle );
}

void	glDoor::close		( float mFraction )
{
	if (mFraction > 1.0) mFraction = 1.0;
	if (mFraction < 0.0) mFraction = 0.0;
	m_fraction_open = 1.0 - mFraction;
	m_y_angle       = get_angle();
	//printf("door y_angle = %6.3f\n", m_y_angle );
}

void	glDoor::set_hinge	( bool mRightSide )
{
	if (mRightSide)
		m_door_body.grab_left();
	else 
		m_door_body.grab_right();
}

float	glDoor::get_angle( )
{
	float range = (m_max_angle - m_min_angle);
	float angle = (m_fraction_open * range) + m_min_angle;
	if (m_direction_positive==false)
		angle = -angle;
	return angle;
}

void	glDoor::set_range( float mMinAngle, float mMaxAngle )
{
	m_min_angle = mMinAngle;
	m_max_angle = mMaxAngle;
}

void	glDoor::create		( )
{
	m_door_body.generate_vertices( );
	m_door_body.change_color( 0xFF5F3F20 );
	m_door_body.grab_back ( );
	m_door_body.grab_bottom ( );
	m_door_body.grab_right  ( ); 	// grab the hinge side

	m_door_body.generate_VBO();
	m_door_body.generate_IBO();
	m_door_body.Relocate( 0.0, 0., 0.0);
} 

void glDoor::draw_body( )
{
	m_door_body.draw();
}

