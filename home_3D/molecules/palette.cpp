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


/*const float DEFAULT_PALETTE_LENGTH = 0;
const float DEFAULT_PALETTE_WIDTH  = 0;
const float DEFAULT_PALETTE_ROWS   = 0;*/


Palette::Palette( int mBrickType )
{ 
	m_brick_type = mBrickType;
	
	Initialize();
}
void	Palette::Initialize			 ( )
{
      m_object_type_name = "palette";
  //  m_object_class  = 17;
}

glBox* Palette::create_one_brick( bool mHalf 	)
{ 
	static glBox	b;
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
    m_components.push_back(&b);
	return &b;
}

void Palette::create_one_layer( int mCols, float mRows, float mHeight )
{ 
	glBox *b = create_one_brick(false);		// make a brick later.
	float x,y;
	int count=0;
	for (int l=0; l<mRows; l++)
		for (int w=0; w<mCols; w++)
		{
			x = l*b->width*1.05;
			y = w*b->depth*1.05;
			b->grab_bottom();
			b->grab_left  ();
			b->relocate( x, mHeight, y );			
			m_bricks.push_back(*b);
			count++;
		}
	printf("Palette:Create_one_layer()  made %d bricks\n", count);
}

void	Palette::create( int mCols, int  mRows, int mLayers )
{ 
	glBox* b = create_one_brick( false );
	m_length = b->depth * mCols; 
	m_width  = b->width * mRows;
	
	for (int h=0; h<mLayers; h++)
		create_one_layer( mCols, mRows, h*b->height );
    //gl_register();
    
}

float	Palette::get_brick_length ( )
{ 
	return m_bricks[0].depth;
}
float	Palette::get_brick_height ( )
{ 
	return m_bricks[0].height;
}

/*void	Palette::draw()
{ 
	glTranslatef(m_x,    m_y,  m_z);
	for (int i=0; i<m_bricks.size(); i++)
		m_bricks[i].draw();
	glTranslatef( -m_x, -m_y, -m_z);
} */

