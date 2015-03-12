/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/
#ifndef _ROOM_H_
#define _ROOM_H_

#include <vector>
using namespace std;
#include "wall.hpp"

//#include "Map2D.hpp"

//class glMap2D;

class glRoom 
{
public:
	glRoom	(  );

	void 	create_apartment( );
	void	draw(); 

	float 	 		  m_height;
	unsigned long int m_color;

	// FOR 2D Drawing (and maze solving):
	glMap2D*	create_2D_drawing( );
	glMap2D	m_2D;

//	void	extract_2D    ( ); 		//	abstract Walls / computes corners too.
//	void	print_2D_info ( );		// 
//	void	draw_corners  ( );		// corners 
		
	vector<glWall>    m_walls;
	vector<abWall>    m_awalls;
	
	float	m_x;
	float	m_y;
	float	m_z;	
};

#endif

/*	vector<struct Vertex> m_2D;
	vector<GLubyte> m_2D_indices;	
	void 	create_2D_mark ( float mStartx, float mStarty, long int mColor );
	void 	create_2D_marks( float mStartX, float mStartY, float mEndX, float mEndY);
	void 	generate_VBO_2D  ( );
	void 	generate_IBO_2D  ( );
	void	scale_drawing    (float mScale); 
	void	draw_2D		     ( ); 	
	GLuint	m_VBO;
	GLuint	m_IBO; */