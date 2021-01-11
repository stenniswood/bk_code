#ifndef _GL_SETUP_HPP_
#define _GL_SETUP_HPP_


#include <GL/glew.h>
#include <GL/freeglut.h>


extern GLuint graph_program;
extern GLuint text_program;


extern int mode;
extern float offset_x;
extern float scale_x;


int  init_resources( );
void print_keys    ( );
void keys_special  ( int key, int x, int y) ;
int  gl_main	   ( );
void free_resources( );


#endif
