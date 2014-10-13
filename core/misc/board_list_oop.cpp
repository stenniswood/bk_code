#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "bk_system_defs.h"
#include "Container.hpp"
#include "board_list_oop.hpp"
#include "can_id_list.h"
#include "can_txbuff.hpp"
#include "CAN_Interface.hpp"
#include "leds.h"


struct sCAN blMsg;

char* board_id_lookup_table[] = {
"none",
"Button",
"Tilt",
"Analog", 
"BigMotor",
"QuadCopter",
"LCD",
"Power",
"BlueTooth",
"SmartBackplane",
"Developer",
"BigMotorEn",
"Edge"
};

BoardList::BoardList()
{
	populate_valid = TRUE;
}

/* Comparison Function pointer for find routine */
BOOL mappedID_finder( Item* mboard, byte mMappedID )
{
	struct stBoardInfo* board = (struct stBoardInfo*)mboard->getData();
	if ( board->MappedID == mMappedID )
		return TRUE;
	return FALSE;
}
/* Comparison Function pointer for find routine */
BOOL instance_finder( Item* mboard, byte mInstance )
{
	struct stBoardInfo* board = (struct stBoardInfo*)mboard->getData();
	if ( board->Instance == mInstance )
		return TRUE;
	return FALSE;
}

/* Use this to search thru the list of attached boards. 
	Send one of the above Finder functions for comparison of the desired field within the 
	board structure.  Thus use this to Find board Instance number, or the MappedID 
	(for motor board vectorizing)  */
Item* BoardList::FindBoard(byte mValue, BOOL (*finder)(Item* mboard, byte mInstance))
{
	Item* board = getHead();
	while (board)
	{
		if (finder(board, mValue))
			return board;
		board = board->getNext();
	}
}

/* Look up and assign a vector id */
void BoardList::AssignMappedID( byte instance, byte VectorIndex )
{
	Item* board = FindBoard(instance, instance_finder );
	struct stBoardInfo* dptr = (struct stBoardInfo*)board->getData();
	dptr->MappedID = VectorIndex;
}

// MyBoardList.setItem( tmp );  gui control

int BoardList::process_CAN_msg( struct sCAN* mMsg )
{
	Item* item = NULL;
	struct stBoardInfo* board;	
	int result,retval;
	switch(mMsg->id.group.id)
	{
	case ID_BOARD_PRESENCE_BROADCAST : 
		board = AddBoard( mMsg->id.group.instance, mMsg->data[0] );
		print_board( (struct stBoardInfo*)item->getData() );
		populate_valid = FALSE;
		return TRUE;
		break;
	case ID_BOARD_REVISION : 
		item = bl.FindBoard( mMsg->id.group.instance, instance_finder );
		board = (struct stBoardInfo*)item->getData();
		board->HardwareRev 		= mMsg->data[0];
		board->SoftwareRevMajor = mMsg->data[1];
		board->SoftwareRevMinor = mMsg->data[2];
		board->Manufacturer		= (mMsg->data[3]<<8) | (mMsg->data[4]);
		populate_valid = FALSE;
		return TRUE;
		break;
	case ID_BOARD_SERIAL_NUMBER : 
		item = bl.FindBoard( mMsg->id.group.instance, instance_finder );
		board = (struct stBoardInfo*)item->getData();
		board->SerialNumber_lo  =  mMsg->data[3];
		board->SerialNumber_lo |= (mMsg->data[2]<<8);
		board->SerialNumber_hi |= (mMsg->data[1]);
		board->SerialNumber_hi |= (mMsg->data[0]<<8);
		populate_valid = FALSE;
		return TRUE;
		break;
	case ID_BOARD_DESCRIPTION : 
		item  = bl.FindBoard( mMsg->id.group.instance, instance_finder );
		board = (struct stBoardInfo*)item->getData();		
		populate_valid = FALSE;
		return TRUE;
		break;
	default:
		break;
	}
}

struct stBoardInfo* BoardList::AddBoard( byte instance, byte model_index )
{
	static Item* item = NULL;
	item  = new Item();	
	
	// FILL IN INFO:
	struct stBoardInfo* board ;//= new struct stBoardInfo;
	//struct stBoardInfo* board = (struct stBoardInfo*)malloc( 1 * sizeof(struct stBoardInfo));
	board->Instance = instance;
	board->Model    = board_id_lookup_table[model_index];
	board->ModelID  = model_index; 
	
	// Add to Wrapper class:
	item->setData( board );

	// Add to list:
	Add( item );

	//printf("Inside AddBoard!!!!!!!!!!!\n");
	return (struct stBoardInfo*)item->getData();
}

/* For populating user GUI control      */
char* BoardList::getDescription( Item* mItem )
{
	struct stBoardInfo* board = (struct stBoardInfo*)mItem->getData();	
	char* tmp_str = new char[50+strlen(board->Model)];
	//char* tmp_str = (char*)malloc( 50+strlen(board->Model) );
	sprintf(tmp_str, "Board #%d : %d %s  \n", board->Instance, board->ModelID, board->Model );
	//printf(tmp_str);
	return tmp_str;
}

void BoardList::print_board( struct stBoardInfo* mboard )
{
	printf("Board #%d : %d %s  \n", mboard->Instance, mboard->ModelID, mboard->Model );	
}

void BoardList::print_all_boards( )
{
	//printf("Print_all_boards() ");
	//printf("Printing board list...\n");
	//struct stBoardInfo* board = boards_present_head;	
	Item* board = getHead();
	while (board)
	{
		//printf("Printing board %X\n", board);
		print_board( (struct stBoardInfo*)board->getData() );
		board = board->getNext();
	}
}
#ifdef ADRENALINE
void BoardList::PopulateListBox( ListBox* mListBox )
{
	if (populate_valid==FALSE)
	{
		Item* tmp = NULL;
		char* tmp_str = NULL;
		mListBox->ClearItems();
		int   num=getNumItems();
		for (int i=0; i<num; i++)  {
			tmp     = getItem(i);
			tmp_str = getDescription( tmp );
			mListBox->setItem( tmp_str );
		}
	}
}
#endif


BOOL BoardList::compare_instance(struct stBoardInfo *a, struct stBoardInfo *b)
{
	if (a->Instance > b->Instance)
		return TRUE;
	return FALSE;
}
BOOL BoardList::compare_boardtype(struct stBoardInfo *a, struct stBoardInfo *b)
{
	if (a->ModelID > b->ModelID)
		return TRUE;
	else if (a->ModelID == b->ModelID)
	{
		if (a->Instance > b->Instance)
			return TRUE;
	}
	return FALSE;
}

void BoardList::RequestBoardsPresent(byte mResponseTypeRequested)
{
	blMsg.id         = create_CAN_eid_b(0, ID_BOARD_PRESENCE_REQUEST, 0 );
	blMsg.data[0]    = mResponseTypeRequested;
	blMsg.header.DLC = 1;
	blMsg.header.rtr = 0;
	AddToSendList( &blMsg );
	print_message( &blMsg );
	printf("----->Sending RequestBoardsPresent\n");
}

/*
INPUT:	mMode - SYSTEM_LED_MODE_DEVICE, SYSTEM_LED_MODE_PATTERN
				SYSTEM_LED_MODE_STROBE, SYSTEM_LED_MODE_MYINSTANCE
		mPattern - lower nibble is led pattern		
*/
void BoardList::System_leds( byte mDestInstance, byte mMode, byte mPattern )
{
	blMsg.id = create_CAN_eid( ID_SYSTEM_LED_REQUEST, mDestInstance );
	blMsg.data[0] = (mMode & 0x0F);
	blMsg.data[1] = (mPattern & 0x0F);
	if (mMode == SYSTEM_LED_MODE_STROBE)
		 blMsg.data[1] = (mPattern >0);		// On or Off.
	blMsg.header.DLC = 2;
	blMsg.header.rtr = 0;	
	AddToSendList( &blMsg );
}

/*
INPUT:	mMode - SYSTEM_LED_MODE_DEVICE, SYSTEM_LED_MODE_PATTERN
				SYSTEM_LED_MODE_STROBE, SYSTEM_LED_MODE_MYINSTANCE
		mPattern - lower nibble is led pattern		
*/
void BoardList::Send_system_shutdown( )
{
	blMsg.id = create_CAN_eid( ID_SYSTEM_SHUT_DOWN, 0 );
	blMsg.header.DLC = 0;
	blMsg.header.rtr = 0;
	AddToSendList( &blMsg );
}


/*void BoardList::PopulateListBox	( ListBox* mListBox)
{
	char tmp_str[255];

	Item* board = getHead();
	while (board)
	{
		sprintf(tmp_str, "Board #%d : %d %s  \n", mboard->Instance, mboard->ModelID, mboard->Model );	
		MyBoardList.setItem( tmp_str );	
		// also print to terminal:
		//	print_board( (struct stBoardInfo*)board->getData() );
		board = board->getNext();
	}
}*/
	
	
/* preform a bubble sort on the linked list */
//void BoardList::sort_board_list( BOOL (*comparison)(Item *a, Item *b) )
//{
//	list.sort_list( comparison );

/*	struct stBoardInfo *a = NULL;
	struct stBoardInfo *b = NULL; 
	struct stBoardInfo *c = NULL;
	struct stBoardInfo *e = NULL; 
	struct stBoardInfo *tmp = NULL; 

 // the `c' node precedes the `a' and `e' node 
 // pointing up the node to which the comparisons
 // are being made. 
 
 while(e != boards_present_head->Next) {
 c = a = boards_present_head;
 b = a->Next;
  while(a != e) {
   
   if(comparison(a, b)) {
    if(a == boards_present_head) {
     tmp = b->Next;
     b->Next = a;
     a->Next = tmp;
     boards_present_head = b;
     c = b;
    } else {
     tmp = b->Next;
     b->Next = a;
     a->Next = tmp;
     c->Next = b;
     c = b;
    }
   } else {
    c = a;
    a = a->Next;
   }
   b = a->Next;
   if(b == e)
    e = a;
  }
 }  
} */
