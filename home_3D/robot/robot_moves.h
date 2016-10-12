//
//  robot_moves.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/9/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__robot_moves__
#define __home3D__robot_moves__

#include <stdio.h>
#include <vector>
#include "robot.hpp"


/*  Robot's Left and Right Arm angle definitions:
    Upper Arm Rotate :  Goes toward front and back of body.  Rotate about x axis.  Motor #1
                        + direction is forward for both arms.
    Shoulder Swing   :  Is the pivot the upper arm.  Rotate about local z axis.    Motoor #2
                        + direction is away from body for both arms.
    Upper arm Rotate :  rotation is along the length of the upper arm.  Rotate about local y axis.  Motor #3
                        + direction is away from body for both arms.
 
    Angles are designated so that the sign is the same for both arms.
    The 0.0's are for arms down by side, palms inward.

 
     Robot's Left and Right Leg angle definitions :
     Upper Leg Swing  :  Goes toward front and back of body.  Rotate about x axis.  Motor #1
     + direction is forward (in front of the body)
     Upper Leg Rotate :  Rotates left and right.              Rotate about y axis.  Motor #1
     + direction is right (clockwise from top view)
     Knee Rotate :  is the pivot for the lower leg.           Rotate about local x axis.    Motor #2
     + direction is toward the rear (only way the knee can move)
     Ankle Angle :  is the pivot for the foot.                Rotate about local x axis.    Motor #2
     + direction is flexion (ie lifting toes).  Extension is -
 
     Angles are designated so that the sign is the same for both legs.
     The 0.0's are for legs in standing position.
 */

class glStaticMovesRobot : public glRobot
{
public:
    glStaticMovesRobot ( );
    ~glStaticMovesRobot( );
    
    void            load_file          ( string mFilename              );
    void            save_file          ( string mFilename              );   // not implemented yet!
    void            activate_position  ( int mIndex );
    
    // Hard Coded Positions:
    float           compute_max_squat_distance(                        );
    void            squat_distance     ( float mDeltaY                 );
    void            squat_fraction     ( float mfraction               );       // fraction of what's possible (ie knee stop)
    void            squat              ( float mAngle                  );

    void            stand              (                               );
    void            sit                (                               );
    void            floor_sit          (                               );
    void            one_knee_kneel     ( bool mLeft );        // out in front
    void            kneel              (  );        // out in front

    void            lift_leg_from_standing( float mInches, bool mLeftLeg );    
    void            lift_left_leg      ( float mAngle );
    void            lift_right_leg     ( float mAngle );
    void            lift_left_keep_lower_vertical( float mAngle, bool mLeftLeg );
    
    void            w_question         (  );
    void            hands_on_hip       (  );
    void            folded_arms        (  );
    void            zombie_arms        (  );        // out in front
    void            arms_down_by_side  (  );

    void            arm_straight_up  ( bool mLeftHand );
    
    void            wave_1( bool mLeftHand );
    void            wave_2( bool mLeftHand );
    
    void            arm_stretched_to_side_left (  );
    void            arm_stretched_to_side_right(  );
    
    void            attention_1     (  );
    void            attention_2     (  );  // waving both arms above head

    void            lift_with_both_hands_1( float hand_separation ); // for lifting a table.
    void            lift_with_both_hands_2( float lift_height );     // how much to raise the table by.
    
    void            scan_arms       ( );
    bool            demo            ( );
    
    vector<struct stBodyPosition>   m_positions;
    
};


#endif /* defined(__home3D__robot_moves__) */
