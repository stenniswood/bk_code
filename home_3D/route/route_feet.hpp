//
//  route_feet.h
//  home3D
//
//  Created by Stephen Tenniswood on 2/17/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__route_feet__
#define __home3D__route_feet__

#include <vector>
#include "route.hpp"
#include "robot_moves.h"
#include "robot.hpp"
#include "robot_moves.h"
//#include "walking_robot.h"


using namespace std;

//class glRobot;
//class glWalkingRobot;
// See Robot.hpp : m_torso.width!

/* We may also like to keep a path.  And update the robot foot prints along the way.
   for instance if it stops (stands) to open door.  Then resumes.
 */

class glRouteWithFeet : public glRoute
{
public:
    glRouteWithFeet ( );
    glRouteWithFeet ( glRouteWithFeet& mCopy );
    ~glRouteWithFeet( );

    virtual void	start_over_at              ( struct Vertex mStart );
    void            reset                      ( );

    // FOOT PRINTS :
    void            reset_foot_prints          ( );
    void 			compute_angle              ( struct stFootPosition& mfoot );
    void 			compute_angles             ( );
    
    //virtual void    create_from_multi          ( glMultiRoute& mMulti );
    void 			colorize_center            ( int mVertexIndex );

    void			generate_steps_vertices	   ( );
    void 			change_vertices_colors     ( );	// makes robot feet in red.
    
    virtual void	draw_body                  ( );
    void			print_step                 ( int mIndex, bool mLeft=true );
    
    bool			m_show_robot_feet;

    vector<struct stFootPosition>		m_left_steps;
    vector<struct stFootPosition>		m_right_steps;
    
    // IBO / VBO:
    int		m_number_steps_indices;
    int		m_number_steps_vertices;
};


#endif 
/* defined(__home3D__route_feet__) */

//bool            m_valid_route;
//void            place_robot_over_vertex    ( size_t mVertexIndex, glRobot &mRobot );
//float 			compute_robot_feet_standing( int mIndex,         glWalkingRobot &mRobot );
//void            compute_foot_print         ( float mVertexIndex, glWalkingRobot& mRobot,        bool mLeft       );
//void			compute_robot_steps		   ( float mStride,      glWalkingRobot& mRobot,        bool mLeftFootFirst=false );
//void			compute_robot_path		   ( float mStride,      bool    mLeftFootFirst, glWalkingRobot& mRobot );
// Does entire path (start to finish - standing at both postions)




