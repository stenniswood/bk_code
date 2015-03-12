#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include <vector> 
using namespace std;

#ifdef  __cplusplus
extern "C" {
#endif


extern std::vector<Appendage> Appendages;

// Notice Config file is different than the vector sequence file!
extern char ConfigFileName[];

// FUNCTIONS:
void 	read_config			( char* mFilename, Robot& mRobot );

// "PROTECTED" FUNCTIONS:
char**  split				( char *src_str, const char deliminator, int *num_sub_str );
int* 	atoi_array			( char** string, int num 	);
float*  atof_array			( char** string, int num 	);

char*	getWord				( char* mLine 				);
char*	skipNext			( char* mStr				);
void 	getLine				( FILE* f, char* mLine 		);
byte	getIntArray			( char* mLine, int* mInts	);
void	read_instance_line	( FILE* f, Robot& mRobot	);
void	read_stop_line		( FILE* f, Robot& mRobot	);  // motor stop positions

char*	readvec				( char* mName 				);

#ifdef  __cplusplus
}
#endif

#endif


