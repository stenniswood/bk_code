/* Squatting, kneeling, balancing 1 leg, dancing, sitting, laying, fallen on back, on front,
 getting up.  How to capture entire set?  Keep it simple.
 
NOTE: Origin of the robot is the center of the Torso.
 
 */

#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <vector>
#include "robot_arm.hpp"
#include "robot_leg_piece.hpp"
#include "robot_head.h"
#include "glMolecule.h"
#include "robot_gripper.h"
#include "robot_foot.hpp"
#include "gl_misc.h"

//#include "route_feet.hpp"
//class glRouteWithFeet;
//using namespace std;

float degrees( float mRad );

const int ROBOT_MODE_WALKING_FORWARD  = 100;
const int ROBOT_MODE_WALKING_BACKWARD = 200;
const int ROBOT_MODE_STANDING         = 300;
const int ROBOT_MODE_SQUATTING        = 400;

// One entry for each DC motor:
struct stBodyPosition {     // all angles given in degrees!
    // Legs:
    float   l_hip_rotate;
    float   l_hip_swing;
    float   l_hip_fb_swing;
    float   l_knee;
    float   l_ankle;

    float   r_hip_rotate;
    float   r_hip_swing;
    float   r_hip_fb_swing;
    float   r_knee;
    float   r_ankle;

    // ARMS:
    float   l_shoulder_rotate;
    float   l_shoulder_swing;
    float   l_shoulder_fore_rotate;
    float   l_elbow;
    float   l_wrist_rotate;
    float   l_wrist_swing;

    float   r_shoulder_rotate;
    float   r_shoulder_swing;
    float   r_shoulder_fore_rotate;
    float   r_elbow;
    float   r_wrist_rotate;
    float   r_wrist_swing;
    
    float   head_turn;
    float   head_tilt;
};

/* Two different Approaches toward getting coordinates of a particular body part.
 
 A) Everything should return in Robot coordinates.
    I take my robot.  "grab it's arm" and ask it where the wrist is.  This is relative to all other parts of the body - ie Robot coordinates.
    Advatanges   :  More usefull from the callers point of view.  Never any confusion what coordinate system.
    Disadvantages:  Difficulty Propogating up the object hierarchy?
    -- This Wins --
 
 
 B) Each part should return in it's local coordinate system.
    Advatanges   :
    Disadvantages:  Always having to get the coordinate, then map it.
 
*/
class glRobot : public glMolecule
{
public:
	glRobot( );

	void            Initialize          ( );
    void            create_components   ( );
    virtual void    relocate            ( float mX, float mY, float mZ );    

    //
    MathVector      get_eye_location_world_coords    ( );        // in world cooridnates for viewing.
    MathVector      get_eye_location_robot_coords    ( );        // in world cooridnates for viewing.
    glm::vec4       get_eye_target_world_coords      ( );        // in world cooridnates for viewing.
    glm::vec4       get_eye_target_robot_coords      ( );        // in world cooridnates for viewing.
    
    //
    void            extract_body_pose    ( struct stBodyPosition* bp             );
    void            set_body_pose        ( struct stBodyPosition* mBodyPosition  );

    float           get_servo_angle      ( int mServoIndex  );
    void            set_servo_angle      ( int mServoIndex, float mServoAngle  );
    
    // Use these to coordinate (ie. they result in left & right legs moving)
    bool			check_limits             ( );

    // vector pointing straight in front of torso (based on m_xyz_angles )
    // unit vector pointing forward (in world coords)
    MathVector      get_robot_front_vector   ( );
    void            move_fore_aft            ( float mForeAftAmount );
    bool            is_in_front_of           ( MathVector mWorldLoc );

    glm::vec4       get_left_hip_coord       ( );
    glm::vec4       get_right_hip_coord      ( );
    glm::vec4       get_left_heel_coord      ( );
    glm::vec4       get_right_heel_coord     ( );

    
    void			calc_world_left_foot_positions  ( ); // especially the feet positions based on angles.
    void			calc_world_right_foot_positions ( );
    void			calc_world_feet_positions       ( )
                    { calc_world_left_foot_positions(); calc_world_right_foot_positions(); };

    void			center_body_between             ( float mLeftFoot[3], float mRightFoot[3] );
    void            angle_body_vertex               ( struct Vertex mVertex, struct Vertex mVertex2 );
    void            angle_body_between              ( struct stFootPosition mLeftFoot,
                                                      struct stFootPosition mRightFoot,
                                                      float  mAngleOffset );
    void            place_over_vertex               ( struct Vertex mVertex );
    void            adjust_body_vertical            ( float mGroundHeight   );
    void            adjust_body_vertical_for_leg    ( float mGroundHeight, bool mLeftLeg);    
    void            test                            ( );
    void            update_nose_line                ( );
    
    // FEET PLACEMENT :
    void            flat_footed                     ( glFoot&  mfoot );
    void            place_feet_at                   ( struct stFootPosition lfp, struct stFootPosition rfp, bool mLeftTurn  );
    void			place_foot_at                   ( struct stFootPosition fp, glLeg& mSwing, glLeg& mStance               );
    //void			place_right_foot_at( struct stFootPosition fp );
    void			push_forward_distance           ( float mDistance );        // increase distance towards Front vector.  relocate()
    
	void			place_left_heel_at              ( float mHeel[3] );
	void			place_right_heel_at             ( float mHeel[3] );

	void			place_left_toe_at               ( struct Vertex mHeel );
	void			place_right_toe_at              ( struct Vertex mHeel );

	void			get_left_foot_location          ( struct stFootPosition* mFoot );
	void			get_right_foot_location         ( struct stFootPosition* mFoot );
    float           get_feet_separation             (  )  { return m_torso.width/2.0; }

	bool			m_foot_planted;		// Means immovable
    int             m_mode;             // see above defines


    
    /* Each leg has been relocated relative to the torso.  So m_x,m_y,m_z for m_left_leg identify 
        where the leg attaches to the torso.  To get real world coordinates, need to map these thru
        the body matrix for the torso.  (calc_hip_positions())     */

    glHead          m_head;    
    glArm           m_left_arm;
    glArm           m_right_arm;
    glBox           m_torso;
	glLeg			m_left_leg;
	glLeg			m_right_leg;

    glLine          m_nose_line;        // a projection to show what is "just beyond his nose"
    
};



#endif

