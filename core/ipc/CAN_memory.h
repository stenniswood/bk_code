/******************** CAN IPC MEMORY MAP ********************************************
This file serves as a bridge between either 
		a wifi connection to a module with a CAN board.  or
		a PiCAN board applet.
It allows the avisual GUI to function independant of how the hardware is connected.
The User CAN receive and transmit CAN data
Author: Stephen Tenniswood
Date  : Jan 8, 2015
**************************************************************************************/
#ifdef  __cplusplus
extern "C" {
#endif

#include "CAN_base.h"

#define IPC_KEY_CAN 1239

extern char* 	can_shared_memory;
extern int 		can_segment_id;
extern struct   can_ipc_memory_map* ipc_memory_can;

#define MAX_CAN_RX_MESSAGES 100
#define MAX_CAN_TX_MESSAGES 100
/******************** CAN IPC MEMORY MAP *****************/
struct can_ipc_memory_map
{
	long int 	StatusCounter;
	char	 	ConnectionStatus[64];	// such as "board not present", or "CAN hardware operational",
										// or "CAN over tcp/ip"
	byte 	 	RxHead;
	byte 	 	RxTail;
	struct sCAN Received[MAX_CAN_RX_MESSAGES];
	byte		RxOverFlow;		// indicator if not receiving quickly enough.

	byte 	 	TxHead;
	byte 	 	TxTail;
	struct sCAN Transmit[MAX_CAN_TX_MESSAGES];
	byte		TxOverFlow;		// indicator if trying to send too quickly.
};
/******************** CAN IPC MEMORY MAP *****************/
/*********************************************************/
//void copy_can_msg( struct sCAN* mDest, struct sCAN* mSrc );
//void dump_raw_ipc			();
//void dump_ipc				();	// formated as CAN messages & string.

// The SERVER calls these to setup the memory.  (ie. abkInstant or amon )
int  can_allocate_memory	();
void can_deallocate_memory	(int msegment_id);

// CLIENT uses these to connect:
int  can_attach_memory		();
void can_reattach_memory	();
void can_detach_memory		();

int  can_get_segment_size	();
void can_fill_memory		();


void ipc_write_can_connection_status( char* mStatus   );
void ipc_write_can_message( struct sCAN* mMsg );

void ipc_add_can_rx_message( struct sCAN* mMsg );

BOOL   		 _isRxMessageAvailable( );
struct sCAN* GetNext			 ( );



//void can_save_segment_id	(char* mFilename);
//int  read_can_segment_id		(char* mFilename);

#ifdef  __cplusplus
}
#endif
