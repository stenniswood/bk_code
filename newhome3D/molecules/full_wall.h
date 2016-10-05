//
//  full_wall.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__full_wall__
#define __home3D__full_wall__

#include <stdio.h>
#include "glMolecule.h"
#include "glComputableWall.h"
#include "door.hpp"
#include "light_switch.hpp"
#include "gl_window.h"
#include "verbal_object.h"


#define hang_picture add_fixture


class glFullWall : public glMolecule
{
public:
    glFullWall (  );
    glFullWall ( glFullWall& mSrc );
    ~glFullWall(  );
    
    void            Initialize        ( );
    void            create_components ( );
    void            clear             ( );
    
    virtual void	setup               ( );
    virtual void 	set_color           ( Color mColor   );
    virtual void 	set_color           ( long mColor    );
    void            set_texture         ( int mSelection );
    
    void            set_length          ( float mLength )  { m_wall_length = mLength;  m_bare_wall.m_wall_length=mLength; };
    
    void            add_fixture         ( VerbalObject*   mFixture, float mDistanceAlong, float mHeight   );
    void            add_door            ( float mPositionLengthwise, glDoorWay*   mDoor                   );
    void            add_window          ( float mPositionLengthwise, float mSillHeight, glWindow* mWindow );
    void            extract_2d_line_info();
    
    
    glComputeableWall           m_bare_wall;
    vector<glDoorWay*>          m_doors;
    vector<VerbalObject*>       m_fixtures;         // light switch, plug, combos, phone jack, cable, painting, etc.
    vector<struct stWindow>     m_windows;          // bare wall has the window bay info
    
private:
    float                       m_wall_length;      // passed to bare wall
};




#endif /* defined(__home3D__full_wall__) */
