#ifndef _HISTOGRAM_ 
#define _HISTOGRAM_

#include "Graphbase.hpp"
#include "dataset.hpp"


class Histogram : public Graphbase
{
public:
	Histogram();
	Histogram( int Left, int Right, int Top, int Bottom );
	
	float 	set_bins		( int NumberOfBins ) { bins = NumberOfBins; };
	int   	get_bin_count ( int binNumber );

	float 	get_highest_sample_count	();
	float 	evaluate_gaussian			(float avg, float stddev, float x);
	int   	draw_bell_curve			();

	float 	calc_scale	(  );
	void  	histo_plot	(  );
	int   	draw_body		(  );
	void  	draw_stats	(  );
	
private:
	float 	xscale, xoffset;
	float 	yscale, yoffset;
	float	bins;

	// Valid only after calling calc_scale()
	float   highest_bin_count;
	// hold these between drawings for realtime updates.
	float	bin_xpixel_spacing;
	float	bin_value_spacing;
	float	three_sigma;		// 
	float	center_value;		// aka average.
	float	center_xpix;
};

 
#endif
