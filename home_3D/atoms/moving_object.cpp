//
//  moving_object.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/27/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "moving_object.h"
#include "all_objects.h"
#include "physical_world.h"


glMovingObject::glMovingObject ( )
:   m_net_forces(3),
    m_acceleration("accel",    3),
    m_velocity    ("velocity", 3),
    m_position    ("position", 3),
    m_R_net_torques (3),
    m_R_acceleration(3),
    m_R_velocity    (3),
    m_R_position    (3),
    Inertia("inertia",3)
{
    coef_of_restitution = 0.85;
    Inertia[0] = 1.0;
    Inertia[1] = 1.0;
    Inertia[2] = 1.0;
    is_active = false;
    is_participating    = false;   // no physics by default.
    m_collision_handled = false;
}

glMovingObject::~glMovingObject( )
{
    
}

void glMovingObject::clear_net_forces()
{
    m_net_forces[0] = 0;
    m_net_forces[1] = 0;
    m_net_forces[2] = 0;
    
    m_R_net_torques[0] = 0;
    m_R_net_torques[1] = 0;
    m_R_net_torques[2] = 0;
}
// net forces / inertia
// All forces should have been added via add_force() prior to calling this function!
void glMovingObject::f_ma(  )
{
    if (is_participating==false) return;
    
    MathVector inv_inertia(3);
    inv_inertia[0] = 1/ Inertia[0];
    inv_inertia[1] = 1/ Inertia[1];
    inv_inertia[2] = 1/ Inertia[2];
    m_acceleration   = m_net_forces    * inv_inertia;
    m_R_acceleration = m_R_net_torques * inv_inertia;
}
void glMovingObject::update_position( float time_period )
{
    if (is_participating==false) return;

    m_velocity += m_acceleration * time_period;
    m_prev_position = m_position;   // for restoring after a colision.
    
    m_position += m_velocity * time_period;
}
void glMovingObject::update_rotation( float time_period )
{
    if (is_participating==false) return;
    
    m_R_velocity += m_R_acceleration * time_period;
    m_R_position += m_R_velocity * time_period;
}

void glMovingObject::add_force( MathVector mNewForce )      // acting on the object.
{
    if (is_participating==false) return;
    
    m_net_forces += mNewForce;
}

bool glMovingObject::evaluate_collision( glMovingObject* mOther )
{
    if (m_y<0)
        return true;

    return false;
}

