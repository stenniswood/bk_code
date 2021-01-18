#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <string.h>

#include "graph.hpp"

#define WIDTH   960
#define HEIGHT  480


Graph my_graph( "MPU6050 -vs- Time", "Time", "SensorOutput" );	
	
	

static gboolean on_draw (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	my_graph.set_window( widget );
		
    GdkWindow *window = gtk_widget_get_window(widget);
    
	my_graph.draw_graph(cr);

    return FALSE;

    /* Change the transformation matrix */
    //cairo_translate (cr, da.width / 2, da.height / 2);
}


int init_graph_data()
{
	struct stSeriesInfo si;
	strcpy (si.name, "sine wave");
	si.data  = new struct stDataPoint[WIDTH];
	
	int i=0;
	for (i=0; i< (WIDTH) ; i++)
	{
		float x = 100. * ((float)(i) / (float)WIDTH);
		si.data[i].x = x;
		si.data[i].y = 300. * sin (  2.0*M_PI * 10.0 * (x/100.) );
	}
	si.data_length = WIDTH;

	my_graph.add_data_series( si );	
	return i;
}
int init_graph_data2()
{
	struct stSeriesInfo si;
	strcpy (si.name, "small sine wave");
	si.data  = new struct stDataPoint[WIDTH];
	
	int i=0;
	for (i=0; i< (WIDTH) ; i++)
	{
		float x = 100. * ((float)(i) / (float)WIDTH);
		si.data[i].x = x;
		si.data[i].y = 100. * sin (  2.0*M_PI * 5.0 * (x/100.) );
	}
	si.data_length = WIDTH;
	my_graph.add_data_series( si );
	return i;
}

int main (int argc, char **argv)
{	
    GtkWidget *window;
    GtkWidget *da;

    gtk_init (&argc, &argv);
    window = gtk_window_new     (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);

    gtk_window_set_title (GTK_WINDOW (window), my_graph.get_title() );
    g_signal_connect     (G_OBJECT (window), "destroy", gtk_main_quit, NULL);

    da = gtk_drawing_area_new ( );
    gtk_container_add         ( GTK_CONTAINER (window), da);


	// INITIALIZE GRAPH:
	my_graph.show_grid(false);
	init_graph_data();
	init_graph_data2();


    g_signal_connect (G_OBJECT (da),
            "draw",
            G_CALLBACK (on_draw),
            &my_graph);

    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}
