#include "serverthread.hpp"

#ifdef  __cplusplus
extern "C" {
#endif 




extern BOOL CAN_ListeningOn;	// if true we will be receiving CAN Traffic.
extern BOOL CAN_SendingOn;		// if true we will be sending   CAN Traffic.

void 	Init_CAN_Protocol();
int 	Parse_CAN_Statement( const char* mSentence, ServerHandler* mh );

int		start_amon();


#ifdef  __cplusplus
}
#endif 

