//
//  gl_window.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/18/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__gl_window__
#define __home3D__gl_window__

#include <stdio.h>
#include "glMolecule.h"
#include "gl_container.hpp"
#include "frame.h"
#include "wall.hpp"


const float DEFAULT_WINDOW_HEIGHT = 36.;
const float DEFAULT_SILL_HEIGHT   = 48.;
const float DEFAULT_WINDOW_THICKNESS = 1.;




/* This is a side by side window! */
class glWindow : public glMolecule
{
public:
    glWindow(  );
    ~glWindow( );
    
    void            set_bay_params(
                                   float mWidth,
                                   float mHeight       = DEFAULT_WINDOW_HEIGHT,
                                   float mSillHeight   = DEFAULT_SILL_HEIGHT,
                                   float mThick        = DEFAULT_WINDOW_THICKNESS   );
    
    void            set_params  (float mWidth,
                                 float mHeight       = DEFAULT_WINDOW_HEIGHT,
                                 float mSillHeight   = DEFAULT_SILL_HEIGHT,
                                 float mThick        = DEFAULT_WINDOW_THICKNESS,
                                 bool mSideBySide = true);
    void            open        ( float mfraction );
    void            create_components( );

    float       m_full_width;
    float       m_full_height;
    float       m_sill_height;
    float       m_frame_thickness;
    float       m_open_fraction;
    
    struct stSize   m_size;
    glFrame*               m_piece1;
    glFrame*               m_piece2;
};


class glOverUnderWindow : public glMolecule
{
public:
    glOverUnderWindow(  );
    ~glOverUnderWindow( );
    
    void            set_params  (float mWidth,
                                 float mHeight       = DEFAULT_WINDOW_HEIGHT,
                                 float mSillHeight   = DEFAULT_SILL_HEIGHT,
                                 float mThick        = DEFAULT_WINDOW_THICKNESS,
                                 bool mSideBySide = true);
    
    void            create_components( );
    
    float       m_full_width;
    float       m_full_height;
    float       m_sill_height;
    
    glFrame               m_piece1;
    glFrame               m_piece2;
};


#endif /* defined(__home3D__gl_window__) */
