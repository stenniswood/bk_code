//
//  gl_misc.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/10/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__gl_misc__
#define __home3D__gl_misc__

#include <stdio.h>
#include "vector.hpp"
#include "gl_object.hpp"
//#include "glComputableWall.h"


/* This can be used to show a 3D (or 2D) line anywhere in the opengl space.
 Original purpose is to show possible robot positions based on visual input.
 */

class glLine : public glObject
{
public:
    glLine();
    glLine(MathVector start, MathVector end);
    glLine(LineSegment mls );
    ~glLine();

    virtual void	gl_register  (  );
    virtual void    gl_unregister(  );
    virtual void	draw_body	 ( 	);          // Override this with open gl commands.

    LineSegment  ls;
    MathVector m_Start;
    MathVector m_End;
    float      m_thickness;
    bool       m_is_shown;
};

class glWallLine : public glLine
{
public:
    glWallLine();
    glWallLine(MathVector start, MathVector end);
    glWallLine(LineSegment mls );
    ~glWallLine();
//    bool    wall_comes_between( glComputeableWall* mWall );     // aids in determining if another wall is located between this line and the derived wall.
                // which would mean, not a valid location on the map - the camera cannot see thru one wall to get the depth of the one behind it!
    
    float m_perp_distance;
    int   m_wall_index;       // the line was derived paralell from this wall.  see glDwellingLevel
    
};


#endif /* defined(__home3D__gl_misc__) */
