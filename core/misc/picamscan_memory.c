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
#include <stdlib.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include "bk_system_defs.h"
#include "interrupt.h"
#include "picamscan_memory.h"
//#include "atmel_spi.h"


word Analog_Values[NUM_ANALOG_CHANNELS];

char* 	shared_memory;
int 	segment_id;
byte	pi_LED_state;
int 	LS_Values;
word 	Buttons;
		
		
void picam_save_segment_id()
{
	FILE* fd = fopen("picamscan_shared_memory_segment_id.cfg", "rw");
	//char* segment_id = itoa(segment_id);	
	fprintf( fd, "%d", segment_id );
	fclose( fd );
}

int picam_attach_memory() 
{
	struct 		shmid_ds shmbuffer;
	int 		segment_size;
	const int 	shared_segment_size = 0x6400;

	/* Allocate a shared memory segment. */
	segment_id = shmget (IPC_PRIVATE, shared_segment_size,
					 	 IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

	/* Attach the shared memory segment. */
	shared_memory = (char*) shmat (segment_id, 0, 0);
	printf ("shared memory attached at address %p\n", shared_memory); 
	
	/* Determine the segment’s size. */
	shmctl (segment_id, IPC_STAT, &shmbuffer);
	segment_size = shmbuffer.shm_segsz;
	printf ("segment size: %d\n", segment_size);
}

void picam_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	shared_memory = (char*) shmat (segment_id, (void*) 0x5000000, 0); 
	printf ("shared memory reattached at address %p\n", shared_memory); 
}

void picam_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (shared_memory);
}

void picam_deallocate_memory()
{
	/* Deallocate the shared memory segment. */ 
	shmctl (segment_id, IPC_RMID, 0);
}

/* Update all the variable for the PiCamScan board */
void update_variables()
{
	ipc_write_analogs();
	ipc_read_lowside_outputs();
	ipc_write_buttons();
	ipc_write_leds();
}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the atiltcam task,
	the writes will be transfer to the cell phone.
	the reads  will be transfering from the cell phone.
*/
void ipc_write_analogs()
{
	word* analog_mem = (word*) shared_memory + ANALOG_OFFSET;
	*(analog_mem+0) = Analog_Values[0];
	*(analog_mem+1) = Analog_Values[1];
	*(analog_mem+2) = Analog_Values[2];
	*(analog_mem+3) = Analog_Values[3];
				
	*(analog_mem+4) = Analog_Values[4];
	*(analog_mem+5) = Analog_Values[5];
	*(analog_mem+6) = Analog_Values[6];
	*(analog_mem+7) = Analog_Values[7];	
}

void ipc_read_lowside_outputs()
{
	word* lowside_byte = (word*) shared_memory + LOWSIDE_OFFSET;
	LS_Values = *lowside_byte;	
}

void ipc_write_buttons()
{
	word* button_byte = (word*) shared_memory + LOWSIDE_OFFSET;
	Buttons = *button_byte;	
}

/* 	Leds could come from this module, or from bk_Instant, depending on the application.
	atiltcam	<== =  bk_Instant
	||				
	led drivers   */
void ipc_write_leds()
{
	word* leds_byte = (word*) shared_memory + LOWSIDE_OFFSET;
	pi_LED_state = *leds_byte;	
}


