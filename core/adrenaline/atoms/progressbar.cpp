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

#include "control.hpp"
#include "progressbar.hpp"
#include "display.h"

#define margin_percent		   0.07
#define LINE_HEIGHT_MULTIPLIER 1.5

ProgressBar::ProgressBar(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}

ProgressBar::ProgressBar()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
	width=100; height=20;	
}

void ProgressBar::Initialize()
{
	Control::Initialize();
	strcpy (class_name, "ProgressBar");					
	show_percent = false;
	fill_color = 0xFFFF0000;
	text_color = 0xFFFFFFFF;
}
void ProgressBar::set_percentage( float Percentage )
{ 
	//printf("Progress::setPercentage()=%6.1f\n", Percentage);
	fraction = Percentage/100.0; 
}
void ProgressBar::print_info( )
{
	printf("ProgressBar::print_info():  Max=%6.2f; Current=%6.2f; Percent=%6.2f\n",
			Max, Current, fraction*100 );
}

/*(void ProgressBar::set_position_above    ( Control* Sibling )
{
	bottom  = Sibling->bottom + Sibling->height + DefaultPadding;
	if (show_percent)
		bottom += text_size*LINE_HEIGHT_MULTIPLIER;
}*/

int ProgressBar::draw()
{	
	Control::draw();
	float text_size = 14.0;

	float X_gap =border_roundness_x/3.0;
	float Y_gap =2.0;
	
	// Draw Boundary:
	float x1 = left+X_gap;
	float y1 = bottom+Y_gap; 
	float x2 = (fraction*(width-2*X_gap));
	float y2 = +height-2*Y_gap;


	// Draw Filled Portion
	StrokeWidth	(1.0);
	Stroke_l	(0xFF000000);
	Fill_l		(fill_color);	
	/*VGfloat stops[] = {
		0.0, 1.0, 0.0, 0.0, 1.0,
		0.5, 1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 0.0, 0.0, 1.0
	};
	FillLinearGradient(x1, y1, x2, y2, stops, 3); */
	Rect( x1, y1, x2, y2 );

	if (show_percent)
	{
		float center_x = width/2.0 + left;
		float text_y   = bottom - text_size*1.5;
		char percent[16]; 
		sprintf( percent, "%3.0f%%", fraction*100.0 );
		Stroke_l	(text_color);
		Fill_l		(text_color);			
		TextMid( center_x, text_y,  percent,  SerifTypeface, text_size );
	}
	return 1;
}


/**********************
 Would rather borrow someone elses widgets. 
 	ie. gnone, or 
 FOR LINUX, SEE:
 
 	scripts/kconfig/lxdialog/checklist.o
 	scripts/kconfig/lxdialog/checklist.o
*************************/ 	
