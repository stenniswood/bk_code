//
//  robot_arm.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 2/22/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
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
#include "robot_arm.hpp"



glArm::glArm( )
: m_upper_arm( UPPER_ARM_LENGTH ),
  m_fore_arm ( FORE_ARM_LENGTH )
{
    Initialize();
}

void glArm::Initialize( )
{
    m_left_arm              = false;
    m_upper_arm.m_color     = 0xFFDF9000;
    m_upper_arm.m_ends_color= 0xFFDF9000;
    m_fore_arm.m_color      = 0xFFD0902F;
    m_fore_arm.m_ends_color = 0xFFD0902F;

    m_shoulder_angle        = 20.0;
    m_shoulder_rotate_angle = 0.0;
    m_elbow_angle           = 30.0;
    m_wrist_angle           = 0.0;
    m_wrist_rotate_angle    = 0.0;
}

void glArm::create_components( )
{
    m_upper_arm.setup();
    m_fore_arm.setup ();
    m_gripper.setup  ();
    m_components.push_back( &m_upper_arm );
    m_components.push_back( &m_fore_arm  );
    m_components.push_back( &m_gripper   );

    
    m_shoulder_pan = new glBox();
    m_shoulder_pan->set_color( 0xFF7f7f00 );
    m_shoulder_pan->width  = 5.0;
    m_shoulder_pan->height = 5.0;
    m_shoulder_pan->depth  = 1.0;
    m_shoulder_pan->setup();
    m_shoulder_pan->gl_register();
    
    // Zero is top of upper arm.
    float offset = m_upper_arm.m_extrusion_length + FORK_CENTER_LENGTH;
    m_upper_arm.add_offset( 0, -offset, 0 );    // ie hold at top

    // Zero is elbow of fore arm:
    offset = m_fore_arm.m_extrusion_length + FORK_CENTER_LENGTH;
    m_fore_arm.add_offset( 0, -offset, 0 );    // ie hold fore arm at its top

    // Want it to pivot at the elbow:
    offset += TONGUE_CENTER_LENGTH;
    m_upper_arm.relocate( 0.0,  0.,  0.0 );
    //m_fore_arm.relocate ( 0.0,  -UPPER_ARM_LENGTH,  0.0 );
    m_fore_arm.relocate ( 0.0,  -m_upper_arm.m_joint_length,  0.0 );

    position_gripper();
}

void glArm::reposition_fore_arm()        // updates the translates based on upper arm.
{
    // NO!  Don't do this because Everything is relative coordinates.
    
    MathVector fa(3);
    fa[0]=0.0;    fa[1]=-m_upper_arm.m_joint_length;        fa[2]=0.0;
    //fa = m_upper_arm.map_coords(fa);
    m_fore_arm.relocate ( fa[0],  fa[1],  fa[2] );
}

// How to find the end of the arm?
// What coordinates to put into?
void glArm::position_gripper()
{
/* has no bearing since drawn with draw_body() skipping translation!
 m_gripper.relocate( 50, 5, 50 ); */
    
    m_gripper.m_x_angle = 0.0;
    m_gripper.m_y_angle = 0.0;
    m_gripper.m_z_angle = 0.0;
}

bool glArm::evaluate_angles()
{
    if (m_shoulder_rotate_angle > MAX_SHOULDER_ROTATE_ANGLE)  		return false;
    if (m_shoulder_rotate_angle < MIN_SHOULDER_ROTATE_ANGLE)  		return false;

    if (m_upper_arm_rotate_angle > MAX_UPPERARM_ROTATE_ANGLE)  		return false;
    if (m_upper_arm_rotate_angle < MIN_UPPERARM_ROTATE_ANGLE)  		return false;
    
    if (m_shoulder_angle > MAX_SHOULDER_SWING_ANGLE)  		return false;
    if (m_shoulder_angle < MIN_SHOULDER_SWING_ANGLE)  		return false;
    
    if (m_elbow_angle > MAX_ELBOW_ANGLE)                    return false;
    if (m_elbow_angle < MIN_ELBOW_ANGLE)                    return false;
    
    if (m_wrist_angle > MAX_WRIST_ANGLE)                    return false;
    if (m_wrist_angle < MIN_WRIST_ANGLE)                    return false;
 
    if (m_wrist_rotate_angle > MAX_WRIST_ROTATE_ANGLE)  	return false;
    if (m_wrist_rotate_angle < MIN_WRIST_ROTATE_ANGLE)  	return false;

    return true;
}

bool glArm::set_shoulder_angle( float mAngle )
{
    if (mAngle > MAX_SHOULDER_SWING_ANGLE)  return false;
    if (mAngle < MIN_SHOULDER_SWING_ANGLE)  return false;
    if (m_left_arm)
        m_shoulder_angle = mAngle;
    else
        m_shoulder_angle = -mAngle;
    
    transfer_angles();
    reposition_fore_arm();
    return true;
}

bool glArm::set_shoulder_rotate_angle( float mAngle )
{
    if (mAngle > MAX_SHOULDER_ROTATE_ANGLE)  return false;
    if (mAngle < MIN_SHOULDER_ROTATE_ANGLE)  return false;
    m_shoulder_rotate_angle = mAngle;
    transfer_angles();
    reposition_fore_arm();
    return true;
}
bool glArm::set_upper_arm_rotate_angle( float mAngle )
{
    if (mAngle > MAX_UPPERARM_ROTATE_ANGLE)  return false;
    if (mAngle < MIN_UPPERARM_ROTATE_ANGLE)  return false;
    if (m_left_arm)
        m_upper_arm_rotate_angle = mAngle;
    else
        m_upper_arm_rotate_angle = -mAngle;
    transfer_angles();
    reposition_fore_arm();
    return true;
}

float glArm::get_shoulder_angle       (  )
{
    if (m_left_arm)
        return m_shoulder_angle;
    else
        return -m_shoulder_angle;
}
float glArm::get_shoulder_rotate_angle( )
{
    return m_shoulder_rotate_angle;
}
float glArm::get_upper_arm_rotate_angle( )
{
    if (m_left_arm)
        return m_upper_arm_rotate_angle;
    else
        return -m_upper_arm_rotate_angle;    
}



bool glArm::set_elbow_angle         ( float mAngle )
{
    if (mAngle > MAX_ELBOW_ANGLE)  return false;
    if (mAngle < MIN_ELBOW_ANGLE)  return false;
    m_elbow_angle = mAngle;
    return true;
}
bool glArm::set_wrist_angle         ( float mAngle )
{
    if (mAngle > MAX_WRIST_ANGLE)  return false;
    if (mAngle < MIN_WRIST_ANGLE)  return false;
    m_wrist_angle = mAngle;
    return true;
}
bool glArm::set_wrist_rotate_angle ( float mAngle )
{
    if (mAngle > MAX_WRIST_ROTATE_ANGLE)  return false;
    if (mAngle < MIN_WRIST_ROTATE_ANGLE)  return false;
    m_wrist_rotate_angle = mAngle;
    return true;
}

bool glArm::increase_shoulder_angle	( float mRelativeAngle )
{
    float  angle = m_shoulder_angle + mRelativeAngle;
    if (m_left_arm) {
        if (angle > MAX_SHOULDER_SWING_ANGLE)
            angle = MAX_SHOULDER_SWING_ANGLE;
        if (angle < MIN_SHOULDER_SWING_ANGLE)
            angle = MIN_SHOULDER_SWING_ANGLE;
        m_shoulder_angle = angle;
    } else {
        angle = -m_shoulder_angle + mRelativeAngle;
        if (angle > MAX_SHOULDER_SWING_ANGLE)
            angle = MAX_SHOULDER_SWING_ANGLE;
        if (angle < MIN_SHOULDER_SWING_ANGLE)
            angle = MIN_SHOULDER_SWING_ANGLE;
        m_shoulder_angle = -angle;
    }
    transfer_angles();
    return true;
}
bool glArm::increase_shoulder_rotate_angle( float mRelativeAngle )
{
    m_shoulder_rotate_angle += mRelativeAngle;
    if (m_shoulder_rotate_angle > MAX_SHOULDER_ROTATE_ANGLE)
        m_shoulder_rotate_angle = MAX_SHOULDER_ROTATE_ANGLE;
    if (m_shoulder_rotate_angle < MIN_SHOULDER_ROTATE_ANGLE)
        m_shoulder_rotate_angle = MIN_SHOULDER_ROTATE_ANGLE;
    transfer_angles();
    return true;
}
bool glArm::increase_upper_arm_rotate_angle( float mRelativeAngle )
{
    m_upper_arm_rotate_angle += mRelativeAngle;
    if (m_upper_arm_rotate_angle > MAX_UPPERARM_ROTATE_ANGLE)
        m_upper_arm_rotate_angle = MAX_UPPERARM_ROTATE_ANGLE;
    if (m_upper_arm_rotate_angle < MIN_UPPERARM_ROTATE_ANGLE)
        m_upper_arm_rotate_angle = MIN_UPPERARM_ROTATE_ANGLE;
    transfer_angles();
    return true;
}

bool glArm::increase_elbow_angle	( float mRelativeAngle )
{
    m_elbow_angle += mRelativeAngle;
    if (m_elbow_angle > MAX_ELBOW_ANGLE)
        m_elbow_angle = MAX_ELBOW_ANGLE;
    if (m_elbow_angle < MIN_ELBOW_ANGLE)
        m_elbow_angle = MIN_ELBOW_ANGLE;
    return true;
}
bool glArm::increase_wrist_angle( float mRelativeAngle )
{
    m_wrist_angle += mRelativeAngle;
    if (m_wrist_angle > MAX_WRIST_ANGLE)  m_wrist_angle = MAX_WRIST_ANGLE;
    if (m_wrist_angle < MIN_WRIST_ANGLE)  m_wrist_angle = MIN_WRIST_ANGLE;
    return true;
}
bool glArm::increase_wrist_rotate_angle( float mRelativeAngle )
{
    m_wrist_rotate_angle += mRelativeAngle;
    if (m_wrist_rotate_angle > MAX_WRIST_ROTATE_ANGLE)  m_wrist_rotate_angle = MAX_WRIST_ROTATE_ANGLE;
    if (m_wrist_rotate_angle < MIN_WRIST_ROTATE_ANGLE)  m_wrist_rotate_angle = MIN_WRIST_ROTATE_ANGLE;
    return true;
}

float glArm::get_arm_length  ()
{
    float length = m_upper_arm.m_joint_length + m_fore_arm.m_joint_length;
    return length;
}


#include <math.h>

/*static float degrees( float angle )
{  return angle*180./M_PI; } */

/*
   Incoming coordinates must be in robot coords
 
 Assume isoceles triangle (ie. the upper and fore arm lengths are the same)
 mX is robot width axis.
 mZ is fore/aft axis.
 mY is up/down axis.
*/
bool glArm::inverse_xyz( float mX, float mY, float mZ )
{
    float deltaX = (mX - m_x);      // x is robot width!
    float deltaY = (mY - m_y);
    float deltaZ = (mZ - m_z);      // fore/aft
    float distance = sqrt( deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ );
    float elbow_angle;

    float arm_length = (m_upper_arm.m_joint_length + m_fore_arm.m_joint_length);
    if (distance >= arm_length)
        return false;
    else
        elbow_angle = asin( (distance/2.) / m_upper_arm.m_joint_length) * 2.;

    elbow_angle      = degrees( elbow_angle );
    m_elbow_angle    = 180. - elbow_angle;

    float phi        = 90. - elbow_angle/2.;
    float elevation  = degrees( atan2(deltaY,deltaZ) );
    m_shoulder_angle = 90. + elevation - phi;
    
//    printf("mX=%6.3f; mY=%6.3f; mZ=%6.2f; dist=%6.3f   m_elbow=%6.2f; shoulder=%6.3f\n",
//            mX, mY,  mZ, distance, m_elbow_angle, m_shoulder_angle );
    return true;
    // x is robot width
    // m_shoulder_rotate_angle = degrees(atan2( deltaX, deltaZ ));
}

void glArm::forward_xyz( float *mX, float *mY, float *mZ )
{
    glm::mat4 ua_bm = m_upper_arm.get_body_matrix();
    glm::mat4 fa_bm = m_fore_arm.get_body_matrix();
    glm::mat4 bm = ua_bm * fa_bm;
    
    glm::vec4 wrist_loc;
    wrist_loc[0] = 0.0;  wrist_loc[1] = m_fore_arm.m_joint_length;  wrist_loc[2] = 0.0;     wrist_loc[3] = 1.0;
    glm::vec4 arm_coord;
    arm_coord = bm * wrist_loc;
}

/* forward calculation :
    Result is in Robot coordinates (because mapped thru upper arm)
*/
MathVector glArm::get_elbow_position(  )
{
    transfer_angles();
    
    // VECTOR IN Upper (Arm Coorindates) :
    glm::vec4 elbow_loc;
    elbow_loc[0]=0.0;  elbow_loc[1]= -m_upper_arm.m_joint_length;  elbow_loc[2] = 0.0;  elbow_loc[3] = 0.0; //1.0 should still work!

    // UPPER ARM TRANSFORM MATRIX:
    glm::mat4 upper_arm_bm  = m_upper_arm.get_body_matrix(0, 2, 1);

    // MAP IT:
    glm::vec4 elbow_loc_mapd = upper_arm_bm * elbow_loc;    // Upper Arm coords -> Arm cooridnates.
    elbow_loc_mapd[3] = 1.0;

    glm::vec4 elbow_loc_robot_coords;
    elbow_loc_robot_coords = map_coords( elbow_loc_mapd );  // Arm Coords -> Robot coords.
    
    MathVector arm_coord(3);
    arm_coord[0] = elbow_loc_robot_coords[0];
    arm_coord[1] = elbow_loc_robot_coords[1];
    arm_coord[2] = elbow_loc_robot_coords[2];
    return arm_coord;

    //    glm::vec4 elbow_loc_mapd     =  m_upper_arm.map_coords( elbow_loc );
    //
    //wrist_loc3 = m_upper_arm.map_coords( wrist_loc2 );
    /*wrist_loc4[0] = elbow_loc_mapd[0]+m_x;
     wrist_loc4[1] = elbow_loc_mapd[1]+m_y;
     wrist_loc4[2] = elbow_loc_mapd[2]+m_z; */
}

MathVector glArm::get_wrist_position(  )
{
    transfer_angles();

    // FORM MATRICES:
    glm::mat4 upper_arm_bm  = m_upper_arm.get_body_matrix(0, 2, 1);
    
    // To go from Upper arm to Fore arm:
    //m_fore_arm.relocate( 0.0, -m_upper_arm.m_joint_length, 0.0 );     // upper arm coordinates.
    glm::mat4 fore_arm_bm   = m_fore_arm.get_body_matrix(0, 1, 2);

    // WRIST LOCATION (in fore arm coords):
    glm::vec4 wrist_loc ;
    glm::vec4 wrist_loc2;
    glm::vec4 wrist_loc3;
    glm::vec4 wrist_loc4;
    wrist_loc[0]=0.0;  wrist_loc[1]= -m_fore_arm.m_joint_length;  wrist_loc[2] = 0.0;  wrist_loc[3] = 1.0;

    wrist_loc2 = fore_arm_bm * wrist_loc;       // in upper arm coordinates
    wrist_loc3 = upper_arm_bm * wrist_loc2;     // now in arm coords
    wrist_loc3[3] = 1.0;
    
    //glm::vec4 elbow_loc_mapd     =  m_upper_arm.map_coords( elbow_loc );  // need to make this orderable.
    //wrist_loc3 = m_upper_arm.map_coords( wrist_loc2 );
    //wrist_loc4 = map_coords( elbow_loc_mapd );
    wrist_loc4 = map_coords( wrist_loc3 );
    
    MathVector arm_coord(3);
    arm_coord[0] = wrist_loc4[0];
    arm_coord[1] = wrist_loc4[1];
    arm_coord[2] = wrist_loc4[2];
    return arm_coord;
}

// Inverse equations :
void glArm::set_wrist_position       ( MathVector& mArmCoord )
{
    
}

/****************************************/
MathVector      glArm::get_index_finger_position (  )                          /* in Robot coordinates */
{
    MathVector retval(3);
    
    return retval;
}
MathVector      glArm::get_middle_finger_position(  )                          /* in Robot coordinates */
{
    MathVector retval(3);
    
    return retval;
}
MathVector      glArm::get_pinky_finger_position (  )                          /* in Robot coordinates */
{
    MathVector retval(3);
    
    return retval;
}
MathVector      glArm::get_thumb_position (  )                                 /* in Robot coordinates */
{
    MathVector retval(3);
    
    return retval;
}
glFinger*      glArm::get_index_finger (  )
{
    return &(m_gripper.m_pointing);
}
glFinger*      glArm::get_middle_finger(  )
{
    return &(m_gripper.m_index);
}
glFinger*      glArm::get_pinky_finger (  )
{
    return &(m_gripper.m_pinky);
}
glThumb*       glArm::get_thumb        (  )
{
    return &(m_gripper.m_thumb);
}
/****************************************/



// Call repetitively to scan:
void glArm::move_arms_up_down( float mForeAft )
{
    static float y = 0.0;
    static float x = 0.0;
    static int   state = 0.0;
    const float maxY  = 12.0;
    const float maxX  = 12.0;
    const float increment = 0.5;
    switch ( state )
    {
    case 0: if (y < maxY)  {
            y += increment;
            inverse_xyz( m_x, m_y+y, mForeAft );
        } else {
            state = 1;
        }
        break;
    case 1: if (y > -maxY) {
            y -= increment;
            inverse_xyz( m_x, m_y+y, mForeAft );
        } else {
            state = 2;
        }
        break;
    case 2: if (x < maxX) {
            x  += increment;
            inverse_xyz( m_x, m_y+y, mForeAft+x );
        } else {
            state = 3;
        }
        break;
    case 3: if (x > 0) {
            x  -= increment;
            inverse_xyz( m_x, m_y+y, mForeAft+x );
        } else {
            state = 0;
        }
            break;

        default: break;
    }
}
using namespace glm;

void glArm::transfer_angles  ()
{
    m_upper_arm.m_x_angle = m_shoulder_rotate_angle;       //
    m_upper_arm.m_y_angle = m_upper_arm_rotate_angle;               // about the plane defined by m_shoulder_rotate_angle
    m_upper_arm.m_z_angle = m_shoulder_angle;       //

    m_fore_arm.m_x_angle = -m_elbow_angle;
    m_fore_arm.m_y_angle = 0.0;
    m_fore_arm.m_z_angle = 0.0;

    m_gripper.m_z_angle  =  m_wrist_rotate_angle;
    m_gripper.m_x_angle  =  m_wrist_angle;
}


/*
          Returns a matrix with translate and rotations (same as drawn) for compute purposes.
 INPUTS:  allow different ordering of the rotations (default arguments go rotate X,rotate Y,rotate Z)
 */
#define arm_fa_rotate           m_shoulder_rotate_angle         // This is 'i' key!
#define arm_shoulder_angle      m_shoulder_angle                // This is 'o' key!
#define arm_longitudinal_angle  m_upper_arm_rotate_angle


/* These angles are relative to begin with!  So there never was a need to specify the
    shoulder angle about an absolute rotated vector.  Why did I think I had too?!
    2 Days lost on this!! Argggh!
 */

void glArm::draw_body ( )
{
    glPushMatrix();
    transfer_angles();
    
    glTranslatef( m_upper_arm.m_x, m_upper_arm.m_y, m_upper_arm.m_z );
    
    // MOTOR #1 ROTATION :
    glRotatef( m_shoulder_rotate_angle, 1.0, 0.0, 0.0f );
    m_shoulder_pan->draw_body();
    // draw disc too.
    
    // MOTOR #2 ROTATION :
    glRotatef( m_shoulder_angle, 0, 0, 1 );
    
    // MOTOR #3 ROTATION:
    glRotatef( m_upper_arm_rotate_angle, 0, 1, 0 );
    m_upper_arm.draw_body();

    // ELBOW:
    glTranslatef( 0, -m_upper_arm.m_joint_length, 0 );
    glRotatef   ( m_fore_arm.m_x_angle,    1.0, 0.0, 0.0 );
    //m_fore_arm.m_y_angle = 0;
    //m_fore_arm.m_z_angle = 0;
    m_fore_arm.draw_body();

    
    glTranslatef( 0.0, -m_fore_arm.m_joint_length+0, 0.0 );

    glRotatef   ( m_wrist_angle+180,    1.0, 0.0, 0.0 );
    glRotatef   ( m_wrist_rotate_angle, 0.0, 1.0, 0.0 );
    m_gripper.draw_body();
    
    glPopMatrix();
}

