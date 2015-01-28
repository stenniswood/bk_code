#ifndef _EXTRUSION_H_
#define _EXTRUSION_H_

#include <vector>
using namespace std;

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

class glExtrusion 
{
public:
	glExtrusion( );
	void			create(float mLength = -1);
	void			Relocate( float mX, float mY, float mZ );

	virtual void	generate_layer_vertices	( );
	void			extrude_vertices		( float mExtrusionLength, int mLoftAxis );
	void 			generate_vertices_colors( );
	virtual void	generate_vertices		( );
	void 			change_color			( long mColor );

	virtual GLbyte 	generate_disc_indices( GLbyte mStartingVertexIndex );
	virtual void 	generate_side_indices( 	    );
	virtual void 	generate_indices	 ( 		);
	void 			generate_otherside_indices();
	
	void 	generate_IBO			( );
	void 	generate_VBO			( );
	void	draw					( );

	void	print_indices			( );
	void	print_vertices			( bool mShowColors = true );

	float	m_x;
	float	m_y;
	float	m_z;

	float	m_extrusion_length;
	bool	m_is_closed;

	unsigned long int m_color;
	unsigned long int m_ends_color;

	// IBO:
	int		m_layer_one_indices;
	int		m_number_side_indices;		// 2x m_disc_indices
	vector<GLubyte>	m_indices;

	// VBO:
	int   	m_layer_one_vertices;		// 2x m_layer_one_vertices
	vector<struct Vertex>	m_vertices;

	GLuint	m_VBO;
	GLuint	m_IBO;
};


#endif
