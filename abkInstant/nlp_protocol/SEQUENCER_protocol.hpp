


#include "serverthread.hpp"


extern BOOL SEQ_ListeningOn;	// if true we will be receiving poses.
extern BOOL SEQ_SendingOn;	// if true we will be sending poses.


void Init_Sequencer_Protocol();
int Parse_Sequencer_Statement( Sentence& mSentence, ServerHandler* mh );



