/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles CAN 2.0B extended identifiers.
BK identifiers are composed of 3 parts:

Each board in the network gets a unique ID known as the instance id.
This is determined upon startup of the network.  The id's are held in 
non-volatile ram until there is a conflict (which would happen by swapping 
a module from another network;  at this time a re-initialization of every
board id is initialized (as in a new network) and the new id's are stored 
into non-volatile memory.

The complete 29 bit Adrenaline ID consists of:	
Bits	Name			Description
-----	----			---------------------------
5 		Block_ID		(for remapping messages)
16 		Message_ID
8		Instance_ID 	because there may be more than 1 button board put into the network!
--------------------------------------------
CAN Extended ID consists of:
11		Identifier
18		Extended ID
--------------------------------------------
So we split it as so:

10..6	Identifier <-->	Block			5 bit 
5..0					Message[15..10] 6 bits
--------------
17..8	ExtendedID		Message[9..0]	10 bits
7..0					Instance[7..0]	8 bits
------------------------------------------------
One further breakdown:
10..6	Identifier <-->	Block			5 bit 
5..3					Message[15..13] 6 bits
2..0					Message[12..10] 3 bits
17..16					Message[9..8] 	2 bits
--------------
15..8	EID8			Message [7..0]	8 bits
7..0	EID0			Instance[7..0]	8 bits
==================================================

// These create a complete ID from portions:
tID		create_CAN_eid( word mIdentifier, byte mInstance )
tID		create_CAN_eid( byte mBlock, word mIdentifier, byte mInstance	)

// These extract portions from a complete ID:
byte   get_block	(  tID mID  )
byte   get_instance	(  tID mID  )	
word   get_id		(  tID mID  )

// These compare portions of the ID
bool   match		( tID m1, tID m2 )
bool   block_match	( tID m1, tID m2 )
bool   id_match		( tID m1, tID m2 )
bool   instance_match( tID m1, tID m2 )

-----------------------------------------------------
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "bk_system_defs.h"
#include "can_eid.h"
#include "can.h"
#include "can_id_list.h"


/*************************************************
create_CAN_eid()
INPUTS:
	Message_ID
	Instance_ID
OUTPUT:
	return - a complete CAN 2.0B id (4 bytes)
**************************************************/
tID		create_CAN_eid( word mIdentifier, byte mInstance )
{
	static tID id;
	byte   id_high     = hi(mIdentifier);
	id.id_array[0] 	   = (0x00) | ((id_high&0xE0)>>5);
	id.id_array[1] 	   = ((id_high & 0x1C)<<3) | (id_high&0x03);
	id.id_array[2] 	   = lo(mIdentifier);	
	id.id_array[3] 	   = mInstance;
/////////////////////////////////////////////	
	parse_CAN_eid_b( &id, id.id_array );
	return id;
}

tID		create_CAN_eid_b( byte mBlock, word mIdentifier, byte mInstance	)
{
	static tID id;
	byte   id_high     = hi(mIdentifier);
	id.id_array[0] 	   = ((mBlock&0x1F)<<3) | ((id_high&0xE0)>>5);
	id.id_array[1] 	   = ((id_high & 0x1C)<<3) | (id_high&0x03);
	id.id_array[2] 	   = lo(mIdentifier);
	id.id_array[3] 	   = mInstance;
    /////////////////////////////////////////////////
	parse_CAN_eid_b( &id, id.id_array );    
	return id;
}

//////////////////////////////////////////////////////	
/*****************
parse_CAN_eid_b()

INPUT 	: marray
OUTPUT	: id
******************/
void parse_CAN_eid_b( tID* id, byte* marray	)
{	
	id->group.block	= ((marray[0] & 0xF8)>>3);

	byte id_high  = marray[1] & 0x03;
	id_high 	|= ((marray[1] & 0xE0)>>3);
	id_high 	|= (((word)(marray[0] & 0x07))<<5);

	id->group.id  = (((word)id_high)<<8);
	id->group.id += marray[2];

	id->group.instance = marray[3];
}

void print_eid( tID* id )
{
	printf("Block=%2x; ID=%2x; Instance=%2x; ", 
		id->group.block, id->group.id, id->group.instance );
}

BOOL isSystemMsg( tID mID )
{
	if ((mID.group.id & ID_SYSTEM_REQUEST_MASK) == ID_SYSTEM_REQUEST_MASK)
		return TRUE;
	return FALSE;
}

/*bool  block_match	( tID m1, tID m2 );
bool  id_match   	( tID m1, tID m2 );
bool  instance_match( tID m1, tID m2 ); */

//===================== ACCESSOR FUNCTIONS: ==============================
byte   get_block(  tID mID  )				
{
	return (mID.group.block);
//	return (mID & 0xFF000000) >> 24;
}
byte   get_instance(  tID mID  )				
{
	return (mID.group.id);
//	return (mID & 0x000000FF);
}
word   get_id(  tID mID  )	
{
	return (mID.group.instance);
//	return (mID & 0x00FFFF00) >> 8;
}

//=================== MATCHING FUNCTIONS: =======================
BOOL   match( tID m1, tID m2 )
{
	return (m1.full_id == m2.full_id);
}

BOOL   block_match( tID m1, tID m2 )
{
	return (m1.group.block == m2.group.block);
//	return (get_block(m1) == get_block(m2));
}
BOOL   id_match( tID m1, tID m2 )
{
	return (m1.group.id == m2.group.id);
}
BOOL   instance_match( tID m1, byte m2 )
{
	return (m1.group.instance == m2);
//	return (get_instance(m1) == get_instance(m2));
}

