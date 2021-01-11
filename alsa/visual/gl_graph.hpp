#ifndef _GL_GRAPH_HPP_
#define _GL_GRAPH_HPP_

#include <GL/glew.h>
#include <GL/glut.h>


struct point {
	GLfloat x;
	GLfloat y;
};

#define SIZE_OF_GRAPH 5000
extern point graph[SIZE_OF_GRAPH];

void graph_display (  );


#endif
