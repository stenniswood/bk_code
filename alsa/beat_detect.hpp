#ifndef _BEAT_DETECT_HPP_
#define _BEAT_DETECT_HPP_


#include "gtk/graph.hpp"
#include "gtk/annotated_graph.hpp"


#define NUM_FRAMES 500024
extern double Energies[NUM_FRAMES*2];
extern size_t EnergiesIndex;
extern double SmoothEnergies[NUM_FRAMES*2];
extern size_t SmoothedLength;


void AddEnergyGraphData(AnnotatedGraph* mGraph );

void	process_waveform	( DSPWave& wav );

double 	compute_energy		( short* mSamps, long int mLength );
void 	peak_pick_energy	( double mEnergies[], int mLength );

void 	print_peaks			( );
void 	print_energy_array	( );
void	print_deltas		( );

void 	compute_peak_deltas();
void 	bin_peak_deltas();
float 	determine_beats_per_min();

int 	bass_drum_detect( short* mData, int mLength, double& mLHRatio );


#endif
