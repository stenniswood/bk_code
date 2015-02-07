/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/
#ifndef _AB_WALL_H_
#define _AB_WALL_H_

#include <vector>
using namespace std;
class glDoor;


/*struct stDoorWay {
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
struct stSize {
	float width;
	float height;
};*/

#define CONVEX  1
#define CONCAVE 2

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

	void	clear			 (	);
	bool	is_valid_location( float mPositionLengthwise, float mWidth = 36.0 );
	bool	add_door	  	 ( float mPositionLengthwise, float mWidth = 36.0, float mHeight=96.0 );
	bool	add_window	  	 ( float mPositionLengthwise, struct stSize mSize, float mHeight=36.0 );

	struct Vertex	get_door_coord		 (  int mDoorIndex  );	
	struct Vertex	get_door_center_coord(  int mDoorIndex  );	 
	struct Vertex	get_door_far_coord	 (  int mDoorIndex  );	 

	void 	extract_corners(vector<abWall>*   mWalls );
	void 	print_info   ( );
	void 	print_dw_info( );
	void	print_corners( );
	
	// INDICES:	
	void 	generate_marks			( );
	void 	sort();

	void	draw(); 
	void	print_marks();	

	float	m_wall_length;		// in inches
	float	m_wall_height;		// in inches	
	float	m_angle;			// 0.0 is vertical (looking at the map)
	float	m_x;
	float	m_z;
	
	vector<struct stDoorWay>	m_doors;
	vector<struct stWindow >	m_windows;
	vector<struct stCorner>		m_corners;
	
};


#endif


