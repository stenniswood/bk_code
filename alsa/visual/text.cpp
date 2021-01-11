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
#include "text.hpp"



const char *fontfilename;
FT_Library ft;
FT_Face    face;
GLuint t_vbo;

GLint attribute_coord;
GLint uniform_tex;
GLint uniform_color;



int init_text()
{
	/* Initialize the FreeType2 library */
	if (FT_Init_FreeType(&ft)) {
		fprintf(stderr, "Could not init freetype library\n");
		return 0;
	}

	/* Load a font */
	fontfilename = "visual/FreeSans.ttf";
	if (FT_New_Face(ft, fontfilename, 0, &face)) {
		fprintf(stderr, "Could not open font %s\n", fontfilename);
		return 0;
	}
	return 1;
}


/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
void render_text(const char *text, float x, float y, float sx, float sy) 
{
	const char *p;
	FT_GlyphSlot g = face->glyph;

	/* Create a texture that will be used to hold one "glyph" */
	GLuint tex;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures  (1, &tex);
	glBindTexture  (GL_TEXTURE_2D, tex);
	glUniform1i    (uniform_tex, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, t_vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/* Loop through all characters */
	for (p = text; *p; p++) {
		/* Try to load and render the character */
		if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
			continue;

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		/* Calculate the vertex and texture coordinates */
		float x2 =  x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w  = g->bitmap.width * sx;
		float h  = g->bitmap.rows  * sy;

/*		txtpoint box[4] = {
			{x2  , -y2,   0, 0},
			{x2+w, -y2,   1, 0},
			{x2  , -y2-h, 0, 1},
			{x2+w, -y2-h, 1, 1},
		};*/
		txtpoint box[4] = {
			{x2  , -y2,   0, 0},
			{x2+w, -y2,   1, 0},
			{x2  , -y2-h, 0, 1},
			{x2+w, -y2-h, 1, 1},
		};

		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		y += (g->advance.x >> 6) * sx;
		x += (g->advance.y >> 6) * sy;
	}

	glDisableVertexAttribArray(attribute_coord);
	glDeleteTextures(1, &tex);
}


float compute_text_width( const char *text,  float sx )
{
	const char *p;
	float x=0;
	FT_GlyphSlot g = face->glyph;

	for (p = text; *p; p++) {
		if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
			continue;
//		float x2 =  x + g->bitmap_left * sx;
//		float w  = g->bitmap.width * sx;
		x += (g->advance.x >> 6) * sx;
	}	
	return x;
}


