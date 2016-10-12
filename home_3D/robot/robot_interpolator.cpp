//
//  robot_interpolator.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/14/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include <fstream>
#include "robot_interpolator.h"

const int NUMBER_SAMPLES = 10;


glRobotInterpolator::glRobotInterpolator()
{
    
}
glRobotInterpolator::~glRobotInterpolator()
{
    
}

/*
 bp->l_hip_rotate     = m_left_leg.get_hip_rotate_angle   ();
 bp->r_hip_rotate     = m_right_leg.get_hip_rotate_angle  ();
 bp->l_hip_swing      = m_left_leg.get_hip_ss_swing_angle ();
 bp->r_hip_swing      = m_right_leg.get_hip_ss_swing_angle();
 bp->l_hip_fb_swing   = m_left_leg.get_hip_angle     ();
 bp->r_hip_fb_swing   = m_right_leg.get_hip_angle    ();
 
 bp->l_knee           = m_left_leg.get_knee_angle    ();
 bp->r_knee           = m_right_leg.get_knee_angle   ();
 bp->l_ankle          = m_left_leg.get_ankle_angle   ();
 bp->r_ankle          = m_right_leg.get_ankle_angle  ();
 
 // ARMS:
 bp->l_shoulder_fore_rotate   = m_left_arm.get_upper_arm_rotate_angle();
 bp->l_shoulder_rotate        = m_left_arm.get_shoulder_rotate_angle();
 bp->l_shoulder_swing         = m_left_arm.get_shoulder_angle();
 bp->l_elbow                  = m_left_arm.m_elbow_angle;
 bp->l_wrist_swing            = m_left_arm.m_wrist_angle;
 bp->l_wrist_rotate           = m_left_arm.m_wrist_rotate_angle;
 
 bp->r_shoulder_fore_rotate   = m_right_arm.get_upper_arm_rotate_angle();
 bp->r_shoulder_rotate        = m_right_arm.get_shoulder_rotate_angle();
 bp->r_shoulder_swing         = m_right_arm.get_shoulder_angle();
 bp->r_elbow                  = m_right_arm.m_elbow_angle;
 bp->r_wrist_swing            = m_right_arm.m_wrist_angle;
 bp->r_wrist_rotate           = m_right_arm.m_wrist_rotate_angle;
 */

void glRobotInterpolator::read_sequence_file()       // These can be played on the "asequencer" app.
{
    
}
/* Note: A vector is all limbs.  Each limb goes on a separate line.
 */
void glRobotInterpolator::save_one_vector( std::ofstream &mofs, int mIndex, bool mSaveArms )
{
    /* Note The order here *must* match the order in robot::extract_robot_position() */
    // Left Leg :
    mofs << "0, 3, ";
    //mofs << m_sequence[mIndex].l_hip_rotate << ", ";
    mofs << m_sequence[mIndex].l_hip_swing << ", ";
    //mofs << m_sequence[mIndex].l_hip_fb_swing << ", ";
    
    mofs << m_sequence[mIndex].l_knee << ", ";
    mofs << m_sequence[mIndex].l_ankle << ", \n";
    
    // Right Leg :
    mofs << "1, 3, ";
    //mofs << m_sequence[mIndex].r_hip_rotate << ", ";
    mofs << m_sequence[mIndex].r_hip_swing << ", ";
    // mofs << m_sequence[mIndex].r_hip_fb_swing << ", ";
    
    mofs << m_sequence[mIndex].r_knee << ", ";
    mofs << m_sequence[mIndex].r_ankle << ", \n";
    
    // Left Arm
   // bool save_arms=false;
    if (mSaveArms) {
        mofs << "2, 6, ";
        mofs << m_sequence[mIndex].l_shoulder_fore_rotate << ", ";
        mofs << m_sequence[mIndex].l_shoulder_rotate   << ", ";
        mofs << m_sequence[mIndex].l_shoulder_swing    << ", ";
        mofs << m_sequence[mIndex].l_elbow             << ", ";
        mofs << m_sequence[mIndex].l_wrist_swing       << ", ";
        mofs << m_sequence[mIndex].l_wrist_rotate      << ", \n";

        // Right Arm
        mofs << "3, 6, ";
        mofs << m_sequence[mIndex].r_shoulder_fore_rotate << ", ";
        mofs << m_sequence[mIndex].r_shoulder_rotate << ", ";
        mofs << m_sequence[mIndex].r_shoulder_swing << ", ";
        mofs << m_sequence[mIndex].r_elbow          << ", ";
        mofs << m_sequence[mIndex].r_wrist_swing    << ", ";
        mofs << m_sequence[mIndex].r_wrist_rotate   << ", \n";
    }
}

void glRobotInterpolator::save_sequence_file( string mFilename )
{
    std::ofstream ofs (mFilename, std::ofstream::out);
    string tmp = "5000";
    ofs << tmp << "\n";
    ofs << "float" << "\n";
    ofs << "counts" << "\n";
    
    for (int i=0; i<m_sequence.size(); i++)
        save_one_vector( ofs, i );

    ofs.close();
}

/* simple average */
float glRobotInterpolator::interpolate_one( float mAngle1, float mAngle2,
                                           float mNumberOfSamples,
                                           float mIndex
                                           )
{
    float slice = (mAngle2 - mAngle1)/(mNumberOfSamples-1);
    float result = slice * mIndex + mAngle1;
    return result;
}

struct stBodyPosition*  glRobotInterpolator::interpolate(
                                struct stBodyPosition* bp1,
                                struct stBodyPosition* bp2,
                                int number_samples,
                                int index_within_samples )
{
    static struct stBodyPosition bp;
    float* r  = (float*)&bp;
    float* s1 = (float*)bp1;
    float* s2 = (float*)bp2;
    int number_of_floats = sizeof( struct stBodyPosition ) / sizeof(float);
    
    for (int i=0; i<number_of_floats; i++)
        r[i] = interpolate_one( s1[i], s2[i], number_samples, index_within_samples );
    
    return &bp;
}


/* results stored int m_sequence vector */
void  glRobotInterpolator::interpolate_all
                            (struct stBodyPosition* bp1,
                             struct stBodyPosition* bp2,
                             int number_samples  )
{
    struct stBodyPosition* tmp;
    for (int i=0; i<number_samples; i++)
    {
        tmp = interpolate( bp1, bp2, number_samples, i);
        m_sequence.push_back( *tmp );
    }
}

bool glRobotInterpolator::play_next()
{
    if (m_sequence.size())
        set_body_pose( &m_sequence[m_interpolated_index] );
    if (m_interpolated_index < m_sequence.size())
    {
        m_interpolated_index++;
        //m_interpolated_index = 0;
        return true;
    }
    return false;
}

void glRobotInterpolator::morph_demos()
{
    struct stBodyPosition bp1;
    struct stBodyPosition bp2;
    m_sequence.clear();
    
    extract_body_pose(&bp1);  // save current positions
    
    // Activate new positions
    glStaticMovesRobot::demo();
    extract_body_pose(&bp2);  // save current positions
    
    interpolate_all( &bp1, &bp2, NUMBER_SAMPLES);
    m_interpolated_index = 0;
}

