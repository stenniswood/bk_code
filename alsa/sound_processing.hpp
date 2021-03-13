#ifndef _SOUND_PROCESSING_HPP_
#define _SOUND_PROCESSING_HPP_

#include "gtk/graph.hpp"
#include "gtk/annotated_graph.hpp"
#include "gtk/wavegraph.hpp"

#include "gtk/peak_pick.hpp"

#include "wave.hpp"
#include "dspwave.hpp"
#include "beat_detect.hpp"
#include "gtk/gtk_main.hpp"




void	energies_n_peaks( DSPWave& wav );
//void  process_waveform( DSPWave& wav );
void	pitch_detect( DSPWave& wav );
void	quantize_pitches( DSPWave& wav );

void  	pitch_detect ( DSPWave& wav );
void  	find_waveform( DSPWave& RecordedWave, Wave* mSrcWave );


double  spectral_distortion( DSPWave& wav );
void	spectral_segmentation( DSPWave& wav );


void	createGraphs	( Wave* mWave );
void 	AddEnergiesData	( AnnotatedGraph* mGraph );
void 	AddPitchData	( AnnotatedGraph* mGraph );
//void 	AddWaveform		( WaveGraph* mGraph, Wave* mWav );


#endif
