#ifndef CAN_ID_h
#define CAN_ID_h

#ifdef  __cplusplus
extern "C" {
#endif

/********************************************************
NAME		:	DC Motor
Description	:	Header file for an extended CAN Id (29 bit)

Product of Beyond Kinetics, Inc.
*********************************************************/

// CAN 2.0B 29 bit Identifier
struct idType
{
	long int	full_id;
	byte		id_array[4];
	// be careful about these not overlapping with above.	
	struct {  
		byte	block;
		word	id;
		byte	instance;		
	} group;
};
#define tID struct idType

tID		create_CAN_eid  ( word mID,    byte mInstance			);
tID		create_CAN_eid_b( byte mBlock, word mID, byte mInstance	);
void 	parse_CAN_eid_b ( tID* id, byte* marray	);
BOOL 	isSystemMsg		( tID mID );

byte	get_block	  ( tID mID 								);	// Extract the Block number
byte	get_instance  ( tID mID									);	// 
word	get_id		  ( tID mID									);	

BOOL  match			( tID m1, tID m2 );
BOOL  block_match	( tID m1, tID m2 );
BOOL  id_match   	( tID m1, tID m2 );
BOOL  instance_match( tID m1, byte m2 );

#ifdef  __cplusplus
}
#endif

#endif
