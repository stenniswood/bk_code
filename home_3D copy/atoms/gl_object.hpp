#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
using namespace std;

/* 
    This class serves as a base OpenGL Vertex data
    
	It provides for Translation and Rotation of the object (yet to be defined).
	It also creates the VBO and IBO objects (based on what is in the m_vertices and m_indices)
			
	The way to use this class is to derive one from it,
	and specify the 2D shape.
	
	This should also register the object in a glObjectManager() class.
	This class like the display manager will call the create() functions 
	after a valid opengl context has been initialized.

*/

class glObject
{
public:
	glObject( );

	void			relocate( float mX, float mY, float mZ );
	void 			change_color	( long mColor );

	virtual void	draw_body		( );	// Override this with open gl commands.
	void			draw			( );	// provides translation and rotation.

	float	m_x;
	float	m_y;
	float	m_z;

	float	m_x_angle;
	float	m_y_angle;
	float	m_z_angle;

	unsigned long int m_color;

/*
	// IBO:
	void 	generate_IBO			( );
	void	print_indices			( );
	void	print_vertices			( bool mShowColors = true );
	vector<GLubyte>			m_indices;
	GLuint	m_IBO;
	// VBO:
	void 	generate_VBO			( );
	vector<struct Vertex>	m_vertices;
	GLuint	m_VBO; */
};


#endif
