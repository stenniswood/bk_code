//
//  key_press.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 10/4/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <string>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "key_press.hpp"

GLdouble centerX=0;
GLdouble centerY=0;
GLdouble centerZ=0;

GLdouble eyeX=0;
GLdouble eyeY=50;
GLdouble eyeZ=100;

bool 	pauser;

#define eye_increment 5.0;

void handleKeypress(unsigned char key, int x, int y)
{
    static bool result = false;
    
    switch (key) {
        case 'v':   //show_tree = !show_tree;
            break;

        case 'i': centerY += 5.; break;		// look up.
        case 'k': centerY -= 5.; break;
        case 'j': centerX -= 5.; break;
        case 'l': centerX += 5.; break;        
        case 'u': centerZ -= 5.; break;
        case 'o': centerZ += 5.; break;

        case 'w': eyeY += eye_increment; break;		// look up.
        case 's': eyeY -= eye_increment; break;
        case 'a': eyeX -= eye_increment; break;
        case 'd': eyeX += eye_increment; break;        
        case 'q': eyeZ -= eye_increment; break;
        case 'e': eyeZ += eye_increment; break;

        case 27: //Escape key
            exit(0);
    }

  //glMatrixMode(GL_MODELVIEW);
    
    glutPostRedisplay();
}

