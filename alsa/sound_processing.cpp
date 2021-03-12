
#include "sound_processing.hpp"
#include "alsa_record.hpp"


AnnotatedGraph* e_graph = NULL;
AnnotatedGraph* c_graph = NULL;
AnnotatedGraph* ec_graph = NULL;
WaveGraph* 		wave_graph = NULL;


double  EnergyThreshold = 12.0;
bool	InHighEnergy = false;
int		prev_Drum    = 0;


void process_window( short* mSamps, int mLength, size_t mSampleIndex )
{
	//int Start = recorded.TimeToSamples( 0.417 );
	//int End   = recorded.TimeToSamples( 0.560 );
	//std::vector<t_real> autocorr = recorded.full_auto_correlation( Start, End );	
	//write_vector( autocorr );
	//short* corrs = Correlation( owBuffer, 512 );
	//peak_pick( corrs, 2, &peak1, &peak2, &peak3 );

	double energy = log(compute_energy( mSamps, (long int)mLength ));
	double LHRatio = 0.0;
	
	if (energy > EnergyThreshold)			
	{
		int drum = bass_drum_detect( mSamps, mLength, LHRatio );
		if (InHighEnergy==false) 
		{
			float t = recorded.SamplesToTime( mSampleIndex );

			printf("Energy: t=%6.3f; %6.3lf: %7.3lf ", t, energy, LHRatio);  
			switch (drum)
			{
			case 2 : printf("Drum=Snare\t" );	break;
			case 1 : printf("Drum=Bass \t"  );   break;
			default : break;
			}	
		}
		prev_Drum    = drum;	
		InHighEnergy = true;
	}
	else 
	{
		if (InHighEnergy)
			printf("Energy Done: %ld;\n", mSampleIndex);
		InHighEnergy = false;
	};
	Energies[EnergiesIndex++] = energy;
}


void  energies_n_peaks( DSPWave& wav )
{
	long int len = wav.get_samples_recorded();

	size_t   FrameCounter  = 0;
	size_t   SampleCounter = 0;	
	long int frames = (len / WINDOW_SIZE);
	short*   ptr = wav.m_data;
	
	// COMPUTE ENERGIES (FOR EACH WINDOW):
	for (int f=0; f<frames; f++)
	{
		SampleCounter = (FrameCounter++ * WINDOW_SIZE);	
		process_window( ptr, WINDOW_SIZE, SampleCounter );
		ptr += WINDOW_SIZE;
	}
	printf("%d Energy computed done.\n", EnergiesIndex );
	// ENERGIES[]  --> peak_indices[]  --> peak_deltas[]  
	
//	print_energy_array		();
	
	//peak_pick_energy	(SmoothEnergies, SmoothLength);
/*	compute_peak_deltas ();
	bin_peak_deltas		();
	determine_beats_per_min();
	
	print_peaks     	();
	print_deltas		();  */
}

void  pitch_detect( DSPWave& wav )
{

}
void  quantize_pitches( DSPWave& wav )
{
	// and assign time stamps	
}

void  find_waveform( DSPWave& RecordedWave, Wave* mSrcWave )
{

}


double  spectral_distortion( DSPWave& wav )
{

}

std::vector<int> m_indices;
void  spectral_segmentation( DSPWave& wav )
{

}

void createGraphs()
{
	// Energies/Beat
	e_graph = create_annotated_graph( "Energy -vs- Time", "Window", "Energy", 
			Energies, EnergiesIndex  );

	// Single Full Correlation:
	c_graph = create_annotated_graph( "Correlation -vs- Shift", "Correlation", "Shift", 
			Energies, EnergiesIndex  );

	// Correlation of Entire Waveform:
	ec_graph = create_annotated_graph( "Correlation -vs- Shift", "Correlation", "Shift", 
			Energies, EnergiesIndex  );

	// Waveform Plot : 
	
	
	AddEnergiesData ( e_graph );
	AddPitchData	( c_graph );	
//	AddWaveform ( wave_graph, wave );		
	
}

/*  */
void AddEnergiesData(AnnotatedGraph* mGraph )
{
	DataSeries  si;
	// RAW ENERGIES
	si.set_name( "Energy_Raw" );
	si.set_data( Energies, EnergiesIndex );	
	mGraph->add_data_series( si );

	// Smoothed ENERGIES
	si.delete_all();
	si.set_name( "Energy_Smoothed" );
	si.set_data( SmoothEnergies, SmoothLength  );	
	//si.set_data( SmoothEnergies, SmoothLength );	
	mGraph->add_data_series( si );
	int smoothed_series = mGraph->find_series_name( "Energy_Smoothed" );
	//printf("Added Smoothed Data Series : %d\n", smoothed_series );
	
	// Annotate Peaks Detected:
	for (int i=0; i<peak_indices.size(); i++)
	{
		mGraph->mark_data( smoothed_series, peak_indices[i], RED, STYLE_X );
	}
	printf("Added Annotations\n");
}

void AddPitchData(AnnotatedGraph* mGraph )
{
	DataSeries  si;
	// RAW ENERGIES
	si.set_name( "Energy_Raw" );
	si.set_data( Energies, EnergiesIndex );	
	mGraph->add_data_series( si );

	// Smoothed ENERGIES
	si.delete_all();
	si.set_name( "Energy_Smoothed" );
	si.set_data( SmoothData.data(), SmoothData.size()  );	
	//si.set_data( SmoothEnergies, SmoothLength );	
	mGraph->add_data_series( si );
	int smoothed_series = mGraph->find_series_name( "Energy_Smoothed" );
	//printf("Added Smoothed Data Series : %d\n", smoothed_series );
	
	// Annotate Peaks Detected:
	for (int i=0; i<peak_indices.size(); i++)
	{
		mGraph->mark_data( smoothed_series, peak_indices[i], RED, STYLE_X );
	}
	printf("Added Annotations\n");
}

void AddWaveform( WaveGraph* mGraph, Wave* mWav )
{

}