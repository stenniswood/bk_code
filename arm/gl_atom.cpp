#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
//#include "GLES/gl.h"
//#include "GLES/glext.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <GL/glu.h>
#endif
//#include "all_objects.h"
#include "gl_atom.hpp"

#define Debug 0

glAtom::glAtom( )
{
    m_uses_normals = false;
    m_object_class  = 2;
	m_IBO = 0;
	m_VBO = 0;
    m_grab_at = GRAB_no_request;
    m_texture = NULL;
}

glAtom::~glAtom( )
{
    // What if Texture is shared between objects?!?!
    if (m_texture)
        delete m_texture;
    // So give texture a reference count.
    //m_scene.remove(this);
}

void glAtom::reset( )
{
    gl_unregister();
    m_vertices.clear();
    m_indices.clear();
    m_IBO = 0;
    m_VBO = 0;
    
    m_max.position[0] = 0;		// zero's since relative to m_x,m_y,m_z
    m_max.position[1] = 0;
    m_max.position[2] = 0;
    m_min.position[0] = 0;		// zero's since relative to m_x,m_y,m_z
    m_min.position[1] = 0;
    m_min.position[2] = 0;
}

void glAtom::change_color( long mColor )
{
	if (Debug) printf("change_color( %4lx ) %lu\n", mColor, m_vertices.size() );
	for (int i=0; i<m_vertices.size(); i++)
	{
		m_vertices[i].color[0] = ((mColor & 0x00FF0000)>>16);
		m_vertices[i].color[1] = ((mColor & 0x0000FF00)>>8);
		m_vertices[i].color[2] = ((mColor & 0x000000FF));		
		m_vertices[i].color[3] = ((mColor & 0xFF000000)>>24);
	} 
}
void glAtom::set_vertex_color( struct Vertex_pnc& mVertex )
{
    mVertex.color[3] = ((m_color & 0xFF000000)>>24);
    mVertex.color[0] = ((m_color & 0x00FF0000)>>16);
    mVertex.color[1] = ((m_color & 0x0000FF00)>>8);
    mVertex.color[2] = ((m_color & 0x000000FF));    
}
void glAtom::set_vertex_color( struct Vertex_pc& mVertex )
{
    mVertex.color[3] = ((m_color & 0xFF000000)>>24);
    mVertex.color[0] = ((m_color & 0x00FF0000)>>16);
    mVertex.color[1] = ((m_color & 0x0000FF00)>>8);
    mVertex.color[2] = ((m_color & 0x000000FF));
}

void glAtom::scale_drawing( float mScale )
{
    for (int v=0; v<m_vertices.size(); v++)
    {
        m_vertices[v].position[0] *= mScale;
        m_vertices[v].position[2] *= mScale;
    }
}

void glAtom::grab_top( )
{
    m_grab_at = GRAB_top;
    add_offset( 0., -m_max.position[1], 0. );
    m_min.position[1] += -m_max.position[1];
    m_max.position[1] =0.;
}
void glAtom::grab_bottom( )
{
    m_grab_at = GRAB_bottom;
    add_offset( 0., -m_min.position[1], 0. );
    m_max.position[1] += -m_min.position[1];
    m_min.position[1] = 0.;
}
void glAtom::grab_left( )
{
    m_grab_at = GRAB_left;
    add_offset( -m_max.position[0], 0., 0. );
    m_min.position[0] += -m_max.position[0];
    m_max.position[0] =0.;
}
void glAtom::grab_right( )
{
    m_grab_at = GRAB_right;
    add_offset( -m_min.position[0], 0., 0. );
    m_max.position[0] += -m_min.position[0];
    m_min.position[0] = 0.;
}
void glAtom::grab_front( 	)
{
    m_grab_at = GRAB_front;
    add_offset( 0., 0., -m_max.position[2] );
    m_min.position[2] += -m_max.position[2];
    m_max.position[2] =0.;
}
void glAtom::grab_back( 	)
{
    m_grab_at = GRAB_back;
    add_offset( 0., 0., -m_min.position[2] );
    m_max.position[2] += -m_min.position[2];
    m_min.position[2] = 0.;	
}

/* Use this to grasp the object in a different location.
 ie. to swing a door, it's better to set the 0.0 at the edge of the box.
 
 Note:  Use glTranslate() to move the object (ie. set m_x,m_y,m_z)	  */
void glAtom::add_offset(float mX, float mY, float mZ)
{
    for (int i=0; i<m_vertices.size(); i++)
    {
        m_vertices[i].position[0] += mX;
        m_vertices[i].position[1] += mY;
        m_vertices[i].position[2] += mZ;
    }
}

void glAtom::release_memory      (   )
{
    m_vertices.clear();
    m_indices.clear();
}
void glAtom::compute_min_max( 	)
{
    compute_min();
    compute_max();
}

void glAtom::compute_max( 	)
{
    m_max.position[0] = 0;		// zero's since relative to m_x,m_y,m_z
    m_max.position[1] = 0;
    m_max.position[2] = 0;
    
    for (int i=0; i<m_vertices.size(); i++)
    {
        if (m_vertices[i].position[0] > m_max.position[0])
            m_max.position[0] = m_vertices[i].position[0];
        
        if (m_vertices[i].position[1] > m_max.position[1])
            m_max.position[1] = m_vertices[i].position[1];
        
        if (m_vertices[i].position[2] > m_max.position[2])
            m_max.position[2] = m_vertices[i].position[2];
    }
}

void glAtom::compute_min( )
{
    m_min.position[0] = 0;		// zero's since relative to m_x,m_y,m_z
    m_min.position[1] = 0;
    m_min.position[2] = 0;
    
    for (int i=0; i<m_vertices.size(); i++)
    {
        if (m_vertices[i].position[0] < m_min.position[0])
            m_min.position[0] = m_vertices[i].position[0];
        
        if (m_vertices[i].position[1] < m_min.position[1])
            m_min.position[1] = m_vertices[i].position[1];
        
        if (m_vertices[i].position[2] < m_min.position[2])
            m_min.position[2] = m_vertices[i].position[2];
    }
}

void glAtom::print_min_max( )
{
    printf("glAtom::Min Vertex values <%6.3f, %6.3f, %6.3f>  \n",
           m_min.position[0],
           m_min.position[1],
           m_min.position[2]  );
    
    printf("glAtom::Max Vertex values <%6.3f, %6.3f, %6.3f>  \n",
           m_max.position[0],
           m_max.position[1],
           m_max.position[2]  );
}

void	glAtom::print_indices( )
{
    printf("print_indices() %lu\n", m_indices.size() );
    for (int i=0; i<m_indices.size(); i++)
    {
        printf("%d : %u\n", i, m_indices[i] );
    }
}

void	glAtom::print_indexed_vertices( )
{
    GLuint index = 0;
    printf("print_indexed_vertices() %lu\n", m_indices.size() );
    for (int i=0; i<m_indices.size(); i++)
    {
        index = m_indices[i];
        printf("%3d: %3d - %5.2f %5.2f %5.2f\n", i, index,
               m_vertices[index].position[0],
               m_vertices[index].position[1],
               m_vertices[index].position[2] );
    }
}

void glAtom::generate_indices( )
{
    // derived class
}
void glAtom::generate_vertices( )
{
    // derived class
}

void	glAtom::print_vertices	( bool mShowColors)
{
    printf("print_vertices()  Number of Vertices=%lu\n", m_vertices.size() );
    for (int i=0; i<m_vertices.size(); i++)
    {
        printf("%d : %5.1f %5.1f %5.1f\t", i, 
               m_vertices[i].position[0],
               m_vertices[i].position[1],
               m_vertices[i].position[2] );
        if (mShowColors)
            printf(": %d\t%d\t%d\t%d\n",
                   m_vertices[i].color[0],
                   m_vertices[i].color[1],
                   m_vertices[i].color[2],
                   m_vertices[i].color[3] );		
    }
}

/******************** OPENGL COMMANDS *********************************************/

void 	glAtom::generate_VBO( )
{ 
	//Create a new VBO and use the variable id to store the VBO id
	glGenBuffers( 1, &m_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );	// Make the new VBO active  GL_COLOR_ARRAY

	// Generate & Upload vertex data to the video device 
	int size = (int)m_vertices.size() * sizeof(Vertex_pnc);
	glBufferData		 ( GL_ARRAY_BUFFER, size, m_vertices.data(), GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0   );   // vertex positions
	glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex_pnc), (void*)(offsetof(struct Vertex_pnc,color)));	// color
    if (m_uses_normals)
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_pnc), (void*)(offsetof(struct Vertex_pnc,normal)) );
    glBindBuffer		 ( GL_ARRAY_BUFFER, m_VBO           );
    
	//Draw Triangle from VBO - do each time window, view point or data changes
	//Establish its 3 coordinates per vertex with zero stride in this array; necessary here

	//glVertexAttribPointer(1, 3, GL_FLOAT, 		GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(struct Vertex, normal)));	// normals
	glEnableVertexAttribArray( 0 );	
	glEnableVertexAttribArray( 2 );	// GLKVertexAttribColor	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

// IBO:
void	glAtom::generate_IBO	( ) 
{ 
	size_t  size = m_indices.size();
	GLuint* tmp  = m_indices.data();

	glGenBuffers( 1, &m_IBO );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (size)*sizeof(GLuint), 
				 tmp, GL_STATIC_DRAW );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
}

void glAtom::setup( )
{
    generate_vertices();
    generate_indices ();
    compute_min_max();
    glObject::setup();
}

/* For create()  1 grab function may be called prior. */
void glAtom::create( )
{
    setup      ();
    switch(m_grab_at)
    {
        case GRAB_top   : grab_top();               break;
        case GRAB_bottom: grab_bottom();            break;
        case GRAB_front : grab_front();             break;
        case GRAB_back  : grab_back();              break;
        case GRAB_left  : grab_left();              break;
        case GRAB_right : grab_right();             break;
        default: break;
    }
    gl_register();
}

void glAtom::map_vertices( glm::mat4 mMatrix )
{
    glm::vec4 vec;
    for (int v=0; v<m_vertices.size(); v++)
    {
        vec[0] = m_vertices[v].position[0];
        vec[1] = m_vertices[v].position[1];
        vec[2] = m_vertices[v].position[2];
        vec[3] = 1.0;
        vec = mMatrix * vec;        
        m_vertices[v].position[0] = vec[0];
        m_vertices[v].position[1] = vec[1];
        m_vertices[v].position[2] = vec[2];
    }
}

// Override this with open gl commands.
void glAtom::draw_primitive( )
{
    glDrawElements(GL_QUAD_STRIP, (int)m_indices.size(), GL_UNSIGNED_INT, 0 );
}

/* MOST LIKELY - OVERRIDE THIS FUNCTION! */
void glAtom::draw_body()
{
    // Make the new VBO active. Repeat here incase changed since initialisation
    glBindBuffer( GL_ARRAY_BUFFER, 		   m_VBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );
    glVertexPointer(3, GL_FLOAT, 		 sizeof(struct Vertex), NULL);
    // Establish array contains m_vertices (not normals, colours, texture coords etc)
    glEnableClientState(GL_VERTEX_ARRAY);
    if (m_uses_normals)
        glEnableClientState( GL_NORMAL_ARRAY );

    if (m_texture==NULL)
    {
        glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));
        glEnableClientState(GL_COLOR_ARRAY );
    } else {
        m_texture->pre_draw();
    }
    
    // Draw Bottom :
    draw_primitive();
    
    if (m_texture)
        m_texture->after_draw();
}


void glAtom::gl_register()
{
    glObject::gl_register();
    generate_VBO();
    generate_IBO();
}

void glAtom::gl_unregister()
{
    glObject::gl_unregister();
    if (m_VBO)
        glDeleteBuffers(1,  &m_VBO );
    if (m_IBO)
        glDeleteBuffers(1,  &m_IBO );
}
