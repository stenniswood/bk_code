#ifdef  __cplusplus
extern "C" {
#endif


void Init_Camera_Protocol();
BOOL Parse_Camera_Statement( char* mSentence );									

BOOL Perform_Camera_actions( char* mAction, char* mAdjectives, char* mObjects );




#ifdef  __cplusplus
}
#endif

