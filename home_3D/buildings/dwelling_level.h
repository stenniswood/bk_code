//
//  dwelling_level.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/7/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#ifndef __home3D__dwelling_level__
#define __home3D__dwelling_level__

#include <stdio.h>
#include "Map2D.hpp"
#include "full_wall.h"
#include "gl_object.hpp"
#include "glMolecule.h"
#include "floor.h"
#include "glComputableWall.h"
#include "vector.hpp"
#include "gl_misc.h"


struct room {
    float sx;
    float sz;
    float ex;
    float ez;
    string name;
};

/* One level of the dwelling.  glDwelling will be multiple levels. */
class glDwellingLevel  : public glMolecule
{
public:
    glDwellingLevel ();
    ~glDwellingLevel();

    float            get_max_x       ();
    float            get_max_z       ();
    
    void             mirror_image_x( );
    void             mirror_image_z( );
    
    void             setup();
    virtual void     create_floor();        // to cover all.
    glDwellingLevel* create_copy();
    glDwellingLevel* create_mirror_image();
    
    void             generate_exterior_walls();
    
    void             close_all_doors( float mFraction );
    glDoorWay*       crosses_doorway    ( MathVector pt1, MathVector pt2 );
    void             update_line_info   ( );      // wall origins and vectors.
    void             create_room        ( float width, float length );
    glDoor*          map_wall_coord     ( MathVector mWC );

    virtual bool     evaluate_collision( glSphere* mOther );
    
    // FOR 2D Drawing (and maze solving):
    void             create_map         ( glMap2D& mMap );
    // LOCATION FUNCTIONS (from visual & other means to find the location) :    
    void             find_all_possibilies( float mLinearDistance, float mWallAngle, float mHeight );
    void             filter_blocked_possibilies( );
    void             accept_heading_measurement( float mAngleToNorth );
    
    
    // Not implemented yet... but not difficult.
    vector<Texture*>        m_door_textures;     // hold here instead of each door for memory savings.
    
    vector<glWallLine>      m_line_segs;
    
    float                   m_height;
    glFloor*                m_floor;
    vector<glFullWall*>     m_fwalls;
    vector<glObject*>       m_furniture;
    vector<glObject*>       m_things;
};


#endif /* defined(__home3D__dwelling_level__) */
