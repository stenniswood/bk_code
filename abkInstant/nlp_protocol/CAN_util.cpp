#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "bk_system_defs.h"
#include "protocol.h"
#include "devices.h"
#include "CAN_base.h"
#include "CAN_memory.h"
#include "CAN_protocol.hpp"
#include "serverthread.h"



/*****************************************************************
This is server code.  So the tokens come from the other end.
INPUT :		DataPack - 
return: 
	length of the bytes used up (no extra for bumping to next msg).
*****************************************************************/
byte extract_CAN_msg(struct sCAN* Msg, byte* DataPack)
{
	int retval = 0;
	// where is ID & instance?
    //	unpackage_short( mBuffer, 8, Msg->id.group.id );
    Msg->id.group.block	   = DataPack[0];
	Msg->id.group.id	   = (DataPack[0+1]) | (DataPack[0+2]<<8);
	Msg->id.group.instance = DataPack[0+3];			// +4

	Msg->header.DLC = (DataPack[0+4] & 0x0F);
	//printf("DLC=%d\n", Msg->header.DLC); 
	Msg->header.rtr = ((DataPack[0+4] & 0xF0)>>4);	// +5
	for (int i=0; i<Msg->header.DLC; i++)
	  Msg->data[i] = DataPack[i+0+5];
	return Msg->header.DLC + 5;
}

void dump_buffer(BYTE* buffer, int bufSize)
{
	printf( "BuffSize=%d: ", bufSize );
	for (int i=0; i<bufSize; i++)
		printf("%x ", buffer[i] );
	printf("\n");
}

/* return:  number of bytes added to beginning of buffer */
int pack_CAN_msg( struct sCAN* msg, BYTE* buffer, int bufSize )
{
	// NLP "token" 
	strcpy(buffer, "CAN_message");
	// now pack CAN in binary form:
	int len = strlen(buffer);
	BYTE* ptr = buffer + len + 1 /*null terminator*/;

    *ptr = msg->id.group.block;				ptr++;
	*ptr = (msg->id.group.id & 0xFF);		ptr++;	  
	*ptr = (msg->id.group.id & 0xFF00)>>8;	ptr++;	  
	*ptr = msg->id.group.instance; 			ptr++;
	*ptr = (msg->header.DLC) | ((msg->header.rtr)<<4);		ptr++;

	for (int i=0; i<msg->header.DLC; i++, ptr++)
		*ptr = msg->data[i];
	
	int addedSize = len+1 + 5 + msg->header.DLC;
	//printf("addedSize: len=%d +1+5 + DLC=%d = %d\n", len, msg->header.DLC, addedSize );
	
	//dump_buffer( buffer, addedSize );
	
	return addedSize;
}


/* 	Interface between Command Central and CAN
	Process and Transmit CAN messages 
*/
void can_interface()
{
	static int tail    =0;
	static int taillaps=0;
	struct sCAN* tmpCAN = NULL;
	int    bufSize = 255;
	unsigned char  buff[255];
	
	if (CAN_SendingOn)
	{
		//printf("CAN_SendingOn is ON!\n");
		
		// PUMP TRANSMIT MESSAGE QUEUE:
		BOOL available = shm_isRxMessageAvailable( &tail, &taillaps  );
			/* the messages will be pulled off of the Received buffer.
			   and stored in Recieved buffer at the other instant end.  */

		if (available)
		{
			//printf("CAN Msg Available!\n");
			// GET IT!
			tmpCAN = shm_GetNextRxMsg( &tail, &taillaps );
			// PACK IT!
			int dataSize = pack_CAN_msg( tmpCAN, buff, bufSize );
			// SHIP IT!
			SendTelegram( buff, dataSize );
		}
	}
	// the other direction is be done in serverthread Parse_CAN_Statement()!
}

