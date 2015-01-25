/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/
#include "txt_container.hpp"
#include <vector>
using namespace std;


class glTable
{
public:
	glTable(  );

	void	Initialize	( );
	void	Relocate	( float mX, float mY, float mZ );

	void	generate_vertices();
	void 	generate_IBO( );
	void 	generate_VBO( );		
	void	draw(); 

	txtContainer 		m_table_top;
	vector<glCylinder>	m_table_legs;	
	
	float	m_table_length;		// Length along 1 side.
	float	m_table_width;		// Distance across the table.
	float	m_table_height;		// how high off ground.
	
	GLuint	m_VBO;
	GLuint	m_IBO;

	float	m_x;
	float	m_y;
	float	m_z;	
};


