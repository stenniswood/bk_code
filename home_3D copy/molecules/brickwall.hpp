/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/
#include "txt_container.hpp"
#include <vector>
using namespace std;

//  most common cement block nominal size is 16 × 8 × 8 in
const float CEMENT_BLOCK_LENGTH		= 16.0; 
const float CEMENT_BLOCK_THICKNESS	= 8.0; 
const float CEMENT_BLOCK_HEIGHT 	= 8.0; 

const float FACE_BRICK_LENGTH    = 7.625; 		// inch
const float FACE_BRICK_THICKNESS = 3.625;
const float FACE_BRICK_HEIGHT    = 2.125; 

const int CEMENT_BLOCK_ID  = 1;
const int FACE_BRICK_ID = 2;

class glBrickWall
{
public:
	glBrickWall( int mBrickType = CEMENT_BLOCK_ID );

	void	Initialize	( );
	void	Relocate	( float mX, float mY, float mZ );

	glContainer* create_one_brick( bool mHalf );
	void	create_one_row( int mNumberBlocks, bool mOffset, float mHeight );
	float	create		  ( float mLength, int  mRows);

	float	get_brick_length ( );
	float	get_brick_height ( );
	void	generate_vertices( );
	void	draw(); 

	int		m_number_of_rows;		// height
	float	m_wall_length;			// in inches
	int 	m_brick_type;			// bricks or cylinder blocks (for size & texture purposes)	
	
	vector<glContainer>	m_bricks;	

	float	m_x;
	float	m_y;
	float	m_z;	
};


