// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "bk_system_defs.h"

#define margin_percent 0.07

Graphbase::Graphbase(int Left, int Right, int Top, int Bottom )
: Control( Left,Right,Top,Bottom )
{
}

Graphbase::Graphbase()
:Control()
{
}

void  Graphbase::Initialize	()
{
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
	Control::set_position(Left,Right, Top,Bottom);
	
}
void  Graphbase::move_to( int mLeft, int mBottom )
{		
	Control::move_to( mLeft, mBottom );
}

void Graphbase::draw_horizontal_lines(  ) 
{
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);

	//printf("NumberLines=%d\n", NumberHorizontalLines);
	float divs 		= NumberHorizontalLines;
	float alpha 	= 1.0;
	float y_spacing = ((float)(height)) / (divs);	

	// HORIZONTAL:
	Fill(255, 0, 125, alpha);
	for (VGfloat y=bottom; y <= bottom+height; y+=y_spacing) 
	{
		Line(left, y, left+width, y);
	}
}

void Graphbase::draw_vertical_lines( ) 
{
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);

	int i;
	VGfloat x;
	VGfloat alpha = 1.0;				// start solid
	VGfloat x_spacing = (width / 10.0);

	// VERTICAL
	Fill(255, 0, 125, alpha);
	for (int x = left; x <= left+width; x += x_spacing) 
	{
		Line(x, bottom+height, x, bottom);
	}
}

int Graphbase::draw_y_axis_label() 
{
	if (yAxisLabel==NULL) return -1;
	
	VGfloat fade  	= (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha 	= 1.0;
	int yrange 		= (height);
	int tenpercent 	= (float)(width)*margin_percent;
	int x 	= left-tenpercent;
	int y 	= bottom+yrange/2.0;

// size :  yrange/strlen(yAxisLabel)
	Translate	( x, y );
	Rotate		( 90   );
	Fill		( 0, 255, 125, alpha);
	TextMid  	( 0, 0, yAxisLabel, SerifTypeface, ylabel_size );
	Rotate   	( -90    );	
	Translate	( -x, -y );
	return TRUE;
}

int Graphbase::draw_x_axis_label() 
{
	if (xAxisLabel==NULL) return -1;
	
	VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat alpha = 1.0;
	int xrange = (width);
	int yrange = (height);	
	int tenpercent = (float)(height)*margin_percent;
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
	int xrange = (width);
	int yrange = (height);	
	int tenpercent = (float)(height)*margin_percent;	

	Fill(0, 255, 125, alpha);
	// size : yrange/strlen(title)
	TextMid( left+xrange/2.0, bottom+height+tenpercent, title, SerifTypeface, title_size );
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
	//printf("New Series!\n");
}

void Graphbase::removeDataSeries( DataSet* OldSeries )
{
	DataSet* Pptr = OldSeries->getPrev( );
	DataSet* Nptr = OldSeries->getNext( );

	Nptr->setPrev( Pptr );
	Pptr->setNext( Nptr );
	
	OldSeries->setNext( NULL );
	OldSeries->setPrev( NULL );	
	
	if (DataHead == OldSeries)
		DataHead = Nptr;
	if (DataTail == OldSeries)
		DataTail = NULL;
	//printf("Old Series!\n");		
}


int Graphbase::draw() 
{
	Control::draw();
	
	Fill(44, 77, 232, 1);				   // Big blue marble
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);

	draw_title			 (  );
	draw_x_axis_label	 (  );
	draw_y_axis_label	 (  );

	draw_horizontal_lines(  );
	//printf("NumberLines=%d\n", NumberHorizontalLines);	  
	if (ShowVerticalLines)	draw_vertical_lines();

	draw_body			 (  );
	return TRUE;
}



// grid draws a grid
/*void Graphbase::grid(VGfloat x, VGfloat y, int n, int w, int h) {
	VGfloat ix, iy;
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);
	for (ix = x; ix <= x + w; ix += n) {
		Line(ix, y, ix, y + h);
	}

	for (iy = y; iy <= y + h; iy += n) {
		Line(x, iy, x + w, iy);
	}
}*/
