/* A cabinet is a box with a door and a drawer.
	The door may be open [0..90] degrees.
	The drawer may be slide out up to the 90% of the depth.
	
	A counter top is a collection of cabinets.
*/
#ifndef _CABINET_HPP_
#define _CABINET_HPP_

#include "all_objects.h"
#include <vector>
using namespace std;


class glCabinetEndPiece : public glExtrusion
{
public:
	glCabinetEndPiece( float mDepth, float mWidth, float mHeight );
	virtual void	generate_vertices( );
	virtual size_t 	generate_disc_indices( GLuint mStartingVertexIndex );
    
	float 	m_height;
	float	m_depth;
	float	m_Thick;
	
	float 	m_toe_height;
	float	m_toe_inward;		
};


class glCabinet : public glMolecule
{
public:
	glCabinet(  );

	void	Initialize	( );
    virtual void 	setup  		( );
	void 	colorize	( );
	void	create		( );
    
	void	open_drawer ( float mFractionOpen ) {
		m_drawer.open(mFractionOpen);
	};
	void	open_door   ( float mFractionOpen ) {
		m_door.open (mFractionOpen);
	};
	
	
    glCabinetEndPiece	m_end1;
	glCabinetEndPiece	m_end2;
	glBox               m_separator;
	glBox               m_drawer_top_bar;
	glBox               m_below_door;
	glBox               m_shelf;
	
	glBox               m_body;
	glDrawer	 		m_drawer;
	glDoor 				m_door;

	float	m_depth;		// Length along 1 side
	float	m_width;		// Distance across the cabinet
	float	m_height;		// how high off ground
	
	float	m_y_angel;
};


#endif
