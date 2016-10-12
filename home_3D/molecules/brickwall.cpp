#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "all_objects.h"
#include "brickwall.hpp"

#define Debug 0


glBrickWall::glBrickWall( int mBrickType )
{
    //glBareWall.m_object_type_name = "brick wall";
    m_brick_type = mBrickType;
}

glBrickWall::~glBrickWall( )
{
    delete m_one_brick_texture;
    for (int i=0; i<m_bricks.size(); i++)
        delete m_bricks[i];
}


void glBrickWall::select_brick_type( int mType )
{
    switch( mType)
    {
    case CEMENT_BLOCK_ID:                   // cynder blocks
            m_brick_length = CEMENT_BLOCK_LENGTH;
            m_brick_height = CEMENT_BLOCK_HEIGHT;
            m_brick_depth  = CEMENT_BLOCK_THICKNESS;
        break;
    case FACE_BRICK_ID :
            m_brick_length = FACE_BRICK_LENGTH;
            m_brick_height = FACE_BRICK_HEIGHT;
            m_brick_depth  = FACE_BRICK_THICKNESS;
        break;
    default:
        break;
    }
    
}

void glBrickWall::set_length_height   ( float mLength, float mHeight )       // this or set_params()
{
    m_wall_length    = mLength;
    m_number_of_rows = mHeight / m_brick_height;
}

void glBrickWall::set_params          ( int mNumBricks, int mNumRows )
{
    m_wall_length    = mNumBricks * m_brick_length;
    m_number_of_rows = mNumRows;
}

txtContainer* glBrickWall::create_one_brick( bool mHalf )
{
    txtContainer*  mb   = new txtContainer();
    
    mb->width  = m_brick_length;
    mb->height = m_brick_height;
    mb->depth  = m_brick_depth;
    if (mHalf)
		mb->width /= 2.;		// just the length of it changes!

	mb->set_color( 0xFF7FFF7F );
	mb->setup();
    
    mb->m_texture = m_one_brick_texture;
    mb->grab_bottom();
	mb->grab_left();
    return mb;
}

void glBrickWall::create_one_row( int mNumberBlocks, bool mOffset, float mHeight)
{
	// The wall will go from offset to mLength:
    txtContainer* ptr=NULL;
	float PositionX = 0;
	if (mOffset)
		if (Debug) printf("Create_one_row() starts with half!\n" );	
	//printf("half brick:w=%6.2f; full:w=%6.2f\n", halfB.width, tmpB.width );
    //if (i==0) printf("Starting Brickwall with FULL brick.  w=%6.2f\n", tmpB.width);

	for (int i=0; i<mNumberBlocks; i++)
	{
		if ((i==0) && (mOffset))
		{			
            ptr = create_one_brick(true);
		} else
            ptr = create_one_brick(false);
		
        ptr->relocate( PositionX, mHeight, 0.0 );
        m_bricks.push_back    ( ptr );
        m_components.push_back( ptr );
        PositionX += ptr->width + MORTOR_THICKNESS;
    }
	//if (Debug) printf("Create_one_row() bricks=%lu\n", m_bricks.size() );
}
/*
Return:	 the actual length
         float mLength, int  mRows
*/
void glBrickWall::create_components()
{
    m_one_brick_texture = new Texture();
    m_one_brick_texture->load_image("textures/cinder-block-texture.jpg");
    
	float h = m_brick_height+MORTOR_THICKNESS;
	float l = m_brick_length;

	float number_needed = m_wall_length / l;
	//if (Debug) printf("Create_one_row() Length=%6.1f / width=%6.1f  ; number bricks needed =%5.2f\n", mLength, l, number_needed );

	bool offset = false;
	for (int r=0; r<m_number_of_rows; r++)
	{
		create_one_row( number_needed, offset, r*h );
		offset = !offset;
	}
}


