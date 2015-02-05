/* Strategy:
		We'll keep a list of vertices in 2 dimensions; starting with 
		4 defining a rectangle length and height of wall.
		
		if a door is added.  the vertices are added.
		
		To generate the indices:
			scan the vertices starting at the left.
*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"
#include "wall.hpp"
#include "door.hpp"

#define Debug 0

static vector<int>  floor_marks;		// store the indices (not the value)
static vector<int>  ceiling_marks;		// store the indices (not the value)

abWall::abWall( )
{ 
	m_wall_length = 10.*12.;			// default 10' long 
	m_wall_height = STANDARD_WALL_HEIGHT;
	m_angle 	= 0.0;
	clear();
}

abWall::abWall( float mLength )
{ 
	m_wall_length = mLength;
	m_wall_height = STANDARD_WALL_HEIGHT;
	m_angle = 0.0;
	clear();	
}

void abWall::clear()
{
	m_doors.clear();
	m_windows.clear();
	floor_marks.clear();
	ceiling_marks.clear();
}

bool	abWall::is_valid_location( float mPositionLengthwise, float mWidth )
{
	float start = 0;
	float stop  = 0;
	float NewDoorStop = 0;
	NewDoorStop = mPositionLengthwise+mWidth;
	
	// NO OVERLAPPING ANOTHER DOOR :
	for (int d=0; d<m_doors.size(); d++)
	{
		start = m_doors[d].PositionLengthwise;
		stop  = m_doors[d].PositionLengthwise+m_doors[d].Width;

		if ((mPositionLengthwise>start) && 
		    (mPositionLengthwise<stop) )
		{
			printf("Requested position is in middle of door #%d\n", d);
		    return false;
		}
		if ((NewDoorStop > start) && (NewDoorStop < stop) )
		{
			printf("Requested End Position is in middle of door #%d\n", d);
		    return false;			    
		}
	}	
	// OVERLAPPING A WINDOW?
	
	// WITHIN WALL LENGTH?
	if ((mPositionLengthwise + mWidth) > m_wall_length)
	{
		printf("Requested position is after wall end!\n");	
		return false;
	}
	if ((mPositionLengthwise) < 0.0)
	{
		printf("Requested position is before wall start!\n");
		return false;
	}
	return true;	
}

/*
return:	true 	if added.
		false	not added - another in the way.
*/
bool  abWall::add_door( float mPositionLengthwise, float mWidth, float mHeight )
{
	if (is_valid_location(mPositionLengthwise, mWidth)==false)
	{
		printf("Cannot add door at %6.1f of width=%6.1f, height=%6.1f.  Wall length is %6.1f\n",
					mPositionLengthwise, mWidth, mHeight, m_wall_length );
		return false;
	}
	struct stDoorWay d;
	d.FirstVertexIndex = 0;
	d.PositionLengthwise = mPositionLengthwise;
	d.Width  = mWidth;
	d.Height = mHeight;
	m_doors.push_back(d);
	
	return true;
}


bool abWall::add_window( float mPositionLengthwise, struct stSize mSize, float mHeight )
{
	if (is_valid_location(mPositionLengthwise, mSize.width)==false)
	{
		printf("Cannot add window at %6.1f of width=%6.1f, height=%6.1f.  Wall length is %6.1f\n",
					mPositionLengthwise, mSize.width, mHeight, m_wall_length );
		return false;
	}
	struct stWindow w;	
	w.PositionLengthwise = mPositionLengthwise;
	w.Width  	 = mSize.width;
	w.Height 	 = mSize.height;
	w.SillHeight = mHeight;
	m_windows.push_back(w);
	return true;
}


// arrange doors and windows by ascending order
void abWall::sort()
{
	//m_doors.sort(  );	
	//m_windows.sort( );
}

void abWall::print_corners( )
{
	for (int c=0; c<m_corners.size(); c++)
	{
		printf("\tCorner #%d : x,z=%6.1f, %6.1f\t", c, m_corners[c].x, m_corners[c].z );
		if (m_corners[c].type == CONVEX )		
			printf("type=CONVEX\n" );
		else 
			printf("type=CONCAVE\n" );		
	}
}

void abWall::print_dw_info( )
{
	for (int d=0; d<m_doors.size(); d++)
	{
		printf("\tDoor   #%d : Distance: %6.1f\t", d, m_doors[d].PositionLengthwise );
		printf("w=%6.1f; h=%6.1f;\n", m_doors[d].Width, m_doors[d].Height );
	}
	for (int w=0; w<m_windows.size(); w++)
	{
		printf("\tWindow #%d : Distance: %6.1f;\tw=%6.1f; h=%6.1f\t", w, 
				m_windows[w].PositionLengthwise,
				m_windows[w].Width, m_windows[w].Height );
		printf("Sill Height=%6.1f;  \n", m_windows[w].SillHeight );
	}
}

void abWall::print_info( )
{
	printf("length=%6.1f;  height=%6.1f;  Angle=%6.1f deg\t", m_wall_length, m_wall_height, m_angle );
	printf("<x,z> = <%6.1f, %6.1f> \n", m_x, m_z );
}

/* This will compare this wall to all others in the vector and see
	if any of them form a corner with this one.	*/
void abWall::extract_corners(vector<abWall>*  mWalls )
{
	bool crosses;
	bool one_end_less;
	bool one_end_more;
	struct stCorner c;
	
	for (int w=0; w<(*mWalls).size(); w++)
	{
		if ( (*mWalls)[w].m_angle != m_angle)
		{
			one_end_less = ( (*mWalls)[w].m_z < m_z);
			one_end_more = ( (*mWalls)[w].m_z > m_z);
			crosses = one_end_less && one_end_more;

			if (m_angle==0)
			{			
				c.x    = (*mWalls)[w].m_x;
				c.z    = m_z;			
				c.type = CONCAVE;
			} else {
				c.x    = m_x;	
				c.z    = (*mWalls)[w].m_z;
				c.type = CONCAVE;			
			}
			m_corners.push_back( c );			
		}
	}
}

/* Call this :
	After constructing, AFTER adding any doors.

NOTE: If you want to change the vertices color, it must be done inside this function.
	  (before generate_VBO() and after all vertices are created!)
*/

/* 
	Mark the wall all along the way with both doors and Windows
	for simplicity of generating the indices.	
*/
void abWall::generate_marks( )
{
	int d = 0;
	int w = 0;
	float d_value = 0.0;
	float w_value = 0.0;
	floor_marks.push_back(0);		// start of wall.
	ceiling_marks.push_back(3);		// start of wall.	
	bool done = false;
	int prev_ceiling = 0;
	int prev_floor   = 0;
	int v_index = 0;
	while (!done)
	{
		if (d < m_doors.size())
			d_value = m_doors[d].PositionLengthwise;
		else 
			d_value = m_wall_length;
		if (w<m_windows.size())
			w_value = m_windows[w].PositionLengthwise;
		else 
			w_value = m_wall_length;

		if (d_value < w_value)	{
			//printf("Adding door to indices marks\n");
			// ADD INDICES for drawing WALL PANELS (not headers)
			// So allow wall to come right up to the door:			
			v_index =  m_doors[d].FirstVertexIndex + 0;	// door floor
			floor_marks.push_back  ( v_index );
			v_index =  m_doors[d].FirstVertexIndex + 2;	// door ceiling index.
			ceiling_marks.push_back( v_index );
			
			// Then allow wall start again right after the door:			
			v_index =  m_doors[d].FirstVertexIndex + 3;	// door far floor
			floor_marks.push_back  ( v_index );
			v_index =  m_doors[d].FirstVertexIndex + 5;	// door far ceiling index.
			ceiling_marks.push_back( v_index );			
			// Then it comes back to ____.
			// 		index 3 for the first door.
			// for 2nd and later doors, comes back to prev door ceiling, then floor.				
			d++;
			//if (d>=m_doors.size()) done = true;
		} else if (w<m_windows.size()) {			
			// ADD INDICES for drawing WALL PANELS (not headers)
			// So allow wall to come right up to the door:			
			v_index =  m_windows[w].FirstVertexIndex + 0;	// window floor
			floor_marks.push_back  ( v_index );
			v_index =  m_windows[w].FirstVertexIndex + 3;	// window ceiling index.
			ceiling_marks.push_back( v_index );
			
			// Then allow wall start again right after the door:		
			v_index =  m_windows[w].FirstVertexIndex + 4;	// window far floor
			floor_marks.push_back  ( v_index );
			v_index =  m_windows[w].FirstVertexIndex + 7;	// window far ceiling index.
			ceiling_marks.push_back( v_index );			
			w++;
		}
		if ((d>=m_doors.size())	&& (w>=m_windows.size()))
			done = true;		
	}
	// Last add the far wall ends:
	floor_marks.push_back  ( 1 );
	ceiling_marks.push_back( 2 );
}

void abWall::print_marks( )
{
	printf("ceil:\t");
	for (int i=0; i<ceiling_marks.size(); i++)
	{
		printf("%d, ", ceiling_marks[i]);		
	}
	printf("\nfloor:\t");
	for (int i=0; i<floor_marks.size(); i++)
	{
		printf("%d, ", floor_marks[i] );
	}
	printf("\n");
}


void	abWall::draw()
{
}

#define Door m_doors[mDoorIndex]

struct Vertex abWall::get_door_coord( int mDoorIndex )
{
	struct Vertex v;
	if (m_angle == 0)
	v.position[0] = m_z;
	v.position[2] = m_x+m_wall_length;	
	return v;
}

struct Vertex abWall::get_door_center_coord( int mDoorIndex )
{
//	struct Vertex v1 =  m_vertices[Door.FirstVertexIndex];
	struct Vertex vc;
/*	struct Vertex v2 =  m_vertices[Door.FirstVertexIndex+1];

	vc.color[0] = v1.color[0];
	vc.color[1] = v1.color[1];
	vc.color[2] = v1.color[2];
	vc.color[3] = v1.color[3];
	vc.position[1] = v1.position[1];
	
	vc.position[0] = (v1.position[0] + v2.position[0]) / 2.;
	vc.position[2] = (v1.position[2] + v2.position[2]) / 2.; */
	return vc;	
}

struct Vertex abWall::get_door_far_coord(  int mDoorIndex  )
{
	struct Vertex v;
	if (m_angle == 0)
	v.position[0] = m_z;
	v.position[2] = m_x+m_wall_length;
	return v;
}


