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
#include "scatter_plot.hpp"

static long int ColorSequence[10] = { 
0x00FF0000, 0x0000FF00, 0x000000FF, 0x007F7F7F,
0x007F3F00, 0x00007F3F, 0x003F007F, 
0x007F7F00, 0x00007F7F, 0x00FFFFFF };

ScatterGraph::ScatterGraph()
:Graphbase()
{	
	stroke_width = 5.0;
	//ShowVerticalLines=false;
	auto_scale = false;
	min =0;
	max =0;
	Initialize();
}

ScatterGraph::ScatterGraph( int Left, int Right, int Top, int Bottom )
:Graphbase(Left,Right,Top,Bottom)
{
	stroke_width = 5.0;
	//ShowVerticalLines=false;
	auto_scale = false;
	min =0;
	max =0;
	Initialize();
}

void ScatterGraph::line_plot( DataSet* data, long int Color )
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

void ScatterGraph::draw_data_series( )
{
	int i=0;
	for (int s=0; s<data_series.size(); s++)
	{
		line_plot( &data_series[s], ColorSequence[i] );
		i++;
	}	
}

float ScatterGraph::calc_auto_scale( )
{
	// get max of all data series:
	float tmp_max,tmp_min;
	for (int s=0; s<data_series.size(); s++)
	{
		tmp_max = data_series[s].get_max();
		tmp_min = data_series[s].get_min();
		if (tmp_max > max)	max = tmp_max;
		if (tmp_min > min)	min = tmp_min;
	}	
	// round max and min 
	return 1.0;
}

void ScatterGraph::calc_scale( )
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

int ScatterGraph::draw_body() 
{
	Fill  ( 44,  77, 232, 1  );				   // Big blue marble
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);
	printf("ScatterGraph::draw_body()\n");	  
	
	//calc_scale( );
	draw_data_series( );
}

