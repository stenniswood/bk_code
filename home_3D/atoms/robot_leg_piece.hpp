/* This class will generate OpenGL Vertex data
	for a Robot leg piece.  Indented on the one end and forked on the other.
	This can be used for both upper and lower legs (at least as far as simulation 
	is concerned!)

	It actually consists of 2 extrusion pieces.
	The thin part for the outside and fork.
	and the thick inner part.
*/

#include <vector>
using namespace std;

/*class glOuter : public glExtrusion
{
public:
	glOuter();
	virtual GLbyte 	generate_disc_indices( GLbyte mStartingVertexIndex );
};*/

class glInner : public glExtrusion
{
public:
	glInner();
			void	generate_shank_vertices( );		// 2D
			void	generate_round_vertices( );		// 2D
	virtual void	generate_layer_vertices( );		// 2D
	virtual GLbyte 	generate_disc_indices  ( GLbyte mStartingVertexIndex=0 );
	virtual void	generate_vertices		( );
	
	int		m_vertex_count;
	float	m_leg_length;
	// rectangle and 1/2 circle and small inner circle (for shank)	
};


class glLegSegment
{
public:
	glLegSegment(  );

	void	Initialize	( );
	void	relocate	( float mX, float mY, float mZ );

	void	generate_vertices();
	void	generate_indices ();	
	void 	generate_IBO( );
	void 	generate_VBO( );		
	void	draw(); 

	glContainer 		m_table_top;
	vector<glCylinder>	m_table_legs;	
	
	float	m_thick;			// Length along 1 side.

	GLuint	m_VBO;
	GLuint	m_IBO;

	float	m_x;
	float	m_y;
	float	m_z;
	
	glInner m_outside;
	glInner m_center;	
	glInner m_inside;	
};


