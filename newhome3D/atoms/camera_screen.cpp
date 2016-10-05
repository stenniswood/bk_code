//
//  camera_screen.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/17/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//


#include "camera_screen.h"
#include "all_objects.h"

const int    IMAGE_WIDTH     = 1280;
const int    IMAGE_HEIGHT    =  720;
const int    CHANNEL_COUNT   = 3;
const int    DATA_SIZE       = IMAGE_WIDTH * IMAGE_HEIGHT * CHANNEL_COUNT;


CameraTexture::CameraTexture()
: cam(0)
{
    //namedWindow("Video",1);
}

CameraTexture::~CameraTexture()
{
    // clean up PBOs
    glDeleteBuffersARB( 2, m_pbos );
}

void CameraTexture::generate_PBO()
{
    glGenBuffersARB(2, m_pbos);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_pbos[0]);
    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_pbos[1]);
    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

void CameraTexture::draw()
{
    glEnable            (GL_TEXTURE_2D              );
    glBindTexture       (GL_TEXTURE_2D,   m_TBO     );
    glBindBuffer        (GL_ARRAY_BUFFER, m_TBOTexCoords );
    glEnableClientState (GL_TEXTURE_COORD_ARRAY     );
    glTexCoordPointer   (2, GL_FLOAT,   0, NULL     );
    
    glBindBufferARB     (GL_PIXEL_UNPACK_BUFFER_ARB, m_pbos[m_index] );
    glTexSubImage2D     (GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, format2, GL_UNSIGNED_BYTE, 0 );
    // copy pixels from PBO to texture object; Use offset instead of ponter.
}

void CameraTexture::after_draw()
{
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable           (GL_TEXTURE_2D);
}

int CameraTexture::timeslice()
{
    if(!cam.isOpened())  // check if we succeeded
        return -1;

    cam >> m_image;         // get a new frame from camera
  //imshow("Video", m_image);
    m_index     = (m_index + 1) % 2;
    m_nextIndex = (m_index + 1) % 2;
    
    glEnable     (GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_TBO);
    // bind PBO to update pixel values
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_pbos[m_nextIndex] );
    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_DRAW_ARB );
    GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    if (ptr)
    {
        // update data directly on the mapped buffer
        if ((m_image.data) && (ptr))
            memcpy( ptr, m_image.data, DATA_SIZE );
        glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);   // release pointer to mapping buffer
    }
    // it is good idea to release PBOs with ID 0 after use.
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    glDisable       (GL_TEXTURE_2D);
    
 /* glGenerateMipmap(GL_TEXTURE_2D);
    glTexEnvf       (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); */
    return 1;
}

void CameraTexture::stop()
{
    cam.release();
}

/*********************************************************/

glCameraScreen::glCameraScreen()
{
    m_texture = new CameraTexture();
    m_samples_per_inch  = 4;
    Initialize( 10, 10.0 );
}

glCameraScreen::~glCameraScreen()
{
    
}


