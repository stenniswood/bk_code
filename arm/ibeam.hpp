/* 
    This class will generate OpenGL Vertex data
    
	for an object with a given set of points (POLYGON)
	It extrudes it a specified distance to a parallel 
	set of points.
	
	It draws the object as 2 polygons (top & bottom)
	and a produces the connecting sides as GL_QUAD_STRIP pairs.
	
	This can be used for a cylinder, stairway, floorplan,
	etc.
	
	The way to use this class is to derive one from it,
	override the generate_vertices() function, and specify the 2D
	shape.
*/
#include "extrusion.hpp"

#ifndef _IBEAM_H_
#define _IBEAM_H_


class glIbeam : public glExtrusion 
{
public:
	glIbeam( float mExtrusionLength );

	virtual void	generate_vertices( );
	virtual size_t 	generate_disc_indices  ( GLuint mStartingVertexIndex );
	virtual void	draw_body			   ( );
	
};


#endif
