/*
//
//  walking_robot.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/21/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
// NOTE: Origin of the robot is the center of the Torso.

    Stride is defined as the distance from heel to heel.  The max hip swing 
    angles will be based on this.
*/

#ifndef __home3D__walking_robot__
#define __home3D__walking_robot__

#include <stdio.h>
#include "robot.hpp"
#include "robot_leg_piece.hpp"
#include "route_feet.hpp"
#include "robot_interpolator.h"


class glWalkingRobot : public glRobotInterpolator
{
public:
    glWalkingRobot( );
    virtual void    draw_body               ( );        // needed to draw the route as well.
    
    // SEQUENCING :
    void            take_step_backward      ( );
    void            take_step_forward       ( );
    void            take_step               ( bool mForward = true );
    void            goto_start_of_path      ( );

    void            move_to_stand           ( bool mMoveLeftLeg       );    // brings forward leg back together
    void            start_walking           ( );
    void            stop_walking            ( );
    
    void            glide_thru_path         ( );        // no leg/arm movements.
    void            time_slice              ( );
    void            standing_turn_sequence  ( float mDegrees, bool mLeftLegPivot=true );
    
    // PATH PLANNING :
    void            compute_foot_print          ( int mVertexIndex, bool mLeft );       // for start and end of path.
    
    void            compute_robot_steps         ( bool mLeftFootFirst );
    void            add_standing_feet_position  ( int mIndex);       // for start and end of path.
    void            compute_full_path           ( bool mLeftFootFirst );
    void            plan_steps                  ( float mStride                      );  // uses m_route

    // WALKING PHASES :
    /* Phases i..X all do just half of a complete left,right stride.  */
    void            walk_phase_0       (                               );
    void            walk_phase_1       (                               );
    void            walk_phase_2       (                               );
    void            walk_phase_3       (                               );
    void            walk_phase_i       (                               );   // Heel Strike
    void            walk_phase_iii     (                               );
    void            walk_phase_iv      (                               );
    void            walk_phase_v       (                               );
    void            walk_phase_vi      (                               );
    void            walk_phase_vii     (                               );
    void            walk_phase_x       (                               );   // Last move before next heel strike!
    
    // Intermediate (1/2 way) stance leg straight.  Swing leg toe lift knee bent.
    void            walk_phases        (   );
    void            walk_backward_phases(  );

    void            flat_footed        ( glFoot&  mfoot                );
//  void            interpolate_phase  ( glRouteWithFeet mRoute        );

    void            add_to_sequence    (                               );
    
    // VARIABLES :
    glRouteWithFeet m_route;
    
    glLeg*          get_stance_leg( );
    glLeg*          get_swing_leg ( );

    struct stFootPosition   m_stance_fp;
    struct stFootPosition   m_swing_fp ;
    
    int             m_phase;
    bool            m_stance_left;      // replace m_left_foot_turn (though opposite!)  turn means moving leg.
    int             m_left_step_index;
    int             m_right_step_index;
    bool            m_at_end_of_route;
    
    bool            m_is_walking;

    // These hold a "recording" of the limb movements for viewing in 3D:
    void            reset_limbs_path        ( );     // store in the m_*_arm_path variables.
    void            sample_limbs_path       ( );     // store in the m_*_arm_path variables.
    void            gl_register_limbs_path  ( );     // store in the m_*_arm_path variables.
    void            gl_unregister_limbs_path( );
    
    float               m_stride;
    bool                m_glide_over_path_mode;
    bool                m_show_paths;
    
    bool                m_last_direction_forward;
    int                 m_walk_phase_left;
    int                 m_walk_phase_right;

    // For Tracking Limb Positions:
    glRoute             m_left_arm_path;
    glRoute             m_right_arm_path;
    glRoute             m_left_leg_path;
    glRoute             m_right_leg_path;

    // Arm Desired Path :
    glRoute             door_path;
    int                 m_glide_index;
};


#endif  /* defined(__home3D__walking_robot__) */
