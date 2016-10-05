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

#include "room.hpp"


//#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define TOP_AND_BOTTOM 2
#define CLOSE_OUT 1

Room::Room(  )
{
	m_origin.set_xyz( 0.0, 0.0, 0.0 );
}

Room::~Room(  )
{
    
}

void Room::create_walls()
{
    
}
