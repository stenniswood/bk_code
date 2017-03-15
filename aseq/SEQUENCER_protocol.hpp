

#include "nlp_sentence.hpp"
//#include "server_handler.hpp"
//#include "serverthread.hpp"


extern BOOL SEQ_ListeningOn;	// if true we will be receiving poses.
extern BOOL SEQ_SendingOn;	// if true we will be sending poses.


void parse_motor_speed_command   (Sentence& mSentence, float mValues[], int& mSize);
void parse_motor_position_command(Sentence& mSentence, float mValues[], int& mSize);


void Init_Sequencer_Protocol();
int Parse_Sequencer_Statement( Sentence& mSentence);
//, ServerHandler* mh 


