/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/
#ifndef _PALETTE_
#define _PALETTE_
#include "txt_container.hpp"

#include <vector>
using namespace std;

// The brick dimensions are currently defined in Brickwall
// and will later be defined in the brick class.  
// A derivative of txt_container with texture for all sides of the cube.


// Typical palette weight - 28 to 56 lbs depending on gravel mix (heavy or light) 
// and size of the openings.
const float PALETTE_WEIGHT_LIGHT     = 28.0;
const float PALETTE_WEIGHT_HEAVEY    = 56.0;
const int   PALETTE_NUMBER_OF_BLOCKS = 112;
// 4x7 = 24  x  4 = 112



/*
 Simple palette.  All bricks lined up the same way. 
*/
class Palette : public glMolecule
{
public:
	Palette( int mBrickType = CEMENT_BLOCK_ID );

	void	Initialize			 ( );
	
	glBox* create_one_brick( bool mHalf 					);
	void	create_one_layer	 ( int mCols, float mRows,   float mHeight );
	void	create		 		 ( int mCols=7, int  mRows=4, int layers=4 );

	float	get_brick_length ( );
	float	get_brick_height ( );
	//void	generate_vertices( );
	//void	draw();

	int		m_number_of_rows;		// height
	float	m_length;				// in inches
	float	m_width;				// in inches	
	int 	m_brick_type;			// bricks or cylinder blocks (for size & texture purposes)	
	
	vector<glBox>	m_bricks;

};


#endif
