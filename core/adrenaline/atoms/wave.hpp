/**********************************************************************
Wave - This class loads and saves a waveform.

Not best for streaming audio.
Author:  Steve Tenniswood
***********************************************************************/
#ifndef _WAVEFORM_
#define _WAVEFORM_


#include <vector>
#include <string>
using namespace std;

const unsigned short WAVE_FORMAT_PCM 		= 0x0001;
const unsigned short WAVE_FORMAT_IEEE_FLOAT = 0x0003;
const unsigned short WAVE_FORMAT_ALAW  		= 0x0006; 	// 	8-bit ITU-T G.711 A-law
const unsigned short WAVE_FORMAT_MULAW 		= 0x0007; 	// 	8-bit ITU-T G.711 µ-law
const unsigned short WAVE_FORMAT_EXTENSIBLE = 0xFFFE;

//typedef unsigned char  byte;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

typedef struct {
  WORD  wFormatTag;
  WORD  nChannels;
  DWORD nSamplesPerSec;
  DWORD nAvgBytesPerSec;
  WORD  nBlockAlign;
  WORD  wBitsPerSample;
  //WORD  cbSize;
} WAVEFORMATEX;

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

/* This structure holds a chunk of audio data */
/*typedef struct stWave_data {
  BYTE*              lpData;
  DWORD              dwBufferLength;		// Length, in bytes, of the buffer. 
  DWORD              dwBytesRecorded;		// specifies how much data is in the buffer. 
  DWORD              dwFlags;
  DWORD              dwLoops;
};*/

class Wave  
{
public:
	Wave	(				  );
	Wave	( Wave* mOriginal );
	Wave	( BYTE mChannels, int mSamplesPerSecond, int mBufferSizeSamples, short* mData=NULL );
	virtual ~Wave();

	WAVEHDR* 		extract_channel_data( int mChannel		  );
	void 			ConsolidateList		( 					  );
	void 			AppendData 			( WAVEHDR& mHeader	  );

	void 			Save			( string mFileName		);
	bool			Load			( string inFile			);

	inline long int	get_average_bytes_per_second( ) { return (m_samples_per_second*m_number_channels*2); };
	inline long int	get_data_length_bytes		( ) { return (m_buffer_length); };
	inline long int	get_samples_recorded		( );
	inline long int	get_samples_allocated		( );

	void			set_wave_format	( WAVEFORMATEX* mFormat );
	WAVEFORMATEX*	GetWaveFormat	(				);
	WAVEHDR*		GetWaveHeader	(				);
	bool 			read_chunk		(FILE* mFile, char mName[4], long int* mSize);


	float*			get_VG_path_coords( int mChannel, float mPixelHeight  );	// returns array of x,y floats.
	
	inline short	GetSample		(long int mSampIndex, int mChannel);
	inline short* 	GetSamplePtr	(long int mSampIndex, int mChannel);
    
	inline float 	TimeToSamples(float mSeconds)  {	return m_samples_per_second * mSeconds;			}
	inline float 	SamplesToTime(int mSamples)    {	return  (float)mSamples / (float)m_samples_per_second;	}

	string 			get_format_string	();
	string 			GetSampleRateStr	();
    string 			GetNumSamplesStr	();
	string 			GetTotalTimeStr		();

	WORD			m_format_tag;
	int	  			m_number_channels;
	int 			m_samples_per_second;
	int	  			m_block_align;		// channels * bits_per_channel / 8 
	int  			m_bits_per_sample;	// 

	short*	        m_data;
	long int        m_buffer_length;		// Length, in bytes, of the buffer. 
	long int        m_bytes_recorded;		// specifies how much data is in the buffer. 

	long int		m_samp_index;
};






#endif 

