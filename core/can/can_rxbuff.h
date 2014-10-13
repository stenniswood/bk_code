#ifndef _CAN_RXBUFF_H_
#define _CAN_RXBUFF_H_


#ifdef  __cplusplus
extern "C" {
#endif


BOOL AddToRxList		( struct sCAN* mMsg );
BOOL RxMessageAvailable	( );
struct sCAN* GetNext	( );


#ifdef  __cplusplus
}
#endif

#endif
