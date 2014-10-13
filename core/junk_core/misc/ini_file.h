#ifdef  __cplusplus
extern "C" {
#endif


void getLine	( FILE* f, char* mLine );
char* getWord	( char* mLine 		   );


char* readvec	( char* mName 		   );
void show_file	( char* mFileName 	   );

void save		( int argc, char* argv[], char* mVectName, byte vector_size );

#ifdef  __cplusplus
}
#endif
