//
//  robot_gripper.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/22/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include <fstream>
#include <iostream>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "all_objects.h"
#include "robot_gripper.h"


glFinger::glFinger()
: m_proximal(24),m_middle(24),m_distal(24)
{
    
}
glFinger::~glFinger()
{
    
}
void glFinger::create_components  (  )
{
    m_proximal.set_color  (0xFFFFFF2F);
    m_proximal.m_radius = FINGER_RADIUS;
    m_proximal.set_height (1.0);
    m_proximal.setup();
    m_components.push_back( &m_proximal );

    m_middle.set_color(0xFFFF0F0F);
    m_middle.m_radius = FINGER_RADIUS;
    m_middle.set_height(1.0);
    m_middle.setup();
    m_components.push_back( &m_middle );

    m_distal.set_color(0xFF2F2F2F);
    m_distal.m_radius = FINGER_RADIUS;
    m_distal.set_height(0.5);
    m_distal.setup();
    m_components.push_back( &m_distal );

    // POSITION :
    m_proximal.relocate( 0.0, 0.0, 0.0 );
    float dz = m_proximal.get_height();

    m_middle.relocate( 0.0, dz, 0.0 );
    m_middle.m_x_angle = 45;
    dz += m_middle.get_height();
    
    m_distal.relocate( 0.0, dz, 0.0 );
    m_distal.m_x_angle = 30;
}

void glFinger::set_proximal_angle (float mDegrees)
{
    m_proximal.m_y_angle = mDegrees;
}
void glFinger::set_middle_angle   (float mDegrees)
{
    m_middle.m_y_angle = mDegrees;
}
void glFinger::set_distal_angle   (float mDegrees)
{
    m_distal.m_y_angle = mDegrees;
}

void glFinger::draw_body ( )
{
    glPushMatrix();
    
    glTranslatef( m_proximal.m_x, m_proximal.m_y, m_proximal.m_z );
    glRotatef   ( m_proximal.m_x_angle,   1.0, 0.0, 0.0 );
    //glRotatef   ( m_proximal.m_y_angle,   0.0, 1.0, 0.0 );
    m_proximal.draw_body();

//    glTranslatef( m_middle.m_x,  m_middle.m_y, m_middle.m_z );
    glTranslatef( 0.0, m_proximal.get_height(), 0.0 );
    glRotatef   ( m_middle.m_x_angle,    1.0, 0.0, 0.0 );
    m_middle.draw_body();
    
    glTranslatef( 0.0, m_middle.get_height(), 0.0 );
    glRotatef   ( m_distal.m_x_angle,    1.0, 0.0, 0.0  );
    m_distal.draw_body();
    
    glPopMatrix();
}


/*********************************************************************/

/*********************************************************************/

glThumb::glThumb()
: m_proximal(24), m_distal(24)
{
    
}
glThumb::~glThumb()
{
    
}
void     glThumb::create_components()
{
    m_proximal.setup  ( );
    m_distal.setup    ( );
    
    m_metacarpal.width  = 1.0;
    m_metacarpal.height = FINGER_RADIUS;
    m_metacarpal.depth  = FINGER_RADIUS;
    m_metacarpal.setup();

    m_components.push_back( &m_proximal   );
    m_components.push_back( &m_distal     );
    m_components.push_back( &m_metacarpal );

    m_metacarpal.relocate( 0, 0, 0  );
    float dz = m_metacarpal.width;
    m_proximal.relocate ( 0, 0, 0 );
    
    dz += m_proximal.get_height();
    m_distal.relocate   ( 0, dz, 0  );

}
void glThumb::set_metacarpal_angle (float mDegrees)
{
    m_metacarpal.m_y_angle = mDegrees;
}

void glThumb::set_proximal_angle (float mDegrees)
{
    m_proximal.m_y_angle = mDegrees;
}
void glThumb::set_distal_angle (float mDegrees)
{
    m_distal.m_y_angle = mDegrees;
}


/*********************************************************************/

/*********************************************************************/

glGripper::glGripper( )
{
    
}

void glGripper::create_components( )
{
    m_first.setup ();
    m_second.setup();
    m_third.setup ();

    m_hand.width  = 3.;
    m_hand.height = 0.5;
    m_hand.depth  = 2.5;
    m_hand.set_color( 0xFFFF2f);
    m_hand.setup  ();
    m_hand.grab_front();

    m_thumb.setup ();

    m_components.push_back( &m_first  );
    m_components.push_back( &m_second );
    m_components.push_back( &m_third  );
    m_components.push_back( &m_hand   );
    m_components.push_back( &m_thumb  );
    
    m_first.relocate ( -m_hand.width/2 +0.0, m_hand.depth, 0.0 );
    m_second.relocate( -m_hand.width/2 +1.0, m_hand.depth, 0.0 );
    m_third.relocate ( -m_hand.width/2 +2.0, m_hand.depth, 0.0 );

    m_hand.relocate  ( -0.5, 0.0, 0.0 );
    m_hand.m_x_angle = 90;
    m_thumb.relocate ( -m_hand.width/2 +0.0, m_hand.depth/2, 0.0 );
}

void glGripper::fingers_straight     ( )
{
    m_first.set_proximal_angle( 0.0 );
    m_second.set_proximal_angle( 0.0 );
    m_third.set_proximal_angle( 0.0 );
    
    m_first.set_middle_angle( 0.0 );
    m_second.set_middle_angle( 0.0 );
    m_third.set_middle_angle( 0.0 );
    
    m_first.set_distal_angle ( 0.0 );
    m_second.set_distal_angle( 0.0 );
    m_third.set_distal_angle ( 0.0 );
}

void glGripper::extract_angles  ( struct stGripperPosition* mGrip )
{
    mGrip->first_proximal  =    m_first.m_proximal.m_y_angle;
    mGrip->second_proximal =    m_second.m_proximal.m_y_angle;
    mGrip->third_proximal  =    m_third.m_proximal.m_y_angle;
    
    
    mGrip->first_middle     = m_first.m_middle.m_y_angle;
    mGrip->second_middle    = m_second.m_middle.m_y_angle;
    mGrip->third_middle     = m_third.m_middle.m_y_angle;
    
    
    mGrip->first_distal  =  m_first.m_distal.m_y_angle;
    mGrip->second_distal =  m_second.m_distal.m_y_angle;
    mGrip->third_distal  =  m_third.m_distal.m_y_angle;
    
    mGrip->thumb_metacarpal = m_thumb.m_metacarpal.m_y_angle;
    mGrip->thumb_proximal   = m_thumb.m_proximal.m_y_angle;
    mGrip->thumb_distal     = m_thumb.m_distal.m_y_angle;
    
}

void glGripper::set_angles      ( struct stGripperPosition* mGrip )
{
    m_first.set_proximal_angle  ( mGrip->first_proximal );
    m_second.set_proximal_angle ( mGrip->second_proximal);
    m_third.set_proximal_angle  ( mGrip->third_proximal );
    
    m_first.set_middle_angle    ( mGrip->first_middle  );
    m_second.set_middle_angle   ( mGrip->second_middle );
    m_third.set_middle_angle    ( mGrip->third_middle  );
    
    m_first.set_distal_angle    ( mGrip->first_distal  );
    m_second.set_distal_angle   ( mGrip->second_distal );
    m_third.set_distal_angle    ( mGrip->third_distal  );

    m_thumb.set_metacarpal_angle( mGrip->thumb_metacarpal );
    m_thumb.set_proximal_angle  ( mGrip->thumb_proximal   );
    m_thumb.set_distal_angle    ( mGrip->thumb_distal     );
}

void load_angles( ifstream& ifile, struct stGripperPosition* gp )
{
    int num_floats = sizeof(struct stGripperPosition) / sizeof(float);
    float*     ptr = (float*) gp;
    string move, head,tail;
    
    getline( ifile, head);
    for (int i=0; i<num_floats; i++)
    {
        ifile >> *(ptr+i);
    }
    getline( ifile, tail);  // not sure why, but this is needed.
    getline( ifile, tail);
}

void glGripper::read_angles     ( ifstream& ifile )
{
    struct stGripperPosition position;
    load_angles( ifile, &position );
    set_angles ( &position);
}

void glGripper::write_angles( ofstream& ofile )
{
    struct stGripperPosition gp;
    extract_angles(&gp);

    int num_floats = sizeof(struct stGripperPosition) / sizeof(float);
    float* ptr = (float*) &gp;
    
    ofile << "--GripperPosition" << endl;
    for (int i=0; i<num_floats; i++)
        ofile << *(ptr+i) << endl;
    ofile << "--End GripperPosition" << endl;
    
}


void glGripper::fingers_bend_proximal( float mAngle )
{
    m_first.set_proximal_angle ( mAngle );
    m_second.set_proximal_angle( mAngle );
    m_third.set_proximal_angle ( mAngle );
}
void glGripper::fingers_bend_mid     ( float mAngle )
{
    m_first.set_middle_angle ( mAngle );
    m_second.set_middle_angle( mAngle );
    m_third.set_middle_angle ( mAngle );
}
void glGripper::fingers_bend_tips    ( float mAngle )
{
    m_first.set_distal_angle ( mAngle );
    m_second.set_distal_angle( mAngle );
    m_third.set_distal_angle ( mAngle );
}

// one pointing finger
void glGripper::light_switch( )
{
    m_first.set_proximal_angle(0.0);
    m_first.set_middle_angle  (0.0);
    m_first.set_distal_angle  (0.0);
    
    m_second.set_proximal_angle(90);
    m_second.set_middle_angle  (90);
    m_second.set_distal_angle  (90);
    
    m_third.set_proximal_angle(90);
    m_third.set_middle_angle  (90);
    m_third.set_distal_angle  (90);
}
