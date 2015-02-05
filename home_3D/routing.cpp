
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


void extract_line_info()
{
	struct line_info li;
	
	li.rise = sin( -radians(Wall.m_angle) ) * Wall.m_wall_length;
	li.run  = cos( -radians(Wall.m_angle) ) * Wall.m_wall_length;
	
}

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

int hits_wall(  )
{
	
	return ;
	
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

/* 
  This will return either the left or right corner, end of wall, or closest door.

*/
void find_accessible_doors(struct Location mLatest)
{
	for (int d=0; d<Wall.m_doors.size(); d++)
	{
		// Create a point in center of door.
		// If the line segment from there to our current location crosses any other walls, then not accessible.
		// if not then add to decision tree leafs.
		Location door_center;
		door_center = 
	
	}
}

/*		result = Lines_intersect_at( w, 0., -1.);
		if (result)		r2 = *result;		
		result = Lines_intersect_at( w, 1., 0);
		if (result)		r3 = *result;		
		result = Lines_intersect_at( w,-1., 0);
		if (result)		r4 = *result;  */ 

void find_next_way_point(struct Location mLatest)
{
	// SCAN 4 DIRECTIONS:
	// 		Find intersection of 
	switch
	search_all_lines( float mSearchAngle, struct Location mStart, float mSearchX, float mSearchY )
	
	// SCAN 4 DIAGONALS:
	
	
}

	// Since our search is parameterized in simple vectors 
	// ( 1,0) or ( 0, 1) 
	// (-1,0) or ( 0,-1)      
	
	// ( 1,1) or (-1,-1)
	// (-1,1) or ( 1,-1)
