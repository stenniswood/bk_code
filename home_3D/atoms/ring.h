//
//  ring.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/30/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__ring__
#define __home3D__ring__

#include <stdio.h>
#include "gl_atom.hpp"

class glRing : public glAtom
{
public:
    glRing();
    ~glRing();

    void    set_finger_params( float mRadius, int mSlices, float mStartAngle=0, float mEndAngle=360 );
    void    set_circle_params( float mRadius, int mSamples );

    virtual void    generate_vertices( );
    virtual void 	generate_indices( );
    virtual void    generate_indices2( );
    void    draw_primitive   ( );
    
    // Ring Slice Info:
    float   m_start_degrees;
    float   m_end_degrees;
    float   m_finger_radius;
    float   m_number_of_rings;
    
    // Cirlce Info:
    float   m_circle_radius;
    int     m_number_of_samples;
    
};

#endif /* defined(__home3D__ring__) */
