#ifndef _HISTOGRAM_HPP_
#define _HISTOGRAM_HPP_

#include "graph.hpp"


class Histogram : public Graph
{
public:
	Histogram( const char* mTitle, const char* mxAxis, const char* myAxis );
	~Histogram();

	void 		 draw_graph_data( cairo_t *cr, int mSeriesIndex );
	virtual void draw_graph		( cairo_t *cr );

	void 	compute_average		( int mSeriesIndex);
	void 	compute_stddev 		( int mSeriesIndex);

	void 	compute_stats		( int mSeriesIndex );
	void	bin_data			( int mSeriesIndex, int mNumBins = 10);

private:
	float 	m_mean;
	float	m_mode;
	float	m_median;
	float	m_stddev;
	
	double*	m_bins;
	int		m_num_bins;


};


#endif

