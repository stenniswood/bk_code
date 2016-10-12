//
//  gl_misc.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 8/10/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//


#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "gl_misc.h"
#include "physical_world.h"
#include "glComputableWall.h"


glLine::glLine()
:m_Start(3),m_End(3)
{
    m_color     = 0xFF0000FF;
    m_thickness = 1.0;
    m_is_shown  = true;
}

glLine::glLine(MathVector start, MathVector end)
{
    m_Start = start;
    m_End   = end;
    m_color = 0xFF0000FF;
    m_is_shown = true;
}
glLine::glLine(LineSegment mls )
:m_Start(3),m_End(3)
{
    ls      = mls;
    m_color = 0xFF0000FF;
    m_Start = ls.m_origin;
    m_End   = ls.m_origin + ls.m_vector * ls.m_length;
    m_is_shown = true;
}

glLine::~glLine()
{
    
}

void glLine::gl_register  (  )
{
    // do nothing.  b/c not worth storing 2 vertices.
    theWorld.add_object(this);
}
void glLine::gl_unregister(  )
{
    // do nothing.  b/c not worth storing 2 vertices.
    theWorld.remove_object(this);
}

void glLine::draw_body	 ( 	)          // Override this with open gl commands.
{
    if (m_is_shown==false) return ;
    
    float cr = (m_color & 0xFF0000)>>16;
    float cg = (m_color & 0x00FF00)>>8;
    float cb = (m_color & 0x0000FF)>>0;

    glLineWidth(m_thickness);
    glBegin    (GL_LINES);
    //glNormal3f(0.0, 1.0f, 0.0f );
    glColor3f (cr, cg, cb);
    glVertex3f( m_Start[0], m_Start[1], m_Start[2] );
    glVertex3f( m_End[0],   m_End[1],   m_End[2]   );
    glEnd();
    glLineWidth(1.0);
}


/***********************************************************************************/

glWallLine::glWallLine()
{
    
}
glWallLine::glWallLine(MathVector start, MathVector end)
{
    
}
glWallLine::glWallLine(LineSegment mls )
{
    
}
glWallLine::~glWallLine()
{
    
}

  // aids in determining if another wall is located between this line and the derived wall.
/*bool glWallLine::wall_comes_between( glComputeableWall* mWall )
{
    // STUB!
    
    return false;
}*/

