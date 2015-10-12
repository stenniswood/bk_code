/* StackedBarGraph - shows values of a measurement 
 *			X Axis is the measurement index
 *			Y Axis shows the value
 *			Multiple data series will be added side by side. with different colors.

 *** Not implemented yet.  Code is a normal bar_graph!	***

 *
 * OpenVG program
 * Author:		Steve Tenniswood
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "histogram.hpp"
#include "bar_graph_stacked.hpp"



StackedBarGraph::StackedBarGraph()
:Graphbase()
{	
	
}

StackedBarGraph::StackedBarGraph( int Left, int Right, int Top, int Bottom )
:Graphbase(Left,Right,Top,Bottom)
{	
	
}

// Number of Bins must be set before call this!
void StackedBarGraph::calc_scale( )
{
	// COMPUTE STATS : 
	data_series[0].compute_stats();
	
	find_max();
	find_min();

	// Use SixSigma instead on Max/Min!
	int number_bars 		= data_series[0].get_size();
	int number_data_series 	= count_data_series();
	float group_xpixel_spacing= (width / number_bars);	
	float bar_width_spacing = (group_xpixel_spacing*0.9) / number_data_series;

	bar_width = bar_width_spacing * 0.9;
	bar_space = bar_width_spacing * 0.1;		
	
	yscale   	= (height / max);
}

//float sqrt_2pi = sqrt(2*M_PI);


int StackedBarGraph::draw_body() 
{ 	
	calc_scale();

	Fill  	(  44,  77, 232, 1  );	   		// Big blue marble
	Stroke	( 255, 128, 128, 0.5);
	StrokeWidth(2.0);

	int margin = 3;
	float x = left+margin;
	int bars = data_series[0].get_size();	
	DataSet* ptr = NULL;
	for (int i=0; i<bars; i++ )
	{

		for (int s=0; s<data_series.size(); s++)
		{
			float value = data_series[0].get_datum(i);
			Line( x, bottom,  x+bar_width, bottom+(value*yscale) );
			x  += (bar_width+bar_space);
		}		
	}
}

