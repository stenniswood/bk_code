//
//  moving_object.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/27/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__moving_object__
#define __home3D__moving_object__

#include <stdio.h>
#include "vector.hpp"


#define m_x m_position[0]
#define m_y m_position[1]
#define m_z m_position[2]

#define	m_x_angle m_R_position[0]
#define m_y_angle m_R_position[1]
#define m_z_angle m_R_position[2]


class glMovingObject
{
public:
    glMovingObject ( );
    ~glMovingObject( );

    virtual void    f_ma            ( );                        // net forces / inertia
    virtual void    update_position ( float time_period = 0.01 );
    virtual void    update_rotation ( float time_period = 0.01 );
    void            add_force       ( MathVector mNewForce );   // acting on the object.
    void            clear_net_forces( );
    virtual bool    evaluate_collision( glMovingObject* mOther );
    
    // TRANSLATIONAL:
    MathVector m_net_forces;
    MathVector m_acceleration;
    MathVector m_velocity;
    MathVector m_position;
    MathVector m_prev_position;     // not sure this is right way to do it (used to restore after a collision)

    // ROTATIONAL:
    MathVector Inertia;               // 3 axis.
    
    float coef_of_restitution;      // ratio of the velocities before and after a collision.
    
    MathVector m_R_net_torques;
    MathVector m_R_acceleration;
    MathVector m_R_velocity;
    MathVector m_R_position;
    
    bool    is_active;          // out of bounds?
    bool    is_participating;   // participates in physics simulation.
    bool    m_collision_handled;
    
};




#endif /* defined(__home3D__moving_object__) */
