/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/

//const int number_of_vertices = 10;

class glStairway
{
public:
	glStairway	(  );

	void	generate_side_vertices  ( int mNumberOfSteps );
	void	generate_side2_vertices ( );
	void 	generate_vertices_colors( );

	GLbyte 	generate_side_IBO	( GLubyte* mptr, GLbyte mStartingVertexIndex );
	GLbyte 	generate_side2_IBO	( GLubyte* mptr, GLbyte mStartingVertexIndex );	
	void 	generate_stair_IBO	( GLubyte* isptr );
	void 	generate_IBO( );
	void 	generate_VBO( );		
	void	draw(); 
	
	float 	 m_rise;
	float 	 m_run;		
	float 	 m_width;

	bool	 m_is_closed;
	unsigned long int m_color;
	GLbyte   m_number_of_indices;
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


