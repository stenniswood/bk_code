#ifndef _GL_GRAPH_HPP_
#define _GL_GRAPH_HPP_

#include <GL/glew.h>
#include <GL/glut.h>


extern GLint attribute_coord2d;
extern GLint uniform_offset_x;
extern GLint uniform_scale_x;
extern GLint uniform_sprite;
extern GLuint texture_id;
extern GLint uniform_mytexture;
extern GLuint vbo;


struct point {
	GLfloat x;
	GLfloat y;
};

#define SIZE_OF_GRAPH 5000
extern point graph[SIZE_OF_GRAPH];

void graph_display (  );


#endif
