/*#ifdef  __cplusplus
extern "C" {
#endif */

#include "nlp_sentence.hpp"

extern int start_amon();	// extern from instant_main.cpp
extern BOOL CAN_ListeningOn;	// if true we will be receiving CAN Traffic.
extern BOOL CAN_SendingOn;		// if true we will be sending   CAN Traffic.

void 	Init_CAN_Protocol();
int 	Parse_CAN_Statement( Sentence& mSentence );

int		start_amon();


/*#ifdef  __cplusplus
}
#endif 
*/