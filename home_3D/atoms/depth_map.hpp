//
//  dwelling.h - Use with Kinect.  Shows a depth image in 3D.
//  home3D
//
//  Created by Stephen Tenniswood on 10/11/16
//  Copyright (c) 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef __depth_map__
#define __depth_map__

#include <stdio.h>


class glDepthMap : public glAtom
{
public:
    glDepthMap ();
    ~glDepthMap();

	uint16_t		compute_depth(int x, int y);
	
    void    		set_demo  			( );
    void    		set_image 			( char* mImage, int width, int height );
    void    		load_image			( Mat* mLevel );    
    
    virtual void	generate_vertices( );
    void 			draw_body		 ( );    
        
	int 	m_height;
	int 	m_width;
};


#endif /* defined(__home3D__dwelling__) */

