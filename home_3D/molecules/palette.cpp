#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include "all_objects.h"


const float DEFAULT_PALETTE_LENGTH = 0;
const float DEFAULT_PALETTE_WIDTH  = 0;
const float DEFAULT_PALETTE_ROWS   = 0;


Palette::Palette( int mBrickType )
{ 
	m_brick_type = mBrickType;
	
	Initialize();
}
void	Palette::Initialize			 ( )
{ 
}
void Palette::Relocate( float mX, float mY, float mZ )
{
	m_x = mX;
	m_y = mY;
	m_z = mZ;
	
}

glContainer* Palette::create_one_brick( bool mHalf 	)
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
	b.m_color = 0xFF00FF00;
	b.create();
	return &b;
}

void Palette::create_one_layer( int mCols, float mRows, float mHeight )
{ 
	glContainer *b = create_one_brick(false);		// make a brick later.
	float x,y;
	int count=0;
	for (int l=0; l<mRows; l++)
		for (int w=0; w<mCols; w++)
		{
			x = l*b->width*1.05;
			y = w*b->depth*1.05;
			b->grab_bottom();
			b->grab_left  ();
			b->Relocate( x, mHeight, y );			
			m_bricks.push_back(*b);
			count++;
		}
	printf("Palette:Create_one_layer()  made %d bricks\n", count);
}

void	Palette::create( int mCols, int  mRows, int mLayers )
{ 
	glContainer* b = create_one_brick( false );
	m_length = b->depth * mCols; 
	m_width  = b->width * mRows;
	
	for (int h=0; h<mLayers; h++)
		create_one_layer( mCols, mRows, h*b->height );	
}

float	Palette::get_brick_length ( )
{ 
	return m_bricks[0].depth;
}
float	Palette::get_brick_height ( )
{ 
	return m_bricks[0].height;
}

void	Palette::generate_vertices( )
{ 

}
void	Palette::draw()
{ 
	glTranslatef(m_x,    m_y,  m_z);
	for (int i=0; i<m_bricks.size(); i++)
		m_bricks[i].draw();
	glTranslatef( -m_x, -m_y, -m_z);
} 
