#ifndef _WAVEGRAPH_HPP_
#define _WAVEGRAPH_HPP_

#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <vector>
#include "dataseries.hpp"
#include "../wave.hpp"
#include "graph.hpp"


class WaveGraph : public Graph
{
public:
	WaveGraph( const char* mTitle, const char* mxAxis, const char* myAxis );
	~WaveGraph();

	void 	set_wave	( Wave* mWave );	
	void 	set_data	( short* mData, size_t mLength );
	void 	compute_bins( int mBinSizeSamples );

	virtual void 	draw_graph  ( cairo_t *cr );
	void 			draw_waveform_zoomed_in  ( cairo_t *cr );
	void 			draw_waveform_zoomed_out ( cairo_t *cr );

	
private:
	std::vector<int>	m_lows;
	std::vector<int>	m_highs;
	stColor 			m_color;
	float				m_zoom;

	Wave*				m_wave;
};

#endif
