

extern char response[1024];
extern char Keyword[128];
extern char Value[128];

void pendant_active_stream(bool mValue);
void left_loadcells_active_stream (bool mValue);
void right_loadcells_active_stream(bool mValue);


void split_keyword_value( char* mLine, char* mKeyword, char* mValue );
int process_server_cmd( char* mKeyword, char* mValue );


