#include <assert.h>
#include "graph.hpp"
#include "annotated_graph.hpp"



AnnotatedGraph::AnnotatedGraph( const char* mTitle, const char* mxAxis, const char* myAxis )
:Graph( mTitle, mxAxis, myAxis )
{
}
AnnotatedGraph::~AnnotatedGraph()
{
}

void	AnnotatedGraph::mark_data( int mDataSeries, int mSeriesIndex, struct stColor mColor, long int dStyle )
{
	assert( (mDataSeries>=0 ) && ( mDataSeries<series_data.size() ));
	
	struct stGraphPoint gp;
	struct stDataPoint* dp = series_data[mDataSeries].get_data( mSeriesIndex );
	if (dp) {
		gp.x 		  = dp->x;
		gp.y 		  = dp->y;
	}
	return;
	
	gp.color      = mColor;
	gp.draw_style = dStyle;
		//printf("Adding Annotation : <%6.2f,%6.2f>; color=%x\n", gp.x, gp.y, gp.color );
		
	m_mark_points.push_back( gp );
}

void	AnnotatedGraph::mark_point( double mX, double mY, stColor mColor, long int dStyle )
{
	 struct stGraphPoint gp;
	 gp.x = mX;		gp.y = mY;
	 gp.color = mColor;
	 gp.draw_style = dStyle;
	m_mark_points.push_back( gp );
}

void	AnnotatedGraph::mark_line ( double mX, double mY, double mX2, double mY2, stColor mColor )	
//void	AnnotatedGraph::mark_line ( struct stLine&       mMarkLine  )
{
	struct stGraphPoint gp1;
	struct stGraphPoint gp2;
	gp1.x = mX;				gp1.y = mY;
	gp1.color = mColor;		gp1.draw_style = STYLE_UNDEFINED;
	gp2.x = mX2;			gp2.y = mY2;
	gp2.color = mColor;		gp2.draw_style = STYLE_UNDEFINED;
	
	struct stLine line;
	line.start = gp1;
	line.end   = gp2;
	line.color = mColor;	
	m_mark_lines.push_back( line );
}

double ymin=0.0;

void AnnotatedGraph::draw_graph( cairo_t *cr )
{
	// Base class:
	Graph::draw_graph( cr );
	
//	for (int series=0; series<series_data.size(); series++)
//		ymin = MIN( ymin, series_data[series].get_min_y() );
	
	draw_annotations( cr );
}	

void 	AnnotatedGraph::draw_point( cairo_t *cr, int mIndex )
{
	switch (m_mark_points[mIndex].draw_style)
	{
	case STYLE_UNDEFINED : break;
	case STYLE_X 		: draw_x	 ( cr, mIndex );  		break;
	case STYLE_CIRCLE 	: draw_circle( cr, mIndex );		break;
	case STYLE_SQUARE 	: draw_square( cr, mIndex );		break;
	default : break;
	}
}

#define X_SIZE 4
#define Y_SIZE 4

void 	AnnotatedGraph::draw_x	   ( cairo_t *cr, int mIndex )
{
	struct stColor c = m_mark_points[mIndex].color;
	cairo_set_source_rgba (cr, c.red, c.green, c.blue, c.alpha);

	double x = dataPoint_x_to_Pixel(m_mark_points[mIndex].x);
	double y = dataPoint_y_to_Pixel(m_mark_points[mIndex].y);

//	printf("drawing X : <%6.2f,%6.2f>", x, y );
		cairo_move_to (cr, x-X_SIZE, y-Y_SIZE );	// L TO R;  Top to Bottom
		cairo_line_to (cr, x+X_SIZE, y+Y_SIZE );		
		cairo_stroke  (cr);	

		cairo_move_to (cr, x-X_SIZE, y+Y_SIZE );	// L TO R;  B to TOP
		cairo_line_to (cr, x+X_SIZE, y-Y_SIZE );		
		cairo_stroke  (cr);	
}

void 	AnnotatedGraph::draw_circle( cairo_t *cr, int mIndex )
{

}

void 	AnnotatedGraph::draw_square( cairo_t *cr, int mIndex )
{
	double x = m_mark_points[mIndex].x;
	double y = m_mark_points[mIndex].y;

		cairo_move_to (cr, x-X_SIZE, y-Y_SIZE );
		cairo_line_to (cr, x-X_SIZE, y+Y_SIZE );	// Side Left		
		cairo_stroke  (cr);	
		cairo_move_to (cr, x+X_SIZE, y-Y_SIZE );
		cairo_line_to (cr, x+X_SIZE, y+Y_SIZE );	// Side Right
		cairo_stroke  (cr);	

		cairo_move_to (cr, x-X_SIZE, y-Y_SIZE );	// Top
		cairo_line_to (cr, x+X_SIZE, y-Y_SIZE );		
		cairo_stroke  (cr);	
		cairo_move_to (cr, x-X_SIZE, y+Y_SIZE );	// Bottom
		cairo_line_to (cr, x+X_SIZE, y+Y_SIZE );		
		cairo_stroke  (cr);	
}

void 	AnnotatedGraph::draw_line  ( cairo_t *cr, int mLIndex )
{
	double sx = m_mark_lines[mLIndex].start.x;
	double sy = m_mark_lines[mLIndex].start.y;
	double ex = m_mark_lines[mLIndex].end.x;
	double ey = m_mark_lines[mLIndex].end.y;

	cairo_move_to (cr, sx, sy );
	cairo_line_to (cr, ex, ey );	// Side Left		
	cairo_stroke  (cr);	
}

void	AnnotatedGraph::draw_annotations( cairo_t *cr )
{
	for (int p=0; p<m_mark_points.size(); p++)
	{
		draw_point( cr, p );
	}
	for (int l=0; l<m_mark_lines.size(); l++)
	{
		draw_line( cr, l );
	}
	
}


