/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple counter top.  ie. multiple cabinets
	
*/

#include <vector>
using namespace std;


class glCounter
{
public:
	glCounter(  );

	void	Initialize	( );
	void	Relocate	( float mX, float mY, float mZ );
	
	void 	generate_IBO( );
	void 	generate_VBO( );		
	void	draw(); 
	
	vector<glCabinet> 	m_cabinets;
	
	float	m_counter_length;		// Length along 1 side.
	float	m_number_cabinets;		// Distance across the table.
	
	GLuint	m_VBO;
	GLuint	m_IBO;

	float	m_x;
	float	m_y;
	float	m_z;	
};


