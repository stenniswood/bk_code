#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>


#include "graph.hpp"
 



#define LABEL_FONT_SIZE 2.0
#define TITLE_FONT_SIZE 3.0

const struct stColor BLACK	   	  = { 0.0, 0.0, 0.0, 1.0 };	// 0x80 B9 e4
const struct stColor DARK_GREY    = { 0.20, 0.20, 0.20, 1.0 };	// 0x80 B9 e4
const struct stColor LIGHT_GREY   = { 0.35, 0.35, 0.35, 1.0 };	// 0x80 B9 e4
const struct stColor WHITE        = { 1.0, 1.0, 1.0, 1.0 };
const struct stColor LIGHT_BLUE   = { 0.5, 0.725, 0.894, 1.0 };	// 0x80 B9 e4
const struct stColor LIGHT_GREEN  = { 0.435, 0.666, 0.435, 1.0 }; // 6f aa 6f
const struct stColor YELLOW 	  = { 0.925, 0.905, 0.212, 1.0 }; // ec e7 36
const struct stColor ORANGE 	  = { 0.847, 0.603, 0.145, 1.0 };  // d8 9a 25
const struct stColor RED 		  = { 0.776, 0.110, 0.110, 1.0 };  // c6 1c 1e
const struct stColor RED_PURPLE   = { 0.471, 0.1765, 0.522, 1.0 };  // 78 2d 85
const struct stColor BLUE_PURPLE  = { 0.208, .192, 0.541, 1.0 };  // 35 31 8a  
const struct stColor BLUE 		  = { 0.161, 0.286, 0.608, 1.0 };  // 29 49 9b
const struct stColor BLUE_GREEN   = { 0.227, 0.459, 0.463, 1.0 };  // 3a 75 76
const struct stColor GREEN 		  = { 0.227, 0.447, 0.180, 1.0 };  // 3a 72 2e
const struct stColor YELLOW_GREEN = { 0.569, 0.698, 0.173, 1.0 };  // 91 b2 2c 
const struct stColor YELLOW_BROWN = { 0.325, 0.275, 0.102, 1.0 };  // 53 46 1a
const struct stColor RED_BROWN    = { 0.337, 0.137, 0.133, 1.0 };  // 56 23 22


struct stColor pallette[14] = {
	WHITE, LIGHT_BLUE, LIGHT_GREEN, YELLOW,
	ORANGE, RED, RED_PURPLE, BLUE_PURPLE, 
	BLUE, BLUE_GREEN, GREEN, YELLOW_GREEN,
	YELLOW_BROWN, RED_BROWN
};

struct stTheme black_theme = {
	BLACK,
	WHITE,
	DARK_GREY,
	GREEN,
	LIGHT_GREY,
	14
};
struct stTheme white_theme = {
	WHITE,
	BLACK,
	BLUE,
	GREEN,
	LIGHT_GREY,
	0
};



Graph::Graph( const char* mTitle, const char* mxAxis, const char* myAxis )
{
	strcpy(title, 	    mTitle );
	strcpy(xaxis_label, mxAxis );
	strcpy(yaxis_label, myAxis );
	
	m_show_grid   = true;
	m_show_legend = true;
	theme = &white_theme;
	p_index = 0;	
}

void Graph::set_theme( struct stTheme* mNewTheme )
{
	theme = mNewTheme;
}

static gboolean on_draw (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	Graph* m_graph = (Graph*)user_data;
	m_graph->set_window( widget );    
	m_graph->draw_graph(cr);

    return FALSE;
}

gboolean on_timeout(gpointer user_data)
{
	Graph* g = (Graph*)user_data;	
	gtk_widget_queue_draw( g->da );

    return G_SOURCE_CONTINUE;
}

void Graph::create_window	( int mWidth, int mHeight )
{
    GtkWidget *widow;

    widow = gtk_window_new     	(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (widow), mWidth, mHeight);
    gtk_window_set_title 		(GTK_WINDOW (widow), title );    
    g_signal_connect     		(G_OBJECT   (widow), "destroy", gtk_main_quit, NULL);

    da = gtk_drawing_area_new ( );
    gtk_container_add         ( GTK_CONTAINER (widow), da);

    g_signal_connect (G_OBJECT (da),
            "draw",
            G_CALLBACK (on_draw),
            this );
    gtk_widget_show_all (widow);  
    
    g_timeout_add(20,  on_timeout, this);                  
}

void Graph::set_window( GtkWidget *widget )
{
	window = gtk_widget_get_window(widget);
	/* Determine GtkDrawingArea dimensions */
    gdk_window_get_geometry (window,
            &screen.x,
            &screen.y,
            &screen.width,
            &screen.height);
    //printf("window geometry: %d %d %d %d \n", screen.x, screen.y, screen.width, screen.height );

	cr_x_scale = (float)screen.width  / 100.;
	cr_y_scale = (float)screen.height / 100.;	
	
	x_axis_margin = 0.1 * 100.;
	y_axis_margin = 0.05 * 100.;	// is in x units
	
	// NOTE:  we have to do this after DrawTitle() b/c that's where the y_title_margin gets set.
	//y_scale = compute_yscale_all_series(  );
		
	//printf("cr_x,y_scale = %7.4f %7.4f\n", cr_x_scale, cr_y_scale);
}

Graph::~Graph()
{
	int slen = series_data.size();
	for (int series=0; series<slen; series++)
	{	
		series_data[series].delete_all();		
	}
}

int Graph::find_series_name( const char* mName )
{
	int slen = series_data.size();
	for (int series=0; series<slen; series++)
	{
		int result = strcmp( series_data[series].get_name(), mName );
		if (result)
			return series;
	}	
	return -1;
}

/* Keep same buffer but scroll everything by 1 datapoint.  Append the new Data point
	to the end.
*/
void Graph::scroll_new_data( int mSeriesIndex, struct stDataPoint mNewDataPoint )
{
	series_data[mSeriesIndex].scroll_new_data(mNewDataPoint);	
}

void  Graph::add_data_series( DataSeries& mSeries )
{
	int slen = series_data.size();
	
	// Auto select color :
	int num_p_colors = round((float)sizeof(pallette)/(float)sizeof(stColor));
	int p = (p_index % num_p_colors);
	if (p == theme->skip_pallette_index)  p_index++;
	p = (p_index % num_p_colors);
		
	//printf("palette colors = %7.4f\n", num_p_colors );	
	mSeries.set_color( pallette[p] );
	p_index++;
	
	series_data.push_back( mSeries );	
}

/*gfloat Graph::get_min_x(struct stDataPoint* mDP, int mLength )
{
	gfloat min_dp = 999999;	
	for (int i=0; i<mLength; i++)
	{
		if (mDP[i].y < min_dp)	min_dp = mDP[i].y;
	}
	return min_dp;
}

gfloat Graph::get_max_x(struct stDataPoint* mDP, int mLength )
{
	gfloat max_dp = -999999;
	
	for (int i=0; i<mLength; i++)
	{
		if (mDP[i].y > max_dp)	max_dp = mDP[i].y;
	}
	return max_dp;
}

gfloat Graph::get_min_y(struct stDataPoint* mDP, int mLength )
{
	gfloat min_dp = +FLT_MAX;
	
	for (int i=0; i<mLength; i++)
	{
		if (mDP[i].y < min_dp)	min_dp = mDP[i].y;
	}
	return min_dp;
}

gfloat Graph::get_max_y(struct stDataPoint* mDP, int mLength )
{
	gfloat max_dp = -FLT_MAX;
	
	for (int i=0; i<mLength; i++)
	{
		if (mDP[i].y > max_dp)	max_dp = mDP[i].y;
	}
	return max_dp;
} */


gfloat Graph::get_y_scale(int mSeriesIndex )
{
	gfloat mmax = series_data[mSeriesIndex].get_max_y( );	
	gfloat mmin = series_data[mSeriesIndex].get_min_y( );
	
	gfloat screen_y_allotment = 100. - x_axis_margin - y_title_margin;
	gfloat scale = screen_y_allotment / (mmax-mmin);		// only half the screen.  Negative gets the other half.
	return scale;
}

gfloat Graph::compute_yscale_all_series(  )
{
	int slen         = series_data.size();
	gfloat min_scale = 100000;

	for (int series=0; series<slen; series++)
	{
		gfloat tmps = get_y_scale( series );
		if (tmps < min_scale) min_scale = tmps;	
	}
	return min_scale;	
}

gfloat Graph::compute_xscale_all_series(  )
{
	int slen = series_data.size();
	gfloat max_scale = 100000;
	gfloat min_x = 999999;
	gfloat max_x = -999999;
	
	for (int series=0; series<slen; series++)
	{
		gfloat tmpminx = series_data[series].get_min_x( );
		gfloat tmpmaxx = series_data[series].get_max_x( );	

		if (tmpminx < min_x) min_x = tmpminx;	
		if (tmpmaxx < max_x) max_x = tmpminx;
	}
// todo...
	
	return max_scale;	
}


/////////////////// DRAWING FUNCTIONS ////////////////////////////////////////
void Graph::draw_title( cairo_t *cr )
{
	cairo_identity_matrix ( cr );
    cairo_scale (cr, cr_x_scale, cr_x_scale);	

	cairo_set_source_rgba (cr, theme->title_text.red, theme->title_text.green, 
								theme->title_text.blue, theme->title_text.alpha);

	cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, TITLE_FONT_SIZE);

	cairo_text_extents_t extents;
	cairo_text_extents(cr, title, &extents);
	
	gfloat x = 50. - extents.width/2.;
	gfloat y = extents.height;

	y_title_margin = (y + extents.height*0.0)*cr_y_scale/2.;	// *** IMPORTANT LATER ON.

	cairo_move_to(cr, x, y );  
	cairo_show_text(cr, title);
	
	cairo_identity_matrix ( cr );	
    cairo_scale (cr, cr_x_scale, cr_y_scale);	
}


void Graph::draw_axis_labels(  cairo_t *cr )
{
    /* Draws x and y axis */
    cairo_set_source_rgb (cr, theme->axis.red, theme->axis.green, theme->axis.blue);	// gREEN
    cairo_move_to (cr, y_axis_margin, CENTER_Y_DATAPOINT);
    cairo_line_to (cr, 100.,          CENTER_Y_DATAPOINT);
    cairo_move_to (cr, CENTER_X, 100-x_axis_margin );
    cairo_line_to (cr, CENTER_X, y_title_margin );
    cairo_stroke (cr);
	
	// FILL BOTTOM REGION:
	cairo_set_source_rgb (cr, theme->axis_area_background.red, theme->axis_area_background.green, theme->axis_area_background.blue);
	cairo_rectangle (cr, 0, 100-x_axis_margin, 100, x_axis_margin);
	cairo_fill (cr);

	// X AXIS:
	cairo_set_source_rgba (cr, theme->title_text.red, theme->title_text.green, theme->title_text.blue, theme->title_text.alpha);
	cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, LABEL_FONT_SIZE);

	cairo_text_extents_t extents;
	cairo_text_extents(cr, xaxis_label, &extents);
	//printf("xlabel extents = %7.4f %7.4f\n", extents.width, extents.height );
	gfloat tx = 50.-    extents.width/2.;
	gfloat ty = 100-2.0*extents.height;
	cairo_move_to(cr, tx, ty);  
	//printf("xlabel at %7.4f %7.4f\n", tx, ty );
	cairo_show_text(cr, xaxis_label);  


	// Y AXIS:
	// FILL BOTTOM REGION:	
	//printf("x,y_axis margin: %7.4f %7.4f\n", x_axis_margin, y_axis_margin);
	cairo_set_source_rgb (cr, theme->axis_area_background.red, theme->axis_area_background.green, theme->axis_area_background.blue);
	cairo_rectangle (cr, 0, 0, y_axis_margin, 100.0);
	cairo_fill (cr);

}

void Graph::draw_grid( cairo_t *cr )
{
#define NUMBER_GRIDS 10
	cairo_identity_matrix ( cr );
    cairo_scale (cr, cr_x_scale, cr_y_scale);	
	gdouble dx = y_axis_margin, dy = y_title_margin; /* Pixels between each point */
    cairo_translate      (cr, y_axis_margin, y_title_margin );
	cairo_set_source_rgba (cr, theme->grid.red,    theme->grid.green, 
							   theme->grid.blue,   theme->grid.alpha);
		
	gfloat x = 0.;
	gfloat y = 0.;	
	gfloat x_inc = (100-y_axis_margin) / NUMBER_GRIDS;
	gfloat y_inc = (DATA_AREA_Y_SIZE) / NUMBER_GRIDS;
	
	const double dashed1[] = { 0.025, 0.025 };
    int len1  = sizeof(dashed1) / sizeof(dashed1[0]);
	cairo_set_dash(cr, dashed1, len1, 0);
	
	
	// VERTICAL
	for ( ; x<(100.-y_axis_margin); x+= x_inc)
	{
		cairo_move_to (cr, x, 0. );
		cairo_line_to (cr, x, DATA_AREA_Y_SIZE );		
		cairo_stroke (cr);		
	}

	// HORIZONTAL
	for ( ; y<DATA_AREA_Y_SIZE; y+= y_inc)
	{
		cairo_move_to (cr, 0., 				   y );  		
		cairo_line_to (cr, 100.-y_axis_margin, y );		
		cairo_stroke (cr);		
	}
}

void Graph::draw_legend(  cairo_t *cr )
{
	cairo_text_extents_t extents;
	int slen = series_data.size();
	gdouble y = -2.0;
	
	cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 2.0);
	

	for (int series=0; series<slen; series++)
	{	
		cairo_text_extents(cr, series_data[series].get_name(), &extents);		
		
		struct stColor c = series_data[series].get_color();
		cairo_set_source_rgb (cr, c.red, c.blue, c.green);
		cairo_rectangle (cr, +2.0, y-extents.height, 0.25, extents.height);
		cairo_fill (cr);
		
		cairo_move_to(cr, 2.3, y);  
		cairo_show_text(cr, series_data[series].get_name());  

		y+= extents.height;		
	}
}


void Graph::draw_all_series(cairo_t *cr)
{
    cairo_set_line_width (cr, 0.25);    
    cairo_translate      (cr, y_axis_margin, CENTER_Y_DATAPOINT );

	int slen = series_data.size();
	for (int series=0; series<slen; series++)
	{	
		int    dlen = series_data[series].size();		
		//printf("plotting series # %d;  size=%d\n", series, dlen);
						
		for (int ndex = 0; ndex < dlen; ndex++)  {
			struct stDataPoint* dp = series_data[series].get_data( ndex );
			if (dp)
			    cairo_line_to (cr, dp->x,  dp->y * y_scale );
		   //printf("x,y=%7.4f %7.4f\n", d[ndex].x, d[ndex].y );
		}

		/* Draw the curve */
		struct stColor c = series_data[series].get_color();
		cairo_set_source_rgba (cr, c.red, c.green, c.blue, c.alpha);
		cairo_stroke (cr);
	}
}


void Graph::draw_graph(  cairo_t *cr )
{
    cairo_scale (cr, cr_x_scale, cr_y_scale);
    
    /* Determine the data points to calculate (ie. those in the clipping zone */
    gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
    gdouble dx = 5.0, dy = 5.0; /* Pixels between each point */
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, dx);

    //printf("clip_x1=%7.4f; clip_x2=%7.4f\n", clip_x1, clip_x2 );	    // -3.2 to +3.2
    //printf("clip_y1=%7.4f; clip_y2=%7.4f\n", clip_y1, clip_y2 );    
    //printf("dx=%7.4f; dy=%7.4f\n", dx, dy );        
    
    
    /* Draw on a black background */
    cairo_set_source_rgb (cr, theme->background.red, theme->background.green, theme->background.blue);
    cairo_paint (cr);

	draw_title		( cr );
	y_scale = compute_yscale_all_series(  );
	
	draw_axis_labels( cr );
	draw_all_series ( cr );
		
	//if (m_show_legend)		draw_legend	( cr );
	if (m_show_grid)		draw_grid   ( cr );
}

