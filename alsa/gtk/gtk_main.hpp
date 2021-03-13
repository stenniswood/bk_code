#ifndef _GTK_MAIN_HPP_
#define _GTK_MAIN_HPP_

#include <gtk/gtk.h>
#include "graph.hpp"
#include "wavegraph.hpp"
#include "annotated_graph.hpp"
#include "../dspwave.hpp"
#include "../beat_detect.hpp"


int init_energy_graph_data();

int graph_init( );
AnnotatedGraph* create_annotated_graph( char* mTitle, 
					char* mXAxisName, char* mYAxisName, 
					double* mValues, int mNumValues  );
					
WaveGraph* create_wave_graph( char* mTitle, 
			char* mXAxisName, char* mYAxisName );
		
extern AnnotatedGraph* e_graph ;
extern AnnotatedGraph* c_graph ;
extern AnnotatedGraph* ec_graph;
extern WaveGraph* 	wave_graph ;


//int init_graph_data2();

//bool new_data_timeout(gpointer user_data);
void 	init_hist_data	(  );

int graph_main( );



#endif
