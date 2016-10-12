//
//  terrain.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/9/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__terrain__
#define __home3D__terrain__

#include <stdio.h>
#include "gl_atom.hpp"
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include "dwelling_level.h"

using namespace cv;
struct omission_ranges {
    float sx;
    float sz;
    float ex;
    float ez;
    float floor;
};

class glTerrain : public glAtom
{
public:
    glTerrain();
    ~glTerrain();
    
    void    set_stretch ( float mStretchX, float mStretchY );
    void    add_omission( glDwellingLevel* mOmitBuilding );
    void    add_omission( glBox* mOmitBox );
    
    void    load_image( string mFilename );
    void    generate_indices ( );
    void    generate_vertices( );
    void    draw_primitive   ( );
    
    void    compute_normal( );
    void    compute_normals( );
    
    float   m_y_scale;
    float   m_stretch_x;
    float   m_stretch_y;
    
    vector<omission_ranges> m_omissions;
    
	Mat 	m_image;
};
#endif /* defined(__home3D__terrain__) */
