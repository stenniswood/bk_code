#ifndef _SYSTEM_MSGS_CALLBACK_H_
#define _SYSTEM_MSGS_CALLBACK_H_



struct stBoardInfo* AddBoard( byte instance, byte model_index  );
void print_board			( struct stBoardInfo* mboard );
void print_all_boards		( );

BOOL callback_board_presence( struct sCAN* mMsg );



#endif
