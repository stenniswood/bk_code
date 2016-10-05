//
//  robot_head.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/22/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__robot_head__
#define __home3D__robot_head__

#include <stdio.h>
#include <vector>
//#include "all_objects.h"
#include "glMolecule.h"
#include "glSphere.h"
#include "vector.hpp"


const float HEAD_RADIUS = 6.0;
const float EYE_RADIUS  = 0.75;


/*  */
class glHead : public glMolecule
{
public:
    glHead( );
    ~glHead( );
    void	create_components( );

    void    look_left ( float mDegrees );
    void    look_right( float mDegrees );
    void    set_tilt  ( float mDegrees );
    
    // Use these for viewing what the robot views:
    MathVector    get_left_coordinate     (bool mPupil=false);      /* In head coordinates - center of eye (add EYE_RADIUS to Z for pupil) */
    MathVector    get_right_coordinate    (bool mPupil=false);      /* In head coordinates - center of eye (add EYE_RADIUS to Z for pupil) */
    MathVector    get_cyclops_coordinate  ();                       /* In head coordinates */
    glm::vec4     get_cyclops_fov_center_coordinate();              /* In head coordinates */
    
    glSphere*    m_head;
    glSphere*    m_eye1;
    glSphere*    m_eye2;
};


#endif /* defined(__home3D__robot_head__) */
