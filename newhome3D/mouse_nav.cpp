#include "all_objects.h"
#include "cursor_nav.hpp"



/*************************/
/* MOUSE STUFF           */
int     g_bButton1Down;
int     y_when_clicked;
int     x_when_clicked;
float   center_x_when_clicked = 0.;
float   center_y_when_clicked = 0.;
float   center_z_when_clicked = 0.;
double  y_angle_when_clicked  = 0.;
#define TRUE  1
#define FALSE 0

void MouseButton(int button, int state, int x, int y)
{	
  // Respond to mouse button presses.
  // If button1 pressed, mark this state so we know in motion function.
  if (button == GLUT_LEFT_BUTTON)
    {
    	center_x_when_clicked = centerX;
    	center_y_when_clicked = centerY;
    	center_z_when_clicked = centerZ;
        //Mathvector
        y_angle_when_clicked = atan2( centerX - eyeX, centerZ - eyeZ );
    	x_when_clicked = x;
    	y_when_clicked = y;
    	g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
    }
}

void MouseMotion(int x, int y)
{
    // If button1 pressed, zoom in/out if mouse is moved up/down.
	if (g_bButton1Down)
    {
        double geo_distance = sqrt((centerX - eyeX)*(centerX - eyeX) + (centerZ - eyeZ)*(centerZ - eyeZ));
        
        float deltaX = ( x - x_when_clicked );
        float deltaY = ( y - y_when_clicked )/2.;
        //centerY = (center_y_when_clicked + deltaY);

        float radians_per_pixel = 0.25 / 50.;
        float delta_angle = deltaX * radians_per_pixel;
        float new_angle   = y_angle_when_clicked + delta_angle;
        float delta_center_to_eye_X = geo_distance * sin(new_angle);       // center to eye x / z
        float delta_center_to_eye_Z = geo_distance * cos(new_angle);       // center to eye x / z
        //centerX = eyeX + delta_center_to_eye_X;
        //centerZ = eyeZ + delta_center_to_eye_Z;
        
        x -= 320;
        y -= 320;
      	//glutPostRedisplay();
    }
}


