/* 
    This class will generate OpenGL Vertex data.
	for an object with a given set of points which establish a route thru the house 
	or building.
	
	It shows as a redish/yellow line along the floor.  	
    It draws the object as GL_LINE_STRIP.
	
	"Follow the yellow brick road!" concept.  Though red is a bolder color!
	
	This will be generated by the "maze solving" software module.
	
TO USE:
	a) Generating from the Map2D and Routing class.

ALTERNATIVELY:
	a) Create a class of glRoute. 
	b) Fill in the vertices directly.
	c) Call : create_sample()
*/

#ifndef _ROUTE_H_
#define _ROUTE_H_

#include <vector>
#include "gl_misc.h"
#include "vector.hpp"
//#include "multi_path_route.h"

using namespace std;

 
// See Robot.hpp : m_torso.width!
const float RobotFeetSeparation = 18.0;		// distance between legs at hip.
const float RobotFeetLength 	= 11.0;		// distance from heel to toe.

struct stRiseRun {
	float rise;
	float run;
};

/* 
    We may also like to keep a path.  And update the robot foot prints along the way.
    for instance if it stops (stands) to open door.  Then resumes.
*/
class glRoute : public glAtom
{
public:
	glRoute( );
    void             reset                  ( );
    virtual void	 start_over_at          ( struct Vertex mStart );
    void			 start_over_at2         ( MathVector    mStart );

    void 			 add_way_point          ( struct Vertex mStart );
    void 			 add_way_point2         ( MathVector    mPoint );
    void             add_sample             ( MathVector   mPoint  );

// Instead put this in Multiroute and do an extract which gives a glRoute.
//    virtual void     create_from_multi      ( glMultiRoute& mMulti );
    void             show_direct_path       ( bool mDraw = true );
    
    float 			 get_angle_degrees      ( int mIndex );
	void 			 compute_slopes			( );
	MathVector       compute_delta			( int mIndex );
	struct stRiseRun compute_perpendicular	( int mIndex );
    MathVector       get_perpendicular_pt   ( int mIndex, float mPerpendicular_Distance );

    MathVector       get_directional_vector  ( int mIndex );
    MathVector       get_perpendicular_vector( int mIndex );

    
	//void 			 change_vertices_colors	( );	// makes robot feet in red.
	//virtual void	 generate_vertices		( );

	virtual void	 draw_body			 	( );
	void			 print_step			 	( int mIndex, bool mLeft=true );

	struct Vertex 	 m_start;
	struct Vertex 	 m_end;

    float            m_line_width;
    float            m_sample_spacing_inches;       // when add_way_point() how often should we sample the line?
    
    vector<MathVector>      m_xyz;
	vector<MathVector>      m_slopes;		// rise/run of the path
	glLine                  m_direct_path;
    
	// IBO / VBO:
	int             m_number_path_vertices;
};


#endif
