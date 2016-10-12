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


struct Location
{
    float x;
    float y;
};



/*
	Start by drawing the floor plan in 2D 	Done. Stored in room2.hpp "m_2D"
*/
struct Location Start;
struct Location End;

// Extracted info for each wall:
struct line_info
{
	float rise;
	float run;
	float x;		// Wall.m_x
	float y;		// Wall.m_y
	float angle;	// degrees
};
vector<struct line_info>  lines;


struct Location* Lines_intersect_at( line_info li, Location Start, float mSearchX, float mSearchY  )
{
	float K1  = li.x    *li.run - li.y    *li.rise;
	float K2  = mSearchX*li.run - mSearchY*li.rise;
	float K3  = Start.x *li.run - Start.y *li.rise;
	
	if (K2==0) return NULL;		// no solution!
	float t = (K1-K3) / K2;
	
	static struct Location result;
	result.x = t * mSearchX + Start.x;
	result.y = t * mSearchY + Start.y;
	printf( "Lines Intersect at  (%6.2f,%6.2f)\n", result.x, result.y );
	return &result;
}

// Good enough for comparisons.  Not true geometric distance!
float get_distance_measure( struct Location m1, struct Location m2)
{
	float dx = fabs(m1.x - m2.x);
	float dy = fabs(m1.y - m2.y);
	return dx + dy;
}


/* Given a point and a search direction vector, find the first line crossing 
Return:		Index of the nearest line.
*/
int search_all_lines( float mSearchAngle, struct Location mStart, float mSearchX, float mSearchY )
{
	struct Location* result;
	struct Location r1;
	float  nearest_distance = 65535.0;
	int  nearest_line 		= -1;
	
	for (int w=0; w<lines.size(); w++)
	{
		if ((lines[w].angle != mSearchAngle) && 
		    (lines[w].angle != -mSearchAngle))	// Not parallel!
		{
			result = Lines_intersect_at( lines[w], mStart,  mSearchX, mSearchY );
			if (result)
			{
				float distance = get_distance_measure( mStart, *result );
				if (distance < nearest_distance)
				{
					// Even if the lines intersect, we need to check if the line segments intersect
					// if result is in the segment.
					//if (hits_wall(*result, w))
					{
						r1 = *result;
						nearest_distance = distance;
						nearest_line = w;
					}
				}
			}	
		}				
	}
	return nearest_line;
}


