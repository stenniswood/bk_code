/*********************************************************************
Product of Beyond Kinetics, Inc
------------------------------------------
|			Atmel						 |
------------------------------------------
			|SPI|
------------------------------------------
|   atiltcam   |<==>|  bkInstant (wifi)  |
------------------------------------------

This code handles IPC Shared memory for graphical display:  visual
Intended for the PiCamScan board on RaspberryPi.

WRITES TO SHARED MEMORY:
	The "abkInstant" establishes the memory segment.
	and the avisual may connect to it when it's run.
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
#include "visual_memory.h"


char* 	visual_shared_memory;
int 	visual_segment_id;
struct  avisual_ipc_memory_map* ipc_memory_avis=NULL;
 
void dump_ipc()
{
	int length = sizeof(struct avisual_ipc_memory_map);
	for (int i=0; i<length; i++)
	{
		if ((i%32)==0)
			printf("\n");
		printf("%2x ", visual_shared_memory[i] );
	}	
}

void vis_save_segment_id(char* mFilename)
{
	FILE* fd = fopen(mFilename, "w");
	//FILE* fd = fopen("avisual_shared_memseg_id.cfg", "w");
	printf("Segment_id=%d\n", visual_segment_id );
	fprintf( fd, "%d", visual_segment_id );
	fclose( fd );
}
int read_segment_id(char* mFilename)
{
	FILE* fd = fopen( mFilename, "r" );
	fscanf( fd, "%d", &visual_segment_id );
	fclose( fd );
	return visual_segment_id;
}

int vis_allocate_memory( )
{
	const int 	shared_segment_size = sizeof(struct avisual_ipc_memory_map);

	/* Allocate a shared memory segment. */
	visual_segment_id = shmget( IPC_KEY, shared_segment_size, IPC_CREAT | 0666 );

	// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	printf ("shared memory segment_id=%d\n", visual_segment_id );
	return visual_segment_id;	
}

int vis_attach_memory()
{
	/* Attach the shared memory segment. */
	visual_shared_memory = (char*) shmat (visual_segment_id, 0, 0);
	ipc_memory_avis			 = (struct avisual_ipc_memory_map*)visual_shared_memory;
	printf ("shared memory attached at address %p\n", visual_shared_memory); 	
}

void vis_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	visual_shared_memory = (char*) shmat (visual_segment_id, (void*) 0x5000000, 0); 
	ipc_memory_avis			 = (struct avisual_ipc_memory_map*)visual_shared_memory;	
	printf ("shared memory reattached at address %p\n", visual_shared_memory); 
}

void vis_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (visual_shared_memory);
}

int vis_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (visual_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	printf ("segment size: %d\n", segment_size);
	return segment_size;
}
void vis_fill_memory()
{
	memset(visual_shared_memory, 0, vis_get_segment_size());
}

void vis_deallocate_memory(int msegment_id)
{
	/* Deallocate the shared memory segment. */ 
	shmctl (msegment_id, IPC_RMID, 0);
}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the abkInstant task,
	the writes will be transfer to avisual
FORMAT:
	INT 		ID
	char[255] 	Buffer
*/
void ipc_write_command_text( char* mSentence )
{
	int length = strlen(mSentence);	
	int MaxAllowedLength = sizeof(ipc_memory_avis->Sentence);	
	if (length>MaxAllowedLength)
		length = MaxAllowedLength;
		
	ipc_memory_avis->SentenceCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", SentenceCounter, length);
	strcpy(ipc_memory_avis->Sentence, mSentence);
	printf("|%s|\n", ipc_memory_avis->Sentence );
	//dump_ipc();
}

void ipc_write_connection_status( char* mStatus )
{
	int length = strlen(mStatus);	
	int MaxAllowedLength = sizeof(ipc_memory_avis->ConnectionStatus);
	if (length > MaxAllowedLength)
		length = MaxAllowedLength;	// do not write into memory of next variable.

	ipc_memory_avis->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( ipc_memory_avis->ConnectionStatus, mStatus);
	printf( "|%s|\n", ipc_memory_avis->ConnectionStatus );
}

/* See udp_transponder for update_client_list()		*/




