/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "Putting It All Together" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */

#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
//#include <GL/glut.h>
//#include <GLU/glu.h>
#endif

#include <math.h>
#include "imageloader.h"
#include "gl_container.hpp"
#include "cylinder.hpp"
#include "room.hpp"
#include "table.hpp"
#include "stairway.hpp"
#include "cabinet.hpp"
#include "ibeam.hpp"
#include "robot_leg_piece.hpp"

glLegSegment  upper_leg;
glIbeam		ibeam(12.*8.);
glCabinet	cabinet;
glStairway  stairs;
glTable		table;
glRoom		apartment;
glContainer box;
glContainer box2;
glContainer tray;
glContainer tray2;
glCylinder  cyl (12);
glCylinder  cyl2(24);
#define GROUND_SIZE 800.


using namespace std;

const float BOX_SIZE = 27.0f; //The length of each side of the cube
float _angle = 0;            //The rotation of the box
float y_angle = 0;           //The rotation of the box
float x_angle = 0;           //The rotation of the box
float z_angle = 0;           //The rotation of the box
GLuint _textureId;           //The OpenGL id of the texture

float x = 0;            //The position of the box
float y = 0;            //The position of the box
float z = 5;            //The position of the box
GLdouble eyeX;
GLdouble eyeY;
GLdouble eyeZ;
GLdouble centerX,centerY,centerZ;
GLdouble upX, upY, upZ;
GLdouble centerRadius = 100.;
GLdouble centerAngle  = 0.0 ;
GLdouble angle_increment = 5.*3.1415/180.;
void rotate( )
{
	if (centerAngle > 2*M_PI)	centerAngle = 0.	;
	if (centerAngle < 0.    )	centerAngle = 2*M_PI;

	centerX = centerRadius * sin(centerAngle) + eyeX;
	centerZ = centerRadius * cos(centerAngle) + eyeZ;
	printf("centerAngle=%6.3f cx=%6.2f; cy=%6.2f \n", centerAngle, centerX, centerY );
}

void move_forward( float mAmount )
{
	float F_vec_x = eyeX - centerX;
	float F_vec_y = eyeY - centerY;
	float F_vec_z = eyeZ - centerZ;
	float sum = F_vec_x + F_vec_y + F_vec_z;
	
	F_vec_x *= mAmount/sum;
	F_vec_z *= mAmount/sum;
	//printf("Delta x=%6.3f \t", F_vec_x );
	//printf("Delta z=%6.3f \n", F_vec_z );

	eyeX 	+= F_vec_x;
	eyeZ 	+= F_vec_z;
	centerX += F_vec_x;
	centerZ += F_vec_z;
}

void move_sideways( float mAmount )
{
	float F_vec_x = eyeX - centerX;
	float F_vec_y = eyeY - centerY;
	float F_vec_z = eyeZ - centerZ;
	float sum = F_vec_x + F_vec_y + F_vec_z;

	F_vec_x *= mAmount/sum;
	F_vec_z *= mAmount/sum;
	
	eyeX 	+= F_vec_z;
	eyeZ 	+= F_vec_x;
	centerX += F_vec_z;
	centerZ += F_vec_x;
}

bool pause = false;
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 'd': centerAngle -= angle_increment; rotate(); break;
		case 'a': centerAngle += angle_increment; rotate(); break;
		case 'b': cabinet.m_door_angle += 2.; break;
		case 'v': cabinet.m_door_angle += 2.; break;

		case 'o': eyeX -= 5.; centerX -= 5.; break;
		case 'p': eyeX += 5.; centerX += 5.; break;
		case 'q': eyeY -= 5.; centerY -= 5.; break;
		case 'r': eyeY += 5.; centerY += 5.; break;

		case 'e': move_forward( 2.);  	break;
		case 'c': move_forward( -2.); 	break; 
		case 's': move_sideways( 2.);	break;
		case 'f': move_sideways( -2.); 	break;

		case ' ': pause = !pause; break;
		case 27: //Escape key
			exit(0);
	}
}

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) 
{
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA,
				 image->width, image->height,
				 0,
				 GL_RGBA,
				 GL_UNSIGNED_BYTE,
				 image->pixels);
	return textureId;
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	
	//Image* image = loadBMP("vtr.bmp");
	Image* image = loadBMP("me_in_car.bmp");	
	_textureId = loadTexture(image);
	delete image;
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
	gluPerspective(75.0, (float)w / (float)h, 1.0, 400.0);	
}



void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//glTranslatef(0.0f, 0.0f, -200.0f);
	
	GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	
	GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPos[] = {-2 * BOX_SIZE, 200,  4 * BOX_SIZE, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	
	gluLookAt( eyeX,   	  eyeY,      eyeZ,  
			   centerX,   centerY,   centerZ,  
//			   upX,   	  upY,   	upZ   );
			   0,   	  1.0,   	0   );
				
	//glTranslatef(eyeX, eyeY, eyeZ);
/*	glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	glRotatef(z_angle, 0.0f, 0.0f, 1.0f);		*/
	
	glShadeModel(GL_FLAT);

	//glTranslatef(x, y, z);
		
	// Draw triangle VBO : 
	ibeam.draw();
	
	//upper_leg.draw();
	table.draw();
	apartment.draw();
	tray.draw();
	//box.draw ();
	//box2.draw();
	stairs.draw();
	//cyl.draw ();
	//cyl2.draw();
	//cabinet.draw();
	
	// Draw ground:
	glBegin   (GL_QUADS);
	glNormal3f(0.0, 1.0f, 0.0f );
	glColor3f (0.7f, 0.7f, 0.7f);
	glVertex3f(-GROUND_SIZE, 0, -GROUND_SIZE);
	glVertex3f( GROUND_SIZE, 0, -GROUND_SIZE);
	glVertex3f( GROUND_SIZE, 0, GROUND_SIZE );
	glVertex3f(-GROUND_SIZE, 0, GROUND_SIZE );
	glEnd();	
	
	// insert top face... etc. here:
	glBegin(GL_QUADS);	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glColor3f(1.0f, 1.0f, 1.0f);
	// Insert //Front Face code here...

	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);

	glDisable(GL_TEXTURE_2D);
	glEnd();
		
	glutSwapBuffers();
}

//Called every 25 milliseconds
void update(int value) {
	if (pause==false) {	
		_angle += 1.0f;
		if (_angle > 360) {
			_angle -= 360;
		}
		glutPostRedisplay();
	}
	glutTimerFunc(25, update, 0);
}

void init_objects()
{
	//upper_leg
	
	ibeam.m_y = 12.*9.;
	ibeam.create();
	
	apartment.generate_VBO();
	apartment.generate_IBO();

	cabinet.Relocate( 0., 0, 0. );
	cabinet.generate_VBO();
	cabinet.generate_IBO();

	table.Relocate( 144./2., 0, 160./2. );
	table.generate_vertices();
	table.generate_VBO();
	table.generate_IBO();

	stairs.generate_VBO();
	stairs.generate_IBO();
	
	tray.width  = 18.;
	tray.height = 5.;
	tray.depth  = 12.0;
	tray.m_y = tray.height+2.;
	tray.generate_vertices();		
	tray.generate_VBO(); 
	tray.generate_IBO(); 	

	box.width  = 3.;
	box.height = 3.;
	box.depth  = 2.0;
	box.m_y    = 2.;			// on top cylinder
	box.generate_vertices();
	box.generate_VBO();
	box.generate_IBO();

	box2.width  = 4.;
	box2.height = 2.;
	box2.depth  = 2.0;
	box2.m_x 	= 12.;
	box2.m_is_closed = true;
	box2.generate_vertices();
	box2.generate_VBO();
	box2.generate_IBO();

	cyl.m_radius = 4.;
	cyl.m_height = 8.;
	cyl.generate_vertices();	
	cyl.generate_VBO();
	cyl.generate_IBO();	

	cyl2.m_x = 4.;
	cyl2.m_y = 2.;
	cyl2.m_radius = 1.;
	cyl2.m_height = 12.;
	cyl2.generate_vertices();
	cyl2.generate_VBO();
	cyl2.generate_IBO(); 
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 760);
	
	glutCreateWindow("Putting It All Together - beyond-kinetics.com");
	initRendering();
	
	init_objects();
	eyeX = 80;
	eyeY = 120;
	eyeZ = 80;
	
	centerX = 160.;
	centerY = 60.;
	centerZ = 144.;
	
	upX = 80.;
	upY = 80.;
	upZ = 80.;

	glutDisplayFunc (drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc (handleResize);
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}


/*	glBegin(GL_QUADS);
	//Top face
	glColor3f(1.0f, 1.0f, 0.0f);
	glNormal3f(0.0, 1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	
	//Bottom face
	glColor3f(1.0f, 0.0f, 1.0f);
	glNormal3f(0.0, -1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	
	//Left face
	glNormal3f(-1.0, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	
	//Right face
	glNormal3f(1.0, 0.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);	
	glEnd();  */

	//Front face
/*	glBegin(GL_QUADS);

	glNormal3f(0.0, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	
	//Back face
	glNormal3f(0.0, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);

	glEnd();
*/	
