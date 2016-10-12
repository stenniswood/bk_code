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
#include "room2.hpp"


#define TOP_AND_BOTTOM 2
#define CLOSE_OUT 1


string get_name( vector<Room>& mRooms, int mIndex )
{
    if (mIndex==-1)
        return "outside";
    return mRooms[mIndex].m_names;    
}

/*****************************************
 class Room
 ****************************************/
Room::Room( )
{
}

Room::~Room( )
{
}

MathVector Room::get_room_center( )
{
    float w = get_width()/2.;
    float h = get_height()/2.;
    MathVector tmp(3);
    tmp[0] = get_left() + w;
    tmp[0] = get_bottom() + h;
    return tmp;
}

bool Room::contains( MathVector mLocation )
{
    bool inside_x = ((mLocation[0] < get_right()) && (mLocation[0] > get_left()));
    bool inside_y = ((mLocation[2] < get_top())   && (mLocation[2] > get_bottom()));
    
    if (inside_x && inside_y)
        return true;
    return false;
}

void Room::place_next_to_x   ( Room& mReference )
{
    move_to(mReference.get_right(), mReference.get_bottom());
}

void Room::place_next_to_y   ( Room& mReference )
{
    move_to(mReference.get_left(), mReference.get_top());
}

void Room::create_room ( float width, float length )
{
    glFullWall* w = new glFullWall();
    
    // Wall #1
    w->set_length    ( width );
    w->create        (       );
    w->relocate      ( 0,0,0 );
    m_bounding_walls.push_back( w );

    // Wall #2
    w = new glFullWall();
    w->set_length    ( width );
    w->create        (       );
    w->relocate      ( 0,0,length );
    m_bounding_walls.push_back( w );

    // Wall #3
    w = new glFullWall();
    w->set_length    ( length );
    w->create        (        );
    w->relocate      ( 0,0,length );
    w->m_y_angle = 90;
    m_bounding_walls.push_back( w );

    // Wall #4
    w = new glFullWall();
    w->set_length    ( length );
    w->create        (        );
    w->relocate      ( width,0,length );
    w->m_y_angle = 90;
    m_bounding_walls.push_back( w );
}

/* The wall from 2D point of view consists of:
		<m_x, m_z>			and
		m_y_angle 			(in degrees)
		m_wall_length		(in inches )
 Creates Vertex  from the Wall 3D data    */
void Room::update_line_info( )
{
	for (int w=0; w<m_bounding_walls.size(); w++)
	{
		// Extracts the wall info plus copies the Doors and Windows info :
		m_bounding_walls[w]->m_bare_wall.extract_2d_line_info( );
	}
}


