#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "all_objects.h"

// Include OpenCV here for the imread function!
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
// Include OpenCV here for the imread function!

using namespace cv;
using namespace std;


txtContainer::txtContainer(  )
:glBox()
{
    for (int i=0; i<6; i++)
        m_side[i] = NULL;
    
    m_repetitions_x = 1.0;
    m_repetitions_y = 1.0;
    m_object_type_name = "textured box";    
    m_object_class  = 9;
	m_side_applied	= TOP_SIDE_ID;
}

Texture* txtContainer::load_image( string mFilename, int mSide )
{
    m_side[mSide] = new Texture();
    m_side[mSide]->load_image(mFilename);
    return m_side[mSide];
}

void txtContainer::apply_top( Texture* mTexture )     // Show the texture just on 1 side ()
{
    m_side[1] = mTexture;
}
void txtContainer::apply_bottom( Texture* mTexture )
{
    m_side[0] = mTexture;
}
void txtContainer::apply_front( Texture* mTexture )
{
    m_side[2] = mTexture;
}
void txtContainer::apply_back( Texture* mTexture )
{
    m_side[3] = mTexture;
}
void txtContainer::apply_left( Texture* mTexture )
{
    m_side[4] = mTexture;
}
void txtContainer::apply_right( Texture* mTexture )
{
    m_side[5] = mTexture;
}

// Show the texture just on 1 side ()
void txtContainer::wrap_3_sides( )
{
    
}
void txtContainer::wrap_4_sides( )
{
    
}
void txtContainer::wrap_5_sides( )
{
    
}
void txtContainer::wrap_6_sides()
{
    
}

void txtContainer::gl_register(  )
{
    glAtom::gl_register();       // Base class

    for (int s=0; s<6; s++)
        if (m_side[s])
        {
            m_side[s]->generate_TBO();
            m_side[s]->generate_texture_coords();
            m_side[s]->generate_texture_coords();
            m_side[s]->generate_VBOTexCoords();
        }
    //generate_texture_coords_4_side_stretch();
}

/* 
    Top, front, bottom, back sides.
 */
GLuint txtContainer::generate_texture_coords_4_side_stretch( )
{
	float hdh = 1.0/(height+depth+height+depth);
	float hwh = 1.0/(width);
    const float fs = 1.0;
    struct stTextCoord tc;
    
	// U                                            V
    tc.u = hwh*height;          tc.v = hdh*height;            m_texture->m_TexCoords.push_back( tc );
    tc.u = hwh*height;          tc.v = hdh*(height+depth);    m_texture->m_TexCoords.push_back( tc );
    tc.u = hwh*(height+width);  tc.v = hdh*(height+depth);    m_texture->m_TexCoords.push_back( tc );
    tc.u = hwh*(height+width);  tc.v =hdh*height ;            m_texture->m_TexCoords.push_back( tc );

    tc.u = 0.0;                 tc.v = 0.0;         m_texture->m_TexCoords.push_back( tc );
    tc.u = 0.0;                 tc.v = fs;          m_texture->m_TexCoords.push_back( tc );
    tc.u = fs;                  tc.v = fs;          m_texture->m_TexCoords.push_back( tc );
    tc.u = fs;                  tc.v = 0.0;         m_texture->m_TexCoords.push_back( tc );
	return m_texture->m_TexCoords.size()/2.;
}


void txtContainer::draw_body()
{
	//Make the new VBO active. Repeat here incase changed since initialisation
	glBindBuffer    (GL_ARRAY_BUFFER, 		  m_VBO	);
	glBindBuffer    (GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	glVertexPointer (3, GL_FLOAT, 		  sizeof(struct Vertex), NULL );
	glColorPointer  (4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)(offsetof(struct Vertex,color)));

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY  );
	
	if (m_is_closed)                        // GL_QUADS
	{
        //glDrawElements(GL_QUADS, (int)m_indices.size(), GL_UNSIGNED_INT, (GLvoid*)(NULL));             // 0
        int offset = 0;
        for (int s=0; s<6; s++)
        {
            if (m_side[s])
            {
                m_side[s]->draw();
                glDrawElements(GL_QUADS,(int)4, GL_UNSIGNED_INT,
                               (GLvoid*)(offset*sizeof(GL_UNSIGNED_INT)));
                m_side[s]->after_draw();
            }
            else
                glDrawElements(GL_QUADS, (int)4, GL_UNSIGNED_INT,
                               (GLvoid*)(offset*sizeof(GL_UNSIGNED_INT)));             // 0
            offset += 4;
        }
	}
}

/* The problem with putting different images on each face:
    a) Every vertex has to have a corresponding TexCoord.
    
    the order of vertices must match the order of the TexCoords.
 
    However, the order of the vertices wont match the order needed by GL_QUADS.
        the index buffer fixes this.
 
    When doing the front (for instance),  we are using vertex (0,1,5,4)
    And the TexCoords are (0,1,2,3, 4,5,6,7)
 
    so 0=<0,0>;   1=<1,0>;  2=<1,1>;  3=<1,0>
 
    How do we fix this, b/c loading the texture, it would not know the set (0,1,5,4).
 
    Best way really is to store duplicates of the vertices for all 6 faces.
    This would allow normals for all faces too.

    A whole new class needed?
        
 
 
 
 */
void txtContainer::print_info()
{
	printf("txtContainer()  <x,y,z> = <%6.3f %6.3f %6.3f> \n", 
			m_x,m_y,m_z );
	printf(" width=%6.3f;  height=%6.3f; depth=%6.3f> \n", 
			width, height, depth );
}


/* this does a */
GLuint txtContainer::generate_texture_coords( int mSide )
{
    // Texture coords [0.0..1.0]   1.0 represents the far edge of the image.
    // Pretty sure we need 1 coordinate for each vertex.
    //   So if we're just doing 1 face.  It should mean just 4 text coords.
    //m_NumTexCoords = 8;
    /*	const float fsx = m_repetitions_x;
     const float fsy = m_repetitions_y;
     
     // U									V
     struct stTextCoord tc;
     tc.u = 0.0;   tc.v = 0.0;    	m_side[mSide]->m_TexCoords.push_back ( tc );     // 4 vertices Bottom
     tc.u = 0.0;   tc.v = fsy;       m_side[mSide]->m_TexCoords.push_back ( tc );
     tc.u = fsx;   tc.v = fsy;     	m_side[mSide]->m_TexCoords.push_back ( tc );
     tc.u = fsx;   tc.v = 0.0;       m_side[mSide]->m_TexCoords.push_back ( tc );
     
     tc.u = 0.0;   tc.v = 0.0;    	m_side[mSide]->m_TexCoords.push_back ( tc );     // 4 vertices Top
     tc.u = 0.0;   tc.v = fsy;       m_side[mSide]->m_TexCoords.push_back ( tc );
     tc.u = fsx;   tc.v = fsy;     	m_side[mSide]->m_TexCoords.push_back ( tc );
     tc.u = fsx;   tc.v = 0.0;       m_side[mSide]->m_TexCoords.push_back ( tc );
     return m_side[mSide]->m_TexCoords.size()/2.; */
    return 0;
}
