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


glCounter::glCounter( float mLength, float mWidth, float mHeightOffFloor )
{
    m_object_class  = 19;
    m_object_type_name = "counter";
    
	// Counter top:
	m_counter_top.width  = mLength;
	m_counter_top.height =  1.;			// thickness
	m_counter_top.depth  = mWidth;
	m_counter_top.m_y 	 = mHeightOffFloor;
	m_counter_top.m_is_closed = true;
	m_counter_top.m_color = 0xFF7f7f7f;
}

/* This creates the cabinets and place their location along x */
void glCounter::set_number_of_cabinets( int mNumber )
{
	glCabinet cab;
	cab.create( );
	
	// We'll line them up along the m_x axis.
	float x_position = cab.m_width;	
	// because of the 180 rotation, 
	// we have to allow the first cabinet to be shifted.

	for (int c=0; c<mNumber; c++)
	{		
		cab.relocate( x_position, 0., 0. );
		cab.m_y_angel = 180.;
		m_cabinets.push_back( cab );
		x_position += cab.m_width;
	}
}
	
void glCounter::create( )
{
	m_counter_top.generate_vertices();
	m_counter_top.grab_top();
	m_counter_top.grab_right();
	m_counter_top.grab_front();	
	//m_counter_top.change_color( 0xFF7f7f7f );
	
	// Want to be able to grab the cabinets too!!
	// right now it's centering them 
	m_counter_top.generate_VBO();
	m_counter_top.generate_IBO();
}

void glCounter::open( int mIndex, float mFraction )
{
	m_cabinets[mIndex].open_door  ( mFraction );
	m_cabinets[mIndex].open_drawer( mFraction );
}

void 	glCounter::generate_IBO( )
{
	m_counter_top.generate_IBO();
//	for (int i=0; i<m_cabinets.size(); i++)
//		m_cabinets[i].generate_IBO();
}

void 	glCounter::generate_VBO( )
{
	m_counter_top.generate_VBO();
//	for (int i=0; i<m_cabinets.size(); i++)
//		m_cabinets[i].generate_VBO();
}

void	glCounter::draw_body()
{
	m_counter_top.draw();
	for (int i=0; i<m_cabinets.size(); i++)
		m_cabinets[i].draw();
}

void glCounter::Initialize( )
{
	// Table Top (inches):
	
}
