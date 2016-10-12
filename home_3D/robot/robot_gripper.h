//
//  robot_gripper.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/22/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__robot_gripper__
#define __home3D__robot_gripper__

#include <stdio.h>
#include <vector>
//#include "all_objects.h"
#include "gl_atom.hpp"
#include "glMolecule.h"
#include "gl_container.hpp"
#include "cylinder.hpp"



const float FINGER_RADIUS = 0.25;

class glFinger : public glMolecule
{
public:
    glFinger();
    ~glFinger();
    
    void    create_components  (  );
    virtual void     draw_body (  );    
    
    void    set_proximal_angle (float mDegrees);
    void    set_middle_angle   (float mDegrees);
    void    set_distal_angle   (float mDegrees);
    
    glCylinder  m_proximal;
    glCylinder  m_middle;
    glCylinder  m_distal;
};

class glThumb : public glMolecule
{
public:
    glThumb();
    ~glThumb();
    void        create_components();

    void        set_metacarpal_angle( float mDegrees );
    void        set_proximal_angle  ( float mDegrees );
    void        set_distal_angle    ( float mDegrees );

    glCylinder  m_proximal;
    glCylinder  m_distal;
    glBox       m_metacarpal;
};

#define m_first  m_pointing
#define m_second m_index
#define m_third  m_pinky

struct stGripperPosition
{
    float first_proximal;
    float first_middle;
    float first_distal;

    float second_proximal;
    float second_middle;
    float second_distal;

    float third_proximal;
    float third_middle;
    float third_distal;
    
    float thumb_metacarpal;
    float thumb_proximal;
    float thumb_distal;
};

class glGripper : public glMolecule
{
public:
    glGripper   ( );
    void        create_components( );
    
    void        extract_angles  ( struct stGripperPosition* mGrip );
    void        set_angles      ( struct stGripperPosition* mGrip );
    void        read_angles     ( ifstream& ifile );
    void        write_angles    ( ofstream& ofile );

    // Same applied to all fingers :
    void        fingers_straight     ( );
    void        fingers_bend_proximal( float mAngle );
    void        fingers_bend_mid     ( float mAngle );
    void        fingers_bend_tips    ( float mAngle );
    
    // Task Specific:
    void        light_switch    ( );
    
    glBox       m_hand;
    glFinger    m_pointing;
    glFinger    m_index;
    glFinger    m_pinky;
    glThumb     m_thumb;
};


#endif /* defined(__home3D__robot_gripper__) */
