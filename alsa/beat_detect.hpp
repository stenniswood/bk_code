

#define NUM_FRAMES 500024
extern double Energies[NUM_FRAMES*2];
extern size_t EnergiesIndex;


void 	process_waveform();

double 	compute_energy	( short* mSamps, long int mLength );
void 	peak_pick_energy( );

void 	print_peaks		( );
void 	print_array		( );
void	print_deltas	( );

void 	compute_peak_deltas();
void 	bin_peak_deltas();
float 	determine_beats_per_min();

