#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H


//#include "../common/shader_utils.h"
#include "shader_utils.h"

#include "res_texture.c"
#include "gl_graph.hpp"
#include "text.hpp"


GLuint graph_program;
GLuint text_program;
int    mode 	= 0;
float  offset_x = -1500.0;
float  scale_x  = 100.0;


int init_resources() 
{
	init_text();

	text_program = create_program("visual/text.v.glsl", "visual/text.f.glsl");
	if (program == 0)
		return 0;


	graph_program = create_program("visual/graph.v.glsl", "visual/graph.f.glsl");
	if (program == 0)
		return 0;
	attribute_coord2d = get_attrib(program, "coord2d");
	uniform_offset_x = get_uniform(program, "offset_x");
	uniform_scale_x = get_uniform(program, "scale_x");
	uniform_sprite = get_uniform(program, "sprite");
	uniform_mytexture = get_uniform(program, "mytexture");
	if (attribute_coord2d == -1 || uniform_offset_x == -1 || uniform_scale_x == -1 || uniform_sprite == -1 || uniform_mytexture == -1)
		return 0;


	attribute_coord = get_attrib (program, "coord");
	uniform_tex     = get_uniform(program, "tex"  );
	uniform_color   = get_uniform(program, "color");
	if(attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
		return 0;

	/* Enable blending */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create our own temporary buffer for _____
	glGenBuffers(1, &t_vbo);	

	/* Enable point sprites (not necessary for true OpenGL ES 2.0) */
#ifndef GL_ES_VERSION_2_0
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

	/* Upload the texture for our point sprites *
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res_texture.width, res_texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, res_texture.pixel_data);

	
	// Create the vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
*/


	// Tell OpenGL to copy our array to the buffer object
	//glBufferData(GL_ARRAY_BUFFER, sizeof graph, graph, GL_STATIC_DRAW);


	return 1;
}

void print_keys()
{
	printf("Use left/right to move horizontally.\n");
	printf("Use up/down to change the horizontal scale.\n");
	printf("Press home to reset the position and scale.\n");
	printf("Press F1 to draw lines.\n");
	printf("Press F2 to draw points.\n");
	printf("Press F3 to draw point sprites.\n");
}

void keys_special(int key, int x, int y) 
{
	switch (key) {
	case GLUT_KEY_F1:
		mode = 0;
		printf("Now drawing using lines.\n");
		break;
	case GLUT_KEY_F2:
		mode = 1;
		printf("Now drawing using points.\n");
		break;
	case GLUT_KEY_F3:
		mode = 2;
		printf("Now drawing using point sprites.\n");
		break;
	case GLUT_KEY_LEFT:
		offset_x -= 10.0;
		break;
	case GLUT_KEY_RIGHT:
		offset_x += 10.0;
		break;
	case GLUT_KEY_UP:
		scale_x *= 1.5;
		break;
	case GLUT_KEY_DOWN:
		scale_x /= 1.5;
		break;
	case GLUT_KEY_HOME:
		offset_x = -50.0;
		scale_x = 1.0;
		break;
	}

	glutPostRedisplay();
}

void free_resources() 
{
	glDeleteProgram(program);
}


int gl_main( ) 
{
	int argc = 0;
	glutInit(&argc, NULL);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Correlation");

	GLenum glew_status = glewInit();

	if (GLEW_OK != glew_status) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "No support for OpenGL 2.0 found\n");
		return 1;
	}

	GLfloat range[2];

	glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, range);
	if (range[1] < res_texture.width)
		fprintf(stderr, "WARNING: point sprite range (%f, %f) too small\n", range[0], range[1]);


	if (init_resources()) {
		glutDisplayFunc(graph_display);
		glutSpecialFunc(keys_special);
		glutMainLoop();
	}

	free_resources();
	return 0;
}
