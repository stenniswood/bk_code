#ifndef CAN_ID_h
#define CAN_ID_h

#include "CAN_base.h"

#ifdef  __cplusplus
extern "C" {
#endif

/********************************************************
NAME		:	DC Motor
Description	:	Header file for an extended CAN Id (29 bit)

Product of Beyond Kinetics, Inc.
*********************************************************/

void 	parse_CAN_id   ( tID* id, byte* marray	);		// standard 11 bit id.

tID 	create_CAN_diag15765_id( word mDestAddress, word mSourceAddress, 
								 byte tos	);		// per ISO15765-4 (29 bit id)
tID		create_CAN_eid  ( word mID,    byte mInstance			);
tID		create_CAN_eid_b( byte mBlock, word mID, byte mInstance	);
void 	parse_CAN_eid_b ( tID* id, byte* marray	);

tID 	convert_to_id ( char* mtext	);

BOOL 	isSystemMsg		( tID mID );

byte	get_block	  ( tID mID 								);	// Extract the Block number
byte	get_instance  ( tID mID									);	// 
word	get_id		  ( tID mID									);	

BOOL  match			( tID m1, tID m2 );
BOOL  block_match	( tID m1, tID m2 );
BOOL  id_match   	( tID m1, tID m2 );
BOOL  instance_match( tID m1, byte m2 );

void print_eid( tID* id );

#ifdef  __cplusplus
}
#endif

#endif
