
/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees 
   and is provided without guarantee or warrantee expressed or 
   implied. This program is -not- in the public domain. */

/* This program was requested by Patrick Earl; hopefully someone else
   will write the equivalent Direct3D immediate mode program. */

#include <GL/glut.h>

#include "glSphere.h"
#include "ibeam.hpp"
#include "cylinder.hpp"
#include "key_press.hpp"


GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */
GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
  {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertexes. */



glSphere myball(4*NBA_BASKET_BALL_RADIUS, 10, 10 );
glIbeam  mybeam(75);
glCylinder myrod(40);


void drawBox(void)
{
  int i;

  for (i = 0; i < 6; i++) {
    glBegin(GL_QUADS);
        
    glNormal3fv(&n[i][0]);
    glVertex3fv(&v[faces[i][0]][0]);
    glVertex3fv(&v[faces[i][1]][0]);
    glVertex3fv(&v[faces[i][2]][0]);
    glVertex3fv(&v[faces[i][3]][0]); 
    glEnd();
  }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  	glMatrixMode(GL_MODELVIEW);		// GL_MODELVIEW  GL_PROJECTION
	glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,  		/* eye is at (0,0,5) */
    	centerX, centerY, centerZ,      /* center is at (0,0,0) */
    	0.0, 1.0, 0.);      			/* up is in positive Y direction */
	glShadeModel(GL_FLAT);   

//  drawBox();
	myball.draw();
	mybeam.draw();
	myrod.draw();
	
	draw_square();
	
  glutSwapBuffers();
}
int sq_VBO;
void make_square()
{
	struct Vertex v;
	vector<struct Vertex> verts;
	v.position[0] = 50;		v.position[2] = 50;		v.position[2] = 50;
	verts.push_back(v);
	v.position[0] = 50;		v.position[2] = -50;	v.position[2] = 50;
	verts.push_back(v);
	v.position[0] = -50;	v.position[2] = 50;		v.position[2] = 50;
	verts.push_back(v);
	v.position[0] = -50;	v.position[2] = -50;	v.position[2] = 50;
	verts.push_back(v);
	
	glGenBuffers( 1, &sq_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, sq_VBO );	// Make the new VBO active  GL_COLOR_ARRAY

	// Generate & Upload vertex data to the video device 
	int size = (int)verts.size() * sizeof(Vertex_pnc);
	glBufferData		 ( GL_ARRAY_BUFFER, size, verts.data(), GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0   );   // vertex positions
	glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex_pnc), (void*)(offsetof(struct Vertex_pnc,color)));	// color
    glBindBuffer		 ( GL_ARRAY_BUFFER, sq_VBO           );
	glEnableVertexAttribArray( 0 );	
	glEnableVertexAttribArray( 2 );	// GLKVertexAttribColor	
	glBindBuffer(GL_ARRAY_BUFFER, sq_VBO);
	
	
}
void draw_square()
{
	glBindBuffer(GL_ARRAY_BUFFER, sq_VBO);            // for vertex attributes
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboId2);    // for indices

	glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex position array
	//glEnableClientState(GL_NORMAL_ARRAY);             // activate vertex normal array
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);      // activate texture coord array

	// do same as vertex array except pointer
	glVertexPointer(3, GL_FLOAT, stride, offset1);    // last param is offset, not ptr
	glNormalPointer(GL_FLOAT, stride, offset2);
	glTexCoordPointer(2, GL_FLOAT, stride, offset3);

	// draw 6 faces using offset of index array
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);

	glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex position array
	glDisableClientState(GL_NORMAL_ARRAY);            // deactivate vertex normal array
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);     // deactivate vertex tex coord array

	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init(void)
{
	myball.relocate( 0, 0, 0 );
	myball.generate_vertices();
	myball.generate_indices ();
	myball.change_color(0xFF0000);
	myball.gl_register();

	mybeam.relocate( 100, 0, 0 );
	mybeam.setup();
	mybeam.change_color(0x00FF00);
	mybeam.gl_register();

	myrod.relocate( -100, 0, 0 );
	myrod.set_height( 100 );
	myrod.m_radius = 20;
	myrod.setup();
	myrod.change_color( 0x0000FF );	
	myrod.gl_register();

	make_square();

  /* Setup cube vertex data. */
  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

  /* Enable a single OpenGL light. */
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  /* Use depth buffering for hidden surface elimination. */
  glEnable(GL_DEPTH_TEST);

  /* Setup the view of the cube. */
  glMatrixMode(GL_PROJECTION);
  gluPerspective( /* field of view in degree */ 100.0,		// was 40
    /* aspect ratio */ 1.0,
    /* Z near */ 0.0, /* Z far */ 5000.0);
  glMatrixMode(GL_MODELVIEW);
//  gluLookAt(0.0, 50.0, -100.0,  /* eye is at (0,0,5) */
//    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
//    0.0, 1.0, 0.);      /* up is in positive Y direction */

  gluLookAt(eyeX, eyeY, eyeZ,  		/* eye is at (0,0,5) */
    centerX, centerY, centerZ,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      			/* up is in positive Y direction */


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
  /* Adjust cube position to be asthetic angle. */
  //glTranslatef(0.0, 0.0, -1.0);
  //glRotatef(60, 1.0, 0.0, 0.0);
  //glRotatef(-20, 0.0, 0.0, 1.0);
}


int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(1600, 800);
    glutInitWindowPosition(50, 120);  
  glutCreateWindow   ( "6DOF 3D Arm " );
  glutDisplayFunc    ( display );
  glutKeyboardFunc(handleKeypress);
  
  init();
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
