/* This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 
*/

//const int number_of_vertices = 10;

class glCylinder : public glExtrusion
{
public:
	glCylinder( int mNumberSamples );

	virtual void	generate_layer_vertices( );	
	virtual GLuint 	generate_disc_indices  ( GLuint mStart_of_top_index );

	virtual void	draw_body			( ); 

	float 	m_radius;
	int		m_number_of_samples;	

};


