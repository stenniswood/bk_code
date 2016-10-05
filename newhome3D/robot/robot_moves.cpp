//
//  robot_moves.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/9/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include <fstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "robot_moves.h"
#include "all_objects.h"



glStaticMovesRobot::glStaticMovesRobot()
{
    load_file("static_moves.txt");
}

glStaticMovesRobot::~glStaticMovesRobot()
{
    
}

void load_position( ifstream& ifile, struct stBodyPosition* bp )
{
    int num_floats = sizeof(struct stBodyPosition) / sizeof(float);
    float*     ptr = (float*) bp;
    string move, head,tail;
    
    getline( ifile, move);
    getline( ifile, head);
    for (int i=0; i<num_floats; i++)
    {
        ifile >> *(ptr+i);
    }
    getline( ifile, tail);  // not sure why, but this is needed.
    getline( ifile, tail);
}

/* Load ASCII file! Holds static positions.
    These are meant to be used for selecting different poses,
    however could also be used to store a sequence.
 */
void glStaticMovesRobot::load_file( string mFilename )
{
    struct stBodyPosition position;
    ifstream ifile;
    ifile.open( mFilename);
    if (ifile.fail())
    	return;
    while (ifile.eof()==false)                  // Read all positions within the file and store.
    {
        load_position( ifile, &position );
        m_positions.push_back( position );
        m_left_arm.m_gripper.read_angles ( ifile ); // ?? prolly gripper is treated separate from the rest of the body.
        m_right_arm.m_gripper.read_angles( ifile ); // ??
    }
    ifile.close();
}


void save_position( ofstream& ofile, struct stBodyPosition* bp )
{
    int num_floats = sizeof(struct stBodyPosition) / sizeof(float);
    float* ptr = (float*) bp;

    ofile << "--Start of BodyPosition" << endl;
    for (int i=0; i<num_floats; i++)
        ofile << *(ptr+i) << endl;
    ofile << "--End of BodyPosition" << endl;
}

void glStaticMovesRobot::save_file( string mFilename  )
{
    ofstream  outfile;
    outfile.open(mFilename);

    int count=1;
    struct stBodyPosition bp;
    bool   result = false;
    while (result==false)                           // Loop thru all positions within the demo() function.
    {
        result = demo();
        extract_body_pose(&bp);
        outfile << "Move #" << count++ << endl;
        save_position( outfile, &bp );
        m_left_arm.m_gripper.write_angles ( outfile );
        m_right_arm.m_gripper.write_angles( outfile );
    }
    outfile.close();
}

void glStaticMovesRobot::activate_position  ( int mIndex )
{
    set_body_pose( &m_positions[mIndex] );
}

float glStaticMovesRobot::compute_max_squat_distance()
{
    //float max_knee_angle = glm::radians(MAX_KNEE_ANGLE);
    float max_hip_angle  = glm::radians(MAX_KNEE_ANGLE / 2.);

    float upper_vertical = m_left_leg.m_upper_leg.m_joint_length * cos(max_hip_angle);
    // We use max_hip_angle below b/c - it represents the lower leg angle wrt to gravity.
    // the actual knee angle is twice that (first half achieves vertical with graivty ie. makes up
    // for the upper leg):
    float lower_vertical = m_left_leg.m_lower_leg.m_joint_length * cos(max_hip_angle);
    float upper_plus_lower_length = m_left_leg.m_upper_leg.m_joint_length +
                                    m_left_leg.m_lower_leg.m_joint_length;
    float delta = upper_plus_lower_length - upper_vertical - lower_vertical;
    return delta;
}


/*
 mDeltaY    : Distance to shrink from standing.
 */
void glStaticMovesRobot::squat( float mAngle )
{
    // (start simple - ie flat floor.  Later expand to hills)
    lift_left_leg ( mAngle );
    lift_right_leg( mAngle );
    float new_height = m_torso.height/2. + m_left_leg.get_vertical_height();
    relocate(m_x, new_height, m_z);
}

//#define MIN(a,b) (((a)<(b))?(a):(b))
//#define MAX(a,b) (((a)>(b))?(a):(b))

void glStaticMovesRobot::squat_fraction( float mfraction )
{
    float max_angle = MIN(MAX_HIP_ANGLE, MAX_KNEE_ANGLE );
    max_angle = MIN(max_angle, MAX_ANKLE_ANGLE );
    
    float angle = max_angle * mfraction;
    squat( angle );
}
void glStaticMovesRobot::squat_distance( float mDeltaY )
{
    // THE HEIGHT, will be: distance (ankle to Knee) + (knee to hip) + (ankle height)
    // Horizontal distance absorbed by legs.
    float new_height = m_torso.height/2. + m_left_leg.get_length() - mDeltaY;
    relocate(m_x, new_height, m_z);
    lift_leg_from_standing( mDeltaY, true );
    lift_leg_from_standing( mDeltaY, false);
}


void glStaticMovesRobot::stand(  )
{
    float prev_foreaft      = m_left_leg.get_fore_aft();
    //float upper_leg_length  = m_left_leg.m_upper_leg.m_joint_length;
    move_fore_aft( prev_foreaft );
        
    float standing_height    = m_left_leg.get_length();
    float torso_height = m_torso.height/2.;
    relocate( m_x, torso_height+standing_height, m_z );

    m_left_leg.set_hip_angle          ( 0.0 );
    m_left_leg.set_hip_rotate_angle   ( 0.0 );
    m_left_leg.set_hip_sideways_swing_angle( 0.0 );
    m_left_leg.set_knee_angle         ( 0.0 );
    m_left_leg.set_ankle_angle        ( 0.0 );
    
    m_right_leg.set_hip_angle         ( 0.0 );
    m_right_leg.set_hip_rotate_angle  ( 0.0 );
    m_right_leg.set_hip_sideways_swing_angle( 0.0 );
    m_right_leg.set_knee_angle        ( 0.0 );
    m_right_leg.set_ankle_angle       ( 0.0 );
}

void glStaticMovesRobot::sit(  )
{
    //float prev_foreaft      = m_left_leg.get_fore_aft();
    float upper_leg_length  = m_left_leg.m_upper_leg.m_joint_length;
    float delta             = upper_leg_length - 0;//prev_foreaft;
    move_fore_aft( -delta );
    
    m_left_leg.set_hip_angle          ( -90.0 );
    m_left_leg.set_hip_rotate_angle   ( 0.0  );
    m_left_leg.set_hip_sideways_swing_angle( 0.0  );
    m_left_leg.set_knee_angle         ( 90.0 );
    m_left_leg.set_ankle_angle        ( 0.0  );
    
    m_right_leg.set_hip_angle         ( -90.0 );
    m_right_leg.set_hip_rotate_angle  ( 0.0  );
    m_right_leg.set_hip_sideways_swing_angle( 0.0  );
    m_right_leg.set_knee_angle        ( 90.0 );
    m_right_leg.set_ankle_angle       ( 0.0  );

    float sitting_height2   = m_left_leg.get_vertical_height(false);
    float torso_height = m_torso.height/2.;
    relocate( m_x, torso_height+sitting_height2, m_z );
}

void glStaticMovesRobot::floor_sit(  )
{
    m_left_leg.set_hip_angle       ( 90.0 );
    m_left_leg.set_knee_angle      ( 0.0 );
    m_left_leg.set_ankle_angle     ( 0.0 );
    m_left_leg.set_hip_rotate_angle( 0.0 );

    m_right_leg.set_hip_angle       ( 90.0 );
    m_right_leg.set_knee_angle      ( 0.0 );
    m_right_leg.set_ankle_angle     ( 0.0 );
    m_right_leg.set_hip_rotate_angle( 0.0 );
}

void glStaticMovesRobot::one_knee_kneel     ( bool mLeft  )        // out in front
{
    if (mLeft)
    {
        m_left_leg.set_hip_angle          ( 0.0 );
        m_left_leg.set_hip_rotate_angle   ( 0.0 );
        m_left_leg.set_knee_angle         ( 90.0 );
        m_left_leg.set_ankle_angle        ( 90.0 );
        
        m_right_leg.set_hip_angle         ( -90.0 );
        m_right_leg.set_hip_rotate_angle  ( 0.0 );
        m_right_leg.set_knee_angle        ( 90.0 );
        m_right_leg.set_ankle_angle       (  0.0 );
        
    } else {

        m_right_leg.set_hip_angle         ( 0.0 );
        m_right_leg.set_hip_rotate_angle  ( 0.0 );
        m_right_leg.set_knee_angle        ( 90.0 );
        m_right_leg.set_ankle_angle       ( 90.0 );

        m_left_leg.set_hip_angle          ( -90.0 );        // should be defined so that +90 is forward!!
        m_left_leg.set_hip_rotate_angle   ( 0.0 );
        m_left_leg.set_knee_angle         ( 90.0 );
        m_left_leg.set_ankle_angle        (  0.0 );
    }
}
void glStaticMovesRobot::kneel              (  )        // out in front
{
    m_left_leg.set_hip_angle          ( 0.0 );
    m_left_leg.set_hip_rotate_angle   ( 0.0 );
    m_left_leg.set_knee_angle         ( 90.0 );
    m_left_leg.set_ankle_angle        ( 90.0 );
    
    m_right_leg.set_hip_angle         ( 0.0 );
    m_right_leg.set_hip_rotate_angle  ( 0.0 );
    m_right_leg.set_knee_angle        ( 90.0 );
    m_right_leg.set_ankle_angle       ( 90.0 );
}
/*
 Calculate the angles needed to raise the leg from standing position to a certain vertical height (mInches)
 
 */
void  glStaticMovesRobot::lift_leg_from_standing      ( float mInches, bool mLeftLeg )
{
    if (mInches<0)  return ;
        
    float upper_lower_length = m_left_leg.m_upper_leg.m_joint_length + m_left_leg.m_lower_leg.m_joint_length;
    float upper_distance     = mInches * (m_left_leg.m_upper_leg.m_joint_length / upper_lower_length);
    float upper_vertical     = m_left_leg.m_upper_leg.m_joint_length - upper_distance;
    
    // Now find the angle needed to raise upper leg from m_upper_leg.m_joint_length by upper_distance;
    float hypotenus = m_left_leg.m_upper_leg.m_joint_length;
    
    // upper_vertical = hypotenus * cos(mAngle);
    float hip_angle = degrees( acos(upper_vertical / hypotenus) );
    if (mLeftLeg)
        lift_left_leg(hip_angle);
    else
        lift_right_leg(hip_angle);
}

void  glStaticMovesRobot::lift_left_leg      ( float mAngle )
{
    m_left_leg.set_hip_angle  (-mAngle);
    m_left_leg.set_knee_angle (2*mAngle);
    m_left_leg.make_foot_level();
}
void  glStaticMovesRobot::lift_right_leg     ( float mAngle )
{
    m_right_leg.set_hip_angle  (-mAngle);
    m_right_leg.set_knee_angle (2*mAngle);
    m_right_leg.make_foot_level();
}

void  glStaticMovesRobot::lift_left_keep_lower_vertical( float mAngle, bool mLeftLeg )
{
    if (mLeftLeg) {
        m_left_leg.set_hip_angle  (-mAngle);
        m_left_leg.set_knee_angle (mAngle);
        m_left_leg.make_foot_level();
    } else {
        m_right_leg.set_hip_angle  (-mAngle);
        m_right_leg.set_knee_angle (mAngle);
        m_right_leg.make_foot_level();
    }
}

void  glStaticMovesRobot::w_question         (  )
{
    m_left_arm.set_shoulder_rotate_angle   (  0.0 );      // rotate forward backward (1st rotation)
    m_left_arm.set_shoulder_angle          (  45.0 );     // arm swing (ie. 2nd rotation)
    m_left_arm.set_upper_arm_rotate_angle  (  90.0 );     // arm rotate (ie. 3rd rotation)
    m_left_arm.set_elbow_angle             (  90.0 );     //
    m_left_arm.set_wrist_angle             (  45.0 );
    
    m_right_arm.set_shoulder_rotate_angle   (  0.0 );
    m_right_arm.set_shoulder_angle          ( 45.0 );
    m_right_arm.set_upper_arm_rotate_angle  (  90.0 );     // arm rotate (ie. 3rd rotation)
    m_right_arm.set_elbow_angle             (  90.0 );
    m_right_arm.set_wrist_angle             (  45.0 );
}

void  glStaticMovesRobot::hands_on_hip       (  )
{
    m_left_arm.set_shoulder_rotate_angle   (  0.0 );      // rotate forward backward (1st rotation)
    m_left_arm.set_shoulder_angle          (  45.0 );     // arm swing (ie. 2nd rotation)
    m_left_arm.set_upper_arm_rotate_angle  ( -90.0 );     // arm rotate (ie. 3rd rotation)
    m_left_arm.set_elbow_angle             (  90.0 );     //
    m_left_arm.set_wrist_angle             (  90.0 );

    m_right_arm.set_shoulder_rotate_angle   (  0.0 );
    m_right_arm.set_shoulder_angle          ( 45.0 );
    m_right_arm.set_upper_arm_rotate_angle  ( -90.0 );     // arm rotate (ie. 3rd rotation)
    m_right_arm.set_elbow_angle             (  90.0 );
    m_right_arm.set_wrist_angle             (  90.0 );
}

void  glStaticMovesRobot::folded_arms        (  )
{
    m_left_arm.set_shoulder_rotate_angle   ( -20.0 );
    m_left_arm.set_shoulder_angle          (  0.0 );
    m_left_arm.set_upper_arm_rotate_angle  ( -90.0 );     // arm rotate (ie. 3rd rotation)
    m_left_arm.set_elbow_angle             ( 90.0 );

    m_right_arm.set_shoulder_rotate_angle   ( -30.0 );
    m_right_arm.set_shoulder_angle          (  0.0 );
    m_right_arm.set_upper_arm_rotate_angle  ( -90.0 );     // arm rotate (ie. 3rd rotation)
    m_right_arm.set_elbow_angle             ( 90.0 );
}

void glStaticMovesRobot::zombie_arms        (  )        // out in front
{
    m_left_arm.set_shoulder_rotate_angle   ( -90.0 );
    m_left_arm.set_shoulder_angle          (  0.0 );
    m_left_arm.set_upper_arm_rotate_angle  (  0.0 );     // arm rotate (ie. 3rd rotation)
    m_left_arm.set_elbow_angle             (  0.0 );

    m_right_arm.set_shoulder_rotate_angle   ( -90.0 );
    m_right_arm.set_shoulder_angle          (  0.0 );
    m_right_arm.set_upper_arm_rotate_angle  (  0.0 );     // arm rotate (ie. 3rd rotation)
    m_right_arm.set_elbow_angle             (  0.0 );    
}

void glStaticMovesRobot::arms_down_by_side        (  )        // out in front
{
    m_left_arm.set_shoulder_angle          (  0.0 );
    m_left_arm.set_shoulder_rotate_angle   (  0.0 );
    m_left_arm.set_upper_arm_rotate_angle  (  0.0 );     // arm rotate (ie. 3rd rotation)
    m_left_arm.set_elbow_angle             (  0.0 );
    
    m_right_arm.set_shoulder_angle          (  0.0 );
    m_right_arm.set_shoulder_rotate_angle   (  0.0 );
    m_right_arm.set_upper_arm_rotate_angle  (  0.0 );     // arm rotate (ie. 3rd rotation)
    m_right_arm.set_elbow_angle             (  0.0 );
}

void glStaticMovesRobot::wave_1( bool mLeftHand )
{
    glArm* arm = NULL;
    if (mLeftHand)  arm = &m_left_arm;  else  arm = &m_right_arm;

    arm->set_shoulder_rotate_angle   ( -90.0 );
    arm->set_shoulder_angle          ( 45.0 );
    arm->set_upper_arm_rotate_angle  ( -20.0 );     // arm rotate (ie. 3rd rotation)
    arm->set_elbow_angle             ( 100.0 );
    arm->set_wrist_angle             (   0.0 );
    arm->set_wrist_rotate_angle      ( -45.0 );
}
void glStaticMovesRobot::wave_2( bool mLeftHand )
{
    glArm* arm = NULL;
    if (mLeftHand)  arm = &m_left_arm;  else  arm = &m_right_arm;

    arm->set_shoulder_rotate_angle   (  -90.0 );
    arm->set_shoulder_angle          (  45.0 );
    arm->set_upper_arm_rotate_angle  (  20.0 );     // arm rotate (ie. 3rd rotation)
    arm->set_elbow_angle             (  80.0 );
    arm->set_wrist_angle             (   0.0 );
    arm->set_wrist_rotate_angle      ( -45.0 );
}

void glStaticMovesRobot::arm_stretched_to_side_left(  )
{
    m_left_arm.set_shoulder_rotate_angle    ( -45.0  );
    m_left_arm.set_shoulder_angle           ( 90.0 );
    m_left_arm.set_upper_arm_rotate_angle   ( 0.0 );
    m_left_arm.set_elbow_angle              ( 0.0 );
}

void glStaticMovesRobot::arm_stretched_to_side_right(  )
{
    m_right_arm.set_shoulder_rotate_angle   ( 0.0 );
    m_right_arm.set_shoulder_angle          ( 90.0 );
    m_right_arm.set_upper_arm_rotate_angle  ( 0.0 );
    m_right_arm.set_elbow_angle             ( 0.0 );
}

void glStaticMovesRobot::arm_straight_up( bool mLeftHand )
{
    glArm* arm = NULL;
    if (mLeftHand)  arm = &m_left_arm;  else  arm = &m_right_arm;
    
    arm->set_shoulder_angle          (   0.0 );
    arm->set_shoulder_rotate_angle   ( 180.0 );
    arm->set_upper_arm_rotate_angle  (   0.0 );     // arm rotate (ie. 3rd rotation)
    arm->set_elbow_angle             (   0.0 );
}

void glStaticMovesRobot::attention_1(  )
{
    m_left_arm.set_shoulder_rotate_angle   ( -150.0);
    m_left_arm.set_shoulder_angle          (  0.0 );
    m_left_arm.set_upper_arm_rotate_angle  (  0.0 );     // arm rotate (ie. 3rd rotation)
    m_left_arm.set_elbow_angle             (  0.0 );

    m_right_arm.set_shoulder_rotate_angle   (-150.0 );
    m_right_arm.set_shoulder_angle          (  0.0 );
    m_right_arm.set_upper_arm_rotate_angle  (  0.0 );     // arm rotate (ie. 3rd rotation)
    m_right_arm.set_elbow_angle             (  0.0 );
}

void glStaticMovesRobot::attention_2(  )
{
    m_left_arm.set_shoulder_rotate_angle   ( -150.0 );
    m_left_arm.set_shoulder_angle          (  30.0 );
    m_left_arm.set_upper_arm_rotate_angle  (   0.0 );     // arm rotate (ie. 3rd rotation)
    m_left_arm.set_elbow_angle             (   0.0 );

    m_right_arm.set_shoulder_rotate_angle   ( -150.0 );
    m_right_arm.set_shoulder_angle          (  30.0 );
    m_right_arm.set_upper_arm_rotate_angle  (   0.0 );     // arm rotate (ie. 3rd rotation)
    m_right_arm.set_elbow_angle             (   0.0 );
}

// for lifting a table.
void glStaticMovesRobot::lift_with_both_hands_1( float hand_separation )
{
    m_left_arm.set_shoulder_rotate_angle   (  -10.0 );
    m_left_arm.set_shoulder_angle          (  30.0 );
    m_left_arm.set_upper_arm_rotate_angle  (  90.0 );     // arm rotate (ie. 3rd rotation)
    m_left_arm.set_elbow_angle             (   0.0 );
    m_left_arm.m_gripper.fingers_straight  (     );
    m_left_arm.m_gripper.fingers_bend_mid  ( 90.0);
    
    m_right_arm.set_shoulder_rotate_angle   (  -10.0 );
    m_right_arm.set_shoulder_angle          (  30.0 );
    m_right_arm.set_upper_arm_rotate_angle  (  90.0 );     // arm rotate (ie. 3rd rotation)
    m_right_arm.set_elbow_angle             (   0.0 );
    m_right_arm.m_gripper.fingers_straight  (       );
    m_right_arm.m_gripper.fingers_bend_mid  ( 90.0  );
    
}
void glStaticMovesRobot::lift_with_both_hands_2( float lift_height )   // how much to raise the table by.
{

    float elbow_bend = 30;  // ??
    
    m_left_arm.set_elbow_angle             ( elbow_bend );
    m_right_arm.set_elbow_angle            ( elbow_bend );
    
    // optionally can bend wrist?
    // optionally can pull the table closer to body.
    /*m_left_arm.set_shoulder_rotate_angle   (  10.0 );
    m_right_arm.set_shoulder_rotate_angle  (  10.0 );*/
}

float increment = 5.;
#define end_limit 45
#define end_limit2 90

void glStaticMovesRobot::scan_arms( )
{
    static float sa = 0.0;
    static float sra = 0.0;
    static float uar = 0.0;
    sra+= 2;
    if (sra>end_limit)
    {
        sra = end_limit;
        sa += 2;
        if (sa>end_limit2)
            uar += 2;
    }
    
    m_left_arm.set_shoulder_rotate_angle ( -sra );
    m_right_arm.set_shoulder_rotate_angle( -sra );
    
    m_left_arm.set_shoulder_angle       ( sa );
    m_right_arm.set_shoulder_angle      ( sa );
    
    m_left_arm.set_upper_arm_rotate_angle ( uar );
    m_right_arm.set_upper_arm_rotate_angle( uar );
}

bool glStaticMovesRobot::demo ( )
{
    static int  move_number = 5;
    switch (move_number)
    {
        case 5 :    stand( );                                break;
        case 6 :    sit  ( );                                break;
        case 7 :    squat( 90 );                             break;
        case 8 :    one_knee_kneel(true);                    break;
        case 9 :    kneel( );                                break;

        case 10 :    arms_down_by_side  (  );               break;
        case 11 :    w_question         (  );               break;
        case 12 :    hands_on_hip       (  );               break;
        case 13 :    folded_arms        (  );               break;
        case 14 :    zombie_arms        (  );               break;
        case 15 :    arm_straight_up    ( true  );          break;
        case 16 :    arm_straight_up    ( false );          break;
        case 17 :    wave_1             ( true );           break;
        case 18 :    wave_2             ( true );           break;
        case 19 :    arm_stretched_to_side_left (  );       break;
        case 20 :    arm_stretched_to_side_right(  );       break;
        case 21 :    attention_1        (  );               break;
        case 22 :    attention_2        (  );               break;
        case 23 :    move_number = 1.;    return true;      break;
        default:    break;
    }
    move_number++;
    return false;
}



