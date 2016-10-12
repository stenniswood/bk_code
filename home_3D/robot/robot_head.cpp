//
//  robot_head.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/22/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "robot_head.h"
#include "all_objects.h"



glHead::glHead( )
{
    
}
glHead::~glHead(  )
{
    
}
void glHead::look_left ( float mDegrees )
{
    m_y_angle = -mDegrees;
}

void glHead::look_right( float mDegrees )
{
    m_y_angle = mDegrees;
}

void glHead::set_tilt  ( float mDegrees )
{
    m_x_angle = mDegrees;
}

void glHead::create_components( )
{
    m_head = new glSphere(HEAD_RADIUS, 24, 24);
    m_head->set_color( 0xFFFFFF7F );
    m_head->setup();
    m_components.push_back( m_head );

    m_eye1 = new glSphere(EYE_RADIUS, 24, 24);
    m_eye1->set_color( 0xFF7F7F7F );
    m_eye1->setup();
    m_components.push_back( m_eye1 );
    m_eye1->relocate( 2*EYE_RADIUS, 0.0, HEAD_RADIUS );

    m_eye2 = new glSphere(EYE_RADIUS, 24, 24);
    m_eye2->set_color( 0xFF7F7F7F );
    m_eye2->setup();
    m_components.push_back( m_eye2 );
    m_eye2->relocate( -2*EYE_RADIUS, 0.0, HEAD_RADIUS );
}

/* In head coordinates */
MathVector glHead::get_left_coordinate     (bool mPupil)
{
    MathVector eyeL("Left",3);
    eyeL[0] = 2*EYE_RADIUS;
    eyeL[1] = 0.0;
    eyeL[2] = HEAD_RADIUS;
    if (mPupil)
        eyeL[2] = HEAD_RADIUS+EYE_RADIUS;
    return eyeL;
}
/* In head coordinates */
MathVector glHead::get_right_coordinate    (bool mPupil)
{
    MathVector eyeR("Right",3);
    eyeR[0] = 2*EYE_RADIUS;
    eyeR[1] = 0.0;
    eyeR[2] = HEAD_RADIUS;
    if (mPupil)
        eyeR[2] = HEAD_RADIUS+EYE_RADIUS;
    return eyeR;
}

/* In head coordinates */
MathVector glHead::get_cyclops_coordinate  ()
{
    MathVector eyeC("Cylops",3);
    eyeC[0] = 0;
    eyeC[1] = 0;
    eyeC[2] = HEAD_RADIUS+4*EYE_RADIUS;       // we extend out past the eye pupils.  b/c the eyes protrude and block the view!
    return eyeC;
}

glm::vec4 glHead::get_cyclops_fov_center_coordinate  ()
{
    glm::vec4 center;
    center[0] = 0;
    center[1] = 0;
    center[2] = HEAD_RADIUS + 10*HEAD_RADIUS;   // we extend out past the eye pupils.  b/c the eyes protrude and block the view!
    center[3] = 1.0;
    return center;
}




