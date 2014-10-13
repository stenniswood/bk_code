#ifdef  __cplusplus
extern "C" {
#endif




void init_nlp_lcd();
void Parse_lcd_Statement( char* mSentence, sObject* mSubject, 
		 					 std::string* mVerb, sObject* mObject );

#ifdef  __cplusplus
}
#endif
