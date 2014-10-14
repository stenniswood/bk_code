// line graph OpenVG program
// Steve Tenniswood
#include <shapes.h>
#include <fontinfo.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "Graphbase.hpp"
#include "../core/can/bk_system_defs.h"

#define margin_percent 0.07

Graphbase::Graphbase(int Left, int Right, int Top, int Bottom )
{
	left 	= Left;
	right 	= Right;
	top 	= Top;
	bottom	= Bottom;
	NumberHorizontalLines=5;	
	
	xAxisLabel	= NULL;
	yAxisLabel  = NULL;
	title 		= NULL;
	
	title_size  = 16.0;	
	xlabel_size = 14.0;
	ylabel_size = 14.0;	
}

Graphbase::Graphbase()
{
	left 	= 0;
	right 	= 0;
	top 	= 0;
	bottom	= 0;			
	NumberHorizontalLines=5;
	
	xAxisLabel	= NULL;
	yAxisLabel  = NULL;
	title 		= NULL;

	title_size  = 16.0;	
	xlabel_size = 14.0;
	ylabel_size = 14.0;	

}

void Graphbase::set_text( char* Title, char* xLabel, char* yLabel )
{
	title 		= Title;
	xAxisLabel	= xLabel;
	yAxisLabel  = yLabel;
}

void Graphbase::set_title ( char* Title )
{	title = Title;			}

void Graphbase::set_xLabel( char* Label )
{	xAxisLabel	= Label;	}

void Graphbase::set_yLabel( char* Label )
{	yAxisLabel  = Label;	}

void Graphbase::set_position( int Left, int Right, int Top, int Bottom )
{
	left 	= Left;
	right 	= Right;
	top 	= Top;
	bottom	= Bottom;
}
void  Graphbase::move_to( int Left, int Top )
{
	int width  = right-left;	// Preserve width & height!
	int height = top-bottom;
	
	left 	= Left;
	top 	= Top;	
	right 	= left+width;
	bottom	= top-height;
}

void Graphbase::draw_horizontal_lines(  ) 
{
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);

	//printf("NumberLines=%d\n", NumberHorizontalLines);
	float divs = NumberHorizontalLines;
	float alpha = 1.0;
	float y_spacing = ((float)(top - bottom)) / (divs);	
	// HORIZONTAL:
	Fill(255, 0, 125, alpha);
	for (VGfloat y=bottom; y <= top; y+=y_spacing) 
	{
		Line(left, y, right, y);
	}
}

void Graphbase::draw_vertical_lines( ) 
{
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);

	int i;
	VGfloat x;
	VGfloat alpha = 1.0;				// start solid
	VGfloat x_spacing = (right - left) / 10.0;	
	
	// VERTICAL
	Fill(255, 0, 125, alpha);
	for (int x = left; x <= right; x+=x_spacing) 
	{
		Line(x, top, x, bottom);
	}
}

int Graphbase::draw_y_axis_label() 
{
	if (yAxisLabel==NULL) return -1;
	
	VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	int yrange = (top-bottom);
	int tenpercent = (float)(right-left)*margin_percent;
	int x = left-tenpercent;
	int y = bottom+yrange/2.0;

// size :  yrange/strlen(yAxisLabel)
	Translate(x, y);
	Rotate(90);	
	Fill(0, 255, 125, alpha);
	TextMid  ( 0, 0, yAxisLabel, SerifTypeface, ylabel_size );
	Rotate   ( -90    );	
	Translate( -x, -y );
	return TRUE;
}

int Graphbase::draw_x_axis_label() 
{
	if (xAxisLabel==NULL) return -1;
	
	VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	int xrange = (right-left);
	int yrange = (top-bottom);	
	int tenpercent = (float)(top-bottom)*margin_percent;
	int size = xrange/strlen(xAxisLabel);
	
//	Translate(x, y);
	Fill(0, 255, 125, alpha);
	TextMid  ( left+xrange/2.0, bottom-tenpercent, xAxisLabel, 
				SerifTypeface, xlabel_size );
//	Translate( -x, -y );
	return TRUE;
}

int Graphbase::draw_title() 
{
	if (title==NULL) return -1;
	VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	int xrange = (right-left);
	int yrange = (top-bottom);	
	int tenpercent = (float)(top-bottom)*margin_percent;	

	Fill(0, 255, 125, alpha);
	// size : yrange/strlen(title)
	TextMid  ( left+xrange/2.0, top+tenpercent, title, SerifTypeface, 
				title_size );
	return TRUE;
}

void Graphbase::addDataSeries( DataSet* NewSeries )
{
	NewSeries->setPrev( DataTail );
	NewSeries->setNext( NULL );
	if (DataHead == NULL)
		DataHead = NewSeries;
	if (DataTail)
		DataTail->setNext( NewSeries );
	DataTail = NewSeries;
	/