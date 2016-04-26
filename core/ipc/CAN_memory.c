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
#define _XOPEN_SOURCE
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

#include "CAN_memory.h"

 
int 	can_segment_id;
char* 	can_shared_memory;							// unstructured
struct  can_ipc_memory_map* ipc_memory_can=NULL;	// structured memory.





#define Debug 0

static void print_message(struct sCAN* msg)
{	
	printf(" ID=%2X:%4X:%d; ", msg->id.group.block, 
			 msg->id.group.id, msg->id.group.instance );
	//printf("  %s\t", getID_Text( msg->id.group.id ) );
	printf("DLC=%d; ", msg->header.DLC );
	printf("D[]=");
	int i;
	for (i=0; i<8; i++)
	{
		if (i >= msg->header.DLC)
			printf("   ");
		else
			printf("%2x ", msg->data[i]);
	}
	if (msg->header.rtr==1)	
		printf("\tRTR\n");
	else
		printf("\tDF\n");
}
 
void sprint_message(char* mBuffer, struct sCAN* msg)
{	
	char buffer[80];
	sprintf(mBuffer, " ID=%2X:%4X:%d; ", msg->id.group.block, 
			 msg->id.group.id, msg->id.group.instance );
	//printf("  %s\t", getID_Text( msg->id.group.id ) );
	sprintf(buffer, "DLC=%d; ", msg->header.DLC );		strcat (mBuffer, buffer);
	sprintf(buffer, "D[]=");					strcat (mBuffer, buffer);
	int i;
	for (i=0; i<8; i++)
	{
		if (i >= msg->header.DLC) {	
			sprintf(buffer, "   ");						
			strcat (mBuffer, buffer); 
		}
		else
		{	sprintf(buffer, "%2x ", msg->data[i]);		strcat (mBuffer, buffer); }
	}
	if (msg->header.rtr==1)	
	{	sprintf(buffer, "\tRTR\n");				strcat (mBuffer, buffer);	}
	else
	{	sprintf(buffer, "\tDF\n");				strcat (mBuffer, buffer);	}
}


void dump_CAN_ipc()
{
	printf("StatusCounter=%d\n", 	  	ipc_memory_can->StatusCounter);
	printf("Connection Status: %s\n", 	ipc_memory_can->ConnectionStatus );
	
	printf("RxHead=%d\n", 				ipc_memory_can->RxHead );
	printf("RxHead Laps=%d\n", 			ipc_memory_can->RxHeadLap );
	
	printf("TxHead=%d\n", 				ipc_memory_can->TxHead );
	printf("TxHead Laps=%d\n", 			ipc_memory_can->TxHeadLap );
		
//	for (int i=ipc_memory_can->TxHead; i<ipc_memory_can->TxTail; i++)			
//		print_message( &ipc_memory_can->Transmit[i] );	
}

void dump_CAN_raw_ipc()
{
	int length = sizeof(struct can_ipc_memory_map);
	for (int i=0; i<length; i++)
	{
		if ((i%32)==0)
			printf("\n");
		printf("%2x ", can_shared_memory[i] );
	}	
}

void init_can_memory()
{
	if (ipc_memory_can==NULL)	return;
	
	ipc_memory_can->StatusCounter=0;
	strcpy (ipc_memory_can->ConnectionStatus, "no connection");	

	ipc_memory_can->isReceiving=FALSE;		// data coming in over tcp/ip.
		
	// such as "board not present", or "CAN hardware operational",
											// or "CAN over tcp/ip"
	ipc_memory_can->RxHead=0;				
	ipc_memory_can->RxHeadLap=0;			// counts each roll over.
	ipc_memory_can->RxOverFlow=FALSE;		// indicator if not receiving quickly enough.

	ipc_memory_can->isTransmitting=FALSE;	// is being sent over tcp/ip (not local can card)
	ipc_memory_can->TxHead=0;
	ipc_memory_can->TxHeadLap=0;
	ipc_memory_can->TxOverFlow=FALSE;		// indicator if trying to send too quickly.	
}

/* return:  0=> error;  greater than 0 segment_ic */
int can_allocate_memory( )
{
	const int 	shared_segment_size = sizeof(struct can_ipc_memory_map);
	if (Debug) printf ("CAN shm seg size=%d\n", shared_segment_size );
	
	/* Allocate a shared memory segment. */
	can_segment_id = shmget( IPC_KEY_CAN, shared_segment_size, IPC_CREAT | 0666 );
	int errsv = errno;
	if (can_segment_id==-1) {
		if (Debug) printf("can_allocate_memory - shmget ERROR: %s \n", strerror(errsv) );
		return 0;
	} else {
		if (Debug) printf ("CAN Allocated %d bytes shm segment_id=%d\n", shared_segment_size, can_segment_id );
		init_can_memory(); 
	}
		// IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	return can_segment_id;	
}

/* return:  1 => success */
int can_attach_memory()
{
	/* Attach the shared memory segment. */
	can_shared_memory = (char*)shmat( can_segment_id, 0, 0 );
	if (can_shared_memory==(char*)-1) {
		if (Debug) printf("can_attach_memory - ERROR: %s \n", strerror(errno) );
		return 0;
	} else 
		if (Debug) printf ("CAN shm attached at address %p\n", can_shared_memory); 		
	ipc_memory_can	  = (struct can_ipc_memory_map*)can_shared_memory;
	return 1;
}

void can_reattach_memory()
{
	/* Reattach the shared memory segment, at a different address. */ 
	can_shared_memory = (char*) shmat (can_segment_id, (void*) 0x5000000, 0); 
	if (can_shared_memory==(char*)-1) {
		printf("can_attach_memory - ERROR: %s \n", strerror(errno) );
	} else 
		if (Debug) printf ("CAN shm reattached at address %p\n", can_shared_memory); 	
	ipc_memory_can = (struct can_ipc_memory_map*)can_shared_memory;	
}

void can_detach_memory()
{
	/* Detach the shared memory segment. */
	shmdt (can_shared_memory);
	
}

int can_get_segment_size()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (can_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	if (Debug) printf ("CAN segment size: %d\n", segment_size);
	return segment_size;
}
void can_fill_memory()
{
	memset(can_shared_memory, 0, can_get_segment_size());
}

void can_deallocate_memory()
{
	/* Deallocate the shared memory segment. */ 
	shmctl (can_segment_id, IPC_RMID, 0);
}

/* WRITE IMPLIES TO SHARED MEMORY.  And since we are the abkInstant task,
	the writes will be transfer to avisual
FORMAT:
	INT 		ID
	char[255] 	Buffer
*/
void can_ipc_write_can_connection_status( char* mStatus )
{
	int length = strlen(mStatus);	
	int MaxAllowedLength = sizeof(ipc_memory_can->ConnectionStatus);
	if (length > MaxAllowedLength)
		length = MaxAllowedLength;	// do not write into memory of next variable.
	ipc_memory_can->ConnectionStatus[length] = 0;  // terminator.	
	ipc_memory_can->StatusCounter++;

	//printf("%d:Copying %d bytes to shared mem.\n", StatusCounter, length );
	strcpy( ipc_memory_can->ConnectionStatus, mStatus);
	if (Debug) printf( "|%s|\n", ipc_memory_can->ConnectionStatus );
}

void copy_can_msg( struct sCAN* mDest, struct sCAN* mSrc )
{
	memcpy( (void*)mDest, (void*)mSrc, sizeof(struct sCAN) );
}

void ipc_add_can_rx_message( struct sCAN* mMsg )
{	
	copy_can_msg( &(ipc_memory_can->Received[ipc_memory_can->RxHead++]), mMsg );
	
	if (ipc_memory_can->RxHead > MAX_CAN_RX_MESSAGES) {
		ipc_memory_can->RxHead = 0;
		ipc_memory_can->RxHeadLap++;
	}
}

void AddToRxList( struct sCAN* mMsg )
{
	ipc_add_can_rx_message( mMsg );
}
void print_rx_position()
{
	if (ipc_memory_can)
		if (Debug) printf("Added to CAN IPC memory: %d, lap=%d\n", ipc_memory_can->RxHead, ipc_memory_can->RxHeadLap );
}

BOOL  shm_isRxMessageAvailable( int* mTail, int* mTailLaps )
{
	if ((ipc_memory_can==NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
		return FALSE;

	long int tail_count = 				 *mTailLaps * MAX_CAN_RX_MESSAGES + *mTail;	
	long int head_count = ipc_memory_can->RxHeadLap * MAX_CAN_RX_MESSAGES + ipc_memory_can->RxHead;	
	long int msgs_qued  = head_count - tail_count;
	
	
	if (msgs_qued >= MAX_CAN_RX_MESSAGES)		// overrun 
	{					
		//printf("RxHeadLap=%d; RxHead=%d\n", ipc_memory_can->RxHeadLap, ipc_memory_can->RxHead);	
							// scoot tail up keeping as many msgs as possible.
		//printf("head_count=%d;  tail_count=%d; msgs=%d\n", head_count, tail_count, msgs_qued);
		ipc_memory_can->RxOverFlow  = TRUE;
		*mTailLaps = ipc_memory_can->RxHeadLap - 1;
		tail_count = head_count - (MAX_CAN_RX_MESSAGES-10);	// allow room for 10 more, since the Tail is slower than the head.
		*mTail = (tail_count - *mTailLaps * MAX_CAN_RX_MESSAGES);		
		if (Debug) printf("OverFlow; mTail=%d;  mTailLaps=%d; \n", *mTail, *mTailLaps );
	}

	return (head_count > tail_count);
}

struct sCAN* shm_GetNextRxMsg( int* mTail, int* mTailLaps )
{
	static struct sCAN tmp;	

	if ((ipc_memory_can==NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
		return NULL;
	
	copy_can_msg( &tmp, &(ipc_memory_can->Received[*mTail]) );
	(*mTail)++;
	if (*mTail > MAX_CAN_RX_MESSAGES) {
		*mTail = 0;
		(*mTailLaps)++;
	}
	
	if (ipc_memory_can->RxHead == *mTail)
		ipc_memory_can->RxOverFlow = 0;	// clear it since no more.

	return &tmp;
}

/************************ TRANSMIT TX FUNCTIONS *********************************/
void shm_add_can_tx_message( struct sCAN* mMsg )
{
	if ((ipc_memory_can==NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
	{
		printf("Error: Cannot queue message, CAN ipc memory not available!!\n");
		return FALSE;
	}
	printf("TxHead=%d\n", ipc_memory_can->TxHead );
	if (ipc_memory_can->TxHead > MAX_CAN_TX_MESSAGES) {
		ipc_memory_can->TxHead = 0;
		ipc_memory_can->TxHeadLap++;
	}
	copy_can_msg( &(ipc_memory_can->Transmit[ipc_memory_can->TxHead++]), mMsg );	
}
BOOL shm_isTxMessageAvailable( int* mTail, int* mTailLaps  )
{
	if ((ipc_memory_can==NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
		return FALSE;

	long int tail_count = 				 *mTailLaps * MAX_CAN_TX_MESSAGES + *mTail;	
	long int head_count = ipc_memory_can->TxHeadLap * MAX_CAN_TX_MESSAGES + ipc_memory_can->TxHead;	
	long int msgs_qued  = head_count - tail_count;	
	
	if (msgs_qued >= MAX_CAN_TX_MESSAGES)		// overrun 
	{					
		//printf("RxHeadLap=%d; RxHead=%d\n", ipc_memory_can->RxHeadLap, ipc_memory_can->RxHead);	
							// scoot tail up keeping as many msgs as possible.
		//printf("head_count=%d;  tail_count=%d; msgs=%d\n", head_count, tail_count, msgs_qued);
		ipc_memory_can->TxOverFlow  = TRUE;
		*mTailLaps = ipc_memory_can->TxHeadLap - 1;
		tail_count = head_count - (MAX_CAN_TX_MESSAGES-10);	// allow room for 10 more, since the Tail is slower than the head.
		*mTail = (tail_count - *mTailLaps * MAX_CAN_TX_MESSAGES);		
		if (Debug) printf("TxOverFlow; mTail=%d;  mTailLaps=%d; \n", *mTail, *mTailLaps );
	}

	return (head_count > tail_count);
}
struct sCAN* shm_GetNextTxMsg( int* mTail, int* mTailLaps )	// pointer to 1 allocation. overwritten on next call!
{
	if ((ipc_memory_can==NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
		return NULL;

	static struct sCAN tmp;		
	copy_can_msg( &tmp, &(ipc_memory_can->Transmit[*mTail]) );
	(*mTail)++;
	if (*mTail > MAX_CAN_RX_MESSAGES) {
		*mTail = 0;
		(*mTailLaps)++;
	}
	if (ipc_memory_can->TxHead == *mTail)
		ipc_memory_can->TxOverFlow = 0;	// clear it since no more.
	return &tmp;
}
void print_tx_position()
{
	if (ipc_memory_can)
		if (Debug) printf("Added to CAN IPC memory: %d, lap=%d\n", ipc_memory_can->TxHead, ipc_memory_can->TxHeadLap );
}
/************************ END TRANSMIT TX FUNCTIONS *********************************/

void CAN_save_segment_id(char* mFilename)
{
	char line[40];
	//FILE* fd = fopen(mFilename, "w");
	FILE* fd = fopen(mFilename, "w");
	if (Debug) printf("Saved Segment_id=%d\n", can_segment_id );
	sprintf( line, "%d", can_segment_id );
	fwrite( line, strlen(line), 1, fd );
	fclose( fd );
}

int CAN_read_segment_id(char* mFilename)
{
	char tline[40];
	FILE* fd = fopen( mFilename, "r" );
	if (fd==NULL)  {
		printf("CAN_read_segment_id - File ERROR: %s %s\n", strerror(errno), mFilename );	
		return -1;
	}

	int result = fread( tline, 1, 20, fd);		//	fscanf( fd, "%d", &can_segment_id );
	tline[result] = 0;
	int tmp_can_segment_id = atol( tline );
	fclose( fd );
	if (Debug) printf("can_segment_id= %d \n", tmp_can_segment_id );	
	return tmp_can_segment_id;
}


#if (PLATFORM==Mac)
char segment_id_filename[] = "/Users/stephentenniswood/code/bk_code/amonitor/acan_shared_memseg_id.cfg";
#elif (PLATFORM==RPI)
char segment_id_filename[] = "/home/pi/bk_code/amonitor/acan_shared_memseg_id.cfg";
#elif (PLATFORM==linux_desktop)
char segment_id_filename[] = "/home/steve/bk_code/amonitor/acan_shared_memseg_id.cfg";
#endif


BOOL is_CAN_IPC_memory_available()
{
	struct shmid_ds buf;			// shm data descriptor.

	if (Debug) printf("Checking for CAN IPC memory... ");
	// First see if the memory is already allocated:
	can_segment_id = CAN_read_segment_id( segment_id_filename );
	int retval = shmctl(can_segment_id, IPC_STAT, &buf);
	if (retval==-1) {
		if (Debug) printf("Error: %s\n", strerror(errno) );
		return FALSE;
	}
	if (Debug) printf( " Found segment, size=%d and %d attachments.\n", buf.shm_segsz, buf.shm_nattch );
	
	if ((buf.shm_segsz > 0)			// segment size > 0
	    && (buf.shm_nattch >= 1))	// number of attachments.
		return TRUE;
	
	//shmctl(can_segment_id, IPC_RMID, NULL);
	return FALSE;
}

/* The allocating should be done by amon
		Should test for mem segment allocation first.
		Then if requested allocate.
Return : 
	1 => Attached to memory successfully.
	0 => No connection.
*/
int can_connect_shared_memory(char mAllocate)
{
	BOOL available = is_CAN_IPC_memory_available();
	if (available)  {
		can_attach_memory();		
		if ((ipc_memory_can == NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
			return 0;
		return 1;
	} 
	// Not Available:
	if (mAllocate) {
		dprintf("can_connect_shared_memory() allocating.\n");
		// Now Allocate : 
		int result = can_allocate_memory();
		if (!result) 
		{	printf("ERROR: can_connect_shared_memory() cannot allocate.\n");
			return 0;
		}
		can_attach_memory();
		can_fill_memory	 ();
		CAN_save_segment_id(segment_id_filename);
		if ((ipc_memory_can == NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
			return 0;		
	}	
	return 1;
}

//// RECEIVING OVER TCP/IP:
void		set_tcp_receiving_flag_ipc_can()
{
		if ((ipc_memory_can == NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
			return ;
	ipc_memory_can->isReceiving = TRUE;
}
void		clear_tcp_receiving_flag_ipc_can()
{
		if ((ipc_memory_can == NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
			return ;
	ipc_memory_can->isReceiving = FALSE;
}
BOOL		is_tcp_receiving_flag_ipc_can   ()
{
	if ((ipc_memory_can == NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
			return FALSE;
	return ipc_memory_can->isReceiving;
}

//// TRANSMITTING OVER TCP/IP:
void		set_tcp_transmitting_flag_ipc_can()
{
		if ((ipc_memory_can == NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
			return ;
	ipc_memory_can->isTransmitting = TRUE;
}
void		clear_tcp_transmitting_flag_ipc_can()
{
		if ((ipc_memory_can == NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
			return ;
	ipc_memory_can->isTransmitting = FALSE;
}
BOOL		is_tcp_transmitting_flag_ipc_can   ()
{
		if ((ipc_memory_can == NULL) || (ipc_memory_can==(struct can_ipc_memory_map*)-1))
			return FALSE;
	return ipc_memory_can->isTransmitting;	
}


//	if (head_count < tail_count)
//		printf("h/t: %d/%d \n", head_count, tail_count );
	/* Not new way.
	if (ipc_memory_can->RxTail == ipc_memory_can->RxHead)
		ipc_memory_can->RxTail = ipc_memory_can->RxHead = 0; 
	(ipc_memory_can->RxHead > ipc_memory_can->RxTail);		*/	
/*void can_ipc_write_can_message( struct sCAN* mMsg )
{		
	copy_can_msg( &(ipc_memory_can->Transmit[ipc_memory_can->TxHead]), mMsg );

	ipc_memory_can->TxTail++;
	if (ipc_memory_can->TxTail > MAX_CAN_TX_MESSAGES)
		ipc_memory_can->TxTail = 0;
	
	if (ipc_memory_can->TxTail == ipc_memory_can->TxHead)
		ipc_memory_can->TxOverFlow = 1;
}*/
