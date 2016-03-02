#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <inttypes.h>

char**	split		(char *src_str, const char deliminator, int *num_sub_str);

void	getLine_nb	( FILE* f, char* mLine 		);
void	getLine		( FILE* f, char* mLine 		);
char*	getWord		( char* mLine 				);
char*	skipNext	( char* mStr				);
void	getLine		( FILE* f, char* mLine 		);
unsigned char	getIntArray	( char* mLine, int* mInts 	);
char*	readvec		( char* mName 				);

// HELPER FUNCTIONS:
void trim_trail_space  ( char* str );
void trim_leading_space( char* str );
void convert_to_lower_case  ( char* str );
bool is_word_break          ( char mTest );
bool is_punctuation_mark    (char mTest  );     // ,.?;:!



#endif

