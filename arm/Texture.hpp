/* 
	Loads bitmap texture and attaches to a box object.
	
	This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 	
*/
#ifndef _TEXTURE_1234_HPP_
#define _TEXTURE_1234_HPP_

//#include "all_objects.h"
#include <string>
#include <vector>
//#include <opencv/cv.h>
//#include <opencv2/opencv.hpp>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif


//using namespace cv;
using namespace std;

struct stTextCoord
{
    float u;
    float v;
};

class Texture
{
public:
	Texture ( );
	~Texture( );

	void			load_image				(string mFilename);
    GLuint			generate_TBO			( );		// handle
    
	// The object can either create a class derived from this and override this function.
	// Or ignore this function and add coordinates to "m_TexCoords" - better usually.
    GLuint          generate_grid_coords    ( int mWidth, int mHeight );
    GLuint			generate_texture_coords	( int mRotation = 0);		// data to match vertexes.  Unique to each object!
	void			generate_VBOTexCoords	( );		// handle

	virtual void	pre_draw(); 
	virtual void    after_draw();
	virtual void	print_info();

	GLuint			m_TBO;				// The image 
	GLuint			m_TBOTexCoords;		// The coordinates
	vector<struct stTextCoord>	m_TexCoords;

    int             format,format2;
    int             m_repetitions_x;
    int             m_repetitions_y;
//    float           m_tiles;
    float           m_width_in_inches;      // maps 1.0 text coordinate to how many inches?
    float           m_height_in_inches;      // maps 1.0 text coordinate to how many inches?
    
	unsigned char*  m_image;
	int m_cols,m_rows;
	    
};


#endif
