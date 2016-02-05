#ifdef  __cplusplus
extern "C" {
#endif


extern BOOL SEQ_ListeningOn;	// if true we will be receiving poses.
extern BOOL SEQ_SendingOn;	// if true we will be sending poses.

void Init_Sequencer_Protocol();
char* Parse_Sequencer_Statement( char* mSentence );



#ifdef  __cplusplus
}
#endif

