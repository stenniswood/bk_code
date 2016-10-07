//
//  glMolecule.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/8/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "all_objects.h"
#include "glMolecule.h"


glMolecule::glMolecule( )
{
    
}
glMolecule::glMolecule( string mName )
{
    m_object_type_name = mName;
}

glMolecule::~glMolecule( )
{
    //VerbalObject::~VerbalObject();
}

void glMolecule::Initialize	( )
{
    m_object_class  = 16;    
}

void glMolecule::set_acceleration()         // transfers into all atoms/sub-molecules.
{
    int size = (int)m_components.size();
    for (int i=0; i<size; i++)
    {
        m_components[i]->m_acceleration[0] = m_acceleration[0];
        m_components[i]->m_acceleration[1] = m_acceleration[1];
        m_components[i]->m_acceleration[2] = m_acceleration[2];
    }
}
void glMolecule::set_veloctiy()             // transfers into all atoms/sub-molecules.
{
    int size = (int)m_components.size();
    for (int i=0; i<size; i++)
    {
        m_components[i]->m_velocity[0] = m_velocity[0];
        m_components[i]->m_velocity[1] = m_velocity[1];
        m_components[i]->m_velocity[2] = m_velocity[2];
    }
}
void glMolecule::set_angular_acceleration() // transfers into all atoms/sub-molecules.
{
    int size = (int)m_components.size();
    for (int i=0; i<size; i++)
    {
        m_components[i]->m_position[0] = m_position[0];
        m_components[i]->m_position[1] = m_acceleration[1];
        m_components[i]->m_position[2] = m_acceleration[2];
    }
}
void glMolecule::set_angular_veloctiy()     // transfers into all atoms/sub-molecules.
{
    int size = (int)m_components.size();
    for (int i=0; i<size; i++)
    {
        m_components[i]->m_acceleration[0] = m_acceleration[0];
        m_components[i]->m_acceleration[1] = m_acceleration[1];
        m_components[i]->m_acceleration[2] = m_acceleration[2];
    }
}

void glMolecule::set_relative_xyz( glObject* mObj, float mX, float mY, float mZ)
{
    mObj->m_x =  mX;
    mObj->m_y =  mY;
    mObj->m_z =  mZ;
}
void glMolecule::set_relative_x( glObject* mObj, float mX )
{
    mObj->m_x =  mX;
}
void glMolecule::set_relative_y( glObject* mObj, float mY )
{
    mObj->m_y =  mY;
}
void glMolecule::set_relative_z( glObject* mObj, float mZ )
{
    mObj->m_z =  mZ;
}

void glMolecule::compute_max()
{
    for (int i=0; i<m_components.size(); i++)
    {
        m_components[i]->compute_max();
    }
    // The max values have to be mapped thru the components coordinates to get world coords.
    //  m_components[i]->map_coords();
    
    for (int i=0; i<m_components.size(); i++)
    {
        if (m_components[i]->m_max.position[0] > m_max.position[0])
            m_max.position[0] = m_components[i]->m_max.position[0];
        
        if (m_components[i]->m_max.position[1] > m_max.position[1])
            m_max.position[1] = m_components[i]->m_max.position[1];

        if (m_components[i]->m_max.position[2] > m_max.position[2])
            m_max.position[2] = m_components[i]->m_max.position[2];
    }
}
void glMolecule::compute_min()
{
    for (int i=0; i<m_components.size(); i++)
    {
        m_components[i]->compute_min();
        if (m_components[i]->m_min.position[0] < m_min.position[0])
            m_min.position[0] = m_components[i]->m_min.position[0];
        
        if (m_components[i]->m_min.position[1] < m_min.position[1])
            m_min.position[1] = m_components[i]->m_min.position[1];

        if (m_components[i]->m_min.position[2] < m_min.position[2])
            m_min.position[2] = m_components[i]->m_min.position[2];
    }
}
void glMolecule::create_components( )
{
    /* SHOULD ONLY USE setup() for the components.  Create() will call gl_register() which is done later for the whole molecule.  if you don't want duplicates, don't call the glObject* tmp->create() 
    */
}

void glMolecule::setup( )
{
    create_components();
}
void glMolecule::create      ( )
{
    setup();    
    gl_register();
}

void glMolecule::gl_register      ( )
{
    bool prev = theWorld.m_temporarily_block;
    theWorld.block();
    for (int i=0; i<m_components.size(); i++)
        m_components[i]->gl_register();
    
    if (prev==false) {
        theWorld.unblock();
        theWorld.add_object(this);  // We only want it to draw this object (not all the independant atoms.
    }
}

void glMolecule::gl_unregister    ( )
{
    for (int i=0; i<m_components.size(); i++)
        m_components[i]->gl_unregister();
}

void glMolecule::release_memory(  )
{
    for (int i=0; i<m_components.size(); i++)
        m_components[i]->release_memory();
}


void glMolecule::draw_body( )
{
    for (int i=0; i<m_components.size(); i++)
        m_components[i]->draw();
}

/* each atom gets drawn by theWorld->draw() 
 This ruins the nested location however.
 m_x are set as relative to the molecule object.
 
*/

VerbalObject*  glMolecule::get_component( int index )
{
    return ((VerbalObject*)m_components[index]);
}

bool  glMolecule::find_component( string&  mRequestedName,  string& mTypeName,
                                 vector<VerbalObject*>* object_hier,
                                 bool mTypeMustMatch )
{
    bool retval=false;
    VerbalObject* ptr=NULL;
    bool match  = false;
    for (int i=0; i<m_components.size(); i++)
    {
        ptr = get_component(i);
        bool is_initd      = (ptr->m_name.length() > 0);
        int compare_result = ptr->m_name.compare( mRequestedName );
        match  = is_initd && (compare_result==0);
        if (match)
        {
            if (mTypeMustMatch)
            {
                if (mTypeName.compare( ptr->m_object_type_name )==0 ) {
                    object_hier->push_back(ptr);
                    return true;
                }
            }
        }
        if (ptr->is_a_molecule()) {
            retval = ((glMolecule*)ptr)->find_component( mRequestedName, mTypeName, object_hier, mTypeMustMatch );
            if (retval) {
                object_hier->push_back(ptr);
                return true;
            }
        }
    }
    return false;
}

bool  glMolecule::find_component_by_id( long mObject_id,
                                 vector<VerbalObject*>* object_hier )
{
    bool retval=false;
    VerbalObject* ptr=NULL;
    bool match  = false;
    for (int i=0; i<m_components.size(); i++)
    {
        ptr = get_component(i);
        match  = mObject_id == ptr->m_object_id;
        if (match)
        {
            object_hier->push_back(ptr);
            return true;
        }
        if (ptr->is_a_molecule()) {
            retval = ((glMolecule*)ptr)->find_component_by_id( mObject_id, object_hier );
            if (retval) {
                object_hier->push_back(ptr);
                return true;
            }
        }
    }
    return false;
}

