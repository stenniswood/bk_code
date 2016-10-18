/* 
	Loads bitmap texture and attaches to a box object.
	
	This class will generate OpenGL Vertex data
	for a box of given width, height, depth. 	
*/
#ifndef _TXT_CONTAINER_
#define _TXT_CONTAINER_

#include "gl_box.hpp"
#include <string>
#include "imageloader.h"
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

const int BOTTOM_SIDE_ID = 0;
const int TOP_SIDE_ID    = 1;
const int FRONT_SIDE_ID  = 2;
const int BACK_SIDE_ID   = 3;
const int LEFT_SIDE_ID   = 4;
const int RIGHT_SIDE_ID  = 5;


class txtContainer : public glBox
{
public:
	txtContainer();

    //  set_texture:  m_texture = BrickTexture;
	Texture*        load_image				(string mFilename, int mSide=0, int mXReps=1, int mYReps=1);
    void			apply_front				( Texture* mTexture, int mXReps=1, int mYReps=1 );     // Show the texture just on 1 side ()
    void			apply_back				( Texture* mTexture, int mXReps=1, int mYReps=1 );
    void			apply_top				( Texture* mTexture, int mXReps=1, int mYReps=1 );
    void			apply_bottom			( Texture* mTexture, int mXReps=1, int mYReps=1 );
    void			apply_left				( Texture* mTexture, int mXReps=1, int mYReps=1 );
    void			apply_right				( Texture* mTexture, int mXReps=1, int mYReps=1 );

    void			wrap_3_sides            ();     // top, front, bottom
    void			wrap_4_sides            ();     // top, front, bottom
    void			wrap_5_sides            ();     // top, front, bottom
    void			wrap_6_sides            ();     // top, front, bottom

	GLuint			generate_texture_coords_4_side_stretch( );
	virtual void	gl_register             ( );
	
	virtual void	draw_body	(); 
	virtual void	print_info	();

	int				m_side_applied;

	int format,format2,type;
    Texture*         m_side[6];     // each face can have a different texture
    
};



#endif
	//GLuint			generate_texture_coords	( int mSide );