#ifndef _ANNOTATED_GRAPH_HPP_
#define _ANNOTATED_GRAPH_HPP_

#include <vector>
#include "graph.hpp"
#include "dataseries.hpp"

#define STYLE_UNDEFINED 0
#define STYLE_X 	 1
#define STYLE_CIRCLE 2
#define STYLE_SQUARE 3

struct stGraphPoint {
	double x;
	double y;
	struct stColor color;
	int	draw_style;
};

struct stLine 
{
	struct stGraphPoint	start;
	struct stGraphPoint	end;
	stColor color;
};


class AnnotatedGraph : public Graph 
{
public:
	AnnotatedGraph( const char* mTitle, const char* mxAxis, const char* myAxis );
	~AnnotatedGraph();

	void	mark_data ( int mDataSeries, int mSeriesIndex, struct stColor mColor, long int dStyle );
	void	mark_point( double mX, double mY, stColor mColor, long int dStyle );
	void	mark_line ( double mX, double mY, double mX2, double mY2, stColor mColor );

	virtual void 	draw_graph      ( cairo_t *cr );
	void 	draw_point ( cairo_t *cr, int mIndex );
	void 	draw_x	   ( cairo_t *cr, int mIndex );
	void 	draw_circle( cairo_t *cr, int mIndex );
	void 	draw_square( cairo_t *cr, int mIndex );
	void 	draw_line  ( cairo_t *cr, int mLIndex );

	void	draw_annotations( cairo_t *cr );
		
private:
	std::vector<struct stGraphPoint> 	m_mark_points;
	std::vector<struct stLine> 			m_mark_lines;	

};



#endif
