/* A cabinet is a box with a door and a drawer.
	The door may be open [0..90] degrees.
	The drawer may be slide out up to the 90% of the depth.
	
	A counter top is a collection of cabinets.
*/
#include "extrusion.hpp"

#include <vector>
using namespace std;

class glCabinet : public glExtrusion
{
public:
	glCabinet(  );

	void	Initialize	( );
	void	Relocate	( float mX, float mY, float mZ );

	virtual void	generate_vertices();
	void  	generate_cabinet_side_vertices( );	
	void  	generate_cabinet_side_IBO( GLubyte* iptr );

	void 	generate_IBO( );
	void 	generate_VBO( );		
	void	draw(); 

	glContainer 		m_body;
	glContainer 		m_drawer;	
	glContainer 		m_door;	
	float				m_door_angle;  // 0.0=> closed;  90.0==> full open;
	
	float	m_depth;		// Length along 1 side.
	float	m_width;		// Distance across the table.
	float	m_height;		// how high off ground.
	
	float 	m_toe_height;
	float	m_toe_inward;
	
};


