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
#include <string.h>
#include <stdlib.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include "bk_system_defs.h"
#include "interrupt.h"
#include "picamscan_memory.h"
//#include "atmel_spi.h"


char* 	picam_shared_memory;
int 	picam_segment_id;
struct  picam_ipc_memory_map* ipc_memory_picam;



byte	pi_LED_state;
int 	LS_Values;
word 	Buttons;
		
		
void picam_save_segment_id()
{
	FILE* fd = fopen("picamscan_shared_memory_segment_id.cfg", "rw");
	//char* segment_id = itoa(segment_id);	
	fprintf( fd, "%d", picam_segment_id );
	fclose( fd );
}

int picam_allocate_memory( )
{
	const int 	shared_segment_size = sizeof(struct picam_ipc_memory_map);

	/* Allocate a shared memory segment. */
	picam_segment_id = shmget( IPC_KEY_PICAM, shared_segment_size, IPC_CREAT | 0666 );

	// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	printf ("PICAM shared memory segment_id=%d\n", picam_segment_id );
	return picam_segment_id;
}


int picam_attach_memory() 
{
	struct 		shmid_ds shmbuffer;
	int 		segment_size;
	const int 	shared_segment_size = 0x6400;

	/* Allocate a shared memory segment. */
	picam_segment_id = shmget (IPC_PRIVATE, shared_segment_size,
					 	 IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

	/* Attach the shared memory segment. */
	picam_shared_memory = (char*) shmat(picam_segment_id, 0, 0);
	printf ("shared memory attached at address %p\n", picam_shared_memory); 
	
	/* Determine the segment’s size. */
	shmctl (picam_segment_id, IPC_STAT, &shmbuffer);
	segment_size = shmbuffer.shm_segsz;
	printf ("segment size: %d\n", segment_size);
}

void picam_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	picam_shared_memory = (char*) shmat (picam_segment_id, (void*) 0x5000000, 0); 
	printf ("shared memory reattached at address %p\n", picam_shared_memory); 
}

void picam_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (picam_shared_memory);
}

void picam_deallocate_memory()
{
	/* Deallocate the shared memory segment. */ 
	shmctl (picam_segment_id, IPC_RMID, 0);
}

int picam_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (picam_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	//printf ("segment size: %d\n", segment_size);
	return segment_size;
}

void picam_fill_memory()
{
	memset( picam_shared_memory, 0, picam_get_segment_size() );
}


/* Update all the variable for the PiCamScan board */
void update_variables()
{
	//ipc_write_analogs();
	ipc_read_lowside_outputs();
	ipc_write_buttons();
	ipc_write_leds();
}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the atiltcam task,
	the writes will be transfer to the cell phone.
	the reads  will be transfering from the cell phone.
*/
void ipc_write_analogs( word* Analog_Values )
{
	ipc_memory_picam->Analog[0] = Analog_Values[0];
	ipc_memory_picam->Analog[1] = Analog_Values[1];
	ipc_memory_picam->Analog[2] = Analog_Values[2];
	ipc_memory_picam->Analog[3] = Analog_Values[3];
	
	ipc_memory_picam->Analog[4] = Analog_Values[4];
	ipc_memory_picam->Analog[5] = Analog_Values[5];
	ipc_memory_picam->Analog[6] = Analog_Values[6];
	ipc_memory_picam->Analog[7] = Analog_Values[7];				
}

void ipc_read_lowside_outputs()
{
	LS_Values = ipc_memory_picam->Lowside_driver1;
}

void ipc_write_buttons()
{
	Buttons = ipc_memory_picam->buttons;
}

/* 	Leds could come from this module, or from bk_Instant, depending on the application.
	atiltcam	<== =  bk_Instant
	||				
	led drivers   */
void ipc_write_leds()
{
	pi_LED_state = ipc_memory_picam->leds_byte;
}


