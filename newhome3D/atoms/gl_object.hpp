#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>


using namespace std;

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vector.hpp"
#include "moving_object.h"
#include "tk_colors.h"


struct Vertex_p
{
    float position[3];
};
struct Vertex_pn
{
    float position[3];
    GLfloat normal[3];
};
struct Vertex_pnc
{
    float position[3];
    GLubyte color[4];
    GLfloat normal[3];
};
struct Vertex_pc
{
    float position[3];
    GLubyte color[4];
};

#define Vertex Vertex_pnc

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

class glObject : public glMovingObject 
{
public:
	glObject ( );
	~glObject( );
    
    virtual void 	setup           ( );
    virtual void 	create          ( );
	//virtual void	relocate        ( double mX, double mY, double mZ );
	virtual void	relocate        ( float mX, float mY, float mZ );
    virtual void    relocate        ( MathVector& mNewLocation );

    virtual void 	set_color       ( Color mColor      );
	virtual void 	set_color       ( long mColor       );
    void            stamp_color     ( struct Vertex& mV );

    virtual void	gl_register     (  );
    virtual void    gl_unregister   (  );
    
    virtual void    release_memory  (  ) { };
    
    // Returns a matrix with translate and rotations (same as drawn) for compute purposes.
    // mOne,mTwo,mThree - selects the ordering of the rotations.
    glm::mat4       get_body_matrix         ( int mOne=0, int mTwo=1, int mThree=2 );
    glm::mat4       get_body_matrix_inverse ( int mOne=2, int mTwo=1, int mThree=0 );
    glm::vec4       get_position            ( );

    MathVector      map_coords              ( MathVector mObjectCoordinate );
    MathVector      map_to_object_coords    ( MathVector mParentCoordinate );
    glm::vec4       map_coords              ( glm::vec4 mObjectCoordinate  );
    glm::vec4       map_to_object_coords    ( glm::vec4 mParentCoordinate  );
    

	virtual void	draw_body		( );	// Override this with open gl commands.
	void            draw			( );	// provides translation and rotation.
	void			print_location  ( );

    long            m_object_id;            // uniquely identifies this atom.
    long            m_object_class;         // each derived class has it's own identifier
    bool            m_registered;


	unsigned long int m_color;
};



glObject*   find_object_id( long mID );
void        list_object_ids(  );

#endif

