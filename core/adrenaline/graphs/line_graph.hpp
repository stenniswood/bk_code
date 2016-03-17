#ifndef _LINEGRAPH_
#define _LINEGRAPH_

#include <string>
#include "Graphbase.hpp"
#include "dataset.hpp"


class LineGraph : public Graphbase
{
public:
	LineGraph();
	LineGraph( int Left, int Right, int Top, int Bottom );

	void  	set_stroke_width( float new_width	) 	{ stroke_width = new_width; };	
	virtual int   	draw_body		(  				);
	void  	draw_data_series(  				);

	void	set_auto_scale	( bool  mOn		)	{ auto_scale = mOn; };
	void 	calc_scale		(  				);

	std::string	m_stats;
protected:
	void  	line_plot		( DataSet* data, long int Color );
	int 	calc_auto_scale ( 				);		// call calc_scale it will go here.
	
	
private:
	float 	max,min;
	bool	auto_scale;
	
	float 	xscale, xoffset;
	float 	yscale, yoffset;

	float	stroke_width;
};
 
 
#endif
