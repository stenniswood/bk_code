#ifdef  __cplusplus
extern "C" {
#endif



void init_nlp_adrenaline();

void Parse_adrenaline_Statement( char* mSentence, sObject* mSubject, 
		 					 std::string* mVerb, sObject* mObject );
		 					 
		 					 
		 					 
#ifdef  __cplusplus
}
#endif
