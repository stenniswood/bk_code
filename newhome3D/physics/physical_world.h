//
//  physical_world.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/27/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__physical_world__
#define __home3D__physical_world__

#include <stdio.h>
#include <list>
#include "gl_object.hpp"
#include "moving_object.h"
#include "verbal_object.h"


class glPhysicalWorld 
{
public:
    glPhysicalWorld( bool hasGravity=false );
    ~glPhysicalWorld();

    void        release_vertex_memories();
    void        time_slice          ( float time_period=0.01 );
    bool        is_out_of_bounds    ( MathVector mPos);
    void        check_boundaries    ( );
    void        erase_scene         ( );

    void        add_object          ( glObject* mObject );
    void        remove_object       ( glObject* mObject );
    void        block               ( )       { m_temporarily_block = true;  }
    void        unblock             ( )       { m_temporarily_block = false; }
    MathVector  get_gravity         ( );
    MathVector  snells_law          ( glObject* mObject, MathVector mSurfaceVector );
    void        adjust_velocity_for_inelasticity( glObject* mObject, float mSpeedBefore );
    
    void        floor_bounce        ( glObject* mObject );        // use snells law to calculate resulting velocity.
    bool        find_component      ( string&  mRequestedName, string& mTypeName,
                                      vector<VerbalObject*>* object_hier,
                                      bool  mTypeMustMatch=true  );
                                
    bool        find_component_by_id( long mObject_id,
                                      vector<VerbalObject*>* object_hier );
    
    glObject*   find_object_id      ( long mID );
    void        list_object_ids     ( );
    void        draw                ( );
    
    bool             m_temporarily_block;
    list<glObject*>  m_moveables;
    bool             m_has_gravity;
    MathVector       m_gravity;
    
    MathVector       max_corner;
    MathVector       min_corner;
};




extern glPhysicalWorld  theWorld;

//vector<glObject>  m_stationary;
#endif /* defined(__home3D__physical_world__) */
