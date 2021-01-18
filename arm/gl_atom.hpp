#ifndef _ATOM_H_
#define _ATOM_H_

#include <vector>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif


using namespace std;
#include "Texture.hpp"
#include "verbal_object.h"


/* 
	An atom has the raw vertices and indices.  
	A molecule does not (though it maybe consist of atom based objects)
	
    This class serves as a base OpenGL object with Vertex data
    
	It creates the VBO and IBO objects (based on what is in the m_vertices and m_indices)

    Steps needed to create an openGL object:
    First Load data into class members:  m_vertices[] and m_indices[]

    a) Create Vertex data           generate_VBO()
        glGenBuffers();
        glBindBuffer();
        glBufferData();
        tell about the struct of the data - glVertexAttribPointer()
        tell about the colors             - glVertexAttribPointer()
        glEnableVertexAttribArray();        Enable the object
 
    b) Create Index data            generate_IBO()
        glGenBuffers()
        glBindBuffer()
        glBufferData()
 
	The way to use this class is to derive one from it,	and specify the shape.	
*/
/*struct Vertex
{
  float position[3];
  //GLfloat normal[3];
  GLubyte color[4];
};*/

const int GRAB_no_request = 0;
const int GRAB_top    = 1;
const int GRAB_bottom = 2;
const int GRAB_left   = 3;
const int GRAB_right  = 4;
const int GRAB_front  = 5;
const int GRAB_back   = 6;


class glAtom : public VerbalObject
{
public:
	glAtom ( );
	~glAtom( );

    void            reset       ( );
    virtual void 	setup       ( );
    virtual void 	create      ( );    // Better to override setup and call gl_register() after.

	void 	change_color        ( long mColor            );
    void    set_vertex_color    ( struct Vertex_pc & mVertex );
    void    set_vertex_color    ( struct Vertex_pnc& mVertex );
    
    void    scale_drawing       ( float mScale           );
    void    map_vertices        ( glm::mat4 mMatrix      );

	void	grab_top			( 	);
	void	grab_bottom			( 	);
	void	grab_left			( 	);
	void	grab_right			( 	);
	void	grab_front			( 	);
	void	grab_back			( 	);
	void	add_offset          ( float mX, float mY, float mZ ); // to all vertices.

    virtual void	compute_max	( 	);
    virtual void	compute_min	( 	);
    void	compute_min_max		( 	);

    virtual void    release_memory(   );
    void	print_min_max		  (   );
    void    print_indexed_vertices(   );

	// IBO :
    virtual void            generate_indices( );    
	void                    generate_IBO	( );
	void                    print_indices	( );
	vector<GLuint>          m_indices;
	GLuint                  m_IBO;

	// VBO :
    virtual void            generate_vertices( );
	void 					generate_VBO	( );
	void					print_vertices	( bool mShowColors = true );
	vector<struct Vertex_pnc>	m_vertices;
	GLuint					m_VBO;
    bool                    m_uses_normals;

    virtual void	gl_register  (  );
    virtual void    gl_unregister(  );
    virtual void	draw_body	 ( 	);          // Override this with open gl commands.
    virtual void	draw_primitive( );          // Override this with open gl commands.

    int             m_grab_at;                  // Hold caller request for later create()

    Texture*       m_texture;                   //
	//struct Vertex  m_max;
	//struct Vertex  m_min;
};


#endif
