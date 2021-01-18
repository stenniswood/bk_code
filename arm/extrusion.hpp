#ifndef _EXTRUSION_H_
#define _EXTRUSION_H_


#include "gl_atom.hpp"

using namespace std;

/* 
    This class will generate OpenGL Vertex data    
	for an object with a given set of points (POLYGON)
	It extrudes it a specified distance to a parallel 
	set of points.
	
	It draws the object as 2 polygons (top & bottom)
	and a produces the connecting sides as GL_QUAD_STRIP pairs.
	
	This can be used for a cylinder, stairway, floorplan, etc.
	
TO USE:
	a) Derive a class from it.
	b) Override the generate_vertices() function.
	c) call create().
	
ALTERNATIVELY:
	a) Create a class of glExtrusion. 
	b) Fill in the vertices directly.
	c) Call : 
*/

class glExtrusion : public glAtom
{
public:
	glExtrusion( );
    void                set_la(float mLength, int mAxis);
    virtual void        setup ();

    virtual void        generate_vertices		( );
	void                extrude_vertices		( float mExtrusionLength, int mLoftAxis=-1 );
	void                change_vertices_colors	( );

    virtual void        generate_indices		( 		);
    virtual size_t      generate_disc_indices	( GLuint mStartingVertexIndex );
	virtual void        generate_side_indices	( 	    );
	void                generate_otherside_indices();

	virtual void        draw_body			 	( );

	float				m_extrusion_length;
	bool				m_is_closed;
	unsigned long int 	m_ends_color;

	// IBO:
	size_t		m_layer_one_indices;
	size_t		m_number_side_indices;		// 2x m_disc_indices
	// VBO:
	size_t   	m_layer_one_vertices;		// 2x m_layer_one_vertices
	int         m_extrusion_axis;
};


#endif
