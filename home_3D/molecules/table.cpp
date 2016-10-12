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


glTable::glTable(  )
{
	Initialize();
}

glTable::~glTable(  )
{
    // Delete legs:
    if (m_components.size()>=4) {
        delete m_components[1];
        delete m_components[2];
        delete m_components[3];
        delete m_components[4];
    }
}

void glTable::Initialize( )
{
    m_object_type_name = "table";
    m_table_length = 57.;	// 57 Actual Measurements!
    m_table_width  = 38.;	// 38
    m_table_height = 29.;
}

void glTable::create_components( )
{
    // Table Top (inches):
	m_table_top.width  = m_table_width;
	m_table_top.height = 1.;		// Really the table thickness. 
	m_table_top.depth  = m_table_length;
	m_table_top.relocate(0., m_table_height - m_table_top.height/2., 0. );
	m_table_top.m_is_closed = true;
    Texture* txt = m_table_top.load_image("textures/oak_wood.jpg",1);
    m_table_top.apply_front( txt);
    m_table_top.apply_bottom( txt);
    m_table_top.grab_bottom();
    m_table_top.setup();
    m_components.push_back( &m_table_top );

	glCylinder* tmp = new glCylinder(24);
	tmp->m_radius = 1.5;
	tmp->relocate( -m_table_top.width/2., 0., -m_table_top.depth/2. );
    //  m_y is the center of the cylinder (middle of leg)
	m_table_legs.push_back( tmp );
    tmp->set_la(m_table_height, 1 );
    tmp->setup( );
    m_components.push_back( tmp );

    tmp = new glCylinder(24);
	tmp->m_radius = 1.5;
	tmp->relocate(  +m_table_top.width/2., 0., -m_table_top.depth/2. );
	m_table_legs.push_back( tmp );
    tmp->set_la(m_table_height, 1 );
    tmp->setup ();
    m_components.push_back( tmp );
    
    tmp = new glCylinder(24);
	tmp->m_radius = 1.5;
	tmp->relocate( -m_table_top.width/2., 0., +m_table_top.depth/2. );
    m_table_legs.push_back( tmp );
    tmp->set_la(m_table_height, 1 );
    tmp->setup();
    m_components.push_back( tmp );

    tmp = new glCylinder(24);
	tmp->m_radius = 1.5;
	tmp->relocate( m_table_top.width/2., 0., m_table_top.depth/2. );
	m_table_legs.push_back( tmp );
    tmp->set_la(m_table_height, 1 );
    tmp->setup();
    m_components.push_back( tmp );
}


/*void glTable::relocate( float mX, float mY, float mZ )
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
 }*/
