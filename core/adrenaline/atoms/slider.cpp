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
#include "slider.hpp"


#define MARKER_HEIGHT 20
#define MARKER_WIDTH  20

//#define DefaultPadding 10
#define TIC_WIDTH 10
#define margin_percent 0.07

/*SliderControl::SliderControl( float Max, float Min )
:Control()
{
	xAxisLabel	= (char*)"X Axis";
	yAxisLabel  = (char*)"Y Axis";
	title 		= (char*)"SliderControl";	
	set_text( (char*)"SliderControl" );	
	yMax 	= Max;
	yMin 	= Min;
}*/ 

SliderControl::SliderControl() 
: Control()
{
	strcpy (class_name, "SliderControl");					
	xAxisLabel	= NULL; //(char*)" ";
	yAxisLabel  = NULL; //(char*)" ";
	title 		= NULL; //(char*)" ";	
//	set_text( (char*)"SliderControl" );	
	yMax 	= 100;
	yMin 	= 0;
}

SliderControl::~SliderControl() 
{
}
void SliderControl::Initialize(	)
{
}

int SliderControl::calc_metrics()
{ return 0;
}
int	SliderControl::onKey( char mKey )
{ return 0;
}

/*void SliderControl::set_position(int Left, int Right, int Top, int Bottom)
{
	Control:set_position( Left, Right, Top, Bottom );
} */

int SliderControl::set_level(float level)
{
	if (level > yMax)	level = yMax;
	if (level < yMin)	level = yMin;
	
	Level = level;
	return 1;
}

void SliderControl::set_level_percent( float percent )
{
	if (percent > 100.0)	percent = 100.0;
	if (percent <   0.0)	percent = 0.0;

	float range = (yMax - yMin);
	Level  = range*(percent/100.0);
}

void SliderControl::draw_horizontal_tics(  ) 
{
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);

	float alpha = 1.0;
	float y_spacing = ((float)height) / 10.0;	
	// HORIZONTAL
	Fill(255, 0, 125, alpha);
	for (VGfloat y = bottom; y <= (bottom+height); y+=y_spacing) 
	{
		Line(left, y, left+TIC_WIDTH, y);
	}
}

void SliderControl::draw_vertical_line( )
{
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(4);

	VGfloat alpha = 1.0;				// start solid
	//VGfloat x_spacing = (width) / 10.0;	
	
	// VERTICAL
	Fill(255, 0, 125, alpha);
	Line(left, bottom+height, left, bottom);
}

void SliderControl::draw_y_label() 
{
	//VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	int yrange = (height);
	int tenpercent = (float)(width)*margin_percent;
	int x = left-tenpercent;
	int y = bottom+yrange/2.0;
	float size = 14;  //yrange/strlen(yAxisLabel);

	Translate(x, y);
	Rotate(90);	
	Fill(0, 255, 125, alpha);
	TextMid  ( 0, 0, yAxisLabel, SerifTypeface, size );
	Rotate   ( -90    );	
	Translate( -x, -y );
}

void SliderControl::draw_x_label() 
{
	char label[10];

	//VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	//VGfloat alpha = 1.0;
	//int xrange = (width);
	//int yrange = (height);	
	int tenpercent = (float)(height)*margin_percent;
	sprintf(label, "%6.1f", Level);
	int size = 16.0 ; //xrange/strlen(label);	

	Fill(255, 255, 0, 1.0);
	Text( left+TIC_WIDTH, bottom-tenpercent,  label, SerifTypeface, size );

}

void SliderControl::draw_l_title() 
{
	//VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	float xrange  = (width);
	//int yrange    = (height);	
	int tenpercent= (float)(height)*margin_percent;	
	float length  = strlen(title);
	float size    = (xrange+6.0*xrange*margin_percent)/length;	
 	
	Fill( 255, 255, 255, alpha );
	// Fix!! should not go outside client Rect!!
	
	Text( left+TIC_WIDTH, bottom+height+tenpercent, text, SerifTypeface, size );				
}

float SliderControl::get_level_pixel()
{
	// Interpolate:
	float pixel_range = (height);
	float value_range = (yMax - yMin );
	float ypix = ((Level / value_range) * pixel_range) + bottom;
	return ypix;
}
void SliderControl::draw_marker(  )
{
	Stroke(255, 255, 0, 1.0);
	StrokeWidth(5);
	Fill(255, 0, 0, 1);
	
	// DRAW A CIRCLE:
	/*VGfloat x[3];
	VGfloat y[3];

	y[0] = get_level_pixel();
	y[1] = get_level_pixel() + MARKER_HEIGHT/2;
	y[2] = get_level_pixel() - MARKER_HEIGHT/2;

	x[0] = left + TIC_WIDTH;
	x[1] = left + TIC_WIDTH + MARKER_WIDTH;
	x[2] = left + TIC_WIDTH + MARKER_WIDTH;	
	Polygon(x, y, 3); */
	
}

/*float SliderControl::calc_scale(float* data, int n )
{
//	float max = get_max(data, n);
//	float min = get_min(data, n);

	float xpixels = width;
	float ypixels = height;
}*/

int SliderControl::draw() 
{	
	Stroke		(255, 128, 128, 0.5);
	StrokeWidth (2); 
 
	draw_l_title		(  );
	draw_horizontal_tics(  );
	draw_vertical_line  (  );
	draw_x_label	 	(  );
	draw_y_label	 	(  );

	draw_marker			(  );
	return 1;
}

