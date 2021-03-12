

#include "wavegraph.hpp"




WaveGraph::WaveGraph( const char* mTitle, const char* mxAxis, const char* myAxis )
:Graph(mTitle, mxAxis, myAxis )
{
	m_wave  = NULL;	
	m_color = ORANGE;
	m_zoom  = 5.0;
	
}

WaveGraph::~WaveGraph()
{

}

void 	WaveGraph::set_wave( Wave* mWave )
{
	m_wave = mWave;
}

void 	WaveGraph::set_data  ( short* mData, size_t mLength )
{

}
void 	WaveGraph::compute_bins( int mBinSizeSamples )
{
	m_lows.clear();
	m_highs.clear();
	size_t len = m_wave->get_samples_recorded();
	short max_value = 0;
	short min_value = 0;
	for (int i=0; i<len; i+=mBinSizeSamples)
	{
		for (int j=0; j<mBinSizeSamples; j++) {
			if ((*m_wave)[i+j] > max_value)  max_value = (*m_wave)[i+j];
			if ((*m_wave)[i+j] < min_value)  min_value = (*m_wave)[i+j];
		}
		m_lows.push_back ( min_value );
		m_highs.push_back( max_value );
	}
}

void	WaveGraph::draw_graph  ( cairo_t *cr )
{
	Graph::draw_graph( cr );
		
	size_t samps = m_wave->get_samples_recorded();
	m_zoom = samps / screen.width;

	if (m_zoom<1)
		draw_waveform_zoomed_in ( cr );	
	else 
		draw_waveform_zoomed_out( cr );
}

/* zoom of 1 pixel per sample or less.  ie 1 sample per 5 pixels */
void 	WaveGraph::draw_waveform_zoomed_in  ( cairo_t *cr )
{

}

/* zoom of 1 pixel per sample or less.  ie 1 sample per 5 pixels */
void 	WaveGraph::draw_waveform_zoomed_out  ( cairo_t *cr )
{
	size_t len = m_lows.size();
	cairo_set_source_rgba (cr, m_color.red, m_color.green, m_color.blue, m_color.alpha);
	double y1,y2;
	double x=0;
	
	for (int b=0; b<len; b++)
	{		
		x = b;
		y1 = m_lows[b];
		y2 = m_highs[b];
		
		double y1pix = dataPoint_y_to_Pixel( y1 );
		double y2pix = dataPoint_y_to_Pixel( y2 );		
		double xpix = dataPoint_x_to_Pixel( x ); 				
		/* Draw the curve */
		cairo_move_to (cr, xpix,  y1pix );
		cairo_line_to (cr, xpix,  y2pix );	
		cairo_stroke( cr );		
	}
	
}
