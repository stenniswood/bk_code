#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif


char**	split		(char *src_str, const char deliminator, int *num_sub_str);

void	getLine_nb	( FILE* f, char* mLine 		);
void	getLine		( FILE* f, char* mLine 		);
char*	getWord		( char* mLine 				);
char*	skipNext	( char* mStr				);
void	getLine		( FILE* f, char* mLine 		);
byte	getIntArray	( char* mLine, int* mInts 	);
char*	readvec		( char* mName 				);


#ifdef __cplusplus
}
#endif

#endif

