/* BarGraph - shows values of a measurement 
 *			X Axis is the measurement index
 *			Y Axis shows the value
 *			Multiple data series will be added side by side. with different colors.
 *
 * OpenVG program
 * Author:		Steve Tenniswood
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "histogram.hpp"
#include "bar_graph.hpp"
#include "display.h"


#define DEFAULT_BAR_COLOR 0xDF4FAF20


BarGraph::BarGraph()
:Graphbase()
{	
	bar_color = DEFAULT_BAR_COLOR;
}

BarGraph::BarGraph( int Left, int Right, int Top, int Bottom )
:Graphbase(Left,Right,Top,Bottom)
{	
	bar_color = DEFAULT_BAR_COLOR;
}

// Number of Bins must be set before call this!
void BarGraph::calc_scale( )
{
	// COMPUTE STATS : 
	data_series[0]->compute_stats();
	
	find_max();
	find_min();

	// Use SixSigma instead on Max/Min!
	int number_bars 		= data_series[0]->get_size();
	int number_data_series 	= count_data_series();
	float group_xpixel_spacing= (width / number_bars);	
	float bar_width_spacing = (group_xpixel_spacing*0.9) / number_data_series;

	bar_width = bar_width_spacing * 0.9;
	bar_space = bar_width_spacing * 0.1;	
	group_space = group_xpixel_spacing * 0.1;	
	
	yscale   	= (height / m_max);
}

//extern float sqrt_2pi;


int BarGraph::draw_body() 
{ 	
	calc_scale();
	printf("m_max = %6.2f\n", m_max);
	
	Fill_l  ( bar_color );	   		// Big blue marble
	Stroke	( 255, 128, 128, 0.5);
	StrokeWidth(2.0);

	DataSet* ptr 	= NULL;	
	int   	 margin = 3;
	float 	 x 		= left+margin;
	int   	 bars 	= data_series[0]->get_size();	
	printf("BarGraph::draw_body() bars=%d\n", bars );
	
	for (int i=0; i<bars; i++ )
	{
		for (int s=0; s<data_series.size(); s++)
		{
			float value = data_series[s]->get_datum(i);
			Rect( x, bottom,  bar_width, (value*yscale) );
			//printf("BarGraph:: x=%6.1f,b=%6.1f; r=%6.1f t=%6.1f \n", x, bottom, x+bar_width, bottom+(value*yscale) );
			x  += (bar_width+bar_space);
			if (s<(data_series.size()-1))
				ptr = (data_series[s+1]);
		}
		x += group_space;
	}

}

