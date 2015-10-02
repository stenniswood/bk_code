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
#include <sys/stat.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "bk_system_defs.h"
#include "interrupt.h"
#include "visual_memory.h"
#include "ipc_mem_bkinstant.h"


long int	SentenceCounter=0;
char*		Sentence;
long int	StatusCounter=0;
char*		Status;


int connect_shared_abkInstant_memory( )
{
	printf("connect_shared_abkInstant_memory()  \n");
	if (vis_allocate_memory( ) == -1)
	{
		printf("Can't find shared memory from abkInstant!\n");
		return 0;
	}
	vis_attach_memory( );
	//printf("Sentence = %s\n", ipc_memory_avis->Sentence );
	//dump_ipc();
	//printf("Sizeof(int) = %d\n", sizeof(int) );
	return 1;
}

char* get_connection_status()
{
	return (ipc_memory_avis->ConnectionStatus);
}
char* get_sentence()
{
	return (ipc_memory_avis->Sentence);
}

int read_status_counter()
{
	return ipc_memory_avis->StatusCounter;
}
int read_sentence_counter()
{
	return ipc_memory_avis->SentenceCounter;
}

BOOL is_new_connection_status()
{
	int latest_count = read_status_counter();
	if (latest_count > StatusCounter)
	{
		printf("Latest_count = %d/%d\n", latest_count, StatusCounter);			
		StatusCounter = latest_count;
		return TRUE;
	}
	return FALSE;
}

BOOL is_new_sentence()
{
	int latest_count = read_sentence_counter();
	if (latest_count > SentenceCounter)
	{
		printf("Latest_count = %d/%d\n", latest_count, SentenceCounter);			
		SentenceCounter = latest_count;
		return TRUE;
	}
	return FALSE;
}

