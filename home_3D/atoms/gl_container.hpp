/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/
#ifndef _GL_CONTAINER_
#define _GL_CONTAINER_

struct Vertex
{
  float position[3];
  //GLfloat normal[3];
  GLubyte color[4];
};

extern const int NUMBER_OF_CUBE_INDICES;   
extern const int NUMBER_OF_QUAD_INDICES;
const  int number_of_vertices = 10;


class glContainer
{
public:
	glContainer();

	void 	Relocate( float mX, float mY, float mZ );
	void 	create  			( );

	void 	change_color	   	( long mColor );
	void	change_top_color   	( long mColor );	
	void	change_bottom_color	( long mColor );
	void 	generate_IBO		(	);
	void 	generate_VBO		(	);
	
	void	generate_vertices	(	);

	void	grab_top			( 	);
	void	grab_bottom			( 	);
	void	grab_left			( 	);
	void	grab_right			( 	);
	void	grab_front			( 	);
	void	grab_back			( 	);

	void	add_offset( float mX, float mY, float mZ ); // to all vertices.
	void	draw(); 
	void	print_info();

	float 	width;
	float 	height;
	float 	depth;

	float	m_x;
	float	m_y;
	float	m_z;

	bool	m_is_closed;
	unsigned long int m_color;

	GLuint	m_VBO;
	GLuint	m_IBO;

	int   	m_number_of_vertices;
	Vertex  vertices[number_of_vertices];
};



// DO THIS LATER AFTER vertices are generated.
const int TYPE_CLOSED 		= 1;
const int TYPE_OPEN   		= 1;
const int TYPE_WIRE_FRAME 	= 1;


#endif
