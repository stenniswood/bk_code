#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <string.h>

#include "graph.hpp"

#define WIDTH   640
#define HEIGHT  480

#define ZOOM_X  100.0
#define ZOOM_Y  100.0


gfloat f (gfloat x)
{
    return 0.03 * pow (x, 3);
}

static gboolean on_draw (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    GdkRectangle da;            /* GtkDrawingArea size */
    gdouble dx = 5.0, dy = 5.0; /* Pixels between each point */
    gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
    GdkWindow *window = gtk_widget_get_window(widget);

    /* Determine GtkDrawingArea dimensions */
    gdk_window_get_geometry (window,
            &da.x,
            &da.y,
            &da.width,
            &da.height);
    printf("window geometry: %d %d %d %d \n", da.x, da.y, da.width, da.height );
    


    /* Change the transformation matrix */
    cairo_translate (cr, da.width / 2, da.height / 2);
    cairo_scale (cr, ZOOM_X, -ZOOM_Y);

    /* Determine the data points to calculate (ie. those in the clipping zone */
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, dx);

    /* Draws x and y axis */
    cairo_set_source_rgb (cr, 0.0, 1.0, 0.0);
    cairo_move_to (cr, clip_x1, 0.0);
    cairo_line_to (cr, clip_x2, 0.0);
    cairo_move_to (cr, 0.0, clip_y1);
    cairo_line_to (cr, 0.0, clip_y2);
    cairo_stroke (cr);

    /* Link each data point */
    printf("clip_x1=%7.4f; clip_x2=%7.4f\n", clip_x1, clip_x2 );	    // -3.2 to +3.2
    printf("clip_y1=%7.4f; clip_y2=%7.4f\n", clip_y1, clip_y2 );    
    
    printf("dx=%7.4f; dy=%7.4f\n", dx, dy );        

    cairo_set_line_width (cr, 0.01);
	//draw_all_series(cr);
	//draw_title(cr, WIDTH);
	draw_graph(cr,WIDTH);

    return FALSE;
}


int init_graph_data()
{
	struct stSeriesInfo si;
	si.color = pallette[0];
	strcpy (si.name, "sine wave");
	si.data  = new struct stDataPoint[WIDTH];
	
	int i=0;
	for (i=0; i< (WIDTH) ; i++)
	{
		float x = 3.2 * ((float)(i-WIDTH/2) / (float)WIDTH);
		si.data[i].x = x;
		si.data[i].y = 20. * sin (  2.0*M_PI * 10.0 * (x/3.2) );
	}
	si.data_length = i;
	series_data.push_back( si );
	return i;
}
int init_graph_data2()
{
	struct stSeriesInfo si;
	si.color = pallette[1];
	strcpy (si.name, "small sine wave");
	si.data  = new struct stDataPoint[WIDTH];
	
	int i=0;
	for (i=0; i< (WIDTH) ; i++)
	{
		float x = 3.2 * ((float)(i-WIDTH/2) / (float)WIDTH);
		si.data[i].x = x;
		si.data[i].y = 5. * sin (  2.0*M_PI * 5.0 * (x/3.2) );
	}
	si.data_length = i;
	series_data.push_back( si );
	return i;
}

int main (int argc, char **argv)
{	
    GtkWidget *window;
    GtkWidget *da;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);

	// INITIALIZE GRAPH:
	create_graph( "MPU6050 -vs- Time", "Time", "SensorOutput", WIDTH, HEIGHT );	
	init_graph_data();
	init_graph_data2();


    gtk_window_set_title (GTK_WINDOW (window), graph_info.title );
    g_signal_connect (G_OBJECT (window), "destroy", gtk_main_quit, NULL);

    da = gtk_drawing_area_new ();
    gtk_container_add (GTK_CONTAINER (window), da);

    g_signal_connect (G_OBJECT (da),
            "draw",
            G_CALLBACK (on_draw),
            NULL);

    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}
