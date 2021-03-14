#include <stdlib.h>

#include "graph.hpp"
 

//#define MAX(X,Y) ( ((X) > (Y)) ? (X) : (Y))
//#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
  

#define LABEL_FONT_SIZE 14.0
#define TITLE_FONT_SIZE 14.0

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
	BLACK,		/* background */
	WHITE,		/* title_text */
	WHITE,		/* label_text */
	DARK_GREY,  /* axis_area_background */
	GREEN,		/* axis */
	LIGHT_GREY,	/* grid lines */
	14			/* skip_pallette_index */
};
struct stTheme white_theme = {
	WHITE,		/* background */
	BLACK,		/* title_text */
	WHITE,		/* label_text */	
	BLUE,		/* axis_area_background */
	GREEN,		/* axis */
	BLACK,	/* grid lines */
	0			/* skip_pallette_index */
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
	
	x_axis_margin  = 0;
	y_title_margin = 0;
	
	m_xmin_all = 0;
	m_xscale  = 1.0;
	m_x_start = 0.0;
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
static gboolean on_draw_stats(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	Graph* m_graph = (Graph*)user_data;
	struct stTheme* theme = m_graph->get_theme();
	DataSeries*     ds  = m_graph->get_series(0);
	
	
	cairo_set_source_rgba ( cr, theme->title_text.red, theme->title_text.green, 
							    theme->title_text.blue, theme->title_text.alpha );

	cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, TITLE_FONT_SIZE);

	char str[80];
	sprintf(str, "n=%d", ds->size() );	

	cairo_text_extents_t extents;
	cairo_text_extents(cr, str, &extents);
	
	gfloat x = 5;
	gfloat y = 2 + extents.height;

	cairo_move_to(cr, x, y );  	cairo_show_text(cr, str);
	y += extents.height;

	sprintf(str, "Ymin=%6.2f; Ymax=%6.2f", ds->get_min_y(), ds->get_max_y() );	
	cairo_move_to(cr, x, y );  	cairo_show_text(cr, str);
	y += extents.height;
		
	sprintf(str, "Xmin=%6.2f; Xmax=%6.2f", ds->get_min_x(), ds->get_max_x() );	
	cairo_move_to(cr, x, y );  	cairo_show_text(cr, str);
	y += extents.height;
			
    return FALSE;
}

gboolean on_timeout(gpointer user_data)
{
	Graph* g = (Graph*)user_data;	
	gtk_widget_queue_draw( g->da );

    return G_SOURCE_CONTINUE;
}

static void on_show_grid (GtkWidget *widget, gpointer user_data)
{
	Graph* m_graph = (Graph*)user_data;
	if (m_graph->m_show_grid)
		m_graph->show_grid(false);
	else 
		m_graph->show_grid(true);
	printf("SHOW GRID\n");
}

static void hello (void)  {  printf("Hello Legend!\n");  };



void Graph::create_window	( int mWidth, int mHeight )
{
    GtkWidget *widow;

    widow = gtk_window_new     	(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (widow), mWidth, mHeight);
    gtk_window_set_title 		(GTK_WINDOW (widow), title );    
    g_signal_connect     		(G_OBJECT   (widow), "destroy", gtk_main_quit, NULL);

	// TOP LEVEL BOX :     
    GtkWidget*	 box1 = gtk_box_new( GTK_ORIENTATION_VERTICAL, 0 );
	gtk_container_add 		( GTK_CONTAINER (widow), box1 );
	gtk_widget_set_vexpand 	( box1, TRUE );		
	gtk_widget_set_valign   ( box1, GTK_ALIGN_FILL);

	// DRAW AREA:
    da = gtk_drawing_area_new ( );
	gtk_box_pack_start (GTK_BOX (box1), da, TRUE, TRUE, 0);
    g_signal_connect (G_OBJECT (da), "draw", G_CALLBACK (on_draw), this );
 
    // GUI BELOW GRAPH : 
 /*   GtkWidget*	 box2 = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 0 );
	gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_widget_set_hexpand 	( box2, TRUE );		
	gtk_widget_set_halign   ( box2, GTK_ALIGN_FILL); 
	
	// BUTTON
	GtkWidget *button  = gtk_button_new_with_label ("Show Legend");
	gtk_widget_set_margin_top    (button, 10);
	gtk_widget_set_margin_bottom (button, 10);
	gtk_widget_set_margin_start  (button, 10);
	gtk_widget_set_margin_end    (button, 10);
	g_signal_connect (button, "clicked", G_CALLBACK (hello), NULL);  
	gtk_box_pack_start (GTK_BOX (box2), button, FALSE, FALSE, 0);
    
	GtkWidget *button2  = gtk_button_new_with_label ("Show Grid");
	gtk_widget_set_margin_top    (button2, 10);
	gtk_widget_set_margin_bottom (button2, 10);
	gtk_widget_set_margin_start  (button2, 10);
	gtk_widget_set_margin_end    (button2, 10);
	g_signal_connect (button2, "clicked", G_CALLBACK (on_show_grid), (gpointer)this);  
	gtk_box_pack_start (GTK_BOX (box2), button2, FALSE, FALSE, 0);
    
	// STATS/INFO TEXT AREA:
    ta = gtk_drawing_area_new ( );
	gtk_box_pack_start (GTK_BOX (box1), ta, TRUE, TRUE, 0);
    //g_signal_connect (G_OBJECT (da), "draw", G_CALLBACK (on_draw_stats), this );
*/	
	    
    gtk_widget_show_all (widow);  
    
    g_timeout_add(20,  on_timeout, this);                  
}

void Graph::set_window( GtkWidget *widget )
{
	window = gtk_widget_get_window( widget );
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
		if (result==0)
			return series;
	}	
	return -1;
}

DataSeries* Graph::get_series	( int mSeriesIndex )
{
	return &(series_data[mSeriesIndex]);	
}

void Graph::append_new_data( int mSeriesIndex, struct stDataPoint mNewDataPoint )
{
	series_data[mSeriesIndex].append_datum(mNewDataPoint);
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
	
	compute_mins_maxs();	
}



gfloat Graph::get_y_scale(int mSeriesIndex )
{
	double mmax = series_data[mSeriesIndex].get_max_y( );	
	double mmin = series_data[mSeriesIndex].get_min_y( );	
	
	if ((mmax-mmin)==0) return 99999;

	double numerator = (double)DATA_AREA_Y_SIZE;
	double scale = numerator / (mmax-mmin);		// only half the screen.  Negative gets the other half.
	//printf("get_y_scale(%d) :numer=%6.2lf;  minY=%6.3lf; maxY=%6.3lf;  scale=%6.2lf\n", mSeriesIndex, numerator, mmin, mmax, scale );
	return scale;
}

gfloat Graph::compute_yscale_all_series(  )
{
	int slen         = series_data.size();
	if (slen==0) return y_scale;		// maintain derived class may have set it.
	
	gfloat min_scale = 100000;

	for (int series=0; series<slen; series++)
	{
		gfloat tmps = get_y_scale( series );
		//printf("compute_yscale_all_series() : series %d : %6.3f\n", series, tmps );
		
		if ((tmps>0) && (tmps < min_scale)) min_scale = tmps;	
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
		if (tmpmaxx > max_x) max_x = tmpmaxx;
	}
// todo...
	
	return max_scale;	
}

float	Graph::dataPoint_x_to_Pixel	( double mX )
{
	double xd = (mX - m_xmin_all);
	double x  = (DATA_AREA_X_SIZE) * xd * m_xscale / m_xmax_all ;
	
	float final_x = x + y_axis_margin;	// scaled to display scale.
	
	// result will go from [x_axis_margin..screen_width]
	return final_x;
}

float	Graph::dataPoint_y_to_Pixel( double mY )
{	
	float y = (mY - m_ymin_all) * y_scale;
	float y_pix = (DATA_AREA_Y_SIZE - y) + y_title_margin;
	return y_pix;
}

float	Graph::Pixel_to_dataPoint	( double mX, double mY )
{
	return 0;
}


void Graph::compute_mins_maxs()
{
	m_ymin_all = 32767;
	m_xmin_all = 32767;	m_xmax_all = -32767;

	int slen = series_data.size();	
	for (int series=0; series<slen; series++) {
		m_ymin_all = MIN(  m_ymin_all, series_data[series].get_min_y() );

		m_xmin_all = MIN(  m_xmin_all, series_data[series].get_min_x() );
		m_xmax_all = MAX(  m_xmax_all, series_data[series].get_max_x() );
	}
	//printf("MAX : ymin=%6.2f;  %6.2f:x:%6.2f\n", m_ymin_all, m_xmin_all, m_xmax_all );
}


/////////////////// DRAWING FUNCTIONS ////////////////////////////////////////
/////////////////// DRAWING FUNCTIONS ////////////////////////////////////////
/////////////////// DRAWING FUNCTIONS ////////////////////////////////////////

void Graph::draw_title( cairo_t *cr )
{
	cairo_identity_matrix ( cr );
	cairo_set_source_rgba ( cr, theme->title_text.red, theme->title_text.green, 
							    theme->title_text.blue, theme->title_text.alpha );

	cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, TITLE_FONT_SIZE);

	cairo_text_extents_t extents;
	cairo_text_extents(cr, title, &extents);
	
	gfloat x = (screen.width/2) - extents.width/2.;
	gfloat y = 2 + extents.height;

	y_title_margin = (y + 2 + extents.height*0.1); // *cr_y_scale/2.;	// *** IMPORTANT LATER ON.

	cairo_move_to(cr, x, y );  
	cairo_show_text(cr, title);

	// UNDERLINE TITLE BOUNDARY:	
	cairo_move_to (cr, y_axis_margin, y_title_margin );  
    cairo_line_to (cr, screen.width,  y_title_margin );    		
}


void Graph::draw_axis_labels(  cairo_t *cr )
{
    cairo_set_line_width (cr, 3.0);   
	// X AXIS LABEL :
	cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, LABEL_FONT_SIZE);

	cairo_text_extents_t  extents_x, extents_y;
	cairo_text_extents(cr, xaxis_label, &extents_x );
	cairo_text_extents(cr, yaxis_label, &extents_y );

	gfloat tx     = (screen.width/2) - extents_x.width/2.;
	gfloat ty     = (screen.height ) - (0.25 * extents_x.height);
	x_axis_margin = ( 1.5 * extents_x.height);
	y_axis_margin = ( 1.5 * extents_y.height);

    
    /* Draws x axis */
    cairo_set_source_rgb (cr, theme->axis.red, theme->axis.green, theme->axis.blue);	// gREEN
    cairo_move_to (cr, y_axis_margin, CENTER_Y_DATAPOINT );
    cairo_line_to (cr, screen.width,  CENTER_Y_DATAPOINT );

    /* Draws y axis */
    gfloat cx = (screen.width - y_axis_margin)/2.0 + y_axis_margin;    
    cairo_move_to (cr, cx, y_title_margin );
    cairo_line_to (cr, cx, screen.height-x_axis_margin );
    cairo_stroke  (cr);

	
	// FILL BOTTOM REGION:
	cairo_set_source_rgb (cr, theme->axis_area_background.red, theme->axis_area_background.green, theme->axis_area_background.blue);
	cairo_rectangle (cr, 0, screen.height-x_axis_margin, screen.width, x_axis_margin);
	cairo_fill (cr);
	// FILL SIDE REGION:
	cairo_set_source_rgb (cr, theme->axis_area_background.red, theme->axis_area_background.green, theme->axis_area_background.blue);
	cairo_rectangle (cr, 0, 0, y_axis_margin, screen.height );
	cairo_fill (cr);
	
	// DRAW TEXT:
	cairo_set_source_rgb (cr, theme->label_text.red, theme->label_text.green, theme->label_text.blue);
	cairo_move_to   ( cr, tx, ty          );
	cairo_show_text ( cr, xaxis_label     );  


	// Y AXIS:
	// FILL BOTTOM REGION:	
	cairo_save(cr); 
	    cairo_translate (cr, 0, screen.height/2 );
	cairo_rotate(cr, -90.0*(M_PI/180.0) ); 
	cairo_move_to   ( cr, -extents_y.width/2, +extents_y.height );
	cairo_show_text ( cr, yaxis_label );  	
	cairo_restore(cr); 

	//printf("x,y_axis margin: %7.4f %7.4f\n", x_axis_margin, y_axis_margin);
}

#define NUMBER_GRIDS 10
void Graph::draw_grid( cairo_t *cr )
{
    cairo_set_line_width (cr, 2.0);   
	cairo_identity_matrix ( cr );
	cairo_set_source_rgba (cr, theme->grid.red,    theme->grid.green, 
							   theme->grid.blue,   theme->grid.alpha);

	gfloat full_width  = screen.width  - y_axis_margin;
	gfloat full_height = screen.height - x_axis_margin;
	gfloat x = y_axis_margin;
	gfloat y = y_title_margin;	
	gfloat x_inc = full_width / NUMBER_GRIDS;
	gfloat y_inc = (DATA_AREA_Y_SIZE) / NUMBER_GRIDS;
	
	const double dashed1[] = { 0.025, 0.025 };
    int len1  = sizeof(dashed1) / sizeof(dashed1[0]);
	cairo_set_dash(cr, dashed1, len1, 0);
	
	// VERTICAL
	for (x=y_axis_margin; x<screen.width; x+= x_inc)
	{
		cairo_move_to (cr, x, y_title_margin    );
		cairo_line_to (cr, x, full_height );		
		cairo_stroke (cr);		
	}

	// HORIZONTAL
	for ( ; y<DATA_AREA_Y_SIZE; y+= y_inc)
	{
		cairo_move_to (cr, y_axis_margin, y );  		
		cairo_line_to (cr, screen.width,  y );
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


void Graph::draw_series(cairo_t *cr, int mSeriesIndex )
{
	int    dlen = series_data[mSeriesIndex].size() / m_xscale;
	if (dlen==0) return ;
	
	// MOVE TO FIRST DATA POINT : 
	struct stDataPoint* dp = series_data[mSeriesIndex].get_data( 0 );
	double ypix = dataPoint_y_to_Pixel( dp->y );
	double xpix = dataPoint_x_to_Pixel( dp->x ); 				
	cairo_move_to (cr, xpix,  ypix );

	// DRAW REMAINING DATA POINTS:						
	for (int ndex=1; ndex < dlen; ndex++)  {
		dp = series_data[mSeriesIndex].get_data( ndex );
		if (dp) {
			ypix = dataPoint_y_to_Pixel( dp->y );
			xpix = dataPoint_x_to_Pixel( dp->x ); 				
			cairo_line_to (cr, xpix,  ypix );
		}
	}

	/* Draw the curve */
	struct stColor c = series_data[mSeriesIndex].get_color();
	cairo_set_source_rgba (cr, c.red, c.green, c.blue, c.alpha);
	cairo_stroke( cr );
}


void Graph::draw_all_series(cairo_t *cr)
{	
    cairo_set_line_width (cr, 2.0);    

	int slen = series_data.size();			
	for (int series=0; series<slen; series++)
	{	
		draw_series( cr, series );
	}
}


void Graph::draw_graph(  cairo_t *cr )
{
    cairo_scale (cr, cr_x_scale, cr_y_scale);

	// This has to be in a Draw function b/c it requires the screen dimensions and layout.
	y_scale = compute_yscale_all_series(  );		
	//printf("FINAL: yScale = %6.2f; xmax_all= %6.2f\n", y_scale, m_xmax_all );
    
    /* Draw on a black background */
    cairo_set_source_rgb (cr, theme->background.red, theme->background.green, theme->background.blue);
    cairo_paint (cr);

	draw_title		( cr );
	
	draw_axis_labels( cr );
	draw_all_series ( cr );
		
	//if (m_show_legend)		draw_legend	( cr );
	if (m_show_grid)		draw_grid   ( cr );
}



/*
    // Determine the data points to calculate (ie. those in the clipping zone 
    gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
    
    gdouble dx = 5.0, dy = 5.0; /* Pixels between each point 
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, dx);

    //printf("clip_x1=%7.4f; clip_x2=%7.4f\n", clip_x1, clip_x2 );	    // -3.2 to +3.2
    //printf("clip_y1=%7.4f; clip_y2=%7.4f\n", clip_y1, clip_y2 );    
    //printf("dx=%7.4f; dy=%7.4f\n", dx, dy );        

*/