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
#include <stdlib.h>
#include "bk_system_defs.h"
#include "interrupt.h"
#include "atmel_spi.h"
#include "ipc_mem_bkinstant.h"
#include "shared_mem_extractor.h"


/* Update all the variable for the PiCamScan board */
void update_variables()
{
	ipc_read_servos();
	ipc_write_analogs();
	ipc_read_lowside_outputs();
	ipc_write_buttons();
	ipc_write_leds();
}

void ipc_print_mem()
{
	word* mem = (word*)shared_memory;
	printf("Memory:  ");
	for (int i=0; i<32; i++) {
		printf("%4x ", mem[i] );
		if ((i%16)==0)
			printf("\n");
	}
	printf("\n");
}

void ipc_read_servos()
{
	short* servo_1 = (short*)(shared_memory + SERVO_ANGLE_OFFSET);
	Tilt = servo_1[0] / 100.0;
	Pan  = servo_1[1] / 100.0;
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
	LED_status = *leds_byte;	
}

