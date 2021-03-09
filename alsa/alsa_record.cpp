/* 
  A Minimal Capture Program

  This program opens an audio interface for capture, configures it for
  stereo, 16 bit, 44.1kHz, interleaved conventional read/write
  access. Then its reads a chunk of random data from it, and exits. It
  isn't meant to be a real program.

  From on Paul David's tutorial : http://equalarea.com/paul/alsa-audio.html

  Fixes rate and buffer problems

  sudo apt-get install libasound2-dev
  //gcc -o alsa-record-example -lasound alsa-record-example.c && ./alsa-record-example hw:0
  
  g++ -o alsa_record -lasound alsa_record.c wave.o
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "dspwave.hpp"
#include "beat_detect.hpp"



FILE*    fd = 0;

int    				i;
int    				err;
char   				*buffer;
size_t 				buffer_size	  = 0;
snd_pcm_uframes_t  	buffer_frames = 2046;
unsigned int 		rate 		  = 44100;
snd_pcm_t *			capture_handle;
snd_pcm_hw_params_t *hw_params;
snd_pcm_format_t 	format 			= SND_PCM_FORMAT_S16_LE;
static size_t 		loops 			= 0;
bool 				stop_recording  = false;

DSPWave 				recorded( 1, 44100, 3*60*44100*1, NULL );


char alsa_device[80];
char* get_alsa_device()
{
	FILE* fd = fopen("alsa_device.txt", "r");
	size_t num_bytes = fread(alsa_device, 1, 79, fd );
	fclose(fd);
	return alsa_device;
}
	      
int init_hw_record()
{
  char dev_name[] = "plughw:2,0";
	
  if ((err = snd_pcm_open (&capture_handle, dev_name, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    printf ("cannot open audio device %s (%s)\n", 
             dev_name,
             snd_strerror (err));
   return 0;
  }
		   
  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
    printf ( "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
   return 0;
  }
				 
  if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
    printf ("cannot initialize hardware parameter structure (%s)\n",
             snd_strerror(err) );
   return 0;
  }
	
  if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    printf ( "cannot set access type (%s)\n",
             snd_strerror (err));
   return 0;
  }

  if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
    printf ( "cannot set sample format (%s)\n",
             snd_strerror (err));
   return 0;
  }
	
  if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
    printf ( "cannot set sample rate (%s)\n",
             snd_strerror (err));
   return 0;
  }

  if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0) {
    printf ( "cannot set channel count (%s)\n",
             snd_strerror (err));
   return 0;
  }
    /* Set period size to 32 frames. */
	int dir=1;
	snd_pcm_hw_params_set_period_size_near(capture_handle, hw_params, &buffer_frames, &dir);

  if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
    printf ( "cannot set parameters (%s)\n",
             snd_strerror (err));
   return 0;
  }
  
	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(hw_params, &buffer_frames, &dir );
	buffer_size = buffer_frames * 2; /* 2 bytes/sample, 1 channels */
	buffer = (char *)new short[buffer_frames];
	printf("new short[ %ld ]\n", buffer_frames);

  /* We want to loop for 20 seconds */
   unsigned int val = 44100;
   snd_pcm_hw_params_get_period_time(hw_params, &val, &dir);
   loops = 20000000 / val;
   printf("buffer_size = %ld;  loops=%ld\n", buffer_size, loops );

  if ((err = snd_pcm_prepare (capture_handle)) < 0) {
    printf ( "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
   return 0;
  }
  printf( "================== RECORDING =================================\n");
  return 1;
}

void clean_up()
{
  snd_pcm_hw_params_free (hw_params);
  snd_pcm_close (capture_handle);
  delete buffer;
  printf( "======== MEMORY CLEANED ===========\n");	
}


void write_vector( std::vector<t_real> mVec)
{
	char str[256];
	for (int i=0; i<mVec.size(); i++)
	{
		sprintf(str, "%6.3f", mVec[i] );
		size_t len = strlen(str);
		fwrite(str, 1, len, fd);
	}
	fwrite("\n", 1, 2, fd);	
}


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

bool past_end(short* ptr, size_t buff_size )
{
	char* ptr1 = ((char*)ptr+buff_size);
	char* ptr2 = (char*)recorded.m_data + recorded.get_data_length_bytes();
	if (ptr1<ptr2)
		return true;
	else
		return false;
}

void record(  )
{
	size_t BufferSizeSamples = recorded.get_samples_allocated();
	size_t FrameCounter = 0;
	size_t SampleCounter = 0;
	
	short* ptr 		  = recorded.m_data;
	float  time 	  = recorded.SamplesToTime( buffer_frames ); // alsa buffer

	useconds_t usec = ceil(time*1000000);
	printf("Delay time=%6.3f  : %d usecs\n", time, usec );
		printf("smaller Low/High ratio ==> snare drum\n");
		
	EnergiesIndex = 0;
	
	while ( (!stop_recording) && !past_end(ptr, buffer_size) )
	{
		usleep( usec );		
		
		int rc = snd_pcm_readi (capture_handle, buffer, buffer_frames);
		if (rc == -EPIPE) {
			printf( "overrun occurred\n");				/* EPIPE means overrun */
			snd_pcm_prepare(capture_handle);
		} else if (rc < 0) {
			printf( "error from read: %s\n", snd_strerror(rc));					
		} else if (rc != (int)buffer_frames) {
			printf( "short read, read %d frames\n", rc);
		}

		// WINDOW : 	
		for (int w=0; w<4; w++)
		{
			SampleCounter = (FrameCounter++ * 512);
			process_window( (short*)buffer, 512, SampleCounter );
		}

		// RETRIEVE DATA & UPDATE POINTER :
		memcpy( (char*)ptr, buffer, buffer_size );
		ptr += (buffer_frames);	
	}
	printf("======================= FINISHED RECORDING ========================\n" );
	printf("Read %d buffers \n", loops );	
	size_t bytes_rec = (ptr - recorded.m_data) * 2;
	recorded.m_bytes_recorded = BufferSizeSamples*2;	
	printf("Bytes Recorded/Allocated = %ld / \n", bytes_rec, recorded.m_bytes_recorded );

	std::string ofn = "ALSA_Rec.wav";
	recorded.Save( ofn );

	//print_energy_array();
	//process_waveform( );
	
	return ;
}

void* record_thread_func( void* argp )
{
	fd = fopen("Record_Energy.csv", "a+");
	
	int result_rec = init_hw_record();
	if (result_rec==0)
	{
		printf("Cannot open audio HW for recording...\n");
		return NULL;
	}

	record();
	fclose(fd);
	clean_up();

	return NULL;
};
