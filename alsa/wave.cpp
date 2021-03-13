// WaveForm.cpp: implementation of the Wave class.
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <math.h>
#include <stdlib.h>
#include <assert.h> 
 
#include "wave.hpp"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/* Construct an empty object.  With no space allocated */
Wave::Wave()
{
	m_format_tag = WAVE_FORMAT_PCM;
	m_buffer_length = 0;
	m_data = NULL;
	m_bytes_recorded = 0;			// specifies how much data is in the buffer. 

	m_bits_per_sample 	 = 16;		// always!
	m_number_channels 	 = 1;
	m_samples_per_second = 11200;
	m_block_align		 = (m_bits_per_sample*m_number_channels)>>3; // channels * bits_per_channel / 8 
}

/* Construct an empty object.  With space allocated for audio */
Wave::Wave( BYTE mChannels, int mSamplesPerSecond, int mBufferSizeSamples, short* mData )
{
	m_format_tag = WAVE_FORMAT_PCM;
	m_buffer_length = mChannels*mBufferSizeSamples;		// Length, in bytes, of the buffer. 
	if (mData==NULL)
		m_data = new short[m_buffer_length];
	else
		m_data = mData;
	m_bytes_recorded = 0;			// specifies how much data is in the buffer. 	 

	m_bits_per_sample 	 = 16;		// always!
	m_number_channels 	 = mChannels;
	m_samples_per_second = mSamplesPerSecond;
	m_block_align		 = (m_bits_per_sample*m_number_channels)>>3; // channels * bits_per_channel / 8 
}

////////////////////////////////////////////////////////////////
// Now will allocate and copy the WaveHeader!!
// This has been changed.  Becarefull if any caller is trying to
// change the header in one CWaveform and expects is to be reflected 
// in the other.  Now will not.
Wave::Wave( Wave* mOriginal )
{
	m_format_tag = WAVE_FORMAT_PCM;
	m_buffer_length 	= mOriginal->m_buffer_length;
	if (mOriginal->m_data)
	{
		m_data 	= new short[m_buffer_length];
		memcpy( m_data, mOriginal->m_data, m_buffer_length*2 );
	}
	m_bytes_recorded 	= mOriginal->m_bytes_recorded;			// specifies how much data is in the buffer. 

	m_bits_per_sample 	 = mOriginal->m_bits_per_sample;		// always!
	m_number_channels 	 = mOriginal->m_number_channels;
	m_samples_per_second = mOriginal->m_samples_per_second;
	m_block_align		 = mOriginal->m_block_align; 			// channels * bits_per_channel / 8 
}

/////////////////////////////////////////////////////
//  Will allocate if mlpData is NULL.
/////////////////////////////////////////////////////

Wave::~Wave()
{
   if (m_data)
	   delete m_data;
}

float sum_of_weights(  float* mIR, int length )
{
	float sum = 0;
	for (int i=0; i<length; i++)
	{
		sum += mIR[i];
	}
	return sum;
}
void Wave::apply_fir( float* mIR, int length)
{
	// Resize : 
	int num_samples   = ceil((float)m_bytes_recorded/(float)m_number_channels);
	int final_samples = num_samples + length * 2;
	m_buffer_length += length * 2;
	m_bytes_recorded += length * 2;
	
	short *tmp        = new short[final_samples]();
	printf("apply_fir:  1\n");
	float normalizer = sum_of_weights(  mIR, length );
	
	int s=0;
	int ss=0;
	for (s=0; s<length; s++ )
	{
		if ((mIR[s])>0) {
			printf("s=%d %3.3f\n", s, mIR[s]);
			int idex  =    (s * 1.); //(float)m_block_align/(float)m_number_channels);
		
			for (ss=0; ss<num_samples; ss+=1 )
			{
				float scaler = (mIR[s] / normalizer);
				tmp[idex+ss] += (int)(scaler * m_data[ss]);		
			}
		}
	}
	printf("apply_fir: s=%d 2;  ss=%d\n", s, ss );
	
	delete m_data;
	m_data = tmp;
}

void Wave::convert_to_mono(  )
{
	int nSrcSamps = m_bytes_recorded/m_block_align;
	
	/* Adjust Parameters */
	m_number_channels = 1;
	m_block_align     = m_number_channels * 2;
	m_bytes_recorded  = nSrcSamps*m_block_align;
	m_buffer_length   = m_bytes_recorded;
	
	/* Reallocate and copy data */
	size_t samps_needed = nSrcSamps * m_number_channels;
	short* new_data = new short[samps_needed];
	
	int i = 0;
	for (int s=0; s<samps_needed; s+=2)
	{
		new_data[s] = 0.5 * m_data[i+0] + 0.5 * m_data[i+1];
		i+=2;
	}
	delete m_data;
	m_data = new_data;
}

// wave must be mono to start with.
void Wave::convert_to_stereo(  )
{
	int nSrcSamps = m_bytes_recorded/m_block_align;
	
	/* Adjust Parameters */
	m_number_channels = 2;
	m_block_align     = m_number_channels * 2;
	m_bytes_recorded  = nSrcSamps*m_block_align;
	m_buffer_length   = m_bytes_recorded;
	
	/* Reallocate and copy data */
	size_t samps_needed = nSrcSamps * m_number_channels;
	short* new_data = new short[samps_needed];
	
	int i = 0;
	for (int s=0; s<samps_needed; s+=2)
	{
		new_data[s+0] = m_data[i];
		new_data[s+1] = m_data[i];
		i++;
	}
	delete m_data;
	m_data = new_data;
}

void Wave::amplify( float mGain )
{

	size_t n_samples = m_bytes_recorded / (2*m_number_channels);
	for (int i=0; i<n_samples; i+=m_number_channels)
	{
		m_data[i+0] *= mGain;
		if (m_number_channels>1)  m_data[i+1] *= mGain;
	}		
}

void Wave::pan( float L_alpha, float R_alpha )
{	
	// ASSERT STEREO:
	assert(m_number_channels==2); 
	
	size_t n_samples = m_bytes_recorded / 2*2;  // Bytes / 4 
	for (int i=0; i<n_samples; i+=2)
	{
		m_data[i+0] *= L_alpha;
		m_data[i+1] *= R_alpha;
	}	
}

/*
Input:  mInitLeftConst	1.0 [all left];   0.0
		mInitRightConst 0.0 {no right};	  1.0 
*/
void Wave::insert_mono_straight_shot( Wave& mMono, float mInitLeftConst, float mInitRightConst, int mSamplePeriod )
{
	printf("Linear motion coefs: ba=%3.3f %3.3f\n", mInitLeftConst, mInitRightConst );

	int nSrcSamps = mMono.m_bytes_recorded/mMono.m_block_align;

	//float alpha = 1.0;
	float increment = (mInitLeftConst - mInitRightConst) / (float)mSamplePeriod;
	float L_alpha = mInitLeftConst ;
	float R_alpha = mInitRightConst;
	
	for (int i=0; i<nSrcSamps; i+= mMono.m_block_align)
	{
		L_alpha += increment;
		R_alpha += increment;		
		
		m_data[i+0] = m_data[i+0] * L_alpha;
		m_data[i+1] = m_data[i+1] * R_alpha;
		
	}
	printf("Done tremolo\n");
}

void Wave::tremolo( float LeftConst, float RightConst, float mPeriodSeconds )
{
	size_t samplePeriod = ceil(TimeToSamples( mPeriodSeconds ));	
	printf("Tremolo coefs: bl=%5.3f %5.3f T=%d samples\n", LeftConst, RightConst, samplePeriod );
	
	int NumSamples = m_bytes_recorded / m_block_align;
	
	int samps = samplePeriod;
	float L_alpha = 1.0; 
	float R_alpha = 0.0;
	for (int i=0; i<NumSamples; i+=2)
	{		
		L_alpha = LeftConst  * sin ( (float)i / (float)samplePeriod ) + LeftConst;
		R_alpha = RightConst * cos ( (float)i / (float)samplePeriod ) + RightConst;
		
		if (L_alpha<0.0) L_alpha = 0.0;
		if (R_alpha<0.0) R_alpha = 0.0;
		
		//printf("%3.3f %3.3f;  ", L_alpha, R_alpha );
		//if ((i%32)==0) printf("\n");
		//if ( i > (samps/2)) 
		//	{ L_alpha = 0.0; R_alpha=1.0; };
		
		m_data[i+0] = m_data[i+0] * L_alpha;
		m_data[i+1] = m_data[i+1] * R_alpha;
		
	}
	printf("Done tremolo\n");
}

void Wave::Save( std::string mFileName)
{
	//CFile::modeCreate | CFile::modeWrite
	printf("Saving wav %s\n", mFileName.c_str() );
	  
	FILE* File = fopen(mFileName.c_str(), "w+" );
	long int chunkSize = 18; //sizeof(WAVEFORMATEX);
	//printf("WAVEFORMATEX chunkSize = %ld\n", chunkSize );
	
	if (File==NULL)
	{
		printf("Cannot create file: ");
		printf("%s \n", mFileName.c_str() );
		perror("Cannot create file!");
		exit(1);
	}	
			//printf("writing RIFF block 0\n");
	//size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream); 
	size_t result = 0;
	char tag[] = "RIFF";
	result = fwrite(tag, 4, 1, File); 
		//printf("Completed writing RIFF block 1\n");
				
	DWORD fSize = (get_data_length_bytes()+sizeof(WAVEFORMATEX) );
	result = fwrite((&fSize), 			1, 4, File);	
	result = fwrite("WAVE", 			4, 1, File); 
	result = fwrite("fmt ", 			4, 1, File);

	result = fwrite((&chunkSize), 			1, 4, File);
	result = fwrite((&m_format_tag), 		1, 2, File);
	result = fwrite((&m_number_channels), 	1, 2, File);
	result = fwrite((&m_samples_per_second),1, 4, File);
	long val = get_bytes_per_second();
	result = fwrite( &val, 					1, 4, File);
	result = fwrite((&m_block_align), 		1, 2, File);			
	result = fwrite((&m_bits_per_sample), 	1, 2, File);			
	WORD cbSize=0;
	result = fwrite((&cbSize), 				1, 2, File);					

	//printf("Completed writing RIFF block\n");
	
	chunkSize = get_data_length_bytes( );
	//("chunkSize=%d\n", chunkSize );
	
	result = fwrite( "data", 				1, 4, File);
	result = fwrite( &chunkSize, 			1, 4, File);	

	if (m_data != NULL)
		result = fwrite( m_data, 			chunkSize, 1, File);
	fclose(File);
}

/*void Wave::Save(CArchive& File)
{
		string temp,ErrStr;
		char tmp[200];
		long int chunkSize = sizeof(WAVEFORMATEX);
			
	    File.Write("RIFF",4);
		DWORD fSize = (m_waveHeader->GetDataLen()+sizeof(WAVEFORMATEX));
		File << fSize;
	    File.Write("WAVE",4);
		File.Write("fmt ",4);		
		File.Write((char *)(&chunkSize),4);
		File.Write((char *)(&m_waveFormat.wFormatTag),2);
		File.Write((char *)(&m_waveFormat.nChannels),2);		
		File.Write((char *)(&m_waveFormat.nSamplesPerSec),4);
		File.Write((char *)(&m_waveFormat.nAvgBytesPerSec),4);
		File.Write((char *)(&m_waveFormat.nBlockAlign),2);
		File.Write((char *)(&m_waveFormat.wBitsPerSample),2);
		File.Write((char *)(&m_waveFormat.cbSize),2);
		
		chunkSize = m_waveHeader->GetDataLen();
		File.Write("data",4);
		File.Write((char *)(&chunkSize),4);

		if (m_waveHeader->lpData != NULL)
		   File.Write(m_waveHeader->lpData,chunkSize);
//		outFile.close();
}*/
bool Wave::read_chunk(FILE* mFile, char mName[4], long int* mSize)
{
	std::string ErrStr;
	char id[4];
	size_t result;
	// Read ID:
	result = fread( id, 4, 1, mFile );	
	if(strncmp(id,mName,4) != 0)	   
	{  ErrStr = "expected a chunk!";  ErrStr+= mName; throw ErrStr;  }

	result = fread( mSize, 1, 4, mFile );	
	return (result>0);
}

void  Wave::set_wave_format	( WAVEFORMATEX* mFormat )
{
	m_format_tag		=   mFormat->wFormatTag;
	m_number_channels	=   mFormat->nChannels;

	//disregard :  mFormat->nAvgBytesPerSec;
	m_bits_per_sample	=   mFormat->wBitsPerSample;
	if (m_bits_per_sample==0)
		m_bits_per_sample = 16;
	if (mFormat->nBlockAlign)	
		m_block_align	= mFormat->nBlockAlign;
	else 
		m_block_align	= m_number_channels*(m_bits_per_sample>>3);	
	m_samples_per_second=   mFormat->nSamplesPerSec;// / m_block_align;
/*	printf("set_wave_format() block_align = %d\n", m_block_align);
	printf("set_wave_format() m_samples_per_second = %d\n", m_samples_per_second);
	printf("set_wave_format() m_number_channels = %d\n", m_number_channels);	
	printf("set_wave_format() m_bits_per_sample = %d\n", m_bits_per_sample); */
}

BYTE extension[40];
WAVEFORMATEX fmt;
std::string ErrStr;
bool Wave::Open( std::string& Filename)
{
	m_inFile = fopen( Filename.c_str(), "r" );	// binary?	
	if(m_inFile==NULL)
	{  
		perror("fopen");
		ErrStr = "Could not open file: "+Filename; 
		printf("Error:%s\n", ErrStr.c_str() );
		throw ErrStr;
		return false;
	}
	//printf("Opened file: %s\n", Filename.c_str() );	
	return true;
}

bool Wave::ReadHeader( )
{
	//printf("Wave::ReadHeader( )\t");
	long int chunkSize=0;
	
	// Read RIFF :
	char tmp[8];
	int result = fread( tmp, 1, 4, m_inFile );			// "RIFF" 
	tmp[4] = 0;

	int comp = strncmp(tmp,"RIFF",4);
	if(comp != 0)
	{ 
		result = fread( &chunkSize, 1, 4, m_inFile );	// should be 16, 18, or 40	
	    printf("Error: not a wave riff file. Chunk:%s: size=%ld \n", tmp, chunkSize ); 
		for (int i=0; i<4; i++)
			printf("%c", tmp[i] );
		return false;
	}

	result = fread( &fileSize, 1, 4, m_inFile );			// 4 bytes integer FILE SIZE
	
	// Read WAVE :
	result = fread( tmp, 1, 4, m_inFile );	    
	if(strncmp(tmp,"WAVE",4) != 0)	   						// "WAVE"
	{
	  ErrStr ="expected a WAVE chunk in file";  
	  printf("Not a WAVE file.  %s\n", tmp);	
	  return false;  
	}
	
	// FORMAT "fmt "   - might need to seek for the fmt chunk.  If some other chunks come before the "fmt " one.
	result = fread( tmp, 1, 4, m_inFile );
	bool cmp = (strncmp(tmp,"fmt ",4) == 0);
	if (!cmp)	   
	{
		ErrStr = "Ignoring chunk:";  
		ErrStr += tmp[0];	ErrStr += tmp[1];	ErrStr += tmp[2];	ErrStr += tmp[3];
		printf("%s.\n", ErrStr.c_str() );
	
		char dummy[2048];	// DISCARD ANY NON FMT CHUNK.
		do {
			result = fread( &chunkSize, 1, 4, m_inFile );	// should be 16, 18, or 40	
			//printf("chunksize=%ld\n", chunkSize );
			
			result = fread( &dummy, 1, chunkSize, m_inFile );	// should be 16, 18, or 40		
			result = fread( tmp, 1, 4, m_inFile );  // READ NEXT TAG.
			cmp = (strncmp(tmp,"fmt ",4) == 0);
		} while  (!cmp);
		//return false;
	} 

	// FMT Chunk...
	result = fread( &chunkSize, 1, 4, m_inFile );	// should be 16, 18, or 40	
	//printf("fmt Chunksize = %ld\n", chunkSize );
	
	if (chunkSize > sizeof(WAVEFORMATEX))
	{  ErrStr = "fmt chunk size is too large in wave file";  throw ErrStr;  }
	//printf( "fmt  chunkSize=%d bytes;  WAVEFORMATEX=%d bytes  \n", chunkSize, sizeof(WAVEFORMATEX) );	
	//pos = ftell(m_inFile);
	//printf("pos=%d\n", pos );

	result = fread( &fmt, 1, chunkSize, m_inFile );	// should be 16, 18, or 40				
	//printf("size of fmt=%d.  bytes read=%d\n", sizeof(WAVEFORMATEX), result);
	set_wave_format( &fmt );
	

	// DATA "data" CHUNK : 		
	result = fread( tmp, 4, 1, m_inFile );
	//printf("DATA Chunk expected, found %s\n", tmp );
	
	//printf("\n%d : tmp=%x %x %x %x \n", pos, tmp[0], tmp[1], tmp[2], tmp[3] );	
	while( (strncmp(tmp,"data",4) != 0) && (!feof(m_inFile)) )     // Find "data" chunk:
	{
		fseek( m_inFile, -3L, SEEK_CUR );
		result = fread( tmp, 4, 1, m_inFile );		
	}
	if (feof(m_inFile))
	{  ErrStr="data chunk not found in wave";  throw ErrStr; }

	result = fread( &chunkSize, 1, 4, m_inFile );			// data size
	m_buffer_length = m_bytes_recorded = chunkSize;		// chunkSize is Definitely BYTES!


	// FACT "fact" chunk (compressed files gives number of samples)		
/*		result = fread( tmp, 4, 1, m_inFile );	    
		if(strncmp(tmp,"fact ",4) != 0)	   
		{  ErrStr.Format("expected a fact chunk in wave file");  throw ErrStr;  }	
		result = fread( &chunkSize, 1, 4, m_inFile );	// should be 16, 18, or 40
*/
	return true;
}

Wave& Wave::operator=(Wave& mOriginal )
{
	m_format_tag		= mOriginal.m_format_tag;
	m_number_channels	= mOriginal.m_number_channels;
	m_samples_per_second= mOriginal.m_samples_per_second;
	m_block_align		= mOriginal.m_block_align;		// channels * bits_per_channel / 8 
	m_bits_per_sample	= mOriginal.m_bits_per_sample;	// 
	m_first_byte		= mOriginal.m_first_byte;			// for seeking past header.


	m_buffer_length	 = mOriginal.m_buffer_length;		// Length, in bytes, of the buffer. 
	m_bytes_recorded = mOriginal.m_bytes_recorded;		// specifies how much data is in the buffer. 
	m_samp_index	 = mOriginal.m_samp_index;
	m_inFile		 = mOriginal.m_inFile;
	fileSize		 = mOriginal.fileSize;
	
	memcpy(m_data, mOriginal.m_data, mOriginal.m_buffer_length);	
	return *this;
}

// Deletes & Re-mallocs Waveform Data - 
// and leaves unitialized data!
void Wave::Resize( int mNumSamples )
{
	int total_samples = mNumSamples*m_number_channels;
	short *tmp        = new short[total_samples]();
	m_bytes_recorded  = m_buffer_length = total_samples*2;	
	delete m_data;
	m_data = tmp;
}

bool Wave::Load( std::string& Filename)
{
	size_t result;
	
	bool ok = Open(Filename);
	if (ok==false) {
		printf("COULD NOT OPEN!\n");
		fclose( m_inFile );
		return ok;
	}
	
	ok = ReadHeader();			// Virtual (Timit or regular .wav)
	if (ok==false) {
		printf("COULD NOT READ HEADER! %s\n", Filename.c_str() );
		fclose( m_inFile );
		return ok;
	}
	
	if (m_data)
		delete m_data;

	printf("Loaded %s %ld bytes; %ld samples; \n", Filename.c_str(), m_buffer_length, get_samples_allocated() );
	m_data = (short*) new short[m_buffer_length/(m_bits_per_sample/8)];
	result = fread( (void*)m_data, 1, m_buffer_length, m_inFile );	// data size
	fclose( m_inFile );	
	
	return true;
}

float Wave::calc_zoom( int mPixels, float mSamples )
{
/* Returns array of x,y floats
	Zoom units are:  pixels / samples
	So Zoom=2.0 means 2 pixels per sample.    (zoomed in)
	So Zoom=0.5 means 1 pixels per 2 samples. (ie every other sample - zoomed out)

	Zoom may range from 
	(width pixels / 2 samples)		maybe cap sooner, ie (20 pixels per 2 samples)
			to 
	(1 pixel / n samples recorded)
*/ 
	float max_samples = get_samples_recorded();
	if ( mSamples > max_samples )
		mSamples = max_samples;
	
	float zoom = mPixels / mSamples;	
	return zoom;
}


///////////////////////////////////////////////////////////////
// PURPOSE:  Will peruse the buffer of waveforms if needed.
//  this is supposed to be faster since it takes out the big while loop.
// 16 bit mode only works into the HdrList.
void Wave::AppendData(WAVEHDR &mHeader)
{
	BYTE* ptr = (BYTE*)m_data;
   long int BytesAvail = m_buffer_length - m_bytes_recorded;
   if ((mHeader.dwBytesRecorded < BytesAvail))
   {
      memcpy (ptr+m_bytes_recorded, mHeader.lpData, mHeader.dwBytesRecorded );
	  m_bytes_recorded += mHeader.dwBytesRecorded;
   }
   // else case need impl. suggest collecting a linked list of waveforms.
   // then adding a member function to consolidate them after the
   // recording is finished.  Otherwise some long waveforms could generate
   // lots of mem cycles deleting and newing new buffers.
}

#include <sstream>

std::string Wave::get_format_string()
{
	std::ostringstream s;
	s << "Wave Format : ";
	switch(m_format_tag) 
	{
		case WAVE_FORMAT_PCM		:  s << "PCM Data";			break;
		case WAVE_FORMAT_IEEE_FLOAT	:  s << "IEEE float";		break;
		case WAVE_FORMAT_ALAW		:  s << "Alaw Data";		break;
		case WAVE_FORMAT_MULAW		:  s << "Mulaw Data";		break;
		case WAVE_FORMAT_EXTENSIBLE	:  s << "extensible Data";	break;
		default : s << "unknown format";  break;
	}
    float samps_recorded = round(m_bytes_recorded/m_block_align);
    float time = ((float)samps_recorded / (float)m_samples_per_second);

	s << "\nChannels\t="    << m_number_channels     << "\n";
	s << "Sample Rate\t="   << m_samples_per_second  << "\n";
	s << "BlockAlign\t="    << m_block_align 		 << " bytes\n";
	s << "Size\t\t="			<< m_buffer_length 		 << " bytes\n";
	s << "Samples\t\t="		<< m_buffer_length/m_block_align << "\n";
	s << "Time   \t\t="		<< time 				 << " seconds ";
   return s.str();
}

std::string Wave::GetSampleRateStr()
{  
	std::ostringstream s;
	s << "SamplesPerSecond="  << m_samples_per_second;
	return s.str();
}

std::string Wave::GetNumSamplesStr()
{
	std::ostringstream s;
	std::string Tmp;
	float samps_recorded = round(m_bytes_recorded/m_block_align);
	s << m_bytes_recorded << "/" << m_buffer_length << " Bytes recorded. SamplesRecorded="
	  << samps_recorded;
	return s.str();
}

std::string Wave::GetTotalTimeStr()
{
	std::ostringstream s;
    float samps_recorded = round(m_bytes_recorded/m_block_align);
    float time = ((float)samps_recorded / (float)m_samples_per_second);
	s << time << " Seconds";
	return s.str();
}

long int	Wave::get_samples_recorded( )
{
	long int samps_recorded;
	try {
		samps_recorded = round(m_bytes_recorded/m_block_align);
	} catch (...)
	{	printf("Divide by zero = m_block_align\n");
	}
	return samps_recorded;
}
long int Wave::get_samples_allocated( )
{
	return round(m_buffer_length/m_block_align);
}
	 

////////////////////////////////////////////////////
// EXTRACT 1 CHANNEL FROM STEREO SIGNAL and place into a newly malloc WAVEHDR.
// 16 Bit DATA ONLY!
// 
WAVEHDR* Wave::extract_channel_data(int mChannel)
{
	static WAVEHDR hdr;

	// GET WORKING VARIABLES:
	int	NumSamps = get_samples_recorded();
	int BytesPerSample = 2;
	hdr.dwBytesRecorded = hdr.dwBufferLength = NumSamps * 2;

	// CREATE NEW WAVE FORM : 
	short*  mNewData = new short[ NumSamps*BytesPerSample ];
	hdr.lpData = (BYTE*)mNewData;

	// PRIME THE LOOP AND EXTRACT:
	short*	pDest= mNewData;
	BYTE*	pSrc = (BYTE*) m_data + mChannel*BytesPerSample;
	for (int s=0; s < NumSamps; s++)
	{
		*pDest = *(short*)pSrc;
		pSrc  += m_block_align;
		pDest += 1;		// pointer of short - so +1 is 1 short later (ie. 2 bytes).
	}
	return &hdr;
}

////////////////////////////////////////////////
//  index	The sample to index into.
//  16 bit mode now works even into the HdrList.
short Wave::GetSample(long int mSampIndex, int mChannel) 
{
	short* ptr =  GetSamplePtr(mSampIndex, mChannel);	
	if (ptr)
		return *ptr;
	throw (1);
}

short* Wave::GetSamplePtr(long int mSampIndex, int mChannel)
{
	int offset = mSampIndex*m_number_channels;		// shorts
	short* ptr = &(m_data[offset]);
	ptr += mChannel;
	if ((ptr-m_data) <= (m_bytes_recorded>>1))
		return ptr;
	return NULL;	
}

/* Build up the structure. */
WAVEFORMATEX*	Wave::GetWaveFormat	(				)
{
	static WAVEFORMATEX fmt;
	fmt.wFormatTag		=0;
	fmt.nChannels		=m_number_channels;
	fmt.nSamplesPerSec	=m_samples_per_second;
	fmt.nAvgBytesPerSec	=get_bytes_per_second();
	fmt.nBlockAlign		=m_block_align;
	fmt.wBitsPerSample	=2;
	return &fmt;
}

/* Build up the structure. */
WAVEHDR*		Wave::GetWaveHeader	(				)
{
	static WAVEHDR hdr;
	hdr.lpData = (BYTE*)m_data;
	hdr.dwBufferLength	=m_buffer_length;
	hdr.dwBytesRecorded	=m_bytes_recorded;
	hdr.dwUser			=NULL;
	hdr.dwFlags			=0;
	hdr.dwLoops			=0;
	hdr.lpNext			=NULL;
	hdr.reserved		=0;	
	return &hdr;
}

void Wave::Set_Absolute_Position( t_index new_smp )
{
	fseek( m_inFile,  new_smp*m_block_align + m_first_byte, SEEK_SET );
}


/**************************************************************/

/**************************************************************/

TIMITWave::TIMITWave()
{

}
TIMITWave::~TIMITWave()
{

}
/*
NIST_1A
   1024
database_id -s5 TIMIT
database_version -s3 1.0
utterance_id -s8 cjf0_sa1
channel_count -i 1
sample_count -i 46797
sample_rate -i 16000
sample_min -i -2191
sample_max -i 2790
sample_n_bytes -i 2
sample_byte_format -s2 01
sample_sig_bits -i 16
end_head
*/ 
char* TIMITWave::Search_In_Header(const std::string& option)
{
	char str[120];
	char* ptr= str;
	fseek( m_inFile,  0, SEEK_SET );

	while (fgets( str, 120, m_inFile ) != NULL)
	{
		// We have 1 full line, extract the first word (up to space)
		ptr = strchr(str, ' ');
		if (ptr) {
			*ptr = 0;
			int compare = strcmp( str, option.c_str() );

			// found the right option :
			if (compare==0) {
				return (strchr((ptr+1), ' ')+1);
			}
		}
	}	

	if(feof(m_inFile))
	printf("Option %s not found in Timit!", option.c_str() );
	return NULL;
}


bool TIMITWave::ReadHeader		(						)
{
	char str[120];
	// Read First line - Must be NIST_xx !
	if (fgets(str, 120, m_inFile) != NULL)
	{
	   int compare = strncmp(str,"NIST", 4);
	   if (compare!=0) {
		  printf(" Not a NIST file: %s\n", str );
		  return false;
		}
	}
	
	// Read next line:
	if (fgets( str, 120, m_inFile ) != NULL)
		m_first_byte = strtol( str, NULL, 10 );

	char* value = Search_In_Header("channel_count");
	m_number_channels = strtol( value, NULL, 10 );
	printf("channels = %d\n", m_number_channels );

	value = Search_In_Header("sample_count");
	m_buffer_length = m_bytes_recorded = 2*strtol( value, NULL, 10 ) * m_number_channels;
	printf("sample_count: %s = %ld\n", value, m_buffer_length/2 );

	value = Search_In_Header("sample_n_bytes");
	m_block_align = m_number_channels * strtol( value, NULL, 10 );
	printf("sample_n_bytes = %d\n", m_block_align );

	value = Search_In_Header("sample_rate");
	m_samples_per_second = strtol( value, NULL, 10 );
	printf("sample_rate = %d\n", m_samples_per_second );


	value = Search_In_Header("sample_byte_format");
	m_format_tag = strtol( value, NULL, 10 );
	//printf("m_format_tag = %d\n", m_format_tag );

	value = Search_In_Header("sample_sig_bits");
	m_bits_per_sample = strtol( value, NULL, 10 );
	//printf("m_bits_per_sample = %d\n", m_bits_per_sample );

	Set_Absolute_Position(0);
	//printf("repositioned to first sample\n" );
	return true;
}

short	Wave::get_max_sample	()
{
	short max_value=0;
	
	for (int i=0; i < get_samples_recorded(); i++)
	{
		if (m_data[i] > max_value)	max_value = m_data[i];
	}
	return max_value;
}

short	Wave::get_min_sample	()
{
	short min_value=0;
	for (int i=0; i < get_samples_recorded(); i++)
	{
		if (m_data[i] < min_value)	min_value = m_data[i];
	}
	return min_value;
}




/*void Wave::ConsolidateList()
{
	HDR_LIST::iterator iter = m_HdrList.begin();

	long int SizeNeeded = m_waveHeader->GetDataLen();

	for ( ; iter != m_HdrList.end(); iter++)
	{   SizeNeeded += (*iter)->dwBytesRecorded;	}		//  Add each piece.

	CWaveHeader* TmpHdrPtr = new CWaveHeader(SizeNeeded+1);	// allocate a new buffer.

	BYTE* TmpSampPtr = (BYTE *)TmpHdrPtr->GetDataPtr();

	memcpy (TmpSampPtr, m_waveHeader->GetDataPtr(), m_waveHeader->dwBytesRecorded);
    TmpSampPtr += m_waveHeader->GetDataLen();

	for (iter = m_HdrList.begin(); iter != m_HdrList.end(); iter++)
	{
	   long int BytesToCopy = (*iter)->dwBytesRecorded;
       memcpy (TmpSampPtr, (*iter)->GetDataPtr(), BytesToCopy);
	   TmpSampPtr += BytesToCopy;	
	}

	TmpHdrPtr->dwBytesRecorded = SizeNeeded;
	delete m_waveHeader;
	m_waveHeader = TmpHdrPtr;

	// delete each fragment now.
	for (iter = m_HdrList.begin(); iter != m_HdrList.end(); iter++)
		delete (*iter);
	m_HdrList.clear();
} */

/*int Wave::get_VG_path_coords( float* &mCoords, int mChannel, float mPixelHeight, 
							float mZoomFactor, float left, float bottom )
{
	// By skipping every other zoom factor sample, the waveform is not interesting at all
	// the aliasing leaves 
	int samples = get_samples_recorded();
	mCoords 	= new float[samples*2];

	// a VGPath is an array of (x,y) pairs : 
	int index  = 1;
	float center = bottom+mPixelHeight/2.;
	float scale = mPixelHeight/(2*32767.);
	
	for (int i = 0; i<samples; i++)
	{
		(mCoords)[index] = center + scale * GetSample(i, mChannel);
		index += 2;		// skip to next y value.
	}

	// Fill in the X coordinates:
	index = 0;
	for (int i=0; i<samples; i++)
	{
		(mCoords)[index] = left + (i*mZoomFactor);
		index += 2;
	}
	return samples;
} */
