#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"

#include "brickwall.hpp"



#define Debug 0


glBrickWall::glBrickWall( int mBrickType )
{ 
	m_brick_type = mBrickType;
	Initialize();
}
void	glBrickWall::Initialize	( )
{ 
}

void glBrickWall::Relocate( float mX, float mY, float mZ )
{
	m_x = mX;
	m_y = mY;
	m_z = mZ;
}

glContainer* glBrickWall::create_one_brick( bool mHalf )
{
	static glContainer	b;
	if (m_brick_type==CEMENT_BLOCK_ID)
	{
		b.width  = CEMENT_BLOCK_LENGTH;
		b.depth  = CEMENT_BLOCK_THICKNESS;
		b.height = CEMENT_BLOCK_HEIGHT;
	}	
	else if (m_brick_type==FACE_BRICK_ID)
	{
		b.width  = FACE_BRICK_LENGTH;
		b.depth  = FACE_BRICK_THICKNESS;
		b.height = FACE_BRICK_HEIGHT;	
	}
	if (mHalf) {
		b.width /= 2.;		// just the length of it changes!
	}
	b.m_color = 0xFF7FFF7F;
	b.create();
	return &b;
}

float	glBrickWall::get_brick_length( )
{
	if (m_brick_type==CEMENT_BLOCK_ID)
		return CEMENT_BLOCK_LENGTH;
	else if (m_brick_type==FACE_BRICK_ID)
		return FACE_BRICK_LENGTH;
	return 0;
}
float	glBrickWall::get_brick_height( )
{
	if (m_brick_type==CEMENT_BLOCK_ID)
		return CEMENT_BLOCK_HEIGHT;
	else if (m_brick_type==FACE_BRICK_ID)
		return FACE_BRICK_HEIGHT;
	return 0;		
}

const float MORTOR_THICKNESS = 0.625;

void	glBrickWall::create_one_row( int mNumberBlocks, bool mOffset, float mHeight)
{
	// The wall will go from offset to mLength:
	glContainer* tmpB;
	float PositionX = 0;
	if (mOffset)
		if (Debug) printf("Create_one_row() starts with half!\n" );	
	for (int i=0; i<mNumberBlocks; i++)
	{
		if ((mOffset) && (i==0))
			tmpB = create_one_brick(true);
		else 		
			tmpB = create_one_brick(false);
		tmpB->grab_bottom();
		tmpB->grab_left();
		tmpB->Relocate( PositionX, mHeight, 0.0 );
		m_bricks.push_back( *tmpB );
		PositionX += tmpB->width + MORTOR_THICKNESS;
	}	
	if (Debug) printf("Create_one_row() bricks=%lu\n", m_bricks.size() );	
}
/*
Return:	 the actual length
*/
float	glBrickWall::create( float mLength, int  mRows)
{ 
	float h = get_brick_height()+MORTOR_THICKNESS;
	float l = get_brick_length()/2.;

	glContainer* b = create_one_brick( false );
	float number_needed = mLength / b->width;
	
	if (Debug) printf("Create_one_row() Length=%6.1f / width=%6.1f  ; number bricks needed =%5.2f\n", mLength, b->width, number_needed );

	bool offset = false;
	for (int r=0; r<mRows; r++)
	{
		create_one_row( number_needed, offset, r*h );
		offset = !offset;
	}
	return number_needed*b->width;
}
void	glBrickWall::draw()
{
	for (int b=0; b<m_bricks.size(); b++)
		m_bricks[b].draw();
} 


