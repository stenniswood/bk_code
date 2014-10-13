#ifndef _CAN_BUFF_H_
#define _CAN_BUFF_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define MAX_CAN_MSG_MEMORY_SIZE 100

#define NO_HISTORY_OP_MODE 		0
#define NORMAL_HISTORY_OP_MODE 	1
#define MAILBOX_HISTORY_OP_MODE 2

extern byte OperationalMode;

char 		 FindMailBox  ( tID mID 							  );
void 		 copy_can_msg ( struct sCAN* mDest, struct sCAN* mSrc );
void 		 QueueMessage ( struct sCAN* mMsg 					  );
struct sCAN* GetMessagePtr( byte mIndex 				  		  );
BOOL 		 RxMessageAvailable();
struct sCAN* GetNext();


// "PROTECTED/HELPER" Functions:
void addNormalHistory ( struct sCAN* mMsg );
void addMailboxHistory( struct sCAN* mMsg );

#ifdef  __cplusplus
}
#endif

#endif

