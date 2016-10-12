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

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "all_objects.h"
#include "door.hpp"

#define Debug 0

static vector<int>  floor_marks;		// store the indices (not the value)
static vector<int>  ceiling_marks;		// store the indices (not the value)

const float WALL_END_OFFSET = 12.;


abWall::abWall( )
:m_line(3)
{
    Initialize();
}

abWall::abWall( float mLength )
:m_line(3)
{
    Initialize();
    m_wall_length = mLength;
}

void abWall::clear()
{
	m_doors.clear();
	m_windows.clear();
	floor_marks.clear();
	ceiling_marks.clear();
}

void abWall::Initialize( )
{
    m_show_half_height  = false;
    m_wall_length       = 10.*12.;			// default 10' long
    m_wall_height       = DEFAULT_WALL_HEIGHT;
    m_angle             = 0.0;
    m_line.m_vector[0]  = 1;
    m_line.m_vector[2]  = 0;
    clear();
}

/*   For a window or door  */
bool abWall::is_valid_location( float mPositionLengthwise, float mWidth )
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
	d.FirstVertexIndex   = 0;
	d.PositionLengthwise = mPositionLengthwise;
	d.Width  = mWidth;
	d.Height = mHeight;
	m_doors.push_back(d);
    //printf("Added door to abstract wall.  %lu doors\n", m_doors.size() );    
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
/*void abWall::sort()
{
//	m_doors.sort(  );
//	m_windows.sort( );
}*/

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

bool abWall::print_dw_info( )
{
	bool retval = false;
	for (int d=0; d<m_doors.size(); d++)
	{
		printf("\tDoor   #%d : Distance: %6.1f\t", d, m_doors[d].PositionLengthwise );
		printf("w=%6.1f; h=%6.1f;\n", m_doors[d].Width, m_doors[d].Height );
		if (d<m_doors.size()-1) printf("\t");
		retval = true;
	}
	for (int w=0; w<m_windows.size(); w++)
	{
		printf("\tWindow #%d : Distance: %6.1f;\tw=%6.1f; h=%6.1f\t", w, 
				m_windows[w].PositionLengthwise,
				m_windows[w].Width, m_windows[w].Height );
		printf("Sill Height=%6.1f;  \n", m_windows[w].SillHeight );
		if (w<(m_windows.size()-1)) printf("\t");
		retval = true;
	}
	return retval;
}

void abWall::print_info( )
{
	printf("length=%6.1f;  height=%6.1f;\t", m_wall_length, m_wall_height );
    
    m_line.m_origin.print();
    //printf("<x,z> = <%6.1f, %6.1f> \n", m_x, m_z );
}

MathVector	abWall::get_far_end (  )
{
    MathVector end = m_line.m_origin + (m_line.m_vector * m_wall_length);
    return end;
}

/* This will compare this wall to all others in the vector and see
	if any of them form a corner with this one.	*/
void abWall::extract_corners(vector<abWall>*  mWalls )
{
	for (int w=0; w<(*mWalls).size(); w++)
	{
        /*  one_end_less = ( (*mWalls)[w].m_z < m_z);
			one_end_more = ( (*mWalls)[w].m_z > m_z);
			crosses      = one_end_less && one_end_more;
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
		}*/
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

bool  abWall::intersects(  LineSegment mPath, float& t, float& s )
{
    int result = m_line.line_intersection(mPath, t, s);
    if (result)
    {
        // Yes they intersect.
        //      adjust endpoint
        return true;
    }
    else
        return false;
}

/* Assumption is that mIntersection is a 3D point on the base of the wall */
float abWall::get_distance_along( MathVector mIntersection )
{
    // m_origin + m_vector * mIntersection
    MathVector delta = (mIntersection - m_line.m_origin);
    float  distance = delta.magnitude();
    return distance;
}

// returns -1.0 or +1.0
float abWall::is_on_which_side( MathVector mPoint )
{
    // Find which side of the wall we are on (with respect to the unit vector):
    MathVector dv = mPoint - m_line.m_origin;
    float side_projection = dv.dot( m_line.m_vector.get_perp_xz() );
    if (side_projection>=0.0)
    {
        side_projection = 1.0;      printf("is on + side\n");
    } else {
        side_projection = -1.0;     printf("is on - side\n");
    }
    return side_projection;
}

MathVector abWall::get_point_away_from( float mDistanceAlong, float mPerpendicularDistance )
{
    MathVector pt("point away from wall", 3);
    pt = m_line.m_origin + m_line.m_vector * mDistanceAlong;
    pt += m_line.m_vector.get_perp_xz() * mPerpendicularDistance;
    return pt;
}

/* 
 Assumption is that mIntersection is a 3D point on the base of the wall
 */
int abWall::find_closest_door( MathVector mIntersection )
{
    int   min_door_index = -1;
    float min_distance   = m_wall_length;
    int   num_doors      = (int)m_doors.size();
    
    // Find closest Door to the Intersection.
    for (int d=0; d<num_doors; d++)
    {
        MathVector dc = get_door_center_coord( d );
        float distance = mIntersection.get_distance( dc );
        if (distance < min_distance)
        {
            min_door_index = d;
            min_distance = distance;
        }
    }
    return min_door_index;
}


void abWall::find_paths_around ( list<float>& mDistanceAlong )
{
    // LIST ALL DOORS :
    long size = m_doors.size();
    for (int i=0; i<size; i++)
        mDistanceAlong.push_back( get_door_center(i) );
    
    // LIST END POINTS :
    float tmp = -WALL_END_OFFSET ;
    mDistanceAlong.push_back(tmp);
    
    tmp = (m_wall_length+WALL_END_OFFSET);
    mDistanceAlong.push_back(tmp);
}

void abWall::find_paths_around( list<MathVector>& mPoints, float mOffsetAwayFromWall )
{
    // LIST ALL DOORS :
    long size = m_doors.size();
    for (int i=0; i<size; i++)
        mPoints.push_back( get_door_center_coord( i, mOffsetAwayFromWall ) );
    
    // LIST END POINTS :
    MathVector tmp = m_line.m_origin + m_line.m_vector*-WALL_END_OFFSET + m_line.m_vector.get_perp_xz()*mOffsetAwayFromWall;
    mPoints.push_back(tmp);
    
    tmp = m_line.m_origin + m_line.m_vector * (m_wall_length+WALL_END_OFFSET);
    tmp += m_line.m_vector.get_perp_xz()*mOffsetAwayFromWall;
    mPoints.push_back(tmp);
}

int abWall::find_alternate_via( MathVector mIntersection, MathVector& mNewPoint )
{
    // 
    float distance1 = mIntersection.get_distance( m_line.m_origin );
    MathVector far_end = m_line.m_origin + (m_line.m_vector * m_wall_length);
    float distance2 = mIntersection.get_distance( far_end );
 
    //
    int index = find_closest_door(mIntersection);
    MathVector dc = get_door_center_coord( index );
    float distance3 = mIntersection.get_distance( dc );
    float min_dist  = distance1;
    int min = 1;
    
    //
    if (distance2 < min_dist)
    {    min = 2;   min_dist = distance2; }
    if (distance3 < min_dist)
    {    min = 3;   min_dist = distance2; }
    switch (min)
    {
        case 1: mNewPoint = m_line.m_origin;
                break;
        case 2: mNewPoint = far_end;
                break;
        case 3: mNewPoint = dc;
                break;
    }
    return min;
}

float abWall::get_door_near	 (  int mDoorIndex  )
{
    return m_doors[mDoorIndex].PositionLengthwise;
}

float abWall::get_door_center(  int mDoorIndex  )
{
    float distance_to_door_center = m_doors[mDoorIndex].PositionLengthwise + (m_doors[mDoorIndex].Width/2.0);
    return distance_to_door_center;
}

float abWall::get_door_far	 (  int mDoorIndex  )
{
    float distance_to_far = m_doors[mDoorIndex].PositionLengthwise + m_doors[mDoorIndex].Width;
    return distance_to_far;
}


MathVector abWall::get_door_near_coord( int mDoorIndex, float mPerpDistance )
{
    float  distance = get_door_near(mDoorIndex);
    return get_point_away_from( distance, mPerpDistance );
}

MathVector abWall::get_door_center_coord( int mDoorIndex, float mPerpDistance )
{
    float distance = get_door_center(mDoorIndex);
    return get_point_away_from( distance, mPerpDistance );
}

MathVector abWall::get_door_far_coord(  int mDoorIndex, float mPerpDistance  )
{
    float distance = get_door_far(mDoorIndex);
    return get_point_away_from( distance, mPerpDistance );
}


