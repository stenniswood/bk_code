#ifndef _STACKEDBARGRAPH_
#define _STACKEDBARGRAPH_

#include "Graphbase.hpp"
#include "dataset.hpp"


class StackedBarGraph : public Graphbase
{
public:
	StackedBarGraph();
	StackedBarGraph( int Left, int Right, int Top, int Bottom );

	void  	set_stroke_width( float new_width	) 	{ stroke_width = new_width; };	
	int   	draw_body		(  				);
	void  	draw_data_series(  				);

	void	set_max			( float mMax	)	{ max = mMax; 		};
	void	set_min			( float mMin	)	{ min = mMin;		};
	void	set_auto_scale	( bool  mOn		)	{ auto_scale = mOn; };
	void 	calc_scale		(  );
	float 	calc_auto_scale ( );

protected:
	void  line_plot		  ( DataSet* data, long int Color );

private:
	float 	bar_width;		// widht of bar
	float 	bar_space;		// x pixel space between bars.

	float 	max,min;
	bool	auto_scale;
	
	float 	xscale, xoffset;
	float 	yscale, yoffset;

	float	stroke_width;
};
 
 
#endif
