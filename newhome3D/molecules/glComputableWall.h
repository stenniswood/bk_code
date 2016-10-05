//
//  glComputableWall.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__glComputableWall__
#define __home3D__glComputableWall__

#include <stdio.h>
#include "wall.hpp"
#include "gl_misc.h"


class glComputeableWall : public glBareWall
{
public:
    glComputeableWall ( );
    ~glComputeableWall( );
    virtual void    relocate                ( float mX, float mY, float mZ );
    
    int             find_closest_door   ( MathVector mIntersection );
    float           get_distance_along  ( MathVector mIntersection );
    float           is_on_which_side    ( MathVector mPoint        );    // returns -1.0 or +1.0
    
    void            find_paths_around   ( list<float>& mDistanceAlong );
    void            find_paths_around   ( list<MathVector>& mPoints, float mOffsetAwayFromWall );
    int             find_alternate_via  ( MathVector mIntersection, MathVector& mNewPoint );
    bool            intersects          ( LineSegment mPath, float& t, float& s );
    
    // Linear Distance:
    float           get_door_near               ( int mDoorIndex  );
    float           get_door_center             ( int mDoorIndex  );
    float           get_door_far                ( int mDoorIndex  );
    
    struct Vertex	get_door_near_coord         ( int mDoorIndex  );
    struct Vertex	get_door_center_coord       ( int mDoorIndex  );
    struct Vertex	get_door_far_coord          ( int mDoorIndex  );
    
    MathVector      get_door_near_coord_plus	( int mDoorIndex, float mPerpDistance=0.0  );
    MathVector      get_door_center_coord_plus  ( int mDoorIndex, float mPerpDistance=0.0  );
    MathVector      get_door_far_coord_plus     ( int mDoorIndex, float mPerpDistance=0.0  );
    
    MathVector      get_point_away_from  ( float mDistanceAlong, float mPerpendicularDistance, bool mPositiveSide=true );
    MathVector      get_origin           (  );
    MathVector      get_far_end          (  );
    
    // For searching given a camera distance measurement away from the wall.  Creates parallel lines.
    vector<glWallLine>*  create_lines_distance_from( float mLinearDistance, float mWallAngleRadians, float mHeight );     // used for locating on a map.
    void            add_line_seg      ( glWallLine &line, float mHeight, MathVector &end_retraction, vector<glWallLine>* lines );
    bool            wall_is_between   ( glWallLine* mWallLine, glComputeableWall* mOrigWall );
    
    // aids in determining if another wall is located between this line and the derived wall.
    // which would mean, not a valid location on the map - the camera cannot see thru one wall to get the depth of the one behind it!

//  virtual bool    evaluate_collision( glMovingObject* mOther );
    virtual bool    evaluate_collision( glSphere* mOther );

    void            extract_2d_line_info    (  );      // Call this if any of the m_xyz_angles have changed!
    Line            m_line;                            // 2 vectors origin and direction.
};


#endif /* defined(__home3D__glComputableWall__) */
