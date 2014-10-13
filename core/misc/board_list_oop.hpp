#ifndef _BOARD_LIST_H_
#define _BOARD_LIST_H_

#include "Container.hpp"
#include "can_id_list.h"

#ifdef ADRENALINE
#include "listbox.hpp"
#endif


// A Linked List (wrap this in an Item class)
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
	
//	struct stBoardInfo* Next;
//	struct stBoardInfo* Prev;	
};

BOOL instance_finder( Item* mboard, byte mInstance );
BOOL mappedID_finder( Item* mboard, byte mMappedID );


//Class ClientList;  will be VERY similar.

class BoardList : public Container
{
public:
	BoardList();

	// wrapper of Add() in base class	
	struct stBoardInfo*	AddBoard( byte instance, byte model_index  );	

	// Function pointers to FindBoard():
	Item*	FindBoard( byte mValue, BOOL (*finder)(Item* mboard, byte mInstance) );	
	void	AssignMappedID( byte instance, byte VectorIndex );

	BOOL	compare_instance (struct stBoardInfo *a, struct stBoardInfo *b);
	BOOL	compare_boardtype(struct stBoardInfo *a, struct stBoardInfo *b);
	
	int 	process_CAN_msg	( struct sCAN* mMsg 		 );
	
	char*	getDescription	( Item* mboard 				 );
	void 	print_board		( struct stBoardInfo* mboard );
	void 	print_all_boards( 							 );
	BOOL 	comparison		( byte mA, byte mB			 );

#ifdef ADRENALINE
	void 	PopulateListBox	( ListBox* mListBox);
#endif
	
// CAN MESSAGING:
	void 	RequestBoardsPresent(byte mResponseTypeRequested = REQUEST_TYPE_PRESENCE);
	void 	Send_system_shutdown( );
	void 	System_leds		 	( byte mDestInstance, byte mMode, byte mPattern );
	
private:
	BOOL	populate_valid;
};

extern BoardList bl;

#endif
