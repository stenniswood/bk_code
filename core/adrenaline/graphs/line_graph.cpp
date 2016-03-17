// line graph OpenVG program 
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "line_graph.hpp"
#include "display.h" 

 
#define Debug 1 


LineGraph::LineGraph()
:Graphbase()
{	
	stroke_width = 5.0;
	//ShowVerticalLines=false;
	auto_scale = false;
	Initialize();
}

LineGraph::LineGraph( int Left, int Right, int Top, int Bottom )
:Graphbase(Left,Right,Top,Bottom)
{
	stroke_width = 5.0;
	//ShowVerticalLines=false;
	auto_scale = false;
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

	int i=0;
	for (int s=0; s<data_series.size(); s++)
	{
		long int color = data_series[s]->color;	
		line_plot( data_series[s], color );
		i++;
	}	
}

int LineGraph::calc_auto_scale( )
{
	// get max of all data series:
	float tmp_max,tmp_min;
	if (data_series.size()==0)
		return -1;
	//assert( data_series.size()>0 );
	m_max = data_series[0]->get_max();		// initialize with something valid!
	m_min = data_series[0]->get_min();
	dprintf("m_max=%6.1f; m_min=%6.1f\n", m_max, m_min);
	
	// SCAN all data series:
	for (int s=0; s<data_series.size(); s++)
	{
		tmp_max = data_series[s]->get_max();
		tmp_min = data_series[s]->get_min();
		if (tmp_max > m_max)	m_max = tmp_max;
		if (tmp_min < m_min)	m_min = tmp_min;
	}	
	dprintf("m_max=%6.1f; m_min=%6.1f\n", m_max, m_min);	
	// round m_max and m_min 
	return 1;
}

void LineGraph::calc_scale( )
{
	float xpixels = width;
	float ypixels = height; 

	if (auto_scale)
	{
		calc_auto_scale();
	}

	char tmp[80];
	long int samples = data_series[0]->get_size();
	sprintf(tmp, "Max=%6.1f; Min=%6.1f;  Samples=%d\n", m_max, m_min, samples );
	m_stats = tmp;
	
	float div = (m_max-m_min);
	if (div==0) div=1;
	xscale  = 1.0; 	
	yscale  = ypixels/(m_max-m_min);
	xoffset = 0;
	yoffset = m_min;	
}

int LineGraph::draw_body() 
{
	printf("LineGraph::draw_body()\n");	  
	Fill_l(0xFFFFFFFF);				   // Big blue marble
	Text(left+10,bottom+20, m_stats.c_str(),  SerifTypeface, 12 );
	
	Fill(44, 77, 232, 1);				   // Big blue marble
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(2);
	
	//calc_scale( );
	draw_data_series( );
	return 1;
}

