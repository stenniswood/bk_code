#ifndef _CURSOR_NAV_HPP_
#define _CURSOR_NAV_HPP_


#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#endif

#include "route.hpp"


extern GLdouble   eyeX,eyeY,eyeZ;
extern GLdouble   centerX,centerY,centerZ;

void rotate				( );
void move_forward		( float mAmount );
void move_sideways		( float mAmount );
void eye_follow_route	( glRoute& mRoute);
void assume_robot_view	( );



#endif
