// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "Graphbase.hpp"
#include "control.hpp"
#include "progressbar.hpp"
#include "display.h"

#define margin_percent 0.07



ProgressBar::ProgressBar(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	text		= NULL;
	title 		= NULL;
}

ProgressBar::ProgressBar()
{
	set_position( 0, 0, 0, 0 );
	Visible = FALSE;
	text	= NULL;	
	title 	= NULL;
}

int ProgressBar::draw()
{	
	Control::draw();
	float text_size = 14.0;

	float X_gap =border_roundness_x/3.0;
	float Y_gap =2.0;
	
	// Draw Boundary:
	//Rect( left, bottom, +width, +height );
	float x1 = left+X_gap;
	float y1 = bottom+Y_gap; 
	float x2 = (fraction*(width-2*X_gap));
	float y2 = +height-2*Y_gap;

	Fill(255,0,0,1.0);
	Stroke_l(0xFF000000);
	StrokeWidth(1.0);
	
	VGfloat stops[] = {
		0.0, 1.0, 1.0, 1.0, 1.0,
		0.5, 0.5, 0.5, 0.5, 1.0,
		1.0, 0.5, 0.0, 0.0, 1.0
	};
	
	//FillLinearGradient(x1, y1, x2, y2, stops, 3);

	// Draw Filled Portion
	Fill(255,0,0,1.0);
	
	Rect( x1, y1, x2, y2 );
}


/**********************
 Would rather borrow someone elses widgets. 
 	ie. gnone, or 
 FOR LINUX, SEE:
 
 	scripts/kconfig/lxdialog/checklist.o
 	scripts/kconfig/lxdialog/checklist.o
*************************/ 	
