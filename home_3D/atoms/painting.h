//
//  painting.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/24/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__painting__
#define __home3D__painting__

#include <stdio.h>
#include "paper.hpp"
#include "gl_box.hpp"
#include "glMolecule.h"
#include "frame.h"

/* A painting has a frame around it. 
    Ordinary rectangular frame.
 */
class glPainting : public glMolecule
{
public:
    glPainting();
    ~glPainting();

    void            select_work_of_art( int mIndex );
    // may also use load an image with:
    // m_painting->load_texture()

    virtual void    create_components();
    
    glPaper* m_painting;
    glFrame* m_frame;
    glFrame* m_matting;
    
    float   m_frame_width;
    float   m_matting_width;
};


#endif /* defined(__home3D__painting__) */
