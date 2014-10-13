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
#include "Graphbase.hpp"
#include "../core/can/bk_system_defs.h"
#include "control.hpp"
#include "stick_leg.hpp"
#include "display.h"


#define TOPMARGIN 10.0
#define margin_percent 0.07

StickLeg::StickLeg(int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
	mp = NULL;
	AbdominBox.width  = 12.0;
	AbdominBox.height = 13.0;	
	HasBorder = TRUE;
	BackgroundColor = 0x7F205020;
	//printf("================Constructor2!: width=%6.1f  height=%6.1f \n", AbdominBox.width, AbdominBox.height  );	
}

StickLeg::StickLeg()
:Control()
{
	mp = NULL;
	AbdominBox.width  = 12.0;
	AbdominBox.height = 13.0;
	HasBorder = TRUE;	
	BackgroundColor = 0x7F205020;	
}

int StickLeg::draw()
{
	Control::draw();
	
	float angles[3];
	if (mp==NULL) return 0;
	mp->get_angle_vector( angles, 3 );
	//printf("StickLeg:a0=%6.1f a1=%6.1f a2=%6.1f\n", angles[0], angles[1], angles[2]);
	
	Fill_l  ( Leg_color );
	Stroke_l( Leg_color );
	StrokeWidth(3);	
		 
	//printf("================%s: width=%6.1f  height=%6.1f \n", text, AbdominBox.width, AbdominBox.height  );
	//AbdominBox.width  = 10.0;	// these get clobbered if only inited in constructor.
	//AbdominBox.height = 15.0;

	// ABDOMIN:
	// center of abdomin is 3/4 height of control.
	// Add Tilt Sensor AccelAngularPosition.rx rotation here.	
	AbdominBox.bottom = bottom + 3.0*height/4.0 - AbdominBox.height/2.0;
	AbdominBox.left   = left + width/2.0;

	Translate(AbdominBox.left+AbdominBox.width/2.0, AbdominBox.bottom);
	Rotate   (AbAngle);	
	Roundrect(0., 0., AbdominBox.width, AbdominBox.height, 2.0, 2.0);
	
	// UPPER LEG:
	//Translate(AbdominBox.left+AbdominBox.width/2.0,AbdominBox.bottom);					
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
	Line(0., 0., AbdominBox.width, 0.);

	// need VG help on a translate to revert all the way back to normal.
	Rotate(-angles[2]-M_PI/2);	
	Translate(0,-y);		
	Rotate(-angles[1]);	
	Translate(0,-y);		
	Rotate(-angles[0]);
	
	Rotate(-AbAngle);
	Translate(-(AbdominBox.left+AbdominBox.width/2.0),-(AbdominBox.bottom));	
	return 0;
}
