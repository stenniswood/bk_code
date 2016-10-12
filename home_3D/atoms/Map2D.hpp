/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/
#ifndef _MAP2D_H_
#define _MAP2D_H_

#include <vector>
using namespace std;
#include "gl_atom.hpp"
#include "wall.hpp"
#include "multi_path_route.h"
#include "route.hpp"
#include "full_wall.h"

const int FLOOR_HEIGHT = 2.0;
extern vector<MathVector> FinalPath;

class glMap2D : public glAtom
{
public:
	glMap2D (  );
	~glMap2D(  );
    
    // map_route is top level charting function.
    void    map_route           ( glMultiRoute&   mRoute, MathVector mSource, MathVector mDestination );
    bool    keep_adjusting      ( MathVectorTree& mTree,  MathVector mSource, MathVector mDestination ); // recursive
    int     find_closest_intersected_wall( MathVector mSource, MathVector mDestination );
    void    prune_alternates    ( MathVector mDestination );
    void    add_alternates      ( MathVectorTree& mTree   );
    bool    scan_leaves         ( MathVector mDestination );
    void    print_alternates    (                         );

    void    add_before_door     ( MathVectorTree& mTree, int mWallIndex, list<float>& mDistancesAlong, float mSide );
    void    add_after_door      ( MathVectorTree& mTree, int mWallIndex, list<float>& mDistancesAlong, float mSide );
    
    // bool sugest_alternate_route( int mWallIndex, MathVector mIntersection, MathVector& mNewPoint );
    // adjusts safe distance from walls, etc.
	void	refit_route         ( glRoute mMap, bool mLeftFootFirst = false );
	void	compute_corners     ( );

	// FOR 2D Drawing        (and maze solving) :
	void	create_2D_drawing   ( );
	void 	create_2D_mark      ( float mX, float mY, long int mColor );

	void	print_2D_info       ( );		//
	void	draw_corners        ( );		// corners

	virtual void	draw_body   ( );	// Override this with open gl commands.

    vector<glFullWall*>   m_fwalls;
};


#endif

