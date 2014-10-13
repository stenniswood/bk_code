#ifndef _SYSTEM_MSGS_CALLBACK_H_
#define _SYSTEM_MSGS_CALLBACK_H_

#ifdef  __cplusplus
extern "C" {
#endif


struct stBoardInfo* AddBoard( byte instance, byte model_index  );
void print_board			( struct stBoardInfo* mboard );
void print_all_boards		( );

BOOL callback_board_presence( struct sCAN* mMsg );


#ifdef  __cplusplus
}
#endif

#endif
