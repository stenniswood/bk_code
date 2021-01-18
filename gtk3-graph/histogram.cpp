#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>

#include "histogram.hpp"


Histogram::Histogram( const char* mTitle, const char* mxAxis, const char* myAxis )
:Graph( mTitle,  mxAxis,  myAxis)
{
	m_bins   = NULL;
	m_mean 	 = 0.0;
	m_mode   = 0.0;
	m_median = 0.0;
	m_stddev = 0.0;	
}

Histogram::~Histogram()
{

}

void Histogram::compute_average( int mSeriesIndex )
{
	size_t len = series_data[mSeriesIndex].data_length;
	struct stDataPoint* dp  = series_data[mSeriesIndex].data;
	double avg = 0;
	for (int i=0; i<len; i++)
	{
		avg += dp[i].y;
	}
	avg /= len;
	printf("Computed Average = %7.4f;  n=%ld\n", avg, len);
	m_mean = avg;
}

void Histogram::compute_stddev( int mSeriesIndex )
{
	size_t len = series_data[mSeriesIndex].data_length;
	struct stDataPoint* dp  = series_data[mSeriesIndex].data;
	double stddev = 0;
	for (int i=0; i<len; i++)
	{
		float dev = (dp[i].y - m_mean);
		stddev += (dev * dev);
	}	
	stddev /= len;
	stddev = sqrt( stddev );	
	m_stddev = stddev;
}

void Histogram::compute_stats( int mSeriesIndex )
{
	size_t len              = series_data[mSeriesIndex].data_length;
	struct stDataPoint* dp  = series_data[mSeriesIndex].data;
	// Mean:
	compute_average(mSeriesIndex);
	printf("Computed Average = %7.4f;  n=%ld\n", m_mean, len);

	// Median: (equal number of dps above and below)
	
	// Mode:
	
	// StdDev:	
	compute_stddev(mSeriesIndex);
	printf("Computed stddev = %7.4f;  n=%ld\n", m_stddev, len);	
}

/* we onlyy concern ourselves with the Y data. */
void Histogram::bin_data( int mSeriesIndex, int mNumBins )
{
	// Get Min & Max 
	size_t len = series_data[mSeriesIndex].data_length;
	gfloat 	dmin = get_min_y(series_data[mSeriesIndex].data, len );
	gfloat 	dmax = get_max_y(series_data[mSeriesIndex].data, len );

	float range = (dmax - dmin);
	float bin_width = (range / mNumBins);
	
	if (m_bins != NULL) delete m_bins;
	
	m_bins = new double[mNumBins];
	
	
	size_t dlen = series_data[mSeriesIndex].data_length;
	for (int i=0; i<dlen; i++)	
	{
		// Which bin #?
		float delta = (series_data[mSeriesIndex].data[i].y - dmin);
		int bin_num = round(delta / bin_width);
		if ((bin_num > 0) && (bin_num < mNumBins))
			m_bins[bin_num] += 1.0;							
	}	
}


void Histogram::draw_graph_data( cairo_t *cr, int mSeriesIndex )
{
	stColor c = series_data[mSeriesIndex].color;	
	cairo_set_source_rgb ( cr, c.red, c.green, c.blue );
	
	for (int b=0; b<m_num_bins; b++)
	{
		cairo_rectangle(cr, 0, 100-x_axis_margin, 100, x_axis_margin);
		cairo_fill (cr);	
		cairo_stroke(cr);
			
	}
}

void Histogram::draw_graph( cairo_t *cr )
{
    cairo_scale (cr, cr_x_scale, cr_y_scale);
    
    /* Determine the data points to calculate (ie. those in the clipping zone */
    gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
    gdouble dx = 5.0, dy = 5.0; /* Pixels between each point */
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, dx);    
    
    /* Draw on a black background */
    cairo_set_source_rgb (cr, theme->background.red, theme->background.green, theme->background.blue);
    cairo_paint (cr);

	draw_title		( cr );
	y_scale = compute_yscale_all_series(  );
	
	draw_axis_labels( cr );
	draw_all_series ( cr );
		
	//if (m_show_legend)		draw_legend	( cr );
	if (m_show_grid)		draw_grid   ( cr );	
}


