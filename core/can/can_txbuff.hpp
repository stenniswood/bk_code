#ifndef _CAN_TXBUFF_H_
#define _CAN_TXBUFF_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define MAX_CAN_TXMSG_MEMORY_SIZE 100
#define NO_HISTORY_OP_MODE 		0
#define NORMAL_HISTORY_OP_MODE 	1
#define MAILBOX_HISTORY_OP_MODE 2

extern byte    TxTail;
extern uint8_t CANTxMessageQueued;
extern byte	export_message[MAX_CAN_TXMSG_MEMORY_SIZE];

//BOOL 		 Addtest( struct sCAN* mMsg );
void 		 copy_can_msg 	( struct sCAN* mDest, struct sCAN* mSrc );
BOOL 		 AddToSendList	( struct sCAN* mMsg, byte export_msg =1 );	// c does not support default args (= 1)
BOOL 		 SendNext	  	( 				  	 );
struct sCAN* GetTxMessagePtr( byte mIndex =-1	 );		// default to TxTail no default (= -1)
extern byte  TransmissionInProgress;


#ifdef  __cplusplus
}
#endif

#endif

