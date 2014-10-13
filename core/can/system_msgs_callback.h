#ifndef _SYSTEM_MSGS_CALLBACK_H_
#define _SYSTEM_MSGS_CALLBACK_H_

#ifdef  __cplusplus
extern "C" {
#endif
#include "board_list.h"


// there is a cpp version of this which puts the boards into a class list.
BOOL callback_board_presence( struct sCAN* mMsg );

#ifdef  __cplusplus
}
#endif


#endif
