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
#include "line_graph.h"
#include "leveler.hpp"

#define MARKER_HEIGHT 20
#define MARKER_WIDTH  20

//#define DefaultPadding 10
#define TIC_WIDTH 10
#define margin_percent 0.07

Leveler::Leveler(float Max, float Min)
{
	xAxisLabel	= (char*)"Potentiometer 1";
	yAxisLabel  = (char*)"Percentage";
	title 		= (char*)"Knee Joint";	
	yMax 	= Max;
	yMin 	= Min;
}

void Leveler::setPosition(int Left, int Right, int Top, int Bottom)
{
	left 	= Left;
	right 	= Right;
	top 	= Top;
	bottom	= Bottom;
}

void Leveler::setPosition_left_of(Leveler& Sibling, int width, int Top, int Bottom)
{
	right 	= Sibling.left - DefaultPadding;
	left 	= right - width;

	if (Top==-1)	top 	= Top;
	if (Bottom==-1)	bottom	= Bottom;
}

void Leveler::setPosition_right_of(Leveler& Sibling, int width, int Top, int Bottom)
{
	left 	= Sibling.right + DefaultPadding;
	right 	= left+width;
	if (Top==-1)	top 	= Sibling.top;
	if (Bottom==-1)	bottom	= Sibling.bottom;
}

int Leveler::set_level(float level)
{
	if (level > yMax)	return -1;
	if (level < yMin)	return -1;
	
	Level = level;
	return 1;
}

void Leveler::set_level_cap(float level)
{
	if (level > yMax)	level = yMax;
	if (level < yMin)	level = yMin;
	
	Level = level;
}

void Leveler::set_level_percent( float percent )
{
	if (percent > 100.0)	percent = 100.0;
	if (percent <   0.0)	percent = 0.0;

	float range = (yMax - yMin);
	Level  = range*(percent/100.0);
}

void Leveler::draw_horizontal_tics(  ) 
{
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);

	float alpha = 1.0;
	float y_spacing = ((float)(top - bottom)) / 10.0;	
	// HORIZONTAL
	Fill(255, 0, 125, alpha);
	for (VGfloat y = bottom; y <= top; y+=y_spacing) 
	{
		Line(left, y, left+TIC_WIDTH, y);
	}
}

void Leveler::draw_vertical_line( )
{
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(4);

	int i;
	VGfloat x;
	VGfloat alpha = 1.0;				// start solid
	VGfloat x_spacing = (right-left) / 10.0;	
	
	// VERTICAL
	Fill(255, 0, 125, alpha);
	Line(left, top, left, bottom);
}

int Leveler::draw_y_label() 
{
	VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	int yrange = (top-bottom);
	int tenpercent = (float)(right-left)*margin_percent;
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

int Leveler::draw_x_label() 
{
	char label[10];

	VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	int xrange = (right-left);
	int yrange = (top-bottom);	
	int tenpercent = (float)(top-bottom)*margin_percent;
	sprintf(label, "%6.1f", Level);
	int size = 16.0 ; //xrange/strlen(label);	

	Fill(255, 255, 0, 1.0);
	Text( left+TIC_WIDTH, bottom-tenpercent,  label, SerifTypeface, size );
}

int Leveler::draw_l_title() 
{
	VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	float xrange  = (right-left);
	int yrange    = (top-bottom);	
	int tenpercent= (float)(top-bottom)*margin_percent;	
	float length  = strlen(title);
	float size    = (xrange+6.0*xrange*margin_percent)/length;	
 	
	Fill( 255, 255, 255, alpha );
	Text( left+TIC_WIDTH, top+tenpercent, title, SerifTypeface, size );				
}

float Leveler::get_level_pixel()
{
	// Interpolate:
	float pixel_range = (top - bottom);	
	float value_range = (yMax - yMin );
	float ypix = ((Level / value_range) * pixel_range) + bottom;
	return ypix;
}
void Leveler::draw_marker(  )
{
	Stroke(255, 255, 0, 1.0);
	StrokeWidth(5);

	Fill(255, 0, 0, 1);
	VGfloat x[3];
	VGfloat y[3];

	y[0] = get_level_pixel();
	y[1] = get_level_pixel() + MARKER_HEIGHT/2;
	y[2] = get_level_pixel() - MARKER_HEIGHT/2;

	x[0] = left + TIC_WIDTH;
	x[1] = left + TIC_WIDTH + MARKER_WIDTH;
	x[2] = left + TIC_WIDTH + MARKER_WIDTH;	
	Polygon(x, y, 3);
}

/*float Leveler::calc_scale(float* data, int n )
{
//	float max = get_max(data, n);
//	float min = get_min(data, n);

	float xpixels = right - left;
	float ypixels = top - bottom;
}*/

int Leveler::draw() 
{	
	Stroke		(255, 128, 128, 0.5);
	StrokeWidth (2);

	draw_l_title		(  );
	draw_horizontal_tics(  );
	draw_vertical_line  (  );
	draw_x_label	 	(  );
	draw_y_label	 	(  );

	draw_marker(  );
}

