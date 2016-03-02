/*********************************************************************
Product of Beyond Kinetics, Inc
------------------------------------------
|			Atmel						 |
------------------------------------------
			|SPI|
------------------------------------------
|   atiltcam   |<==>|  bkInstant (wifi)  |
------------------------------------------

This code handles IPC Shared memory for graphical display:  audio
Intended for the PiCamScan board on RaspberryPi.

WRITES TO SHARED MEMORY:
	The "abkInstant" establishes the memory segment.
	and the aaudio may connect to it when it's run.
READS:

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdio.h> 
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "bk_system_defs.h"
#include "interrupt.h"
#include "audio_memory.h"


char* 	audio_shared_memory;
int 	audio_segment_id=0;
struct  audio_ipc_memory_map* ipc_memory_aud=NULL;

#define Debug 0


void dump_audio_ipc()
{
	int length = sizeof(struct audio_ipc_memory_map);
	for (int i=0; i<length; i++)
	{
		if ((i%32)==0)
			printf("\n");
		printf("%2x ", audio_shared_memory[i] );
	}	
}

char* get_audio_text( struct wav_header* Wave_hdr )
{
	static char AudioInfo[512];
	sprintf(AudioInfo, "Audio Header:  AudioFormat=%d,  NumChannels=%d,  sample_rate=%d,  \n\
	block_align=%d, bits_per_sample=%d,  \n\
	extra_param_size(size of data packets)=%d\n", 
			Wave_hdr->audio_format,
			Wave_hdr->num_channels,
			Wave_hdr->sample_rate,		// sjt added long
			Wave_hdr->block_align,
			Wave_hdr->bits_per_sample,
			Wave_hdr->extra_param_size );	
	// used for size of data packets
	return AudioInfo;
}

void print_audio_header( struct wav_header* Wave_hdr )
{
	printf("%s", get_audio_text( Wave_hdr ) );
}

void init_audio_memory()
{
	if (ipc_memory_aud==NULL)	return;

	memset(ipc_memory_aud, 0, sizeof(struct audio_ipc_memory_map) );	
}

int audio_allocate_memory( )
{
	const int 	shared_segment_size = sizeof(struct audio_ipc_memory_map);

	/* Allocate a shared memory segment. */
	audio_segment_id = shmget( IPC_AUDIO_KEY, shared_segment_size, IPC_CREAT | 0666 );
	int errsv = errno;
	if (audio_segment_id==-1) {
		printf("audio_allocate_memory - shmget ERROR: %s \n", strerror(errsv) );
		return 0;
	} else {
		if (Debug) printf ("Audio Allocated %d bytes shm segment_id=%d\n", shared_segment_size, audio_segment_id );
		init_audio_memory();
	}
	if (Debug) printf ("AUDIO shared memory segment_id=%d\n", audio_segment_id );
	return audio_segment_id;	
}

int audio_attach_memory()
{
	/* Attach the shared memory segment. */
	audio_shared_memory = (char*) shmat (audio_segment_id, 0, 0);
	ipc_memory_aud			 = (struct audio_ipc_memory_map*)audio_shared_memory;
	if (Debug) printf ("AUDIO shared memory attached at address %p\n", audio_shared_memory); 	
}

void audio_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	audio_shared_memory = (char*) shmat (audio_segment_id, (void*) 0x5000000, 0); 
	ipc_memory_aud			 = (struct audio_ipc_memory_map*)audio_shared_memory;	
	if (Debug) printf ("AUDIO shared memory reattached at address %p\n", audio_shared_memory); 
}

void audio_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (audio_shared_memory);
}

int audio_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (audio_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	if (Debug) printf ("AUDIO segment size: %d\n", segment_size);
	return segment_size;
}

void audio_deallocate_memory(int msegment_id)
{
	/* Deallocate the shared memory segment. */ 
	shmctl (msegment_id, IPC_RMID, 0);
}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the abkInstant task,
	the writes will be transfer to audio
FORMAT:
	INT 		ID
	char[255] 	Buffer
*/
void audio_ipc_write_header( 	struct wav_header* mAudio_header )
{
	int length = sizeof(struct wav_header);

	//printf("%d:Copying %d bytes to shared mem.\n", SentenceCounter, length);
	memcpy((void*)&(ipc_memory_aud->audio_header), mAudio_header, length );
}

void audio_ipc_write_buffer( short* mBuffer, long int mLength )
{
	int MaxAllowedLength = ONE_SECOND*sizeof(sample);
	if (mLength > MaxAllowedLength)
		mLength = MaxAllowedLength;

	ipc_memory_aud->UpdateCounter++;
	ipc_memory_aud->update_samples  = mLength;

	if (Debug) printf("%d: %d samples \n", ipc_memory_aud->UpdateCounter, mLength );
	//printf("%d:Copying %d bytes to shared mem.\n", SentenceCounter, length);
	memcpy( (char*)ipc_memory_aud->audio_data, (char*)mBuffer, mLength );
}

void audio_ipc_write_audio_connection_status( char* mStatus )
{
	int length = strlen(mStatus);	
	int MaxAllowedLength = sizeof(ipc_memory_aud->ConnectionStatus);
	if (length > MaxAllowedLength)
		length = MaxAllowedLength;	// do not write into memory of next variable.

	ipc_memory_aud->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( ipc_memory_aud->ConnectionStatus, mStatus);
	if (Debug) printf( "|%s|\n", ipc_memory_aud->ConnectionStatus );
}

BOOL audio_is_new_rxbuffer()
{
	if (ipc_memory_aud->UpdateCounter > ipc_memory_aud->AcknowledgedCounter)
		return TRUE;
	return FALSE;
}

void audio_ack_new_rxbuffer()
{
	ipc_memory_aud->AcknowledgedCounter = ipc_memory_aud->UpdateCounter;
}

void audio_wait_for_ack_new_rxbuffer()
{
	while (ipc_memory_aud->AcknowledgedCounter < ipc_memory_aud->UpdateCounter)
	{
	}	
}

void audio_save_segment_id(char* mFilename)
{
	char line[40];
	FILE* fd = fopen(mFilename, "w");
	if (Debug) printf("Saved audio Segment_id=%d\n", audio_segment_id );
	sprintf( line, "%d", audio_segment_id );
	fwrite( line, strlen(line), 1, fd );
	fclose( fd );
}

int audio_read_segment_id(char* mFilename)
{
	char tline[40];
	FILE* fd = fopen( mFilename, "r" );
	if (fd==NULL)  {
		printf("audio_read_segment_id - File ERROR: %s %s\n", strerror(errno), mFilename );	
		return -1;
	}

	int result = fread( tline, 1, 20, fd);		//	fscanf( fd, "%d", &can_segment_id );
	tline[result] = 0;
	int tmp_audio_segment_id = atol( tline );
	fclose( fd );
	if (Debug) printf("audio_segment_id= %d \n", tmp_audio_segment_id );	
	return tmp_audio_segment_id;
}


#if (PLATFORM==Mac)
static char segment_id_filename[] = "/Users/stephentenniswood/code/bk_code/abkInstant/audio_shared_memseg_id.cfg";
#elif (PLATFORM==RPI)
static char segment_id_filename[] = "/home/pi/bk_code/abkInstant/audio_shared_memseg_id.cfg";
#elif (PLATFORM==linux_desktop)
static char segment_id_filename[] = "/home/steve/bk_code/abkInstant/audio_shared_memseg_id.cfg";
#endif


BOOL is_audio_ipc_memory_available()
{
	struct shmid_ds buf;			// shm data descriptor.

	if (Debug) printf("Checking for audio IPC memory... ");
	// First see if the memory is already allocated:
	audio_segment_id = audio_read_segment_id( segment_id_filename );
	int retval = shmctl(audio_segment_id, IPC_STAT, &buf);
	if (retval==-1) {
		if (Debug) printf("Error: %s\n", strerror(errno) );
		return FALSE;
	}
	if (Debug) printf( " Found segment, size=%d and %d attachments.\n", buf.shm_segsz, buf.shm_nattch );
	
	if ((buf.shm_segsz > 0)			// segment size > 0
	    && (buf.shm_nattch >= 1))	// number of attachments.
		return TRUE;
	
	return FALSE;
}

int audio_connect_shared_memory(char mAllocate)
{
	BOOL available = is_audio_ipc_memory_available();
	if ((!available) && (mAllocate))  {
		if (Debug) printf("audio_connect_shared_memory() allocating.\n");
		int result = audio_allocate_memory();
		if (!result) 
		{	printf("ERROR: can_connect_shared_memory() cannot allocate.\n");
			return 0;
		}
		audio_attach_memory();
		init_audio_memory  ();
		audio_save_segment_id(segment_id_filename);
		if ((ipc_memory_aud == NULL) || (ipc_memory_aud==(struct audio_ipc_memory_map*)-1))
			return 0;
	} else  {
		audio_attach_memory();		
		if ((ipc_memory_aud == NULL) || (ipc_memory_aud==(struct audio_ipc_memory_map*)-1))
			return 0;
	}
	return 1;
}

/* See udp_transponder for update_client_list()		*/

