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
#include "depth_map.hpp"

int i_width  = 50;
int i_height = 50;
uint16_t depth[50*50];


uint16_t& get_depth( int x, int y )
{
	return depth[x+y*i_width];
}

glDepthMap::glDepthMap ()
{
	set_demo();
}
glDepthMap::~glDepthMap()
{
	
}

void glDepthMap::set_demo( )
{
	// Setup experimental image :
	for (int x=0; x<i_width; x++)
		for (int y=0; y<i_height; y++)
		{
			get_depth( x,y ) = 10 +2*x + 2*y;
		}

}

void glDepthMap::set_image( char* mImage, int width, int height )
{

}

void glDepthMap::load_image( Mat* mLevel )
{

}

uint16_t glDepthMap::compute_depth(int x, int y)
{
	return get_depth(x,y);
}

void glDepthMap::generate_vertices( )
{
	struct Vertex_pnc v;
	for (int x=0; x<width; x++)
		for (int y=0; y<height; y++)
		{
			float depth = compute_depth( x,y );
			v.position[0] = x;
			v.position[1] = y;
			v.position[2] = depth;
			m_vertices.push_back( v );						
		}
}

void glDepthMap::draw_body()
{
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer(GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
	glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
        /* color pointer should be GL_UNSIGNED_LONG shouldn't it?! */    
    
	//Establish array contains vertices (not normals, colours, texture coords etc)
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY );

	// We draw the top and bottoms as GL_POLYGON and the sides as GL_QUAD_STRIP.
	// Both sets of indices stored in GL_INDEX_ARRAY.  With a known offset for each 
	// of the 3 sets.

	// Draw Bottom : 
	glDrawElements(GL_POLYGON, (int)m_layer_one_indices, GL_UNSIGNED_INT, 0 );
 
	// Draw the sides: m_number_side_indices
	glDrawElements(GL_QUAD_STRIP, m_layer_one_vertices*2., GL_UNSIGNED_INT,
				   (GLvoid*)BUFFER_OFFSET( m_layer_one_indices ) );
}
