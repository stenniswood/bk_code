/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
	
	This makes a simple Table: a thin box for the table top.
	and 4 legs (cylinders).
*/
#ifndef _BRICK_WALL_
#define _BRICK_WALL_

#include "txt_container.hpp"
#include <vector>
#include "glMolecule.h"
#include "wall.hpp"

using namespace std;

//  most common cement block nominal size is 16 × 8 × 8 in
const float CEMENT_BLOCK_LENGTH		= 16.0; 
const float CEMENT_BLOCK_THICKNESS	= 8.0; 
const float CEMENT_BLOCK_HEIGHT 	= 8.0; 

const float FACE_BRICK_LENGTH    = 7.625; 		// inch
const float FACE_BRICK_THICKNESS = 3.625;
const float FACE_BRICK_HEIGHT    = 2.125; 

const int   CEMENT_BLOCK_ID  = 1;
const int   FACE_BRICK_ID    = 2;
const float MORTOR_THICKNESS = 0.625;


class glBrickWall : public glMolecule, public glBareWall
{
public:
	glBrickWall					( int mBrickType = CEMENT_BLOCK_ID );
    ~glBrickWall                ( );

    void            select_brick_type( int mType );
    void            set_length_height   ( float mLength, float mHeight );       // this or set_params()
    void            set_params          ( int mNumBricks, int mNumRows );
    
	txtContainer*	create_one_brick	( bool mHalf );
	void            create_one_row		( int mNumberBlocks, bool mOffset, float mHeight );
    virtual void	create_components   ( );

	float			get_brick_length 	( );
	float			get_brick_height 	( );
    float			get_brick_depth     ( );    

    float 	  		m_brick_length;
    float 	  		m_brick_height;
    float 	  		m_brick_depth;

	int		m_number_of_rows;		// height
	float	m_wall_length;			// in inches
	int 	m_brick_type;			// bricks or cylinder blocks (for size & texture purposes)	
	
    Texture* m_one_brick_texture;
	vector<txtContainer*>	m_bricks;
};


#endif
