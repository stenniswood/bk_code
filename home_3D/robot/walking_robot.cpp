//
//  walking_robot.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/21/15.
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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "all_objects.h"
#include "sequencer_memory.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glLine vl;


glWalkingRobot::glWalkingRobot( )
{
    m_walk_phase_left  = 0;
    m_walk_phase_right = 0;
    m_last_direction_forward = true;
    
    m_glide_index = 0;
    m_glide_over_path_mode = false;
    m_show_paths = false;
    m_is_walking = false;
    m_left_step_index  = 0;
    m_right_step_index = 0;
    m_phase         = 0;    // or 10
    m_stance_left   = false;
}

/******************************************************************/
/************* PATH/FEET PLANNING.                *****************/
/******************************************************************/

void glWalkingRobot::goto_start_of_path( )
{
    printf("\n\nStarting path over!\n");
    if (m_route.m_vertices.size()==0)  return; // No path yet!
    
    struct Vertex v1;
    struct Vertex v2;
    v1 = m_route.m_vertices[0];
    v2 = m_route.m_vertices[1];
    place_over_vertex( v1 );    // change this to use above function!
    angle_body_vertex( v1, v2 );
    
    // Adjust body angle.
    m_y_angle          = m_route.get_angle_degrees( 0 );
    m_stance_left = false;
    m_left_step_index  = 0;     // 0 is the standing position.  1 is the first step.
    m_right_step_index = 0;
    m_at_end_of_route  = false;
}

/* Adds feet positions to the route with feet */
void glWalkingRobot::add_standing_feet_position(int mIndex)       // for start and end of path.
{
    stand();
    place_over_vertex( m_route.m_vertices[mIndex] );
    angle_body_vertex( m_route.m_vertices[mIndex], m_route.m_vertices[mIndex+1] );
    
    struct stFootPosition foot;
    calc_world_left_foot_positions ();
    calc_world_right_foot_positions();
    
    // Extract and store feet positions:
    get_left_foot_location(&foot);
    m_route.m_left_steps.push_back( foot );
    get_right_foot_location(&foot);
    m_route.m_right_steps.push_back( foot );
}

/* After the route has been placed.  Call this function to compute
 where the foot should be placed.
 
 Result will be stored in  glRouteWithFeet -  m_left_steps or m_right_steps
 vector.
 */
void glWalkingRobot::compute_foot_print( int mVertexIndex, bool mLeft )
{
    m_route.colorize_center(mVertexIndex);

    // We'll put the foot print parallel to the path.
    // So we need 2 path points to find the angle.
    MathVector rr = m_route.compute_delta( mVertexIndex );
    rr.unitize();
    MathVector perp = rr.get_perp_xz();
    
    float path_center_to_heel = get_feet_separation()/2.;
    if (mLeft==false)
        path_center_to_heel = -path_center_to_heel;
    
    MathVector heel = m_route.get_perpendicular_pt(mVertexIndex, path_center_to_heel);
    
    // HEEL :
    struct stFootPosition fp;
    fp.heel[0] = heel[0];
    fp.heel[1] = heel[1];
    fp.heel[2] = heel[2];
    fp.angle[1]= atan2( rr[0], rr[2] );
    
    // TOE :
    float multiplier = ( m_left_leg.m_foot.m_foot_length );
    fp.toe[0] = heel[0] + rr[0] * multiplier;
    fp.toe[1] = heel[1];
    fp.toe[2] = heel[2] + rr[2] * multiplier;
    
    if (mLeft)
        m_route.m_left_steps.push_back ( fp );
    else
        m_route.m_right_steps.push_back( fp );
}

/*
 Scans the path, computing foot placement for the route.
 Assumes the starting and ending standing position is already done.
 */
void glWalkingRobot::compute_robot_steps( bool mLeftFootFirst )
{
    struct Vertex v;
    v.position[1] = 0.0;
    bool  left_foot_turn  = mLeftFootFirst;
    int   stride_vertices = round(m_stride/2.0);
    int   half_stride_vertices = round(m_stride/4.0);
    
    // First step = vertex 7 (half stride)
    for (int i=half_stride_vertices; i<m_route.m_vertices.size()-1; i+=stride_vertices)
    {
        compute_foot_print( i, left_foot_turn );
        left_foot_turn = !left_foot_turn;
    }
}


/* Computes the feet placement for the entire path.
 Adds the initial and final standing feet positions. 
 */
void glWalkingRobot::compute_full_path( bool mLeftFootFirst )
{
    // Start by standing in the m_start position
    add_standing_feet_position( 1 );
    
    // WALK TO DESTINATION :
    compute_robot_steps( true );
    // ONE EXTRA on the RIght Foot here!!
    
    // END STANDING POSITION :
    size_t end = m_route.m_vertices.size()-2;
    compute_foot_print( (int)end, true );
    compute_foot_print( (int)end, false );
}

void glWalkingRobot::plan_steps( float mStride  )
{
    m_stride = mStride;
    calc_world_feet_positions(  );                      // from current angles (stored in feet)
    
    compute_full_path( true );                          //
    m_route.generate_steps_vertices();                  //
    m_route.gl_register();
    // m_components.push_back(&m_route);
    // done separately because route needs to reverse the rotations before drawing.  Absolute positioning only!
    
    goto_start_of_path();
}

void glWalkingRobot::start_walking( )
{
    vl.gl_register();
    m_is_walking = true;
}
void glWalkingRobot::stop_walking( )
{
    vl.gl_unregister();
    m_is_walking = false;
}

/* Want to call this from a timeslice.
 
 mDegrees ==> positive -> turn right.
              negative -> turn left.
*/
void glWalkingRobot::standing_turn_sequence  ( float mFinalAngleDegrees, bool mLeftLegStartsPivot )
{
    static int  state = 0;
    static bool left_leg = false;
    //static float BodyAngle = 0.0;
    
    //float one_step_range   = (MAX_HIP_ROTATE_ANGLE - MIN_HIP_ROTATE_ANGLE)*0.75;
    //int   num_steps_needed = round(mFinalAngleDegrees / one_step_range);
    //left_leg = (mFinalAngleDegrees>0);
    float angle     = 20.0;
    float new_angle = 0.0;
    
    // State machine:
    switch( state )
    {
        case 0 :  left_leg = mLeftLegStartsPivot;
                state = 1;
                // keep going since no motor action took place... (ie no "break;" here!)
        case 1 :
            // Lift opposite leg
            if (left_leg)
            {
                lift_right_leg(angle);
            } else {
                lift_left_leg(angle);
            }
            state = 2;
            break;
        case 2 :    // Turn pivot leg :
            if (left_leg) {
                new_angle = min( MAX_HIP_ROTATE_ANGLE, mFinalAngleDegrees );
                m_left_leg.set_hip_rotate_angle(new_angle);
                
                // Adjust Body position:
                m_y_angle -=  new_angle;
                glm::vec4 hip = get_left_hip_coord();
                // The hip coordinate is a change in X and Z.  Z has no effect in the body positioning for a turn!
                /*float delta_body_x = hip[0] - hip[0] * cos(glm::radians(new_angle));
                float delta_body_z = hip[0] * sin(glm::radians(new_angle));
                relocate( m_x-delta_body_x, m_y, m_z-delta_body_z ); */
                relocate( m_x, m_y, m_z );
            } else {
                new_angle = min( MAX_HIP_ROTATE_ANGLE, mFinalAngleDegrees );
                m_right_leg.set_hip_rotate_angle(new_angle);
                
                // Adjust Body position:
                m_y_angle -= new_angle;
                glm::vec4 hip = get_right_hip_coord();
                /*float delta_body_x = hip[0] - hip[0] * cos(glm::radians(new_angle));
                float delta_body_z = hip[0] * sin(glm::radians(new_angle));
                relocate( m_x+delta_body_x, m_y, m_z+delta_body_z ); */
                relocate( m_x, m_y, m_z );
            }
            state = 3;
            break;
        case 3 :    // Lower Leg and stand on both.
            if (left_leg) {
                m_right_leg.set_hip_angle(0);
                // can make the turn faster by rotating the lower leg while lowering it.
                // If we have a lot further to turn (
                // float angle_remaining = asdf;
                
                m_right_leg.set_hip_rotate_angle(0);
                m_right_leg.set_knee_angle(0);
                m_right_leg.set_ankle_angle(0);
            } else {
                m_left_leg.set_hip_angle(0);
                m_left_leg.set_hip_rotate_angle(0);
                m_left_leg.set_knee_angle(0);
                m_left_leg.set_ankle_angle(0);
            }
            left_leg = !left_leg;     // switch pivot leg.
            state = 1;
            break;
        case 4 :
            // goto case 1
            state = 1;
            break;
        default:
            break;
    }
    // Lift leg (of
}
/******************************************************************/
/************* PATH FEET PLACEMENT ALL COMPUTED.  *****************/
/************* Manipulate robot body to walk      *****************/
/******************************************************************/
void glWalkingRobot::walk_phase_0(  )      /* Initialize Feet positions */
{
    // put the robot on the standing feet positions.  May not always be called
    // b/c we want to make robot walk from where he currently is.
    place_feet_at( m_route.m_left_steps[0], m_route.m_right_steps[0], m_stance_left );
    m_left_step_index = 1;
}
void glWalkingRobot::walk_phase_1(  )      /* Raise Lead Foot positions */
{
    //float lift_angle = 30.0;
    glLeg* stanceleg = get_stance_leg( );
    glLeg* swingleg  = get_swing_leg ( );
    float lift_angle = (180./ M_PI) * asin( (m_stride/2) / (stanceleg->get_length()) ) / 2;  // half step
    
    swingleg->set_hip_angle  ( -lift_angle );
    swingleg->set_knee_angle ( lift_angle );
    swingleg->set_ankle_angle( 0.0 );
}
void glWalkingRobot::walk_phase_2(  )      /* Move hind hip to meet  */
{
    glLeg* stanceleg = get_stance_leg( );
    glLeg* swingleg  = get_swing_leg ( );
    float hip_angle = (180./ M_PI) * asin( (m_stride/2) / (stanceleg->get_length()) ) / 2;  // half step
    
    swingleg->set_knee_angle    ( 0 );
    stanceleg->set_hip_angle    ( hip_angle  );
    stanceleg->set_ankle_angle  ( hip_angle );
    m_stance_left = ! m_stance_left;
}
void glWalkingRobot::walk_phase_3(  )      /* Move hind hip to meet  */
{
    glLeg* stanceleg = get_stance_leg( );
    glLeg* swingleg  = get_swing_leg ( );
    
    //float hip_angle = (180./ M_PI) * asin( (m_stride/2) / (stanceleg->get_length()) ) / 2;  // half step
    swingleg->set_knee_angle    ( 0 );
    
    stanceleg->set_hip_angle    ( 0  );
    stanceleg->set_ankle_angle  ( 0 );
    m_stance_left = ! m_stance_left;
}

void glWalkingRobot::walk_phase_i(  )       /* HEEL STRIKE PHASE! */
{
    // Here we use the internal counter for which feet position have been reached.
    /* Note: From time of heel strike to phase_x, the stance foot is placed on the same foot print.
            The swing leg is the one that moves.  is on the foot print in phase I and leaves it 
            during the single support phases.
            Thus the robot is propelled forward.
     */
    glLeg* stance = get_stance_leg( );
    glLeg* swing  = get_swing_leg ( );
    
    // SWING LEG (hind leg):
    float knee_angle = 20;
    float hip_angle = (180./ M_PI) * asin( (m_stride/2) / (stance->get_length()) );
    swing->set_hip_angle  ( hip_angle  );
    swing->set_knee_angle ( knee_angle );
    swing->make_foot_level( );
    //swing->set_ankle_angle( 0.0 );

    // STANCE LEG:
    stance->set_hip_angle  ( -hip_angle );
    stance->set_knee_angle ( 0.0 );
    stance->set_ankle_angle( 0.0 );  // rests on heel (which will be rounded, so foot doesn't have to be flat here)
}

/*
 Stance leg has knee bent, ankle normal 90 degs (flat footed forward)

 Phases iv to vi are single support phases.  ie. all weight is on the stance leg.
 */
void glWalkingRobot::walk_phase_iii(  )
{
    glLeg* stance = get_stance_leg( );
    glLeg* swing  = get_swing_leg ( );
    
    //float fore_aft_before   = stance->get_fore_aft ();
    float hip_swing         = stance->get_hip_angle();      // it will be the full amount (enought to meet the stride here)
    float new_hip_stance    = 3 * hip_swing / 4;            // decrease to 3/4 what it was at heel strike
    stance->set_hip_angle ( -new_hip_stance );
    stance->set_knee_angle( new_hip_stance );
    stance->make_foot_level();

    //bring in the back leg too.   Put torso at Rather than bring in, let's make it bigger angle.
    //swing->set_hip_angle(-new_hip_swing);
    float new_hip_swing_leg = 3 * hip_swing / 4;        // decrease to 3/4
    float knee_angle = 20;
    swing->set_hip_angle (new_hip_swing_leg );
    swing->set_knee_angle( knee_angle );
    swing->make_foot_level();
    // this is when we'd be on our toes.
}

// Fully supported now by the stance leg.  swing leg knee bends, ankle up, hip angle 1/2 forward from phase iii
void glWalkingRobot::walk_phase_iv(  )
{
    glLeg* stance = get_stance_leg( );
    glLeg* swing  = get_swing_leg ( );
    
    //float fore_aft_before   = stance->get_fore_aft();
    float hip_swing         = stance->get_hip_angle();  // it will be the full amount (enought to meet the stride here)
    float new_hip_stance    = 1 * hip_swing / 2;        // decrease to 3/8 what it was at heel strike  (1/2 of 3/4 = 3/8!)
    stance->set_hip_angle ( -new_hip_stance );
    stance->set_knee_angle( new_hip_stance );
    stance->make_foot_level();
    
    //bring in the back leg too.   Put torso at Rather than bring in, let's make it bigger angle.
    float old_hip_swing = swing->get_hip_angle();
    float new_hip_swing   = 1 * old_hip_swing / 2;    // decrease to 3/4
    float new_knee_swing  = 30; //-1 * old_hip_swing;        // decrease to 3/4 what it was at heel strike
    swing->set_hip_angle ( -new_hip_swing  );
    swing->set_knee_angle( new_knee_swing );
    swing->make_foot_level();
}

/* Intermediate (1/2 way)   Stance leg straight;  Swing leg knee bent, ankle lifted 
   (to prevent from hitting floor).
   (alternate walk for smooth -carry class of coffee may have 
    a slight bend in hip & knee to keep the robot level)
 */
void glWalkingRobot::walk_phase_v(  )
{
    glLeg* stance = get_stance_leg();
    glLeg* swing  = get_swing_leg ();

    //float fore_aft_before   = stance->get_fore_aft();
    stance->set_hip_angle   ( 0.0);
    stance->set_knee_angle  ( 0.0);
    //stance->set_ankle_angle ( 0.0);
    stance->make_foot_level();
    //float fore_aft_after   = stance->get_fore_aft();
    //float delta = fore_aft_before-fore_aft_after;
    
    // Swing Leg knee/ankle bend.
    const float carry_angle = 15;
    swing->set_hip_angle  ( 0.0    );
    swing->set_knee_angle ( carry_angle *2 );
    swing->make_foot_level();
}

void glWalkingRobot::walk_phase_vi     (  )      // Intermediate (1/2 way) stance leg straight.  Swing leg toe lift knee bent.
{
    glLeg* stance = get_stance_leg();
    glLeg* swing  = get_swing_leg ();
    
    //float fore_aft_before = stance->get_fore_aft();
    
    // decrease stance leg hip angle.  make foot flat. (starting to go behind torso)
    float hip_angle = 180. * asin( m_stride / (2*stance->get_length()) ) / M_PI;
    stance->set_hip_angle   ( 1*hip_angle/2. );
    stance->set_knee_angle  ( 0.0);
    //stance->set_ankle_angle ( +1*hip_angle/2. );
    stance->make_foot_level();
    //float fore_aft_after = stance->get_fore_aft();      // should have increased.
    //float delta = fore_aft_after - fore_aft_before;
    
    // increase swing leg hip angle.
    // Swing Leg knee/ankle bend.
    //const float carry_angle = 15;
    swing->set_hip_angle  (  -1*hip_angle/2. );
    swing->make_foot_level();
}
void glWalkingRobot::walk_phase_vii   (  )      // Intermediate (1/2 way) stance leg straight.  Swing leg toe lift knee bent.
{
    glLeg* stance = get_stance_leg();
    glLeg* swing  = get_swing_leg ();
    
    //float fore_aft_before = stance->get_fore_aft();
    
    // decrease stance leg hip angle.  make foot flat. (starting to go behind torso)
    float hip_angle = 180. * asin( m_stride / (2*stance->get_length()) ) / M_PI;
    stance->set_hip_angle   ( 3*hip_angle/4. );
    stance->set_knee_angle  ( 0.0);
    //stance->set_ankle_angle ( +3*hip_angle/4. );
    stance->make_foot_level();
    //float fore_aft_after = stance->get_fore_aft();      // should have increased.
    //float delta = fore_aft_after - fore_aft_before;
    
    // increase swing leg hip angle.
    // Swing Leg knee/ankle bend.
    const float carry_angle = 15;
    swing->set_hip_angle  ( -3*hip_angle/4. );
    swing->set_knee_angle ( carry_angle );
    swing->make_foot_level();
}

void glWalkingRobot::walk_phase_x(  )
{
    // infinitesimal time before the next heel strike.
    glLeg* stance = get_stance_leg();
    glLeg* swing  = get_swing_leg ();
    
    float fore_aft_before = stance->get_fore_aft();
    
    // decrease stance leg hip angle.  make foot flat. (starting to go behind torso)
    float hip_angle = 180. * asin( m_stride / (2*stance->get_length()) ) / M_PI;
    stance->set_hip_angle   ( hip_angle );
    stance->set_knee_angle  ( 0.0);
    //stance->set_ankle_angle ( +hip_angle );
    stance->make_foot_level();
    
    float fore_aft_after = stance->get_fore_aft();      // should have increased.
    float delta = fore_aft_before - fore_aft_after;
    push_forward_distance(delta);   // do here because the swap of stance and swin cause not to be done in walk()
    // increase swing leg hip angle.
    // Swing Leg knee/ankle bend.
    //const float carry_angle = 15;
    swing->set_hip_angle  ( -hip_angle );
    swing->set_knee_angle (0.0);
    swing->make_foot_level();
    
    // WRAP UP :
    // Lastly, mark that foot step as history.
    if (m_stance_left) {
        if (m_right_step_index == (m_route.m_right_steps.size()-1))
            m_at_end_of_route = true;
        else
            m_right_step_index++;
    } else {
        if (m_left_step_index == (m_route.m_left_steps.size()-1) )
            m_at_end_of_route = true;
        else
            m_left_step_index++;
    }
    m_stance_left = !m_stance_left;

    // Check for end of route.
    if (m_at_end_of_route) {
        m_glide_index = (int)m_route.m_vertices.size()-4;
    }
}



/* Walking State Machine
    We should see the swing leg strike and go the full path to the rear.
    Meanwhile the other leg (becomes stance->swing; phase i)
 */
void glWalkingRobot::walk_phases( )
{
    if (m_is_walking==false)        return;
    glLeg* stance = get_stance_leg();
    glLeg* swing  = get_swing_leg ();
    float  fa_previous = stance->get_fore_aft();
    static int iterations = 0;
    glLeg* prev_stance = stance;
    
    switch (m_phase)
    {
        /* Where is the torso through out these?  
                We'll measure it relative to previous sequence. 
                (ie. first heel strike, body is relative forward from standing to reach the heel)
         */
        case 0: m_sequence.clear();
            iterations = 0;
            walk_phase_0();
            stand();
            m_phase = 1;
            break;
        case 1:
            walk_phase_1();     // swing leg 30deg forward, knee bent 30deg.
            m_phase = 2;
            break;
        case 2:
            walk_phase_2();
            m_phase = 10;
            break;
            
        case 10:                    // Heel strike (swing becomes stance, leg full forward)
            walk_phase_i();         // Stance leg full back (becomes swing)
            m_phase = 11;
            break;
        case 11:                     // Stance leg knee bent is slightly back from heel strike
            walk_phase_iii();        // Prev stance is now swing leg (knee bent, ankle up; slightly forward)
            m_phase = 12;
            break;
        case 12:                    // Stance leg knee bent is slightly back from heel strike
            walk_phase_iv();        // Prev stance is now swing leg (knee bent, ankle up; slightly forward)
            m_phase = 13;
            break;
        case 13:                    // single support stance leg straight
            walk_phase_v();         // swing leg - toes over (vertically below torso; knee bent, ankle up)
            m_phase = 14;
            break;
        case 14:
            walk_phase_vi();        // stance leg slightly backward (foot flat)
            m_phase = 15;           // swing leg (extending knee, hip angle slightly forward)
            break;
        case 15:
            walk_phase_vii();        // stance leg slightly backward (foot flat)
            m_phase = 16;           // swing leg (extending knee, hip angle slightly forward)
            break;

        case 16:
            walk_phase_x();          // Stance leg full behind
            m_phase = 11;            // swing leg (fully extended knee, hip angle full forward - ready to strike)
            iterations++;
            if (iterations==2)
                save_sequence_file("WalkingSequence_2_iterations.txt");            
            break;                  // STANCE & SWING legs switch here!
        default:
            m_phase = 1;
            break;
    }
    // Get Fore Aft :
    stance = get_stance_leg();
    float fa_now               = stance->get_fore_aft();
    float moved_forward_amount = fa_now - fa_previous;
    //if (m_phase != 1)               // we just passed thru phase_x which switches stance and swing legs.
    if (prev_stance == stance)
        move_fore_aft(-moved_forward_amount);
        //push_forward_distance(moved_forward_amount);
    float stance_height = stance->get_vertical_height();
    float swing_height  = swing->get_vertical_height();
    float m_height = max(stance_height, swing_height);
    m_y = 2+ m_torso.height/2. + m_height;
    
    
   // m_x = 50;
   // m_z = 50;
    
    vl.m_Start     = m_position + m_left_leg.m_position;
    vl.m_Start[0] += 2.5;
    vl.m_End       = vl.m_Start;
    vl.m_End[1]   -= m_left_leg.get_vertical_height();
//    m_y = stance->gem_free_hanging	bool	truet_vertical_height() + (-stance->m_y);
    
    add_to_sequence();
    
    update_nose_line();
}

/* Walking State Machine */
void glWalkingRobot::walk_backward_phases(  )
{
    if (m_is_walking==false)        return;
    switch (m_phase)
    {
        case 1:                     // Heel strike
            walk_phase_i();
            m_phase = 10;
            break;
        case 4:                    //
            walk_phase_iv();
            m_phase = 1;
            break;
        case 5: // single support stance leg straight
            walk_phase_v();
            m_phase = 4;
            break;
        case 6:
            walk_phase_vi();
            m_phase = 5;
            break;
        case 10:
            walk_phase_x();
            m_phase = 6;
            break;
        default:
            break;
    }
}

glLeg* glWalkingRobot::get_swing_leg ( )
{
    if (m_stance_left)
        return &(m_right_leg);
    else
        return &(m_left_leg);
}
glLeg* glWalkingRobot::get_stance_leg()
{
    if (m_stance_left)
        return &(m_left_leg);
    else
        return &(m_right_leg);
}

void glWalkingRobot::take_step_backward( )
{
/*    // Assumed "and it's already backward mode" :
    if (m_stance_left) {
        m_left_step_index--;   
        if (m_left_step_index<0) m_left_step_index=0;
        printf("B Left:%5d (r=%d)\n", m_left_step_index, m_right_step_index );
        m_route.print_step( m_left_step_index, true );
        place_feet_at( m_route.m_left_steps[m_left_step_index], m_route.m_right_steps[m_right_step_index], m_stance_left);
        //place_foot_at( mRoute.m_left_steps[m_left_step_index], m_left_leg, m_right_leg );
    } else {
        m_right_step_index--;   
        if (m_right_step_index<0) m_right_step_index=0;
        printf("B Right:(l=%d) r=%5d\n", m_left_step_index, m_right_step_index );
        m_route.print_step(m_right_step_index, false);
        place_feet_at( m_route.m_left_steps[m_left_step_index], m_route.m_right_steps[m_right_step_index], m_stance_left);
        //place_foot_at( mRoute.m_right_steps[m_right_step_index], m_right_leg, m_left_leg  );
    } */
    
    walk_backward_phases();
}

void glWalkingRobot::take_step_forward (  )
{
    walk_phases( );
}

void glWalkingRobot::take_step( bool mForward )
{                                                     
    if (mForward)
    {
        if (m_last_direction_forward==false)
        {
            move_to_stand( !m_stance_left);
        } else
            take_step_forward();
    } else {
        if (m_last_direction_forward==true)
        {
            move_to_stand( !m_stance_left );
        } else
            take_step_backward();
    }
    m_last_direction_forward = mForward;
}

// Add current body position to the sequencer.
void glWalkingRobot::add_to_sequence()
{
	struct stBodyPositionVector bp;
    //struct stBodyPosition bp;
    extract_body_pose(&bp);
    // this has to be changed in robot_interpolator, and there are many references 
    // in that file!  So temporarily comment out here.  
    //m_sequence.push_back( bp );
    seqIPC.ipc_add_sequence( &bp );
}

void glWalkingRobot::flat_footed( glFoot&  mfoot )
{
    // For starters - adjust world coordinates in the foot.
    // see foot
    // Then see about angles
}


void glWalkingRobot::reset_limbs_path( )     // store in the m_*_arm_path variables.
{
    m_left_arm_path.reset ();
    m_right_arm_path.reset();
    m_left_leg_path.reset ();
    m_right_leg_path.reset();
    
    m_left_arm_path.m_line_width  = 3;
    m_right_arm_path.m_line_width = 3;
    
    m_left_arm_path.set_color ( 0xFF0000DF );
    m_right_arm_path.set_color( 0xFFBF0000 );
    
    m_left_leg_path.set_color ( 0xFF0000DF );
    m_right_leg_path.set_color( 0xFFBF0000 );
}

/* Take 1 Sample of all limbs */
void glWalkingRobot::sample_limbs_path( )     // store in the m_*_arm_path variables.
{
    MathVector pos = m_left_arm.get_wrist_position();       // in robot coordinates.
    pos = map_coords           (pos);                       // robot coords -> world coords.
    m_left_arm_path.add_sample(pos);

    pos = m_right_arm.get_wrist_position();
    pos = map_coords(pos);
    m_right_arm_path.add_sample(pos);

    glm::vec4 vec = m_left_leg.get_heel_in_robot_coords();
    //glm::vec4 vec = m_left_leg.get_knee_in_robot_coords();
    glm::vec4 vec2 = map_coords(vec);
    pos[0] = vec2[0];
    pos[1] = vec2[1];
    pos[2] = vec2[2];
    m_left_leg_path.add_sample (pos);
    
    vec  = m_right_leg.get_heel_in_robot_coords();
    //vec  = m_right_leg.get_knee_in_robot_coords();
    vec2 = map_coords(vec);
    pos[0] = vec2[0];
    pos[1] = vec2[1];
    pos[2] = vec2[2];
    m_right_leg_path.add_sample(pos);    
}

/*  Brings forward leg back together
 This is different from just putting legs down straight.
 It moves the specified foot (forward or backward) to
 meet the other foot.
 */
void glWalkingRobot::move_to_stand( bool mMoveLeftLeg )
{
    printf("Moving to Standing position\t");
    
    // Calculate where to put it
    struct stFootPosition stationary;
    struct stFootPosition new_loc   ;
    
    if (mMoveLeftLeg)
    {
        printf("Left Leg\n");
        // Get right leg.  Extrapolate acros width of torso.
        get_right_foot_location( &stationary );
        new_loc.heel[1] = stationary.heel[1];
        new_loc.toe[1]  = stationary.toe[1];
    } else {
        printf("Right Leg\n");
        get_left_foot_location( &stationary );
        
        new_loc.heel[1] = stationary.heel[1];
        new_loc.toe[1]  = stationary.toe[1];
        // Calc slope to torso center:
    }
    printf("stationary heel at %6.3f %6.3f %6.3f \n", stationary.heel[0], stationary.heel[1], stationary.heel[2] );
    printf("stationary toe  at %6.3f %6.3f %6.3f \n", stationary.toe[0],  stationary.toe[1],  stationary.toe[2]  );
    
    // Calc slope to torso center:
    float run  = stationary.heel[0] - stationary.toe[0];
    float rise = stationary.heel[2] - stationary.toe[2];
    float magnitude = sqrt(run*run + rise*rise);
    run  /= magnitude;
    rise /= magnitude;
    
    new_loc.heel[0] = stationary.heel[0] - rise* m_torso.width;
    new_loc.heel[2] = stationary.heel[2] + run * m_torso.width;
    
    new_loc.toe[0] =  -(run  * magnitude - new_loc.heel[0]);
    new_loc.toe[2] =  -(rise * magnitude - new_loc.heel[2]);
    
    new_loc.angle[0] = stationary.angle[0];
    new_loc.angle[1] = stationary.angle[1];
    new_loc.angle[2] = stationary.angle[2];
    
    if (mMoveLeftLeg)
        place_foot_at(new_loc, m_left_leg, m_right_leg);
    else
        place_foot_at(new_loc, m_right_leg, m_left_leg);
    
    printf("new heel at %6.3f %6.3f %6.3f \n", new_loc.heel[0], new_loc.heel[1], new_loc.heel[2] );
    printf("new toe  at %6.3f %6.3f %6.3f \n", new_loc.toe[0],  new_loc.toe[1],  new_loc.toe[2]  );
}
extern glApartment apartment;

void glWalkingRobot::glide_thru_path         ( )        // no leg/arm movements.
{
    static int increment = 2;
    if (m_glide_index<(m_route.m_number_path_vertices-1)) {
        place_over_vertex( m_route.m_vertices[m_glide_index] );
        angle_body_vertex( m_route.m_vertices[m_glide_index], m_route.m_vertices[m_glide_index+1] );
    }
    
    if (m_glide_index>m_route.m_number_path_vertices-1)
        m_glide_index = m_route.m_number_path_vertices-1;
    else
        m_glide_index += increment;
    if (m_glide_index<0) {
        m_glide_index=0;
    }
}

const float HEAD_LOOK_INCREMENT = 5.0;
float head_delta = HEAD_LOOK_INCREMENT;

const int DELAYS = 1*3;  // called every 25ms = 0.2 seconds

void glWalkingRobot::time_slice( )
{
    static int delay_counter = DELAYS;

    if (m_glide_over_path_mode) 
        glide_thru_path();
    else {
        delay_counter--;
        if (delay_counter)  return;
        delay_counter = DELAYS;
        
        // SCAN HEAD (look ahead for path):
        static float head_angle = 0;
        // head_angle += head_delta;
        if (head_angle>80.0)
            head_delta = -HEAD_LOOK_INCREMENT;
        if (head_angle<-80.0)
            head_delta = HEAD_LOOK_INCREMENT;
        m_head.look_left( head_angle );
        walk_phases();
    }
}


void glWalkingRobot::gl_register_limbs_path( )     // store in the m_*_arm_path variables.
{
    m_left_arm_path.gl_register();
    m_right_arm_path.gl_register();
    m_left_leg_path.gl_register();
    m_right_leg_path.gl_register();
}
void glWalkingRobot::gl_unregister_limbs_path( )     // store in the m_*_arm_path variables.
{
    m_left_arm_path.gl_unregister();
    m_right_arm_path.gl_unregister();
    m_left_leg_path.gl_unregister();
    m_right_leg_path.gl_unregister();
}


void glWalkingRobot::draw_body()
{
    // Note: Here the ViewMatrix has already been translated and rotated to the robot frame.
    glMolecule::draw_body();
    
    glRotatef(-m_z_angle, 0.0, 0.0, 1.0 );
    glRotatef(-m_y_angle, 0.0, 1.0, 0.0 );
    glRotatef(-m_x_angle, 1.0, 0.0, 0.0 );
    glTranslatef( -m_x, -m_y, -m_z );
    m_route.draw();

    if ((m_show_paths) && (m_left_arm_path.m_VBO))
    {
        if (m_left_arm_path.m_registered)
            m_left_arm_path.draw_body();

        m_left_arm_path.draw_body();
        m_left_leg_path.draw_body();
        m_right_leg_path.draw_body();
    }
}


/*void glWalkingRobot::interpolate_phase(glRouteWithFeet mRoute)
 {
 switch(m_walk_phase_left)
 {
 case 0: // Heel strike:
 take_step_forward();
 add_to_sequence();
 break;
 case 1: // knee bend (ankle floor)
 add_to_sequence();
 break;
 case 2: // leg straight
 add_to_sequence();
 break;
 case 3: // trailing
 add_to_sequence();
 break;
 case 4: // toe off
 add_to_sequence();
 break;
 default:
 break;
 }
 }*/
