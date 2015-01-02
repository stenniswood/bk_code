

extern const char* musical_scale_names[];
extern float musical_scale_frequencies[]; /* 4th Octave, (starting at middle c) */


void sine_wave_1( short* mData, int mPeriodSamples, int mBuffSize, int mDesiredChannel, int mNumChannels );
void sine_wave_left ( short* mData, int mPeriodSamples, int mBuffSize );
void sine_wave_right( short* mData, int mPeriodSamples, int mBuffSize );


void saw_tooth_1( short* mData, int mPeriodSamples, int mBuffSize, int mDesiredChannel, int mNumChannels );
void saw_tooth_left ( short* mData, int mPeriodSamples, int mBuffSize );
void saw_tooth_right( short* mData, int mPeriodSamples, int mBuffSize );


void silence_channel	( short* mData, int mBuffSize, int mChannel );

void square_wave_1		( short* mData, int mPeriodSamples, int mBuffSize, int mDesiredChannel, int mNumChannels );
void square_wave_left	( short* mData, int mPeriodSamples, int mBuffSize );
void square_wave_right	( short* mData, int mPeriodSamples, int mBuffSize );


float get_note_frequency	( int mIndex, float sample_rate );
int   get_period_in_samples	( float Herz, float sample_rate );
int   get_note_period		( int mIndex , int octave, float sample_rate );


//void  square_wave( short* mData, int mChannels, int mPeriodSamples, int mBuffSize );


