#ifndef _SYSTEM_MSGS_CALLBACK_H_
#define _SYSTEM_MSGS_CALLBACK_H_

/*#ifdef  __cplusplus
extern "C" {
#endif*/

#include "board_list_oop.hpp"

extern BoardList bl;
BOOL callback_board_presence( struct sCAN* mMsg );

/*#ifdef  __cplusplus
}
#endif*/

#endif
