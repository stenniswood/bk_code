#ifndef _BOARD_LIST_H_
#define _BOARD_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif 


// A Linked List:
struct stBoardInfo {
	char* Model;		// Ascii model type ie. "Button", "BigMotor", etc.
	byte ModelID;		// index of the board model.  See end of "can_id_list.h"
	byte Instance;		// Board MyInstance number
	char* Name;			// for example "knee"
	byte MappedID;		// for a leg index id.

	char HardwareRev;
	byte SoftwareRevMajor;
	byte SoftwareRevMinor;
	word Manufacturer;
	word SerialNumber_hi;	
	word SerialNumber_lo;
			
	struct stBoardInfo* Next;
	struct stBoardInfo* Prev;	
};

// Function pointers to FindBoard():
BOOL mappedID_finder( struct stBoardInfo* mboard, byte mMappedID );
BOOL instance_finder( struct stBoardInfo* mboard, byte mInstance );
struct stBoardInfo* FindBoard	  ( byte mValue, BOOL (*finder)(struct stBoardInfo* mboard, byte mInstance) );
void 				AssignMappedID( byte instance, byte VectorIndex );

struct stBoardInfo* AddBoard		( byte instance, byte model_index  );
void 				FreeBoardList	(								   );

BOOL compare_instance (struct stBoardInfo *a, struct stBoardInfo *b);
BOOL compare_boardtype(struct stBoardInfo *a, struct stBoardInfo *b);

void 				sort_board_list ( BOOL (*comparison)(struct stBoardInfo *a, struct stBoardInfo *b) );
void 				print_board		( struct stBoardInfo* mboard );
void 				print_all_boards( 							 );
BOOL 				comparison		( byte mA, byte mB			 );

#ifdef __cplusplus
 }
#endif 

#endif
