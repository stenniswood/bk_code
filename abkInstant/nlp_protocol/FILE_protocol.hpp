#ifdef  __cplusplus
extern "C" {
#endif

void Init_FILE_Protocol();

int Parse_File_Statement(const char* mStatement, ServerHandler* mh );

char* file_inform_client();
char* file_inform_user  ();


#ifdef  __cplusplus
}
#endif

