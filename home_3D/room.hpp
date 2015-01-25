/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/

//const int number_of_vertices = 10;

class glRoom 
{
public:
	glRoom	(  );

	void	generate_floor_vertices  ( );
	void	generate_ceiling_vertices( );
	void 	generate_vertices_colors ( );
	void 	scale_vertices ( float mScale );
	
	GLbyte 	generate_floor_IBO	( GLubyte* mptr, GLbyte mStart_of_top_index );
	
	GLbyte 	generate_ceiling_IBO( GLubyte* mptr, GLbyte mStart_of_top_index );	
	void 	generate_side_IBO	( GLubyte* isptr );
	void 	generate_IBO( );
	void 	generate_VBO( );		
	void	draw(); 
	
	float 	 m_height;
	bool	 m_is_closed;
	unsigned long int m_color;
	GLbyte   m_number_of_indices;
	GLbyte	 m_side_indices;
	GLbyte	 m_floor_indices;

	int		 m_total_vertices;
	GLbyte   m_number_of_floor_vertices;
	Vertex*  m_vertices;

	GLuint	m_VBO;
	GLuint	m_IBO;

	float	m_x;
	float	m_y;
	float	m_z;	
};


