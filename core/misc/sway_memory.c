/*********************************************************************
Product of Beyond Kinetics, Inc
------------------------------------------
|			Atmel						 |
------------------------------------------
			|SPI|
------------------------------------------
|   atiltcam   |<==>|  bkInstant (wifi)  |
------------------------------------------

This code handles IPC Shared memory:  atiltcam
Intended for the PiCamScan board on RaspberryPi.

WRITES TO SHARED MEMORY:
	Analog 	
READS:
	Lowside Outputs	- this app reserves the right to override the outputs based on the application.
	Servo Angles

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdio.h> 
#include <sys/shm.h> 
#include <sys/stat.h>

#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>

#include "bk_system_defs.h"
#include "interrupt.h"
#include "sway_memory.h"
//#include "atmel_spi.h"



char* 	sway_shared_memory;
int 	sway_segment_id;
struct  asegway_ipc_memory_map* ipc_memory_sway=NULL;
		
void sway_save_segment_id()
{
	FILE* fd = fopen("sway_shared_memory_segment_id.cfg", "rw");
	fprintf( fd, "%d", sway_segment_id );
	fclose( fd );
}

int sway_allocate_memory( )
{
	const int 	shared_segment_size = sizeof(struct asegway_ipc_memory_map);

	/* Allocate a shared memory segment. */
	sway_segment_id = shmget( IPC_KEY, shared_segment_size, IPC_CREAT | 0666 );

	// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	printf ("shared memory segment_id=%d\n", sway_segment_id );
	return sway_segment_id;
}

int sway_attach_memory() 
{
	struct 		shmid_ds shmbuffer;
	int 		segment_size;
	const int 	shared_segment_size = 0x6400;

	/* Allocate a shared memory segment. */
	sway_segment_id = shmget (IPC_PRIVATE, shared_segment_size,
					 	 IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

	/* Attach the shared memory segment. */
	sway_shared_memory = (char*) shmat (sway_segment_id, 0, 0);
	ipc_memory_sway = (struct  asegway_ipc_memory_map*)sway_shared_memory;
	printf ("shared memory attached at address %p\n", sway_shared_memory); 
	
	/* Determine the segment’s size. */
	shmctl (sway_segment_id, IPC_STAT, &shmbuffer);
	segment_size = shmbuffer.shm_segsz;
	printf ("segment size: %d\n", segment_size);
}

void sway_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	sway_shared_memory = (char*) shmat (sway_segment_id, (void*) 0x5000000, 0); 
	ipc_memory_sway = (struct  asegway_ipc_memory_map*)sway_shared_memory;	
	printf ("shared memory reattached at address %p\n", sway_shared_memory); 
}

void sway_detach_memory()
{ 
	/* Detach the shared memory segment. */
	shmdt (sway_shared_memory);
	ipc_memory_sway    =NULL;
	sway_shared_memory =NULL;		
}

void sway_deallocate_memory()
{
	/* Deallocate the shared memory segment. */ 
	shmctl (sway_segment_id, IPC_RMID, 0);
}
int sway_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (sway_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	printf ("segment size: %d\n", segment_size);
	return segment_size;
}

void sway_fill_memory()
{
	memset(sway_shared_memory, 0, sway_get_segment_size());
}

/* Update all the variable for the PiCamScan board */
void sway_update_variables()
{

}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the atiltcam task,
	the writes will be transfer to the cell phone.
	the reads  will be transfering from the cell phone.
*/
