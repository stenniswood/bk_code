

extern char notes[][4];
extern float scale_freqs[];
extern float higher_freq_scale_ratios[];

	
int convert_freq_to_samples	( float mFreq, int SamplesPerSecond );
int get_note_index			( char* mNoteName );
int get_note_num_period		( int mNoteNum 	  );
int get_note_period			( char* mNoteName );
const char* get_note_name	( int mNoteNum );


short add_value				( short mData, short mNewData );


void init_musical_scale		();
int  resample( short* mInput, short* mOutput, float mVolumeScale, int mLength, float mMultiplier );

