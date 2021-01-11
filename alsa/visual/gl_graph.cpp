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
#include "gl_setup.hpp"



GLint attribute_coord2d;
GLint uniform_offset_x;
GLint uniform_scale_x;
GLint uniform_sprite;
GLuint texture_id;
GLint uniform_mytexture;

GLuint vbo;

struct graph_info {
	char* Title;
	char* x_axis_label;
	char* y_axis_label;
};

point graph[SIZE_OF_GRAPH];

void draw_title( char* mTitle, float sx, float sy )
{
	float width = compute_text_width(mTitle, sx );	
	float st = (0-width/2);
	render_text( mTitle, st, 1-25*sy, sx, sy);
}

void draw_xaxis_labels(char* xAxisLabel, float sx, float sy )
{
	float width = compute_text_width(xAxisLabel, sx );	
	float st = (0-width/2);
	render_text(xAxisLabel, st, -1+50*sx, sx, sy);
}
void draw_yaxis_labels(char* xAxisLabel, float sx, float sy )
{
	float width = compute_text_width(xAxisLabel, sx );	
	float st = -1;
	glRotated( 90.0, 0, 0, 1 );
	render_text(xAxisLabel, st, -1+50*sx, sx, sy);
}

void fill_data()
{
}

/* This is the callback function in gl_setup() for drawing. */
void graph_display() 
{
	float sx = 2.0 / glutGet(GLUT_WINDOW_WIDTH);
	float sy = 2.0 / glutGet(GLUT_WINDOW_HEIGHT);

	glUseProgram(text_program);

	glClearColor(0.2, 0.2, 0.6, 1);
	glClear     (GL_COLOR_BUFFER_BIT);

	/* Enable blending, necessary for our alpha texture */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLfloat black[4] = { 0, 0.2, 0.2, 1 };	
	GLfloat white[4] = { 1,   1,   1, 1 };
	GLfloat red[4]   = { 1, 0, 0, 1 };
	GLfloat transparent_green[4] = { 0, 1, 0, 0.5 };

	/* Set font size to 48 pixels, color to black */
	FT_Set_Pixel_Sizes(face, 0, 20);
	/* Effects of alignment */
	glUniform4fv(uniform_color, 1, white );

	char title[] = "Autocorrelation -vs- Lags";
	draw_title(title, sx,sy);
	//draw_xaxis_labels("Lags",sx,sy );
	//draw_yaxis_labels("Correlation", sx, sy );	
	
	//glutSwapBuffers();
	//return;
	//============ NOW DRAW THE GRAPH DATA POINTS =============================
	glUseProgram(graph_program);

	glUniform1i(uniform_mytexture, 0);
	glUniform1f(uniform_offset_x, offset_x);
	glUniform1f(uniform_scale_x, scale_x);

	/* Draw using the vertices in our vertex buffer object */
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Tell OpenGL to copy our array to the buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof graph, graph, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attribute_coord2d);
	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);

	/* Push each element in buffer_vertices to the vertex shader */
	switch (mode) {
	case 0:
		glUniform1f(uniform_sprite, 0);
		glDrawArrays(GL_LINE_STRIP, 0, SIZE_OF_GRAPH);
		break;
	case 1:
		glUniform1f(uniform_sprite, 1);
		glDrawArrays(GL_POINTS, 0, SIZE_OF_GRAPH);
		break;
	case 2:
		glUniform1f(uniform_sprite, res_texture.width);
		glDrawArrays(GL_POINTS, 0, SIZE_OF_GRAPH);
		break;
	}

	glutSwapBuffers();
}


