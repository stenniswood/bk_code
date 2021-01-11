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
#include "wave.hpp"

  int i;
  int err;
  char *buffer;
  size_t buffer_size=0;
  snd_pcm_uframes_t  buffer_frames = 1023;
  unsigned int rate = 44100;
  snd_pcm_t *capture_handle;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
  size_t loops = 0;
	      
void init_hw_record()
{
  if ((err = snd_pcm_open (&capture_handle, "plughw:2,0", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    printf ("cannot open audio device %s (%s)\n", 
             "plughw:2,0",
             snd_strerror (err));
    exit (1);
  }

  printf( "audio interface opened\n");
		   
  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
    printf ( "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  printf( "hw_params allocated\n");
				 
  if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
    printf ("cannot initialize hardware parameter structure (%s)\n",
             snd_strerror(err) );
    exit (1);
  }

  printf( "hw_params initialized\n");
	
  if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    printf ( "cannot set access type (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  printf( "hw_params access set\n");
	
  if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
    printf ( "cannot set sample format (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  printf( "hw_params format set\n");
	
  if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
    printf ( "cannot set sample rate (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  printf("hw_params rate set\n");

  if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0) {
    printf ( "cannot set channel count (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  printf( "hw_params channels set\n");

    /* Set period size to 32 frames. */
	int dir=1;
	snd_pcm_hw_params_set_period_size_near(capture_handle, hw_params, &buffer_frames, &dir);

  if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
    printf ( "cannot set parameters (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  printf( "hw_params set\n");
  
	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(hw_params, &buffer_frames, &dir );
	buffer_size = buffer_frames * 2; /* 2 bytes/sample, 2 channels */
	buffer = (char *)new short[buffer_frames];
	printf("new short[ %ld ]\n", buffer_frames);
	
  
  /* We want to loop for 5 seconds */
   unsigned int val = 44100;
   snd_pcm_hw_params_get_period_time(hw_params, &val, &dir);
   loops = 5000000 / val;
   printf("buffer_size = %ld;  loops=%ld\n", buffer_size, loops );

	
  if ((err = snd_pcm_prepare (capture_handle)) < 0) {
    printf ( "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  printf( "audio interface prepared\n");
  //buffer = (char*) malloc(128 * snd_pcm_format_width(format) / 8 * 2);

  printf( "buffer allocated\n");
}

void clean_up()
{
  printf( "hw_params freed 0\n");
  snd_pcm_hw_params_free (hw_params);
  printf( "hw_params freed 1\n");

  printf( "audio interface closing\n");
  snd_pcm_close (capture_handle);
  printf( "audio interface closed\n");

//  free(buffer);
  delete buffer;
  printf( "buffer freed\n");
	
}

int main (int argc, char *argv[])
{
	size_t BufferSizeSamples = 3 * 60 * 44100 * 1;		//  about 8MB
	Wave recorded( 1, 44100, BufferSizeSamples, NULL );

	
	init_hw_record();
	short* ptr 		  = recorded.m_data;
	float  time = recorded.SamplesToTime( buffer_size/2 );

	useconds_t usec = ceil(time*1000000);
	printf("Delay time = %6.3f;  d=%d\n", time, usec );
		
	for (i = 0; i < loops; ++i) {
		usleep( usec );		
		int rc = snd_pcm_readi (capture_handle, buffer, buffer_frames);
		if (rc == -EPIPE) {
			/* EPIPE means overrun */
			printf( "overrun occurred\n");
			snd_pcm_prepare(capture_handle);
		} else if (rc < 0) {
			printf( "error from read: %s\n", snd_strerror(rc));					
		} else if (rc != (int)buffer_frames) {
			printf( "short read, read %d frames\n", rc);
		}
			
		printf( "read %d done\n", i);
		memcpy( (char*)ptr, buffer, buffer_size );
		ptr += (buffer_size >> 1);		
	}
	recorded.m_bytes_recorded = (ptr - recorded.m_data) * 2;
	printf("Bytes Recorded = %ld\n", recorded.m_bytes_recorded );
	//std::string fmt_str       = recorded.get_format_string();
	//printf("%s\n", fmt_str.c_str() );
	recorded.m_bytes_recorded = BufferSizeSamples*2;
	std::string ofn = "ALSA_Rec.wav";
	recorded.Save( ofn );

	
    clean_up();
	return 1;
}
