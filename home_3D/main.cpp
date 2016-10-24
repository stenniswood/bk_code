/* Home3D Robot Simulator
 * www.beyondkinetics.com
 * Stephen Tenniswood

 Start MySQL with:
    sudo /usr/local/mysql/support-files/mysql.server start

 */
#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#endif

#include <list>
#include <math.h>
#include <string>
#include <string.h>

#include "vector.hpp"
#include "imageloader.h"
#include "all_objects.h"
#include "simulator_memory.h"
#include "sequencer_memory.hpp"
#include "client_memory.hpp"
#include "proc_client_cmds.h"
#include "atom_objects.h"
#include "sql_load.hpp"
#include "robot_protocol.hpp"
#include "robot.hpp"
#include "cursor_nav.hpp"
#include "mouse_nav.hpp"
#include "init_samples.hpp"
#include "asset_importer.hpp"       // sjt spin on it


using namespace std;

#define ms_update_rate 20
bool  pauser = true;
union uGlobalVars theGlobals;

#define GROUND_SIZE 1500.
#define eye_follows_robots theGlobals.raw_array[1]
 
const float BOX_SIZE = 27.0f; // The length of each side of the cube

void change_route( MathVector mSource, MathVector mDestination )
{
    printf( "Now Showing Route : \n" );
    mSource.print();
    mDestination.print();
    map2D.map_route         ( multiRoute, mSource, mDestination );
    //robot.m_route.create_from_multi( multiRoute );
    //robot.plan_steps        ( 2.0*12 );
    
    robot.gl_register       ( );
    robot.m_glide_index = 0;
}


// Index into route.m_right_steps[] and left.
int  l_step_index  = 0;
int  r_step_index  = 0;
bool left_leg_turn = true;
bool last_direction_forward = true;
bool show_tree      = false;

glArm*  selected_arm  = &(robot.m_left_arm);          // 0=left; 1=right;
glLeg*  selected_leg  = &(robot.m_left_leg);          // 0=left; 1=right;
bool  last_select_leg = true;

#include "key_press.hpp"

void initRendering()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
}

void handleResize(int w, int h) {
	printf("handleResize( %d, %d)\n", w,h );
	glViewport(0, 0, w, h );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 5000.0);
}

void draw_objects()
{
    theWorld.draw();
}

void drawScene() 
{
	glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);		// GL_MODELVIEW  GL_PROJECTION
	glLoadIdentity();

	GLfloat  ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	
	GLfloat lightColor[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat lightPos[]   = { -2*BOX_SIZE, 200,  4*BOX_SIZE, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos  );
	glShadeModel(GL_SMOOTH);         // GL_FLAT GL_SMOOTH

	//printf("eye: %6.1f,%6.2f,%6.2f;  center: %6.1f,%6.1f,%6.1f\n", 
	//		eyeX,   	  eyeY,      eyeZ,  centerX,   centerY,   centerZ );		
	gluLookAt( eyeX,   	  eyeY,      eyeZ,
			   centerX,   centerY,   centerZ,
			   0.0,   	  1.0,   	0   );

	draw_objects();
    //drawMeshes();       // Imported meshes!

	// Draw ground:
/*	glBegin   (GL_QUADS);
	glNormal3f(0.0, 1.0f, 0.0f );
	glColor3f (1.0f, 0.2f, 0.7f);
	glVertex3f(-GROUND_SIZE, 0, -GROUND_SIZE);
	glVertex3f( GROUND_SIZE, 0, -GROUND_SIZE);
	glVertex3f( GROUND_SIZE, 0, GROUND_SIZE );
	glVertex3f(-GROUND_SIZE, 0, GROUND_SIZE );
	glEnd();	*/

	glutSwapBuffers();
}



/*
	Called every 25 milliseconds
	Runs thru the house along the path specified in "route"
*/
void update(int value) 
{
    float time_period       = (ms_update_rate) / 1000.;
	static int pause_count  = 1;

    //respond_to_client_timeslice( );
    /* CAMERA */
    //static int counter = 0;
    /* if (theGlobals.gv.CameraOn)  {
        if ((counter++ % 10)==0)
           ((CameraTexture*)screen.m_texture)->timeslice();
    } else {
       // ((CameraTexture*)screen.m_texture)->stop();
    } */
    
    //  VIEWPOINT 
    switch (eye_follows_robots) {
        case 0:  /* user moveable viewpoint */  break;
        case 1:  assume_robot_view();           break;
        case 2:  eye_follow_route( *bball_arena.bball_court->m_routes[0] );   break;  // demo sweep over basketball court.
        default: break;
    }
    
    if ((pauser==false) && (pause_count==0))
	{
        // WAVE THE PAPER:
        static float phase = 0.0;
        phase += 2.*M_PI / 180;     // 2 degrees
//        paper.gl_unregister();
//        paper.wave_it(10, 2, phase );
//        paper.gl_register();        
//        theWorld.time_slice( time_period*2 );
//        robot.time_slice();
//        create_nose_line();        
        pause_count = 1;
	} else if (pauser==false) pause_count--;

	glutPostRedisplay();
	glutTimerFunc(ms_update_rate, update, 0);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1600, 800);
    glutInitWindowPosition(50, 120);	
	glutCreateWindow("Robot Simulator - beyond-kinetics.com");
	initRendering();
	
	init_objects ();
	
    eyeX   = 0;      eyeY   = 140;     eyeZ   = 0;
    centerX= 50;     centerY= 50.00;   centerZ= 50;

	glutKeyboardFunc(handleKeypress);
	glutDisplayFunc (drawScene     );
	glutMouseFunc   (MouseButton  );
	glutMotionFunc  (MouseMotion  );
	glutReshapeFunc (handleResize  );
	glutTimerFunc   (ms_update_rate, update, 0 );
	glutMainLoop();    
	return 0;
}


//    connect_shared_client_memory   ( TRUE );
//    connect_shared_sequencer_memory( TRUE );
//    connect_shared_simulator_memory( TRUE );
    //InitRobotTopLevelProtocol();
    //sql_load_scene(1);
//    cli_detach_memory();
//    seq_detach_memory();
//    sim_detach_memory();
