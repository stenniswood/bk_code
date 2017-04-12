// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
//#include "Graphbase.hpp"
#include "bk_system_defs.h"
#include "control.hpp"
//#include "robot.hpp"
#include "stick_figure.hpp"
#include "display.h"



#define TOPMARGIN 10.0
#define margin_percent 0.07
#define MAX_MOTORS 50
#define FOOT_WIDTH 25

#define HAND_WIDTH  5
#define HAND_LENGTH 10

StickFigure::StickFigure(int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	init();
	//printf("================Constructor2!: width=%6.1f  height=%6.1f \n", AbdominBox.width, AbdominBox.height  );	
}

StickFigure::StickFigure()
:Control()
{
	init();
}

void StickFigure::init()
{
	//robot = NULL;
	AbAngle = 0.0;
	Chest_color  = 0xAFAFAF00;
	Chest_width  = 12.0;
	Chest_height = 36.0;
	HasBorder 		 = TRUE;
	background_color = 0x7F205020;
}

void StickFigure::draw_arm(int mStartx, int mStarty, byte mIndex)
{
	Fill_l  ( Limb_color[mIndex] );
	Stroke_l( Limb_color[mIndex] );
	StrokeWidth(3);
	
	//Leg* leg; // = (Leg*)robot->get_limb( mIndex );
	//int  num_motors = leg->getNumItems();

	float angles[MAX_MOTORS];
	//int num_angles;
	//leg->get_angle_vector( angles, &num_angles );

	// Now Draw a stick for each:
	// this lends itself towards recursion, but not necessary

	// UPPER ARM:
	Translate( mStartx, mStarty );
	Rotate(angles[0]);			// hip
	float y = -height/4.0;
	Line(0., 0., 0., y);

	// LOWER ARM:
	Translate(0, y);			// relative 
	Rotate(angles[1]);			// knee
	Line(0., 0., 0., y);

	// HAND:	
	Translate(0,y);	
	Rotate(angles[2]);			// ankle
	Line( 2.,  0., HAND_WIDTH+2, HAND_LENGTH);
	Line( 0.,  0., HAND_WIDTH,   HAND_LENGTH);
	Line(-2.,  0., HAND_WIDTH-2, HAND_LENGTH);

	// need VG help on a translate to revert all the way back to normal
	Rotate		( -angles[2]		);
	Translate	( 0,			 -y);
	Rotate		( -angles[1]	   );
	Translate	( 0,			 -y);
	Rotate		( -angles[0]	   );
	Translate	( -mStartx, -mStarty );
}

float StickFigure::get_leg_length()
{
	// Upper and lower leg :
	return 2.0*(height/4.0)+FOOT_WIDTH;
}
float StickFigure::get_arm_length()
{
	return (2.0*height/4.0);
}

/* Before calling, set the rotation to that of the chest. */
void StickFigure::draw_leg(int mStartx, int mStarty, byte mIndex)
{
	Fill_l  ( Limb_color[mIndex] );
	Stroke_l( Limb_color[mIndex] );
	StrokeWidth(3);	

	//Leg* leg = NULL; // (Leg*)robot->get_limb( mIndex );
	//int  num_motors; // = leg->getNumItems();
	
	float angles[MAX_MOTORS];
	//int num_angles;
	//leg->get_angle_vector( angles, &num_angles ); 

	// Now Draw a stick for each:
	// this lends itself towards recursion, but not necessary.

	// UPPER LEG:
	Translate( mStartx, mStarty );
	Rotate(angles[0]);			// hip
	float y = -height/4.0;
	Line(0., 0., 0., y);

	// LOWER LEG:
	Translate(0, y);			// relative
	Rotate(angles[1]);			// knee
	Line(0., 0., 0., y);

	// FOOT:	
	Translate(0,y);	
	Rotate(angles[2]+M_PI/2);		// ankle
	Line(0., 0., FOOT_WIDTH, 0.);

	// need VG help on a translate to revert all the way back to normal.
	Rotate(-angles[2]-M_PI/2);	
	Translate(0,-y);
	Rotate(-angles[1]);	
	Translate(0,-y);		
	Rotate(-angles[0]);
	Translate( -mStartx, -mStarty );
}

#define scale_x 0.75
#define scale_y 0.75

void StickFigure::draw_chest(float x, float y, float half_chest_width, float half_chest_height)
{
	Fill_l  ( Chest_color );
	Stroke_l( Chest_color );
	StrokeWidth(3);	

	// center of abdomin is 2/3 height of control
	// Add Tilt Sensor AccelAngularPosition.rx rotation here
	Translate( x, y );	
	Rotate   ( AbAngle );
	Scale( scale_x, scale_y );
	Roundrect(  -half_chest_width, -half_chest_height, 
				+Chest_width, +Chest_height, 2.0, 2.0);
}

int StickFigure::draw()
{
	Control::draw();
	//printf("StickFigure:a0=%6.1f a1=%6.1f a2=%6.1f\n", angles[0], angles[1], angles[2]);
	//float center_height_fraction = 2.0/4.0;
	float center_height = 1.1*get_leg_length()*scale_x;
	float half_chest_height = Chest_height/2.0;

	float center_width_fraction = 1.0/2.0;
	float center_width     = center_width_fraction*width;
	float half_chest_width = Chest_width/2.0;

	draw_chest(left+center_width, bottom+center_height, 
			   half_chest_width, half_chest_height );

	// Leave rotated, translated, scaled after drawing
	// Translated to center of chest
	int index = 0; //robot->find_limb_index( "LeftArm" );
	draw_arm( 0, +half_chest_height, index );
	//index = robot->find_limb_index( "RightArm" );
	draw_arm( 0, +half_chest_height, index );

	//index = robot->find_limb_index( "LeftLeg" );
	draw_leg( 0, -half_chest_height, index );	
	//index = robot->find_limb_index( "RightLeg" );
	draw_leg( 0, -half_chest_height, index ); 

	Rotate   ( -AbAngle );
	Scale( 1/scale_x, 1/scale_y );		
	Translate( -(left+center_width), -(bottom+center_height) );

	return 0;
}
