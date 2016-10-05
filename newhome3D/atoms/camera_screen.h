//
//  camera_screen.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/17/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__camera_screen__
#define __home3D__camera_screen__

#include <stdio.h>
#include "Texture.hpp"
#include "paper.hpp"
#include "opencv2/opencv.hpp"
using namespace cv;


class CameraTexture : public Texture
{
public:
    CameraTexture();
    ~CameraTexture();
    
    void            generate_PBO();
    int             timeslice();
    void            stop();

    virtual void	draw();
    virtual void    after_draw();

    GLuint          m_pbos[2];      // IDs of PBO
    VideoCapture    cam;            // open the default camera

    int             m_index;
    int             m_nextIndex;
};


class glCameraScreen : public glPaper
{
public:
    glCameraScreen();
    ~glCameraScreen();
    
};

#endif /* defined(__home3D__camera_screen__) */


