//
//  frame.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/26/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__frame__
#define __home3D__frame__

#include <stdio.h>
#include "gl_box.hpp"
#include "glMolecule.h"


/*
 Ordinary rectangular frame - for door ways, and paintings, etc.
 */
class glFrame : public glMolecule
{
public:
    glFrame();
    ~glFrame();
    virtual void    create_components();
    void        set_params( float mWidth, float mHeight, float mBorderWidth, float mThick);
    
    float   m_frame_thickness;
    float   m_border_width;
    float    m_width;       // inside width
    float    m_height;      // inside height
    
    bool        m_has_bottom;       // or open (ie for door frame)
    
    glBox*   m_left_side;
    glBox*   m_right_side;
    glBox*   m_top_side;
    glBox*   m_bottom_side;
};



#endif /* defined(__home3D__frame__) */
