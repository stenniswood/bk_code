#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "bk_system_defs.h"
#include "board_list.h"


struct stBoardInfo* boards_present_head = NULL;
struct stBoardInfo* boards_present_tail = NULL;

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


BOOL mappedID_finder( struct stBoardInfo* mboard, byte mMappedID )
{
	if ( mboard->MappedID == mMappedID )
		return TRUE;
	return FALSE;
}
BOOL instance_finder( struct stBoardInfo* mboard, byte mInstance )
{
	if ( mboard->Instance == mInstance )
		return TRUE;
	return FALSE;
}
/* Use this to search thru the list of attached boards. 
	Send one of the above Finder functions for comparison of the desired field within the 
	board structure.  Thus use this to Find board Instance number, or the MappedID 
	(for motor board vectorizing)  */
struct stBoardInfo* FindBoard(byte mValue, BOOL (*finder)(struct stBoardInfo* mboard, byte mInstance))
{
	struct stBoardInfo* board = boards_present_head;
	while (board)
	{
		if (finder(board, mValue))
			return board;
		board = board->Next;
	}
}
void AssignMappedID( byte instance, byte VectorIndex )
{
	struct stBoardInfo* board = FindBoard(instance, instance_finder );
	board->MappedID = VectorIndex;		
}

struct stBoardInfo* AddBoard( byte instance, byte model_index  )
{
	struct stBoardInfo* board = malloc( 1 * sizeof(struct stBoardInfo));
	board->Prev = boards_present_tail;	// attach to end of list
	board->Next = NULL;

	//printf("AddBoard: %X %X : %X", 	board, boards_present_head, boards_present_tail);	
	if (boards_present_head == NULL) {
		boards_present_head = board;
	}
	if (boards_present_tail != NULL)
		boards_present_tail->Next = board;		// this one is now end
	boards_present_tail = board;		// this one is now end
	
	// FILL IN INFO:
	board->Instance = instance;
	board->Model    = board_id_lookup_table[model_index];
	board->ModelID  = model_index;
	return board;
}

void FreeBoardList()
{
	struct stBoardInfo* tmp;
	struct stBoardInfo* board = boards_present_head;
	while (board)
	{
		tmp = board->Next;
		free(board);
		board = tmp;
	}
	boards_present_head = NULL;
	boards_present_tail = NULL;
}

void print_board( struct stBoardInfo* mboard )
{
	printf("Board #%d : %d %s  \n", mboard->Instance, mboard->ModelID, mboard->Model );	
}

void print_all_boards( )
{
	//printf("Print_all_boards() ");
	//printf("Printing board list...\n");
	struct stBoardInfo* board = boards_present_head;	
	while (board)
	{
		//printf("Printing board %X\n", board);
		print_board( board );
		board = board->Next;
	}
}

BOOL compare_instance(struct stBoardInfo *a, struct stBoardInfo *b)
{
	if (a->Instance > b->Instance)
		return TRUE;
	return FALSE;
}
BOOL compare_boardtype(struct stBoardInfo *a, struct stBoardInfo *b)
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


/* preform a bubble sort on the linked list */
void sort_board_list( BOOL (*comparison)(struct stBoardInfo *a, struct stBoardInfo *b) )
{
	struct stBoardInfo *a = NULL;
	struct stBoardInfo *b = NULL; 
	struct stBoardInfo *c = NULL;
	struct stBoardInfo *e = NULL; 
	struct stBoardInfo *tmp = NULL; 

 /* 
 // the `c' node precedes the `a' and `e' node 
 // pointing up the node to which the comparisons
 // are being made. 
 */
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
}