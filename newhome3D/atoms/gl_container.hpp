/* This class will generate OpenGL Vertex data for

	a   Box   of given width, height, depth.
*/
#ifndef _GL_BOX_
#define _GL_BOX_

#include "gl_atom.hpp"
#include "glSphere.h"
#include "glMolecule.h"


extern GLubyte q_Indices[];

#define  BUFFER_OFFSET(i) ((GLuint*)NULL + (i))

class glBox : public glAtom
{
public:
	glBox();

	virtual void 	setup  		( );
	virtual void 	create 		( );
	void            generate_vertices	( );

    int             get_largest_axis    ();
	void            change_top_color   	( long mColor );
	void            change_bottom_color	( long mColor );
    void            compute_inertia     ( float mTotalMass );
    
    // two Boxes in close proximity.  Join the sides, so that they operate as a unit.
    // ie creates a parent molecule of which both this and Neighbor are components.
    glMolecule*     join_closest_sides( glBox* mNeighbor );
    
	virtual void	draw_body   ( );
	void            print_info  ( );

    bool            evaluate_collision( glSphere* mOther );
    
	float 	width;          // X axis
	float 	height;         // Y axis
	float 	depth;          // Z axis
	bool	m_is_closed;
};



// DO THIS LATER AFTER vertices are generated.
const int TYPE_CLOSED 		= 1;
const int TYPE_OPEN   		= 1;
const int TYPE_WIRE_FRAME 	= 1;


#endif
