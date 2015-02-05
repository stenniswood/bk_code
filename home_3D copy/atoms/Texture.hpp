/* 
	Loads bitmap texture and attaches to a box object.
	
	This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 	
*/
#ifndef _TEXTURE_
#define _TEXTURE_

#include <string>
#include "imageloader.h"
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

struct stTextCoord {
	float u;
	float v;
};

class Texture
{
public:
	Texture();
	void			load_image				(string mFilename);
	// The object can either create a class derived from this and override this function.
	// Or ignore this function and add coordinates to "m_TexCoords" - better usually.
	GLuint			generate_texture_coords	( );		// data to match vertexes.  Unique to each object!
	
	GLuint			generate_TBO			( );		// handle
	void			generate_VBOTexCoords	( );		// handle


	virtual void	draw(); 
	void			after_draw(); 	
	virtual void	print_info();

	GLuint			m_TBO;				// The image 
	GLuint			m_TBOTexCoords;		// The coordinates
	vector<struct stTextCoord>	m_TexCoords;

	//Image* 			m_image;
	Mat 			m_src;
};


#endif
