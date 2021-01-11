#ifndef _TEXT_HPP_
#define _TEXT_HPP_


struct txtpoint {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};

extern const char *fontfilename;
extern FT_Library ft;
extern FT_Face    face;
extern GLuint     t_vbo;

extern GLint attribute_coord;
extern GLint uniform_tex;
extern GLint uniform_color;


int  init_text();

float compute_text_width( const char *text,  float sx );

void render_text(const char *text, float x, float y, float sx, float sy) ;



#endif
