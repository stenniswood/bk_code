//
//  physical_world.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/27/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "physical_world.h"
#include "all_objects.h"

glPhysicalWorld  theWorld(true);

glPhysicalWorld::glPhysicalWorld(bool hasGravity)
: max_corner("Max", 3),
  min_corner("Min", 3)
{
    m_has_gravity = hasGravity;
    m_gravity = get_gravity();
    
    max_corner.set_xyz( 5000,  5000,  5000);
    min_corner.set_xyz(-5000, -5000, -5000);
}

glPhysicalWorld::~glPhysicalWorld()
{
    
}


glObject* glPhysicalWorld::find_object_id( long mID )
{
    list<glObject*>::iterator iter = m_moveables.begin();
    while (iter != m_moveables.end())
    {
        if ((*iter)->m_object_id == mID)
        {
            printf(" found object_id=%lu! %lu\n", mID, (long) &(*iter) );
            return (glObject*) (*iter);
        }
        iter++;
    }
    return NULL;
}


// for (int i=0; i<m_moveables.size(); i++)
bool  glPhysicalWorld::find_component( string&  mRequestedName,
                                        string& mTypeName,
                                        vector<VerbalObject*>* object_hier,
                                        bool mTypeMustMatch
                                      )
{
    bool            retval  =false;
    VerbalObject*   ptr     =NULL;
    bool            match   =false;
    list<glObject*>::iterator iter = m_moveables.begin();

    while (iter != m_moveables.end())
    {
        ptr = (VerbalObject*)*iter++;
        bool is_initd      = (ptr->m_name.length() > 0);
        int compare_result =  ptr->m_name.compare( mRequestedName );
        match  = is_initd && (compare_result==0);
        if (match)
        {
            if (mTypeMustMatch)
            {
                if (mTypeName.compare( ptr->m_object_type_name )==0 )
                {
                    object_hier->push_back(ptr);
                    return true;
                }
            }
        }
        if (ptr->is_a_molecule()) {
            // Note: this is the Molecule find_component, not the physical world one.
            retval = ((glMolecule*)ptr)->find_component( mRequestedName, mTypeName,
                                                        object_hier,
                                                        mTypeMustMatch );
            if (retval) {
                object_hier->push_back( ptr );
                return true;
            }
        }
    }
    return false;
}

bool  glPhysicalWorld::find_component_by_id( long mObject_id,
                                      vector<VerbalObject*>* object_hier
                                      )
{
    bool            retval  =false;
    VerbalObject*   ptr     =NULL;
    bool            match   =false;
    list<glObject*>::iterator iter = m_moveables.begin();
    
    while (iter != m_moveables.end())
    {
        ptr = (VerbalObject*)*iter++;
        match  = (mObject_id == ptr->m_object_id);
        if (match)
        {
            object_hier->push_back(ptr);
            return true;
        }
        if (ptr->is_a_molecule()) {
            // Note: this is the Molecule find_component, not the physical world one.
            retval = ((glMolecule*)ptr)->find_component_by_id( mObject_id,
                                                        object_hier  );
            if (retval) {
                object_hier->push_back( ptr );
                return true;
            }
        }
    }
    return false;
}

void glPhysicalWorld::erase_scene( )
{
    list<glObject*>::iterator iter = m_moveables.begin();
    while (iter != m_moveables.end())
    {
        delete *iter;
        iter++;
    }
    m_moveables.erase( m_moveables.begin(), m_moveables.end() );
}

void glPhysicalWorld::add_object( glObject* mObject )
{
    if (m_temporarily_block==false) {
        mObject->m_registered = true;
        m_moveables.push_back( mObject );
    }
}

void glPhysicalWorld::remove_object   ( glObject* mObject )
{
    if (m_temporarily_block==false) {
        mObject->m_registered = false;
        m_moveables.remove( mObject );
    }
}

void glPhysicalWorld::list_object_ids(  )
{
    printf("Objects:\n");
    list<glObject*>::iterator iter = m_moveables.begin();
    while (iter != m_moveables.end())
    {
        printf(" %ld, ", (*iter)->m_object_id );
        iter++;
    }
}

extern glApartment apartment;

void glPhysicalWorld::release_vertex_memories()
{
    list<glObject*>::iterator iter = m_moveables.begin();
    for (; iter != m_moveables.end(); iter++)
    {
        (*iter)->release_memory();
    }
}

/* Physics Simulator.  Passive objects only - does not do the robot. */
void glPhysicalWorld::time_slice( float time_period )
{
    list<glObject*>::iterator iter = m_moveables.begin();
    for (; iter != m_moveables.end(); iter++)
    {
        (*iter)->clear_net_forces();
        (*iter)->add_force(m_gravity);
        (*iter)->f_ma();

        // collision with floor:
        bool collided = (*iter)->evaluate_collision(NULL);
        if (collided)
            floor_bounce( *iter );
        
        // colision with all walls:
        if (((VerbalObject*)(*iter))->m_object_type_name.compare("sphere")==0)
        {
            apartment.evaluate_collision( (glSphere*)(*iter) );
        }
        
        // gravity is speed up the ball on the way down.
        // and slowing after snells reflection.  So there is a loss of speed (energy) on the way up!
        // 
        (*iter)->update_position(time_period);
        (*iter)->update_rotation(time_period);
        
    }
    check_boundaries();
}

bool glPhysicalWorld::is_out_of_bounds(MathVector mPos)
{
    if ((mPos[0] > max_corner[0]) || (mPos[0] < min_corner[0]))
        return true;
    if ((mPos[1] > max_corner[1]) || (mPos[1] < min_corner[1]))
        return true;
    if ((mPos[2] > max_corner[2]) || (mPos[2] < min_corner[2]))
        return true;
    return false;
}

void glPhysicalWorld::check_boundaries()
{
    bool oobs=false;
    list<glObject*>::iterator iter = m_moveables.begin();
    for (; iter != m_moveables.end(); iter++)
    {
        oobs = is_out_of_bounds( (*iter)->m_position );
        (*iter)->is_active = !oobs;
    }
}

MathVector glPhysicalWorld::get_gravity( )
{
    MathVector g(3);
    g[0] = 0;  g[1] = 0.0; /* inches/second^2 */   g[2] = 0;
    if (m_has_gravity)
        g[1] = -386.088582677165;
    return g;
}
void glPhysicalWorld::floor_bounce( glObject* mObject )        // use snells law to calculate resulting velocity.
{
    MathVector normal(3);
    normal[0]=0.0;  normal[1]=1.0; normal[2]=0.0;
    float speed_before = mObject->m_velocity.magnitude();
    snells_law( mObject, normal);
    adjust_velocity_for_inelasticity( mObject, speed_before );
    mObject->m_position = mObject->m_prev_position;
}

// use snells law to calculate resulting velocity.
// Angle of incidence = angle of exit.
MathVector glPhysicalWorld::snells_law( glObject* mObject, MathVector mSurfaceVector )
{
    float dot_product    = - 2. * mObject->m_velocity.dot(mSurfaceVector);
    MathVector resultant = mSurfaceVector * dot_product;
    resultant += mObject->m_velocity;
    mObject->m_velocity = resultant;
    return resultant;
}

void glPhysicalWorld::adjust_velocity_for_inelasticity( glObject* mObject, float mSpeedBefore )
{
    mObject->m_velocity.unitize();
    mObject->m_velocity *= mSpeedBefore * mObject->coef_of_restitution;    
}


void glPhysicalWorld::draw()
{
    int i=0;
    list<glObject*>::iterator iter = m_moveables.begin();
    for (; iter != m_moveables.end(); i++,iter++)
    {
        (*iter)->m_x_angle =  (*iter)->m_R_position[0];
        (*iter)->m_y_angle =  (*iter)->m_R_position[1];
        (*iter)->m_z_angle =  (*iter)->m_R_position[2];
        (*iter)->draw();
    }
}


