#ifndef _ATOM_H_
#define _ATOM_H_

#include <vector>
using namespace std;

/* 
	An atom has the raw vertices and indices.  
	A molecule does not (though it maybe consist of atom based objects)
	
    This class serves as a base OpenGL object with Vertex data
    
	It creates the VBO and IBO objects (based on what is in the m_vertices and m_indices)

	The way to use this class is to derive one from it,	and specify the shape.	
*/
/*struct Vertex
{
  float position[3];
  //GLfloat normal[3];
  GLubyte color[4];
};*/


class glAtom : public glObject
{
public:
	glAtom( );

	void			create			( );
	void 			change_color	( long mColor );
	virtual void	draw_body		( );	// Override this with open gl commands.

	// IBO:
	void 			generate_IBO	( );
	void			print_indices	( );
	vector<GLubyte>	m_indices;
	GLuint			m_IBO;

	// VBO:
	void 			generate_VBO	( );
	void			print_vertices	( bool mShowColors = true );
	vector<struct Vertex>	m_vertices;
	GLuint			m_VBO;
};


#endif
