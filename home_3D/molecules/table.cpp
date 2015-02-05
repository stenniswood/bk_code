#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"


glTable::glTable(  )
{
	m_table_length = 57.;	// 57 Actual Measurements!
	m_table_width  = 38.;	// 38
	m_table_height = 29.;
	Initialize();
}

void glTable::relocate( float mX, float mY, float mZ )
{
	m_x = mX;
	m_y = mY;
	m_z = mZ;
	
	m_table_top.m_x = m_x;
	m_table_top.m_y = m_y+m_table_height-m_table_top.height;	// zero is at bottom of table!
	m_table_top.m_z = m_z;

	m_table_legs[0].m_x = m_x -m_table_top.width/2.;
	m_table_legs[0].m_z = m_z -m_table_top.depth/2.;

	m_table_legs[1].m_x = m_x +m_table_top.width/2.;
	m_table_legs[1].m_z = m_z -m_table_top.depth/2.;

	m_table_legs[2].m_x = m_x -m_table_top.width/2.;
	m_table_legs[2].m_z = m_z +m_table_top.depth/2.;

	m_table_legs[3].m_x = m_x +m_table_top.width/2.;
	m_table_legs[3].m_z = m_z +m_table_top.depth/2.;
}

void glTable::Initialize( )
{
	// Table Top (inches):
	m_table_top.width  = m_table_width;
	m_table_top.height = 1.;		// Really the table thickness. 
	m_table_top.depth  = m_table_length;
	m_table_top.m_y    = m_table_height-m_table_top.height/2.;
	m_table_top.m_is_closed = true;

	glCylinder tmp(24);
	tmp.m_radius = 1.5;
	tmp.m_y = 0.;	// Table Height. m_y is the center of the cylinder (middle of leg)

	tmp.m_x = -m_table_top.width/2.;
	tmp.m_z = -m_table_top.depth/2.;
	m_table_legs.push_back( tmp );

	tmp.m_x = +m_table_top.width/2.;
	tmp.m_z = -m_table_top.depth/2.;
	m_table_legs.push_back( tmp );

	tmp.m_x = -m_table_top.width/2.;
	tmp.m_z = +m_table_top.depth/2.;
	m_table_legs.push_back( tmp );

	tmp.m_x =  m_table_top.width/2.;
	tmp.m_z = +m_table_top.depth/2.;	
	m_table_legs.push_back( tmp );
}

void	glTable::create()
{
	m_table_top.generate_vertices();
	m_table_top.generate_IBO();
	m_table_top.generate_VBO();	
	m_table_top.load_image("textures/oak_wood.jpg");	
	m_table_top.generate_TBO( );
	m_table_top.generate_texture_coords( );
	m_table_top.generate_VBOTexCoords	( ); 

	for (int i=0; i<m_table_legs.size(); i++)
	{	
		m_table_legs[i].generate_vertices();
		m_table_legs[i].extrude_vertices( m_table_height );
		m_table_legs[i].change_vertices_colors();
		m_table_legs[i].get_max();
		m_table_legs[i].get_min();
		m_table_legs[i].print_min_max();
		
		m_table_legs[i].grab_bottom();
		
		m_table_legs[i].generate_indices ();		
		m_table_legs[i].generate_VBO();
		m_table_legs[i].generate_IBO();
	}
}

void	glTable::draw()
{
	m_table_top.draw();	
	for (int i=0; i<m_table_legs.size(); i++)
		m_table_legs[i].draw();
}


