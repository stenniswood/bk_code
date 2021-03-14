

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

	m_cursor1 = round( (double)mWave->get_samples_recorded() / 3.0);
	m_cursor2 = round( 2.0 * (double)mWave->get_samples_recorded() / 3.0);	
	printf("Cursor1,2= %d,%d\n", m_cursor1, m_cursor2 );
		
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
	for (int i=0; i<len-mBinSizeSamples-1; i+=mBinSizeSamples)
	{
		max_value = 0;
		min_value = 0;
		for (int j=0; j<mBinSizeSamples; j++) {
			if ((*m_wave)[i+j] > max_value)  max_value = (*m_wave)[i+j];
			if ((*m_wave)[i+j] < min_value)  min_value = (*m_wave)[i+j];
		}
		m_lows.push_back ( min_value );
		m_highs.push_back( max_value );
	}
	printf("Compute bins wavegraph %d.\n", m_lows.size() );
	
	// FOR YSCALE:
	double ymax = m_wave->get_max_sample();
	double ymin = m_wave->get_min_sample();
	printf("ymin=%6.2lf;  ymax=%6.2lf\n", ymin, ymax );

	if ((ymax-ymin)==0) 		y_scale=99999;
	y_scale = (double)DATA_AREA_Y_SIZE / (ymax-ymin);

	printf("Wave length = %6.2f\n", y_scale );
}

void	WaveGraph::draw_graph  ( cairo_t *cr )
{
	Graph::draw_graph( cr );
			
	if (m_highs.size()==0)
	{
		m_xmax_all   = screen.width;
		size_t samps = m_wave->get_samples_recorded();
		m_zoom       = samps / screen.width;
		printf("Drawing Wavegraph; screenwidth=%6.2lf;  z=%6.2f\n", m_xmax_all, m_zoom );
		compute_bins( m_zoom );
	}

	if (m_zoom<10)
		draw_waveform_zoomed_in ( cr );	
	else 
		draw_waveform_zoomed_out( cr );
		
	draw_cursors( cr );
}

/* zoom of 1 pixel per sample or less.  ie 1 sample per 5 pixels */
void 	WaveGraph::draw_waveform_zoomed_in  ( cairo_t *cr )
{

}

float	WaveGraph::dataPoint_x_to_Pixel( double mX )
{
	double x  =  mX;
	float final_x = x + y_axis_margin;	// scaled to display scale.	
	// result will go from [x_axis_margin..screen_width]
	return final_x;	
}

float	WaveGraph::dataPoint_y_to_Pixel( double mY )
{
	float y = ( mY * y_scale );
	float y_pix = (DATA_AREA_Y_SIZE/2.0 - y) + y_title_margin;
	return y_pix;	
}

/* zoom of 1 pixel per sample or less.  ie 1 sample per 5 pixels */
void 	WaveGraph::draw_waveform_zoomed_out  ( cairo_t *cr )
{
	size_t len = m_lows.size();
	cairo_set_source_rgba (cr, m_color.red, m_color.green, m_color.blue, m_color.alpha);
	double y1,y2;
	double x=0;
	
	//printf("draw_waveform_zoomed_out() l=%ld; z=%6.2f\n", len, m_zoom );
	for (int b=0; b<len; b++)
	{		
		x = b;
		y1 = m_lows[b];
		y2 = m_highs[b];
		
		double y1pix = dataPoint_y_to_Pixel( y1 );
		double y2pix = dataPoint_y_to_Pixel( y2 );		
		double xpix  = dataPoint_x_to_Pixel( x ); 
		//printf("b%d : x,y1,y2 = %6.2lf, %6.2lf, %62lf; scale=%6.2f\n", xpix, y1, y1pix, y_scale );
						
		/* Draw the curve */
		cairo_move_to (cr, xpix,  y1pix );
		cairo_line_to (cr, xpix,  y2pix );	
		cairo_stroke( cr );
	}	
}

void 	WaveGraph::draw_cursors ( cairo_t *cr )
{
	double y1pix = (y_title_margin);
	double y2pix = (DATA_AREA_Y_SIZE + y_title_margin);

	double x1pix  = dataPoint_x_to_Pixel( m_cursor1/m_zoom ); 
	double x2pix  = dataPoint_x_to_Pixel( m_cursor2/m_zoom ); 	

//printf("cursor1,2 = %d; %d\n", m_cursor1, m_cursor2 );

	cairo_set_source_rgba (cr, RED.red, RED.green, RED.blue, RED.alpha);
		
		/* Draw cursor 1 */
		cairo_move_to (cr, x1pix,  y1pix );
		cairo_line_to (cr, x1pix,  y2pix );	
		cairo_stroke( cr );

		/* Draw cursor 2 */
		cairo_move_to (cr, x2pix,  y1pix );
		cairo_line_to (cr, x2pix,  y2pix );	
		cairo_stroke( cr );
	
}

