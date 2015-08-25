/* Geared for the binary protocol 	*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include "bk_system_defs.h"
#include "protocol.h"
#include "CAN_Interface.hpp"
#include "can_txbuff.hpp"


#ifdef  __cplusplus
extern "C" {
#endif

struct sCAN msg3;

// For printing socket buffer:
extern void DumpBuffer( BYTE* mbuff, int mlength);

/*************************************************************
The receiver for Socket to Adrenaline network.
**************************************************************/
BOOL CAN_ListeningOn;	// if true we will be receiving CAN Traffic.
BOOL CAN_SendingOn;		// if true we will be sending CAN Traffic.

/*****************************************************************
Initialize internal data structures for CAN protocol (protocol only, 
not all the CAN driving code):
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Init_CAN_protocol()
{
	CAN_ListeningOn=FALSE;
	CAN_SendingOn  =FALSE;
}

/*****************************************************************
Do the work of the Telegram :
return  TRUE = GPIO Telegram was Handled by this routine
		FALSE= GPIO Telegram not Handled by this routine
*****************************************************************/
void Print_CAN_Acknowledgement(UINT mToken)
{
	switch( mToken )
	{
		case CMD_CAN_START	:	printf(" COMMAND CAN START " );		break;
		case CMD_CAN_MSG	:   printf(" COMMAND CAN MSG   " );		break;
		case REQ_CAN_LISTEN	:	printf(" REQUEST CAN 	   " );		break;	
		default:	break;
	}	
}

/*****************************************************************
This is server code.  So the tokens come from the other end.
INPUT :		DataPack - 
Do the work of the Telegram :
return  TRUE = CAN Telegram was Handled by this routine
		FALSE= CAN Telegram not Handled by this routine
*****************************************************************/
#define o 0
byte extract_msg(struct sCAN* Msg, byte* DataPack)
{
	// where is ID & instance?
    //	unpackage_short( mBuffer, 8, Msg->id.group.id );
    Msg->id.group.block	   = DataPack[o];
	Msg->id.group.id	   = (DataPack[o+1]) | (DataPack[o+2]<<8);
	Msg->id.group.instance = DataPack[o+3];

	Msg->header.DLC = (DataPack[o+4] & 0x0F);
	Msg->header.rtr = ((DataPack[o+4] & 0xF0)>>4);
	for (int i=0; i<Msg->header.DLC; i++)
	  Msg->data[i] = DataPack[i+o+5];
	return Msg->header.DLC;	
}

/*byte pack_can_msg(struct sCAN* Msg, byte* DataPack)
{
	DataPack[0] = (Msg->header.DLC) || ((Msg->header.rtr)<<4);
	for (int i=0; i<Msg->header.DLC; i++)
		DataPack[i+1] = Msg->data[i];
	return Msg->header.DLC;
}*/

					
/* This function duplicates the actual CAN_isr() for the PiCAN board
	However the message comes from the LAN socket connection instead.
	So it is all parsed before this gets called. 
*/
void CAN_rx_isr(struct sCAN* mMsg)
{
	printf("<<<<<<<--------socket CAN Received Interrupt------ \n");
	if (msg_callbacks( mMsg, 0) == FALSE)
		print_message( &rxmessage );
}

void send_wifi_CAN(struct sCAN* mMsg, byte* DataPack)
{
	//pack_can_msg(mMsg, DataPack);
	BOOL retval = AddToSendList( mMsg );
}

/*****************************************************************
This is server code.  So the tokens come from the other end.

Do the work of the Telegram :
return  TRUE = CAN Telegram was Handled by this routine
		FALSE= CAN Telegram not Handled by this routine
*****************************************************************/
BOOL Process_CAN_Telegrams( UINT mToken, unsigned char* mheader, int DataLength, int mconnfd )
{
		//DumpBuffer( mheader, DataLength );
		printf("\nInside Process CAN Telegrams Token=%x\n", mToken);
		switch (mToken)
		{
			case CMD_CAN_START:			// Other side is going to start broadcasting.
				CAN_ListeningOn=TRUE;	// just a warning. If we NACK, then they won't.
										//  we are listening to the other side.
				return TRUE;			
				break;
			case CMD_CAN_MSG:		// 1 CAN message has arrived!
			    DumpBuffer( mheader, DataLength);
				extract_msg  ( &msg3, mheader  );
				//msg3.id         = create_CAN_eid_b(0, 0xFC10, 0 );	
				//print_message( &msg3 );
				CAN_rx_isr   ( &msg3 );	  // Handle any callbacks local to the RPI (ie. graphical display of a button push.)
				// But some messages not intended for the RPI should be rebroadcast.
				// So that the intended receiver gets it.  For instance a motor controller may 
				// need the updated tilt sensor reading (which was on another network).
				// Rebroadcast onto the local CAN network:
				AddToSendList( &msg3, 0 );
				return TRUE;
				break;
			case REQ_CAN_LISTEN:	// The other end wishes to hear our CAN traffic
				CAN_SendingOn=TRUE;
				//Send_all_CAN_traffic();
				//send_acknowledgement();
				return TRUE;
				break;
			default:  break;
		}
}

#ifdef  __cplusplus
}
#endif
