//
//  dwelling2.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 4/11/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef dwelling2_hpp
#define dwelling2_hpp

#include <stdio.h>
#include "glMolecule.h"
#include "door.hpp"
#include "full_wall.h"
#include "room2.hpp"

/* One level of the dwelling.  glDwelling will be multiple levels. */
class glDwellingLevel1  : public glMolecule
{
public:
    glDwellingLevel1 ();
    ~glDwellingLevel1();
    
    float            get_max_x       ();
    float            get_max_z       ();
    
    void              setup();
    virtual void      create_floor();
    glDwellingLevel1* create_copy();
    glDwellingLevel1* create_mirror_image();
    
    void             generate_exterior_walls();
    
    void             close_all_doors( float mFraction );
    glDoorWay*       crosses_doorway    ( MathVector pt1, MathVector pt2 );
    void             update_line_info   ( );  // wall origins and vectors.
    void             create_room        ( float width, float length );
    glDoor*          map_wall_coord     ( MathVector mWC );
    
    virtual bool     evaluate_collision ( glSphere* mOther );

    void             mirror_image_x     ( );
    void             mirror_image_z     ( );
    
    // FOR 2D Drawing (and maze solving):
    //void             create_map         ( glMap2D& mMap );
    // LOCATION FUNCTIONS (from visual & other means to find the location) :
    void             find_all_possibilies( float mLinearDistance, float mWallAngle, float mHeight );
    void             filter_blocked_possibilies( );
    void             accept_heading_measurement( float mAngleToNorth );
    
    
    // Not implemented yet... but not difficult.
    vector<Texture*>        m_door_textures;     // hold here instead of each door for memory savings.
    vector<glWallLine>      m_line_segs;
    
    float                   m_height;
    
    
    vector<Room>            m_rooms;
};


#endif /* dwelling2_hpp */
