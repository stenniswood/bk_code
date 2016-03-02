#ifdef  __cplusplus
extern "C" {
#endif


int  extract_pin_number		   ( std::string* mSentence );

void Init_GPIO_Protocol();

BOOL Parse_GPIO_Statement(char* mStatement );



#ifdef  __cplusplus
}
#endif

