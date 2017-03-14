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

#define IPC_KEY_CAN 0x04D2

extern char* 	can_shared_memory;
extern int 		can_segment_id;
extern struct   can_ipc_memory_map* ipc_memory_can; 

#define MAX_CAN_RX_MESSAGES 5000
#define MAX_CAN_TX_MESSAGES 5000

/******************** CAN IPC MEMORY MAP *****************/
struct can_ipc_memory_map
{
	long int 	StatusCounter;
	char	 	ConnectionStatus[64];	// such as "board not present", or "CAN hardware operational",

	char		isReceiving;			// data coming in over tcp/ip.
	int 		RxHeadLap;				// counts each roll over.
	int 	 	RxHead;
	struct sCAN Received[MAX_CAN_RX_MESSAGES];
	byte		RxOverFlow;		// indicator if not receiving quickly enough.
 
	char		isTransmitting;			// is being sent over tcp/ip (not local can card)
	unsigned int 	 TxHeadLap;
	unsigned int 	 TxHead;
	struct sCAN Transmit[MAX_CAN_TX_MESSAGES];
	byte		TxOverFlow;		// indicator if trying to send too quickly.
};
	//byte 	 	RxTail;		 each user should keep his own copy.
/******************** CAN IPC MEMORY MAP *****************/
/*********************************************************/
//void copy_can_msg( struct sCAN* mDest, struct sCAN* mSrc );

// The SERVER calls these to setup the memory.  (ie. abkInstant or amon )
BOOL 	is_CAN_IPC_memory_available();
int  	can_allocate_memory		();
void 	can_deallocate_memory	();

// CLIENT uses these to connect:
int		can_connect_shared_memory(char mAllocate);
void 	init_can_memory		();
void 	can_fill_memory		();
int  	can_attach_memory	();
void 	can_reattach_memory	();
void 	can_detach_memory	();
int		can_get_segment_size();

void 	can_ipc_write_can_connection_status( char* mStatus   );

void 		 sprint_message(char* mBuffer, struct sCAN* msg);
void 		 copy_can_msg ( struct sCAN* mDest, struct sCAN* mSrc );

/***** Rx Buffer Functions (replaces former "can_rxbuff.cpp"):	******/
void 		 ipc_add_can_rx_message  ( struct sCAN* mMsg );
void 		 AddToRxList  			 ( struct sCAN* mMsg );		// wrapper around ipc_add_can_rx_message()
BOOL   		 shm_isRxMessageAvailable( int* mTail, int* mTailLaps  );
struct sCAN* shm_GetNextRxMsg		 ( int* mTail, int* mTailLaps );	// pointer to 1 allocation. overwritten on next call!
void 		 print_rx_position();

/***** Tx Buffer Functions (replaces former "can_txbuff.cpp"):	******/
void 		 shm_add_can_tx_message  ( struct sCAN* mMsg );
BOOL   		 shm_isTxMessageAvailable( unsigned int* mTail, unsigned int* mTailLaps  );
struct sCAN* shm_GetNextTxMsg		 ( unsigned int* mTail, unsigned int* mTailLaps );	// pointer to 1 allocation. overwritten on next call!
void 		 print_tx_position();


// MACROS FOR SETTING IPC FLAGS:
void		set_tcp_receiving_flag_ipc_can  ();
void		clear_tcp_receiving_flag_ipc_can();
BOOL		is_tcp_receiving_flag_ipc_can   ();

void		set_tcp_transmitting_flag_ipc_can  ();
void		clear_tcp_transmitting_flag_ipc_can();
BOOL		is_tcp_transmitting_flag_ipc_can   ();

void 	CAN_save_segment_id(char* mFilename);
int  	CAN_read_segment_id(char* mFilename);

//void 	can_ipc_write_can_message( struct sCAN* mMsg );

#ifdef  __cplusplus
}
#endif
