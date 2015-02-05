/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/

//const int number_of_vertices = 10;

class glCylinder 
{
public:
	glCylinder( int mNumberSamples );
	
	GLbyte 	generate_disc_indices( GLubyte* mptr, GLbyte mStart_of_top_index );
	void 	generate_side_IBO	( GLubyte* isptr );
	void 	generate_IBO		( );
	void 	generate_VBO		( );

	void	generate_vertices	( );
	void 	generate_vertices_colors();	
	void	draw				( ); 
	void 	print_info			( );
	
	float 	m_radius;
	float 	m_height;
	int		m_number_of_samples;

	float	m_x;
	float	m_y;
	float	m_z;

	bool	m_is_closed;
	unsigned long int m_color;
	unsigned long int m_ends_color;	
	GLbyte  m_disc_indices;

	GLuint	m_VBO;
	GLuint	m_IBO;
	int   	m_number_of_vertices;
	int		m_number_side_indices;
	
	Vertex*  vertices;
	
private:
	
};


