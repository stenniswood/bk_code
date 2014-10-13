#ifdef  __cplusplus
extern "C" {
#endif


void init_nlp_picamscan();
void Parse_picamscan_Statement( char* mSentence, sObject* mSubject, 
		 					 std::string* mVerb, sObject* mObject );
		 					 
#ifdef  __cplusplus
}
#endif
