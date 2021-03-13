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
	float 	get_zoom()	{ return m_zoom; };

	virtual void 	draw_graph  ( cairo_t *cr );
	void 			draw_waveform_zoomed_in  ( cairo_t *cr );
	void 			draw_waveform_zoomed_out ( cairo_t *cr );
	void 			draw_cursors ( cairo_t *cr );	

	virtual float	dataPoint_x_to_Pixel( double mX );
	virtual float	dataPoint_y_to_Pixel( double mY );
	
private:
	std::vector<int>	m_lows;
	std::vector<int>	m_highs;
	stColor 			m_color;
	float				m_zoom;
	
	int					m_cursor1;
	int					m_cursor2;

	Wave*				m_wave;
};

#endif
