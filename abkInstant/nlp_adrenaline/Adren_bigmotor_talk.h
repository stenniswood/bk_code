#ifdef  __cplusplus
extern "C" {
#endif



void init_nlp_bigmotor();
void Parse_bigmotor_Statement( char* mSentence, sObject* mSubject, 
		 					 std::string* mVerb, sObject* mObject );


#ifdef  __cplusplus
}
#endif
