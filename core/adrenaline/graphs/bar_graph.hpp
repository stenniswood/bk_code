#ifndef _BARGRAPH_
#define _BARGRAPH_

#include "Graphbase.hpp"
#include "dataset.hpp"


class BarGraph : public Graphbase
{
public:
	BarGraph();
	BarGraph( int Left, int Right, int Top, int Bottom );

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
	float 	group_space;	// x pixel space between each bar group (multi series)

	long int bar_color;		// 
		
	//float 	max,min;		// for vertical scale
	bool	auto_scale;
	
	float 	yscale, yoffset;

	float	stroke_width;
};
 
 
#endif
