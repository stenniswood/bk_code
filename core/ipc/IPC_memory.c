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
#include "bk_system_defs.h"
#include "interrupt.h"
#include "atmel_spi.h"


char* 	shared_memory;
int 	segment_id;
byte	LED_state;
byte	LS_V;
		
		
void save_segment_id()
{
	FILE* fd = fopen("shared_memory_segment_id.cfg");
	char* segment_id = itoa(segment_id);
	fprintf( fd, "%d", segment_id );
	fclose( fd );
}

int attach_memory() 
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

void reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	shared_memory = (char*) shmat (segment_id, (void*) 0x5000000, 0); 
	printf ("shared memory reattached at address %p\n", shared_memory); 
}

void detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (shared_memory);
}

void deallocate_memory()
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
	Buttons = *lowside_byte;	
}

/* 	Leds could come from this module, or from bk_Instant, depending on the application.
	atiltcam	<== =  bk_Instant
	||				
	led drivers   */
void ipc_write_leds()
{
	word* leds_byte = (word*) shared_memory + LOWSIDE_OFFSET;
	LED_state = *leds_byte;	
}


// always support dave on his h speech eh?
// okay let's try that appoach.  isn't that what kathy W is doing.
// uplifting then i get dave's favor more.


// yeah, if lp knew how to handle dave exactly, that would be something i don't know.
// and there probably are levels of things which i don't know yet.

// i never sucessfully did those code reviews.  I never walked away with respect and trust
// from my teammates.  So it seems i better go back redo all the things i worked at before 
//  and this time pass them.  It's the social things i need to learn, not the coding.

// simultaneously this coding stuff is getting very laborious.
// Nothing comes for free.
// I am literally fighting entropy.  Every single aspect of the design needs direct attention.
// You'd think well there's 2 servos just copy one to the next.  But now there's a power issue!
// 

// do i write this because i have no friends.  Yes.
// it's much better to get $50/hr job.  life will be 100% better probably.
// I wish i knew more of what i don't know.  I have no map of what i don't know.

