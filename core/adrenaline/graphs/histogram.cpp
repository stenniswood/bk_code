/* Histogram - shows samples of a measurement (read many times)
 *				X Axis is the measurement going from lowest to highest.
 *				Y Axis shows the number of measurements which fall in that range.
 *				Assumes a normal distribution for the Bell curve which is fitted to the data.
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

Histogram::Histogram()
:Graphbase()
{	
	bins = 10.0;
}

Histogram::Histogram( int Left, int Right, int Top, int Bottom )
:Graphbase(Left,Right,Top,Bottom)
{	
	bins = 10.0;
}

void Histogram::set_bins( int NumberOfBins ) 
{
 bins = NumberOfBins; 
}

float Histogram::get_highest_sample_count( int mSeriesIndex )
{
	float highest= 0.0;
	float start_val  = (center_value - three_sigma);	// first bin on left
	float end_val    = start_val + bin_value_spacing;
	int   count;
		
	for (int i=0; i<bins; i++)
	{		
		count = data_series[mSeriesIndex].count_samples_between( start_val, end_val );
		if (count > highest)
			highest = count;
			
		// Bump to next bin:
		start_val = end_val;
		end_val   = start_val+bin_value_spacing;
	}
	return highest;
}

// Number of Bins must be set before call this!
float Histogram::calc_scale( )
{
	// COMPUTE STATS : 
	//for (int s=0; s<data_series.size(); s++)
		data_series[0].compute_stats();
	
	// WE'LL PLACE AVERAGE RIGHT IN THE CENTER:
	// and allow enough room for 6 sigma (standard formatting)
	center_value = data_series[0].get_average();

	// Use SixSigma instead on Max/Min!
	three_sigma       = 3.0 * data_series[0].get_stddev();
	float six_sigma   = 2.0 * three_sigma;
	bin_value_spacing = six_sigma / bins;

	float xpixels 	= (width);
	float ypixels 	= (height);
	bin_xpixel_spacing = (xpixels / bins);

	highest_bin_count = get_highest_sample_count();

	yscale   	= (ypixels / highest_bin_count);
	center_xpix = left + (xpixels/2.0);
	return yscale;
}

float sqrt_2pi = sqrt(2*M_PI);

float Histogram::evaluate_gaussian(float avg, float stddev, float x) 
{	
	float coefficient     = 1.0/highest_bin_count; //(stddev * sqrt_2pi);
	float exp_numerator   = -(x - avg)*(x - avg);
	float exp_denominator = 2.0*(stddev*stddev);

	float exponent = exp(exp_numerator / exp_denominator);
	return (exponent / coefficient);
}

int Histogram::draw_bell_curve() 
{
	Stroke(44, 77, 232, 1.0);
	StrokeWidth(2);
	
	float xscale = (2*three_sigma) / ((float)(width));	
	float x_value = center_value - three_sigma;
	float prev_y=0;
	float eval_y=0;

	for (int xpix=left; xpix<left+width; xpix++)		// for each pixel
	{
		eval_y = evaluate_gaussian(center_value, data_series[0].get_stddev(), x_value)*yscale;
		Line( xpix, bottom+prev_y,  xpix+1, bottom+eval_y);
		prev_y = eval_y;
		x_value += xscale;
	}
	return 1;	
}

void Histogram::draw_stats() 
{
	Fill  (255,  77, 22, 1  );	   		// Big blue marble
	Stroke(255, 128, 128, 0.75);
  
	char n_str[12];
	sprintf(n_str, "n=%d", data_series[0].get_size() );
	float x = center_xpix + ((center_xpix - left)/4.0);
	float y = bottom+height - ((float)(height)/4.0);
	Text( x,y, n_str, SerifTypeface, 12.0 );

	sprintf(n_str, "avg=%6.1f", data_series[0].get_average() );
	Text( x,y+20.0, n_str, SerifTypeface, 12.0 );	
}

//	
//	printf("============ Histogram: average=%6.2f +- %6.2f 3sigma=%6.2f ===========\n", 
//			data_series[0].get_average(), data_series[0].get_stddev(), three_sigma );
int Histogram::draw_body() 
{ 	
	calc_scale();

	Fill  ( 44,  77, 232, 1  );	   		// Big blue marble
	Stroke(255, 128, 128, 0.5);
	StrokeWidth(bin_xpixel_spacing/2.0);

	int x = left+(bin_xpixel_spacing/2.0);
	float NumberOfSamples = 0;
	
	float start_val = (center_value - three_sigma);	// first bin on left
	float end_val   = start_val + bin_value_spacing;
	for (int i=0; i<bins; i++, x+=bin_xpixel_spacing)
	{
		NumberOfSamples = data_series[0].count_samples_between( start_val, end_val );
		Line( x, bottom,  x, bottom+ (NumberOfSamples*yscale) );

		// Bump to next bin:
		start_val = end_val;
		end_val   = start_val+bin_value_spacing;	
	} 
	
	draw_stats();
	draw_bell_curve();
	return 1;
}

