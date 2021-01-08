/**********************************************************************
Wave - This class loads and saves a waveform.

Not best for streaming audio.
Author:  Steve Tenniswood
***********************************************************************/
#ifndef _WAVEFORM_
#define _WAVEFORM_


#include <vector>
#include <string>

typedef unsigned long t_index;
typedef double t_real;
typedef long t_signed;


using namespace std;

const unsigned short WAVE_FORMAT_PCM 		= 0x0001;
const unsigned short WAVE_FORMAT_IEEE_FLOAT = 0x0003;
const unsigned short WAVE_FORMAT_ALAW  		= 0x0006; 	// 	8-bit ITU-T G.711 A-law
const unsigned short WAVE_FORMAT_MULAW 		= 0x0007; 	// 	8-bit ITU-T G.711 µ-law
const unsigned short WAVE_FORMAT_EXTENSIBLE = 0xFFFE;

#include "global.h"



typedef struct {
  WORD  wFormatTag;
  WORD  nChannels;
  DWORD nSamplesPerSec;			// sample_rate
  DWORD nAvgBytesPerSec;
  WORD  nBlockAlign;			// Channels * bytes_per_sample 
  WORD  wBitsPerSample;
  WORD  cbSize;					// pad this struct.  we read based on file contents.
  BYTE  reserved[8];			// up to 8 bytes - junk if not in file.
} WAVEFORMATEX;

/* This structure holds a chunk of audio data */
typedef struct wavehdr_tag {
  BYTE*              lpData;
  DWORD              dwBufferLength;
  DWORD              dwBytesRecorded;
  DWORD*	          dwUser;
  DWORD              dwFlags;
  DWORD              dwLoops;
  struct wavehdr_tag  *lpNext;
  DWORD*	          reserved;
} WAVEHDR;


class Wave  
{
public:
	Wave	(				  );
	Wave	( Wave* mOriginal );
	Wave	( BYTE mChannels, int mSamplesPerSecond, int mBufferSizeSamples, short* mData=NULL );
	virtual ~Wave();

	bool			Open				( string& Filename	);	// just opens the file
	virtual bool	ReadHeader			(					);	// reads and separates out the entities.
	virtual bool	Load				( string& Filename	);  // opens, reads header & data
	void 			Save				( string mFileName	);	// 
	
	WAVEHDR* 		extract_channel_data( int mChannel		);	// Extract audio data of one channel from stereo signal
	void 			AppendData 			( WAVEHDR& mHeader	);
	void 			Resize	 			( int mNumSamples	);	// Deletes & re-allocates Waveform Data!

	inline long int	get_bytes_per_second ( ) { return (m_samples_per_second*m_number_channels*2); };
	inline long int	get_data_length_bytes( ) { return (m_buffer_length); };
	long int		get_samples_recorded ( );
	long int		get_samples_allocated( );

	void			set_wave_format	( WAVEFORMATEX* mFormat );
	WAVEFORMATEX*	GetWaveFormat	(				);
	WAVEHDR*		GetWaveHeader	(				);
	bool 			read_chunk		( FILE* mFile, char mName[4], long int* mSize);

	inline short	GetSample		( long int mSampIndex, int mChannel);
	inline short* 	GetSamplePtr	( long int mSampIndex, int mChannel);
    
	inline float 	TimeToSamples	( float mSeconds)  {	return m_samples_per_second * mSeconds;			}
	inline float 	SamplesToTime	( int mSamples)    {	return  (float)mSamples / (float)m_samples_per_second;	}

	void 			Set_Absolute_Position( t_index new_smp );	// moves file pointer with fseek
	Wave&			operator=			 ( Wave& mOriginal );
	
// Remove these from this base class:
	float 			calc_zoom		( int mPixels, float mSamples );	

	// EFFECTS:
	void			amplify         ( float mGain );
	void			pan		        ( float L_alpha, float R_alpha );
	void			apply_fir       ( float* mIR, int length );
	void			convert_to_stereo(  );	// wave must be mono to start with.
	void			convert_to_mono  (  );	// wave must be stereo to start with.	
	void			insert_mono_straight_shot( Wave& mMono, float mInitLeftConst, float mInitRightConst, int mSamplePeriod );
	void 			tremolo( float LeftConst, float RightConst, float mPeriodSeconds );
	

	string 			get_format_string	();
	string 			GetSampleRateStr	();
    string 			GetNumSamplesStr	();
	string 			GetTotalTimeStr		();

	WORD			m_format_tag;
	int	  			m_number_channels;
	int 			m_samples_per_second;
	int	  			m_block_align;			// channels * bits_per_channel / 8 
	int  			m_bits_per_sample;		// 
	t_index 		m_first_byte;			// for seeking past header.

	short*	        m_data;
	long int        m_buffer_length;		// Length, in bytes, of the buffer. 
	long int        m_bytes_recorded;		// specifies how much data is in the buffer. 

	long int		m_samp_index;
	
	FILE* 			m_inFile;
	long int 		fileSize;	
};


class TIMITWave : public Wave 
{
public:
	TIMITWave ();
	~TIMITWave();
	
	char*			Search_In_Header(const string& option	);
	virtual bool	ReadHeader		(						);

private:

};
#endif 

