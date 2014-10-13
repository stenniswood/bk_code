#ifdef  __cplusplus
extern "C" {
#endif


void Init_GPIO();
int  extract_pin_number		   ( std::string* mSentence );

void Parse_GPIO_Acknowledgement( sObject* mSubject, sObject* mVerb, 
				 sObject* mObject, std::string* mSentence );





#ifdef  __cplusplus
}
#endif

