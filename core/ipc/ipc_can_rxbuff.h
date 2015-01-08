#ifndef _CAN_RXBUFF_H_
#define _CAN_RXBUFF_H_


#ifdef  __cplusplus
extern "C" {
#endif

// These Functions to be used by Server:
BOOL AddToRxList		( struct sCAN* mMsg );



// These Functions to be used by Client (retrieval):
BOOL   		 isRxMessageAvailable( );
struct sCAN* GetNext			 ( );


#ifdef  __cplusplus
}
#endif

#endif
