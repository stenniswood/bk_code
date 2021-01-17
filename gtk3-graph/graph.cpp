#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>

#include "graph.hpp"


struct stDataPoint data[NUM_DATA_POINTS];
std::vector<struct stSeriesInfo> series_data;
struct stGraphInfo graph_info;


const struct stColor DARK_GREY    = { 0.25, 0.25, 0.25, 1.0 };	// 0x80 B9 e4
const struct stColor LIGHT_GREY   = { 0.35, 0.35, 0.35, 1.0 };	// 0x80 B9 e4

const struct stColor WHITE = { 1.0, 1.0, 1.0, 1.0 };
const struct stColor LIGHT_BLUE   = { 0.5, 0.725, 0.894, 1.0 };	// 0x80 B9 e4
const struct stColor LIGHT_GREEN  = { 0.435, 0.666, 0.435, 1.0 }; // 6f aa 6f
const struct stColor YELLOW 	  = { 0.925, 0.905, 0.212, 1.0 }; // ec e7 36

const struct stColor ORANGE 	  = { 0.847, 0.603, 0.145, 1.0 };  // d8 9a 25
const struct stColor RED 		  = { 1.0, 1.0, 1.0, 1.0 };  // c6 1c 1e
const struct stColor RED_PURPLE   = { 1.0, 1.0, 1.0, 1.0 };  // 78 2d 85
const struct stColor BLUE_PURPLE  = { 1.0, 1.0, 1.0, 1.0 };  // 35 31 8a  
const struct stColor BLUE 		  = { 1.0, 1.0, 1.0, 1.0 };  // 29 49 9b
const struct stColor BLUE_GREEN   = { 1.0, 1.0, 1.0, 1.0 };  // 3a 75 76
const struct stColor GREEN 		  = { 1.0, 1.0, 1.0, 1.0 };  // 3a 72 2e
const struct stColor YELLOW_GREEN = { 1.0, 1.0, 1.0, 1.0 };  // 91 b2 2c 
const struct stColor YELLOW_BROWN = { 1.0, 1.0, 1.0, 1.0 };  // 53 46 1a
const struct stColor RED_BROWN    = { 1.0, 1.0, 1.0, 1.0 };  // 56 23 22

struct stColor pallette[14] = {
	WHITE, LIGHT_BLUE, LIGHT_GREEN, YELLOW,
	ORANGE, RED, RED_PURPLE, BLUE_PURPLE, 
	BLUE, BLUE_GREEN, GREEN, YELLOW_GREEN,
	YELLOW_BROWN, RED_BROWN
};

void create_graph( const char* mTitle, const char* mxAxis, const char* myAxis, float mWindowWidth, float mWindowHeight )
{
	strcpy(graph_info.title, mTitle);
	strcpy(graph_info.xaxis_label, mxAxis );
	strcpy(graph_info.yaxis_label, myAxis );
	
	graph_info.backgroundColor;
	graph_info.show_grid   = true;
	graph_info.show_legend = true;
	
	graph_info.x_axis_margin = 0.1 * mWindowHeight - mWindowHeight/2;
	graph_info.y_axis_margin = 0.1 * mWindowWidth - mWindowWidth/2;	// is in x units
}


gfloat get_min_x(struct stDataPoint* mDP, int mLength )
{
	gfloat min_dp = 999999;	
	for (int i=0; i<mLength; i++)
	{
		if (mDP[i].y < min_dp)	min_dp = mDP[i].y;
	}
	return min_dp;
}

gfloat get_max_x(struct stDataPoint* mDP, int mLength )
{
	gfloat max_dp = -999999;
	
	for (int i=0; i<mLength; i++)
	{
		if (mDP[i].y > max_dp)	max_dp = mDP[i].y;
	}
	return max_dp;
}


gfloat get_max_y(struct stDataPoint* mDP, int mLength )
{
	gfloat max_dp = -600000;
	
	for (int i=0; i<mLength; i++)
	{
		if (mDP[i].y > max_dp)	max_dp = mDP[i].y;
	}
	return max_dp;
}


gfloat get_y_scale(struct stDataPoint* mDP, int mLength )
{
	gfloat mmax = get_max_y( mDP, mLength );	
	gfloat scale = 2.0 / mmax;
	return scale;
}

gfloat compute_yscale_all_series(  )
{
	int slen = series_data.size();
	gfloat max_scale = 100000;
	
	for (int series=0; series<slen; series++)
	{
		gfloat tmps = get_y_scale(series_data[series].data, series_data[series].data_length );
		if (tmps < max_scale) max_scale = tmps;	
	}
	return max_scale;	
}

gfloat compute_xscale_all_series(  )
{
	int slen = series_data.size();
	gfloat max_scale = 100000;
	gfloat min_x = 999999;
	gfloat max_x = -999999;
	
	for (int series=0; series<slen; series++)
	{
		gfloat tmpminx = get_min_x(series_data[series].data, series_data[series].data_length );
		gfloat tmpmaxx = get_max_x(series_data[series].data, series_data[series].data_length );	

		if (tmpminx < min_x) min_x = tmpminx;	
		if (tmpmaxx < max_x) max_x = tmpminx;
	}
// todo...
	
	return max_scale;	
}


void draw_all_series(cairo_t *cr)
{
	int slen = series_data.size();
	gfloat y_scale = compute_yscale_all_series(  );
	
	for (int series=0; series<slen; series++)
	{	
		
		struct stDataPoint* d = series_data[series].data;
		int dlen = series_data[series].data_length;		
		printf("plotting series # %d\n", series);
						
		for (int ndex = 0; ndex < dlen; ndex++) 
		   cairo_line_to (cr, d[ndex].x,  d[ndex].y*y_scale );
	

		/* Draw the curve */
		struct stColor c = series_data[series].color;
		cairo_set_source_rgba (cr, c.red, c.green, c.blue, c.alpha);
		cairo_stroke (cr);
	}
}

void draw_legend(  cairo_t *cr, int window_width  )
{
	cairo_text_extents_t extents;
	int slen = series_data.size();
	gdouble y = -2.0;
	
	cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 0.12);
	

	for (int series=0; series<slen; series++)
	{	
		cairo_text_extents(cr, series_data[series].name, &extents);		
		
		cairo_set_source_rgb (cr, series_data[series].color.red, series_data[series].color.blue, series_data[series].color.green);
		cairo_rectangle (cr, +2.0, y-extents.height, 0.25, extents.height);
		cairo_fill (cr);
		
		cairo_move_to(cr, 2.3, y);  
		cairo_show_text(cr, series_data[series].name);  

		y+= extents.height;		
	}

}
void draw_title( cairo_t *cr, int window_width )
{
	cairo_set_source_rgba (cr, WHITE.red, WHITE.green, WHITE.blue, WHITE.alpha);

    cairo_scale (cr, 1, -1.0);
	cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 0.25);

	cairo_text_extents_t extents;
	cairo_text_extents(cr, graph_info.title, &extents);
	cairo_move_to(cr, - extents.width/2, -2.0-extents.height);  

	cairo_show_text(cr, graph_info.title);  

}



void draw_axis_labels(  cairo_t *cr, int window_width )
{
	cairo_set_source_rgb (cr, DARK_GREY.red, DARK_GREY.blue, DARK_GREY.green);
	cairo_rectangle (cr, -3.2, 2.0, 6.4, 0.5);
	cairo_fill (cr);

	// X AXIS:
	cairo_set_source_rgba (cr, WHITE.red, WHITE.green, WHITE.blue, WHITE.alpha);
	cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 0.12);

	cairo_text_extents_t extents;
	cairo_text_extents(cr, graph_info.xaxis_label, &extents);
	cairo_move_to(cr, -extents.width/2, 2.0+2.0*extents.height);  
	cairo_show_text(cr, graph_info.xaxis_label);  


	// Y AXIS:
	double x_stop = graph_info.y_axis_margin/100 - (-3.2);
	printf("x,y_axis margin: %7.4f %7.4f\n", graph_info.x_axis_margin/100, graph_info.y_axis_margin/100);
	printf("x_stop: %7.4f \n", x_stop );
	cairo_set_source_rgb (cr, DARK_GREY.red, DARK_GREY.blue, DARK_GREY.green);
	cairo_rectangle (cr, -3.2, -2.5, x_stop, 5.0);
	cairo_fill (cr);

}

void draw_grid( cairo_t *cr, int window_width )
{
#define NUMBER_GRIDS 10

	gfloat x = -3.2;
	gfloat y = -2.0;	
	gfloat x_inc = 6.4 / NUMBER_GRIDS;
	gfloat y_inc = 4.0 / NUMBER_GRIDS;
	
	const double dashed1[] = { 0.025, 0.025 };
    int len1  = sizeof(dashed1) / sizeof(dashed1[0]);
	cairo_set_dash(cr, dashed1, len1, 0);
	
	
	for ( ; x<3.2; x+= x_inc)
	{
		cairo_move_to (cr, x, 2.0);  		
		cairo_line_to (cr, x, graph_info.x_axis_margin/100 );		
		cairo_stroke (cr);		
	}

	for ( ; y<2.0; y+= y_inc)
	{
		cairo_move_to (cr, graph_info.y_axis_margin/100, y );  		
		cairo_line_to (cr, 3.2, y );		
		cairo_stroke (cr);		
	}	
	
}


void draw_graph(  cairo_t *cr, int window_width )
{
    /* Draw on a black background */
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_paint (cr);

	draw_all_series ( cr );
	draw_title		( cr, window_width );
	draw_axis_labels( cr, window_width );
	
	if (graph_info.show_legend)		draw_legend	( cr, window_width );
	if (graph_info.show_grid)		draw_grid   ( cr, window_width );
}