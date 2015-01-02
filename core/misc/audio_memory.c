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
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include "bk_system_defs.h"
#include "interrupt.h"
#include "audio_memory.h"


char* 	audio_shared_memory;
int 	audio_segment_id;
struct  audio_ipc_memory_map* ipc_memory_aud=NULL;

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

int aud_allocate_memory( )
{
	const int 	shared_segment_size = sizeof(struct audio_ipc_memory_map);

	/* Allocate a shared memory segment. */
	audio_segment_id = shmget( IPC_AUDIO_KEY, shared_segment_size, IPC_CREAT | 0666 );

	// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	printf ("audio shared memory segment_id=%d\n", audio_segment_id );
	return audio_segment_id;	
}

int aud_attach_memory()
{
	/* Attach the shared memory segment. */
	audio_shared_memory = (char*) shmat (audio_segment_id, 0, 0);
	ipc_memory_aud			 = (struct audio_ipc_memory_map*)audio_shared_memory;
	printf ("shared memory attached at address %p\n", audio_shared_memory); 	
}

void aud_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	audio_shared_memory = (char*) shmat (audio_segment_id, (void*) 0x5000000, 0); 
	ipc_memory_aud			 = (struct audio_ipc_memory_map*)audio_shared_memory;	
	printf ("shared memory reattached at address %p\n", audio_shared_memory); 
}

void aud_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (audio_shared_memory);
}

int aud_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (audio_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	printf ("segment size: %d\n", segment_size);
	return segment_size;
}
void aud_fill_memory()
{
	memset(audio_shared_memory, 0, aud_get_segment_size());
}

void aud_deallocate_memory(int msegment_id)
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
void ipc_write_header( 	struct wav_header* mAudio_header )
{
	int length = sizeof(struct wav_header);

	//printf("%d:Copying %d bytes to shared mem.\n", SentenceCounter, length);
	memcpy((void*)&(ipc_memory_aud->audio_header), mAudio_header, length );
}

void ipc_write_buffer( short* mBuffer, long int mLength )
{
	int MaxAllowedLength = ONE_SECOND*sizeof(sample);
	if (mLength > MaxAllowedLength)
		mLength = MaxAllowedLength;

	ipc_memory_aud->update_counter++;
	ipc_memory_aud->update_samples  = mLength;

	printf("%d: %d bytes \n", ipc_memory_aud->update_counter, mLength );
	//printf("%d:Copying %d bytes to shared mem.\n", SentenceCounter, length);
	memcpy( ipc_memory_aud->audio_data, mBuffer, mLength );
}

void ipc_write_audio_connection_status( char* mStatus )
{
	int length = strlen(mStatus);	
	int MaxAllowedLength = sizeof(ipc_memory_aud->ConnectionStatus);
	if (length > MaxAllowedLength)
		length = MaxAllowedLength;	// do not write into memory of next variable.

	ipc_memory_aud->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( ipc_memory_aud->ConnectionStatus, mStatus);
	printf( "|%s|\n", ipc_memory_aud->ConnectionStatus );
}

/* See udp_transponder for update_client_list()		*/




