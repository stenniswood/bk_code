#ifndef _LINEGRAPH_
#define _LINEGRAPH_

#include "Graphbase.hpp"
#include "dataset.hpp"


class LineGraph : public Graphbase
{
public:
	LineGraph();
	LineGraph( int Left, int Right, int Top, int Bottom );
	
	void  set_stroke_width( float new_width	) 	{ stroke_width = new_width; };	
	int   draw_body		  (  				);
	void  draw_data_series(  );
	float calc_scale	( float max, float min );
	
protected:
	void  line_plot		( DataSet* data, long int Color );

private:
	DataSet* data;
	float 	xscale, xoffset;
	float 	yscale, yoffset;

	float	stroke_width;
};
 
 
#endif
