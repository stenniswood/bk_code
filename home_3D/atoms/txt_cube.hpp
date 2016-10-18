/* 
	Loads bitmap texture and attaches to a box object.
	
	This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 	
*/
#ifndef _TXT_CUBE_
#define _TXT_CUBE_

#include "gl_box.hpp"
#include <string>
#include "imageloader.h"
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

const int BOTTOM_SIDE_ID = 0;
const int LEFT_SIDE_ID   = 1;
const int RIGHT_SIDE_ID  = 2;
const int FRONT_SIDE_ID  = 3;
const int BACK_SIDE_ID   = 4;
const int TOP_SIDE_ID    = 5;


class txtContainer : public glContainer
{
public:
	txtContainer();
	void			load_image				(string mFilename);
	GLuint			generate_TBO			( );
	GLuint			generate_texture_coords	( );
	void			generate_VBOTexCoords	( );
	virtual void 	setup  		(   );
	virtual void	gl_register (   );
	
	virtual void	draw_body	(); 
	virtual void	print_info	();

	GLuint			m_TBO;				// The image 
	GLuint			m_VBOTexCoords;		// The coordinates
	vector<float>	m_TexCoords;
	int				m_side_applied;

	Mat 			m_src;
	
	int format,format2,type;
};

#endif
