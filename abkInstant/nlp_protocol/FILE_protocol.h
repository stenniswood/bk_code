#ifdef  __cplusplus
extern "C" {
#endif

void Init_FILE_Protocol();

BOOL Parse_File_transfer_Statement(char* mStatement, 
									sObject* mSubject, 
									std::string* verb, 
									sObject* object );

char* file_inform_client();
char* file_inform_user  ();


#ifdef  __cplusplus
}
#endif

