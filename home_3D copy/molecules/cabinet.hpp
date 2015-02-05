/* A cabinet is a box with a door and a drawer.
	The door may be open [0..90] degrees.
	The drawer may be slide out up to the 90% of the depth.
	
	A counter top is a collection of cabinets.
*/
//#include "all_objects.h"

#include <vector>
using namespace std;

class glCabinetEndPiece : public glExtrusion
{
public:
	glCabinetEndPiece( float mDepth, float mWidth, float mHeight );
	void	Initialize( );
	virtual void	generate_vertices();

	float 	m_height;
	float	m_depth;
	float	m_Thick;
	
	float 	m_toe_height;
	float	m_toe_inward;		
};

class glCabinet : public glExtrusion
{
public:
	glCabinet(  );

	void	Initialize	( );

	virtual void	generate_vertices();
	void  	generate_cabinet_side_vertices( );	
	void  	generate_cabinet_side_IBO( GLubyte* iptr );

	void	create		( );
	void 	generate_IBO( );
	void 	generate_VBO( );
	void	draw		( );

	glCabinetEndPiece	m_end1;
	glCabinetEndPiece	m_end2;
	
	glContainer 		m_body;
	glDrawer	 		m_drawer;
	glDoor 				m_door;

	float	m_depth;		// Length along 1 side.
	float	m_width;		// Distance across the table.
	float	m_height;		// how high off ground.

	float 	m_toe_height;
	float	m_toe_inward;
	
};


