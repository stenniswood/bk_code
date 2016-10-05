/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/
#ifndef _AB_WALL_H_
#define _AB_WALL_H_

#include "vector.hpp"
#include <vector>
#include <list>

using namespace std;

#define CONVEX  1
#define CONCAVE 2

struct stDoorWay {
    int	  FirstVertexIndex;		// Stores the index into m_vertices
    float PositionLengthwise;	//
    float Width;				//
    float Height;				//
};

struct stWindow {
    int	  FirstVertexIndex;		// Stores the index into m_vertices
    float PositionLengthwise;
    float Width;
    float Height;
    float SillHeight;
};


struct stCorner {
    float x;
    float z;
    //float Width;
    //float Height;
    int	  type;
};

/* Abstract wall for computing a route.  Does not contain opengl data. 

*/
class abWall
{
public:
	abWall( );
	abWall( float mLength );
    void    Initialize        ( );

	void	clear			  ( );
	bool	is_valid_location ( float mPositionLengthwise, float mWidth = 36.0 );
	bool	add_door	  	  ( float mPositionLengthwise, float mWidth = 36.0, float mHeight=96.0 );
	bool	add_window	  	  ( float mPositionLengthwise, struct stSize mSize, float mHeight=36.0 );

    
    int     find_closest_door ( MathVector mIntersection );
    float   get_distance_along( MathVector mIntersection );
    float   is_on_which_side  ( MathVector mPoint        );    // returns -1.0 or +1.0

    void    find_paths_around ( list<float>& mDistanceAlong );
    void    find_paths_around ( list<MathVector>& mPoints, float mOffsetAwayFromWall );
    int     find_alternate_via( MathVector mIntersection, MathVector& mNewPoint );
    bool	intersects        ( LineSegment mPath, float& t, float& s );
    
    float	get_door_near	  (  int mDoorIndex  );
    float	get_door_center   (  int mDoorIndex  );
    float	get_door_far	  (  int mDoorIndex  );
    
    MathVector  get_point_away_from( float mDistanceAlong, float mPerpendicularDistance );
	MathVector	get_door_near_coord	 (  int mDoorIndex, float mPerpDistance=0.0  );
	MathVector	get_door_center_coord(  int mDoorIndex, float mPerpDistance=0.0  );
    MathVector	get_door_far_coord	 (  int mDoorIndex, float mPerpDistance=0.0  );
    MathVector	get_far_end          (  );
    float       get_visible_height   (  )
                { if (m_show_half_height) return m_wall_height/2.0; else return m_wall_height; };

	void 	extract_corners ( vector<abWall>*   mWalls );
	void 	print_info      ( );
	bool 	print_dw_info   ( );
	void	print_corners   ( );

	// INDICES:	
	void 	generate_marks  ( );
	//void 	sort            ( );
	void	draw            ( );
	void	print_marks     ( );

	float	m_wall_length;          // in inches
	float	m_wall_height;          // in inches
	float	m_angle;                // 0.0 is vertical (looking at the map)

    Line    m_line;                 // 2 vectors origin and direction.
	
    bool    m_show_half_height;     // for visibility into other rooms.
	vector<struct stDoorWay>	m_doors;
	vector<struct stWindow >	m_windows;
	vector<struct stCorner>		m_corners;
	
};


#endif


