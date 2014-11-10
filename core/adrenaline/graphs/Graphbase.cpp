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
#include "display.h"

#define margin_percent 0.07

Graphbase::Graphbase(int Left, int Right, int Top, int Bottom )
: Control( Left,Right,Top,Bottom )
{
	Initialize();	
	//calc_body_coords(); 
}

Graphbase::Graphbase()
:Control()
{
	Initialize();
}

void  Graphbase::Initialize	()
{
	NumberVerticalLines  =5;	
	NumberHorizontalLines=5;	
	xAxisLabel	= NULL;
	yAxisLabel  = NULL;
	title 		= NULL;

	draw_options= 0x0F;			// draw by default (labels must be set however)
	title_color = 0xFF00FF7F;	
	xlabel_color= 0xFF00FF7F;
	ylabel_color= 0xFF00FF7F;

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

void Graphbase::calc_body_coords( )
{
	body_left   = left   + ylabel_size*1.5;
	body_bottom = bottom + xlabel_size*1.5;

	body_width  = width - body_left;
	body_height = height - body_bottom - title_size*1.5;	
}

void Graphbase::set_position( int Left, int Right, int Top, int Bottom )
{
	Control::set_position(Left,Right, Top,Bottom);
	calc_body_coords();
}
void  Graphbase::move_to( int mLeft, int mBottom )
{		
	Control::move_to( mLeft, mBottom );
}

void Graphbase::draw_horizontal_lines(  ) 
{
	//Stroke_l( grid_color );		
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);

	float divs 		= NumberHorizontalLines;
	float alpha 	= 1.0;
	float y_spacing = ((float)(height)) / (divs);	

	// HORIZONTAL:
	Fill_l( grid_color );	//  255, 0, 125, alpha);
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
	VGfloat x_spacing = (width / NumberVerticalLines);

	// VERTICAL:
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

	Translate	( x, y );
	Rotate		( 90   );
	Fill_l(xlabel_color);
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
	
	Fill_l(xlabel_color);
	TextMid  ( left+xrange/2.0, bottom-tenpercent, xAxisLabel, 
				SerifTypeface, xlabel_size );
	return TRUE;
}

int Graphbase::draw_title() 
{
	if (title==NULL) return -1;

	VGfloat alpha = 1.0;
	float tenpercent = (float)(height*margin_percent);

	Fill_l(title_color);
	float l = left+width/2.0;
	float b = bottom+height-tenpercent;
	TextMid( l, b, title, SerifTypeface, title_size );
	return TRUE;
}

void Graphbase::add_data_series( DataSet* NewSeries )
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

void Graphbase::remove_data_series( DataSet* OldSeries )
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

	Fill  (44, 77, 232, 1.0);				   // Big blue marble
	Stroke(255, 128, 128, 1.0);
	StrokeWidth(2);

	if (draw_options & SHOW_TITLE)
		draw_title			 (  );
	if (draw_options & SHOW_X_LABEL)
		draw_x_axis_label	 (  );
	if (draw_options & SHOW_Y_LABEL)
		draw_y_axis_label	 (  );

	//printf("yaxis=%s\n", yAxisLabel);
	//printf("NumberLines=%d\n", NumberHorizontalLines);	  

	if (draw_options & SHOW_X_SCALE)
		draw_x_scale (  );
	if (draw_options & SHOW_Y_SCALE)
		draw_y_scale (  );
	
	if (NumberHorizontalLines)	
		draw_horizontal_lines( );
	if (NumberVerticalLines)	
		draw_vertical_lines();

	draw_body			 (  );
	return TRUE;
}

/* Calculates max for all data series */
void Graphbase::find_max()
{
	DataSet* ptr  = DataHead;
	float tmp_max = 0.;
	while (ptr)
	{
		// 
		tmp_max = ptr->get_max();
		if (tmp_max > max)
			max = tmp_max;			
		ptr = ptr->getNext();
	}
}

/* Calculates min for all data series */
void Graphbase::find_min()
{
	DataSet* ptr = DataHead;
	float tmp_min = 0.;	
	while (ptr)
	{
		// 
		tmp_min = ptr->get_min();
		if (tmp_min > min)
			min = tmp_min;			
		ptr = ptr->getNext();
	}
}

// how many data series are there?
int Graphbase::count_data_series()
{
	DataSet* ptr = DataHead;
	int count = 0;
	
	while (ptr)
	{
		count++;
		ptr = ptr->getNext();
	}	
	return count;
}

int Graphbase::draw_x_scale() 
{
	Fill_l(xlabel_color);
	float divs 		= NumberHorizontalLines;
	float alpha 	= 1.0;
	VGfloat x_spacing = (width / NumberVerticalLines);

	for (int x=left; x <=left+width; x+=x_spacing) 
	{
		TextEnd( 0, 0, yAxisLabel, SerifTypeface, ylabel_size );
	}
	return TRUE;
}

int Graphbase::draw_y_scale() 
{
	Fill_l(xlabel_color);
	float divs 		= NumberHorizontalLines;
	float alpha 	= 1.0;
	float y_spacing = ((float)(height)) / (divs);	

	// HORIZONTAL:
	Fill_l( grid_color );	//  255, 0, 125, alpha);
	for (VGfloat y=bottom; y <= bottom+height; y+=y_spacing)
	{
		TextEnd( 0, 0, yAxisLabel, SerifTypeface, ylabel_size );
	}
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
