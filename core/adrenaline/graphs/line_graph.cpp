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
#include "line_graph.hpp"
#include "display.h" 

long int ColorSequence[10] = { 
0x00FF0000, 0x0000FF00, 0x000000FF, 0x007F7F7F,
0x007F3F00, 0x00007F3F, 0x003F007F, 
0x007F7F00, 0x00007F7F, 0x00FFFFFF };

LineGraph::LineGraph()
:Graphbase()
{	
	stroke_width = 5.0;
	//ShowVerticalLines=false;
	DataHead=NULL;
	DataTail=NULL;
	auto_scale = false;
	min =0;
	max =0;
	Initialize();
}

LineGraph::LineGraph( int Left, int Right, int Top, int Bottom )
:Graphbase(Left,Right,Top,Bottom)
{
	stroke_width = 5.0;
	//ShowVerticalLines=false;
	DataHead=NULL;
	DataTail=NULL;
	auto_scale = false;
	min =0;
	max =0;
	Initialize();
}

void LineGraph::line_plot( DataSet* data, long int Color )
{
	Stroke((Color&0x00FF0000)>>16, (Color&0x0000FF00)>>8, (Color&0x000000FF), 1.0);
	StrokeWidth(stroke_width);

	Fill((Color&0x00FF0000)>>16, (Color&0x0000FF00)>>8, (Color&0x000000FF), 1);
	int n = data->get_size();
	float spacing = ((float)(width))/((float)data->get_size());
	int x = left;
	int top = bottom+height;
	float one,two; 
	
	for(int i=0; i<(n-1); i++)
	{
		one = bottom + (float)((*data)[i]   - yoffset)*yscale;
		two = bottom + (float)((*data)[i+1] - yoffset)*yscale;
		if (one > top) 		one = top;
		if (two > top) 		two = top;
		if (one < bottom) 	one = bottom;
		if (two < bottom) 	two = bottom;
		Line( x, 	one,  x+spacing, two );
		x += spacing;
	}		
}

void LineGraph::draw_data_series( )
{
	DataSet* ptr = DataHead;
	int i=0;
	while (ptr)
	{
		line_plot( ptr, ColorSequence[i] );
		ptr = ptr->getNext();
		i++;
	}	
}

float LineGraph::calc_auto_scale( )
{
	// get max of all data series:
	DataSet* ptr = DataHead;
	float tmp_max,tmp_min;
	
	while (ptr)
	{
		tmp_max = ptr->get_max();
		tmp_min = ptr->get_min();
		if (tmp_max > max)	max = tmp_max;
		if (tmp_min > min)	min = tmp_min;			
		ptr = ptr->getNext();
	}	
	// round max and min 
}

void LineGraph::calc_scale( )
{
	float xpixels = width;
	float ypixels = height; 

	if (auto_scale)
	{
		calc_auto_scale();
	}

	xscale  = 1.0; 	
	yscale  = ypixels/(max-min);
	xoffset = 0;
	yoffset = min;	
}

int LineGraph::draw_body() 
{
	Fill(44, 77, 232, 1);				   // Big blue marble
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);
	printf("LineGraph::draw_body()\n");	  
	
	//calc_scale( );
	draw_data_series( );
}

