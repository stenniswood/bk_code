#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"


glObject::glObject( ) 
{ 
	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_x_angle = 0;
	m_y_angle = 0;
	m_z_angle = 0;
}

void	glObject::relocate( float mX, float mY, float mZ ) 
{ 
	m_x = mX;
	m_y = mY;
	m_z = mZ;
}

void 	glObject::change_color	( long mColor ) 
{ 
	m_color = mColor;
}


void	glObject::draw_body				( )	// Override this with open gl commands.
{

}

void	glObject::draw					( ) 
{ 
	glPushMatrix();
	glTranslatef(m_x, m_y, m_z);
	glRotatef(m_x_angle, 1.0, 0.0, 0.0 );
	glRotatef(m_y_angle, 0.0, 1.0, 0.0 );
	glRotatef(m_z_angle, 0.0, 0.0, 1.0 );

	draw_body();
	glPopMatrix();
}

