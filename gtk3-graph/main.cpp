#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <string.h>

#include "graph.hpp"

#define WIDTH   960
#define HEIGHT  480


Graph my_graph( "MPU6050 -vs- Time", "Time", "SensorOutput" );	


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

int count = 0;
gboolean new_data_timeout(gpointer user_data)
{
	Graph* g = (Graph*)user_data;	

	struct stDataPoint dp;
	dp.x = 100;
	dp.y = 300.*(double)random()/(double)RAND_MAX;
	g->scroll_new_data( 0, dp );

	//printf("added %d %7.4f %7.4f\t", count++, dp.x, dp.y );
    return G_SOURCE_CONTINUE;
}

int main (int argc, char **argv)
{	
    gtk_init (&argc, &argv);

	// INITIALIZE GRAPH:
	my_graph.show_grid(false);
	init_graph_data ();
	init_graph_data2();
	my_graph.create_window(WIDTH, HEIGHT);

    g_timeout_add (1,  new_data_timeout, &my_graph);
    
    gtk_main ();

    return 0;
}


