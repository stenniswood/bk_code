// WaveForm.cpp: implementation of the Wave class.
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <math.h>

#include "wave.hpp"

using namespace std;

/* Thank you God!
	Thank you Google!
	 Thank you Larry Page!
	 no don't thank them so much yet.  this still could be a dead end investment.
	 
*/

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

void Wave::Save(string mFileName)
{
	//CFile::modeCreate | CFile::modeWrite
	FILE* File = fopen(mFileName.c_str(), "a" );
	long int chunkSize = sizeof(WAVEFORMATEX);
	
	size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream); 
	size_t result = 0;
	result = fwrite("RIFF", 4, 1, File); 
		
	DWORD fSize = (get_data_length_bytes()+sizeof(WAVEFORMATEX) );
	result = fwrite((&fSize), 			1, 4, File);	
	result = fwrite("WAVE", 			4, 1, File); 
	result = fwrite("fmt ", 			4, 1, File);
	result = fwrite((&chunkSize), 			1, 4, File);
	result = fwrite((&m_format_tag), 		1, 2, File);
	result = fwrite((&m_number_channels), 	1, 2, File);
	result = fwrite((&m_samples_per_second),1, 4, File);
	long val = get_average_bytes_per_second();
	result = fwrite( &val, 1, 4, File);
	result = fwrite((&m_block_align), 		1, 2, File);			
	result = fwrite((&m_bits_per_sample), 	1, 2, File);			
	WORD cbSize=0;
	result = fwrite((&cbSize), 				1, 2, File);					

	chunkSize = get_data_length_bytes( );
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
	string ErrStr;
	char id[4];
	size_t result;
	// Read ID:
	result = fread( id, 4, 1, mFile );	
	if(strncmp(id,mName,4) != 0)	   
	{  ErrStr = "expected a chunk!";  ErrStr+= mName; throw ErrStr;  }

	result = fread( mSize, 1, 4, mFile );	
}

void  Wave::set_wave_format	( WAVEFORMATEX* mFormat )
{
	m_format_tag		=   mFormat->wFormatTag;
	m_number_channels	=   mFormat->nChannels;
	m_samples_per_second=   mFormat->nSamplesPerSec;
	//disregard :  mFormat->nAvgBytesPerSec;
	m_block_align		=   mFormat->nBlockAlign;
	m_bits_per_sample	=   mFormat->wBitsPerSample;
}

BYTE extension[40];
WAVEFORMATEX fmt;
string ErrStr;
bool Wave::Load(string Filename)
{
	char tmp[20];
	long int chunkSize;
	long int fileSize;
	size_t result;
	
	FILE* inFile = fopen( Filename.c_str(), "r" );	// binary?	
	if(inFile==NULL)
	{  ErrStr = "Could not open file: "+Filename;  throw ErrStr; }
	
	// Read RIFF :
	result = fread( tmp, 4, 1, inFile );	    
	if(strncmp(tmp,"RIFF",4) != 0)	   
	{  ErrStr = "not a wave riff file";  throw ErrStr;  }
	
	result = fread( &fileSize, 1, 4, inFile );
	
	// Read WAVE :
	result = fread( tmp, 4, 1, inFile );	    
	if(strncmp(tmp,"WAVE",4) != 0)	   
	{  ErrStr ="expected a WAVE chunk in file";  throw ErrStr;  }
	
	// "fmt " - might need to seek for the fmt chunk.  If some other chunks come before the "fmt " one.
	result = fread( tmp, 4, 1, inFile );	    
	if(strncmp(tmp,"fmt ",4) != 0)	   
	{  ErrStr = "expected a fmt chunk in wave file";  throw ErrStr;  }
	
	result = fread( &chunkSize, 1, 4, inFile );	// should be 16, 18, or 40
	if (chunkSize<sizeof(WAVEFORMATEX))
	{  ErrStr = "fmt chunk size is too small in wave file";  throw ErrStr;  }
	

	result = fread( &fmt, 1, sizeof(WAVEFORMATEX), inFile );	// should be 16, 18, or 40				
	//printf("size of fmt=%d.  bytes read=%d\n", sizeof(WAVEFORMATEX), result);
	set_wave_format( &fmt );

	WORD cbSize=0;		// extension size
	result = fread( &cbSize, 1, 2, inFile );
	if (cbSize)
	{
	 result = fread( extension, cbSize, 1, inFile  );
	 printf( "fread extension, %d bytes\n", result );
	}
	
		// "fact" chunk (compressed files gives number of samples)		
/*		result = fread( tmp, 4, 1, inFile );	    
		if(strncmp(tmp,"fact ",4) != 0)	   
		{  ErrStr.Format("expected a fact chunk in wave file");  throw ErrStr;  }	
		result = fread( &chunkSize, 1, 4, inFile );	// should be 16, 18, or 40
*/
	// READ "DATA" chunk : 
	while( (strncmp(tmp,"data",4) != 0) && (!feof(inFile)) )     // Find "data" chunk:
	{
		fseek( inFile, -3L, SEEK_CUR );
		result = fread( tmp, 4, 1, inFile );	
	}
	if (feof(inFile))
	{  ErrStr="data chunk not found in wave";  throw ErrStr; }


	result = fread( &chunkSize, 1, 4, inFile );	// data size
	m_buffer_length = m_bytes_recorded = chunkSize;			// chunkSize is Definitely BYTES!
	printf("data size = %d bytes;\n", chunkSize );
	
	long int nsamples = chunkSize/2;
	if (m_data == NULL)
	{	
		/* Why does this require 2x the memory as expected.
		the chunkSize should be in BYTES.  and i allocate in shorts
		so should only require 1/2.  however this causes GPF crashes! */
		char* tmp = (char*) malloc (chunkSize);
		//char* tmp = new char[chunkSize*2];
		m_data    = (short*) tmp;
		int siz   = sizeof(*m_data);
		printf("allocated %d bytes;  required: %d /%d= %d shorts \n", 
				siz, chunkSize, sizeof(short), nsamples );
	}

	result = fread( (void*)m_data, 1, chunkSize, inFile );	// data size
	//for (int j=0; j<20; j++)
	//	printf(" m_data[%d]=%4x\n", j, m_data[j] );

	printf("fread %d data bytes\n", result);
	fclose( inFile );

	//if (NumRead < chunkSize)
	//{  ErrStr.Format("Data missing in file.");  throw ErrStr; };
	return true;
}

// returns array of x,y floats.
float* Wave::get_VG_path_coords( int mChannel, float mPixelHeight ) 
{
	int samples = get_samples_recorded();
	int size = samples * 2;
	float* coords = new float[size];

	int index = 0;
	for (int i=0; i<samples; i++)
	{
		coords[index] = mPixelHeight * GetSample(i, mChannel) / 32767.;
		index += 2;
	}

	index = 1;
	for (int i=0; i<samples; i++)
	{
		coords[index] = i;
		index += 2;
	}	
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

string Wave::get_format_string()
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

	s << "\nChannels\t=" << m_number_channels    << "\n";
	s << "Sample Rate\t="  << m_samples_per_second << "\n";
	s << "BlockAlign\t="   << m_block_align 		 << " bytes\n";
   return s.str();
}

string Wave::GetSampleRateStr()
{  
	std::ostringstream s;
	s << "SamplesPerSecond="  << m_samples_per_second;
	return s.str();
}

string Wave::GetNumSamplesStr()
{
	std::ostringstream s;
	string Tmp;
	float samps_recorded = round(m_bytes_recorded/m_block_align);
	s << m_bytes_recorded << "/" << m_buffer_length << " Bytes recorded. SamplesRecorded="
	  << samps_recorded;
	return s.str();
}

string Wave::GetTotalTimeStr()
{
	std::ostringstream s;
    float samps_recorded = round(m_bytes_recorded/m_block_align);
    float time = ((float)samps_recorded / (float)m_samples_per_second);
	s << time << " Seconds";
	return s.str();
}

long int	Wave::get_samples_recorded( )
{
	float samps_recorded = round(m_bytes_recorded/m_block_align);
	return samps_recorded;
}
long int Wave::get_samples_allocated( )
{
	return round(m_buffer_length/m_block_align);
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

////////////////////////////////////////////////////
// EXTRACT 1 CHANNEL FROM STEREO SIGNAL:
// 16 Bit DATA ONLY!!
WAVEHDR* Wave::extract_channel_data(int mChannel)
{
	static WAVEHDR hdr;

	// GET WORKING VARIABLES:
	int	NumSamps = get_samples_recorded();
	int BytesPerSample = 2;
	hdr.dwBytesRecorded = hdr.dwBufferLength = NumSamps * 2;

	// CREATE NEW WAVE FORM:
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
	fmt.wFormatTag;
	fmt.nChannels		=m_number_channels;
	fmt.nSamplesPerSec	=m_samples_per_second;
	fmt.nAvgBytesPerSec	=get_average_bytes_per_second();
	fmt.nBlockAlign		=m_block_align;
	fmt.wBitsPerSample	=2;
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
}
