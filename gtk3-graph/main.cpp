#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <string.h>

#include "graph.hpp"
#include "histogram.hpp"


#define WIDTH   960
#define HEIGHT  480

struct stHData {
	int line;
	int duration;
	int min_between;
};
extern struct stHData hdata[100];



Graph my_graph( "MPU6050 -vs- Time", "Time", "SensorOutput" );	
Histogram my_hist( "Old Faithful -vs- Eruptions", "Eruption Counter", "Minutes between" );	


int init_graph_data()
{
	struct stDataPoint dp;
	struct DataSeries si;
	si.set_name( "sine wave" );

	int i=0;
	for (i=0; i< (WIDTH) ; i++)
	{
		dp.x = 100. * ((float)(i) / (float)WIDTH);
		dp.y = 300. * sin (  2.0*M_PI * 10.0 * (dp.x/100.) );
		si.append_datum( dp );
	}

	my_graph.add_data_series( si );	
	return i;
}
int init_graph_data2()
{
	struct stDataPoint dp;
	struct DataSeries si;

	si.set_name( "small sine wave" );
	
	int i=0;
	for (i=0; i< (WIDTH) ; i++)
	{
		dp.x = 100. * ((float)(i) / (float)WIDTH);
		dp.y = 100. * sin (  2.0*M_PI * 5.0 * (dp.x/100.) );
		si.append_datum( dp );
	}
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

void 	init_hist_data	(  )
{
	DataSeries hd;
	struct stDataPoint mNewDP;
	
	for (int i=0; i<100; i++)
	{
		mNewDP.x = hdata[i].line;
		mNewDP.y = hdata[i].duration;
		hd.append_datum( mNewDP );		
	}
	
	float min_y = hd.get_min_y();
	float max_y = hd.get_max_y();
	printf("MinY:%7.4f  %7.4f\n", min_y, max_y );
	
	my_hist.add_data_series( hd );

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
    
    my_hist.show_grid		( true );
	init_hist_data  		(       );

	my_hist.compute_stats	( 0, 20 );
	my_hist.print_bins();
	my_hist.create_window(640, HEIGHT);    
    
    gtk_main ();

    return 0;
}


struct stHData hdata[100] = {
{1, 3600, 79},
{2, 1800, 54},
{3, 3333, 74},
{4, 2283, 62},
{5, 4533, 85},
{6, 2883, 55},
{7, 4700, 88},
{8, 3600, 85},
{9, 1950, 51},
{10, 4350, 85},
{11, 1833, 54},
{12, 3917, 84},
{13, 4200, 78},
{14, 1750, 47},
{15, 4700, 83},
{16, 2167, 52},
{17, 1750, 62},
{18, 4800, 84},
{19, 1600, 52},
{20, 4250, 79},
{21, 1800, 51},
{22, 1750, 47},
{23, 3450, 78},
{24, 3067, 69},
{25, 4533, 74},
{26, 3600, 83},
{27, 1967, 55},
{28, 4083, 76},
{29, 3850, 78},
{30, 4433, 79},
{31, 4300, 73},
{32, 4467, 77},
{33, 3367, 66},
{34, 4033, 80},
{35, 3833, 74},
{36, 2017, 52},
{37, 1867, 48},
{38, 4833, 80},
{39, 1833, 59},
{40, 4783, 90},
{41, 4350, 80},
{42, 1883, 58},
{43, 4567, 84},
{44, 1750, 58},
{45, 4533, 73},
{46, 3317, 83},
{47, 3833, 64},
{48, 2100, 53},
{49, 4633, 82},
{50, 2000, 59},
{51, 4800, 75},
{52, 4716, 90},
{53, 1833, 54},
{54, 4833, 80},
{55, 1733, 54},
{56, 4883, 83},
{57, 3717, 71},
{58, 1667, 64},
{59, 4567, 77},
{60, 4317, 81},
{61, 2233, 59},
{62, 4500, 84},
{63, 1750, 48},
{64, 4800, 82},
{65, 1817, 60},
{66, 4400, 92},
{67, 4167, 78},
{68, 4700, 78},
{69, 2067, 65},
{70, 4700, 73},
{71, 4033, 82},
{72, 1967, 56},
{73, 4500, 79},
{74, 4000, 71},
{75, 1983, 62},
{76, 5067, 76},
{77, 2017, 60},
{78, 4567, 78},
{79, 3883, 76},
{80, 3600, 83},
{81, 4133, 75},
{82, 4333, 82},
{83, 4100, 70},
{84, 2633, 65},
{85, 4067, 73},
{86, 4933, 88},
{87, 3950, 76},
{88, 4517, 80},
{89, 2167, 48},
{90, 4000, 86},
{91, 2200, 60},
{92, 4333, 90},
{93, 1867, 50},
{94, 4817, 78},
{95, 1833, 63},
{96, 4300, 72},
{97, 4667, 84},
{98, 3750, 75},
{99, 1867, 51},
{100, 4900, 82}
};
