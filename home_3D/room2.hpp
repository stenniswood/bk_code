/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/
#ifndef _ROOM_H_
#define _ROOM_H_

#include <vector>
using namespace std;
#include "wall.hpp"


class glRoom 
{
public:
	glRoom	(  );

	void 	create_apartment( );
	void	draw(); 
	
	float 	 		  m_height;
	unsigned long int m_color;

	vector<glWall>    m_walls;

	GLuint	m_VBO;
	GLuint	m_IBO;

	float	m_x;
	float	m_y;
	float	m_z;	
};


#endif
