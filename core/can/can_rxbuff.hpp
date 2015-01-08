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

void 		 AddToRxList  ( struct sCAN* mMsg 					  );
BOOL 		 isRxMessageAvailable();

struct sCAN* GetMessagePtr( byte mIndex 				  		  );
struct sCAN* GetNextRxMsg ( );
int			 GetRxMode    ( );

// "PROTECTED/HELPER" Functions:
void addNormalHistory ( struct sCAN* mMsg );
void addMailboxHistory( struct sCAN* mMsg );


//********* ID Filter: **********************************
#define NO_FILTER     0
#define REJECT_FILTER 1
#define PASS_FILTER   2

void		set_filter_mode		( byte mMode        );
void 		add_id_to_filter	( tID* mId 		    );
int			id_is_in_filter_list( struct sCAN* mMsg );


#ifdef  __cplusplus
}
#endif

#endif

