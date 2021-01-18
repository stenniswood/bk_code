#include <vector>
#include <string>
#include "drive_five.h"


struct stCPRInfo 
{
	char alias[30];
	char joint[30];
	int  counts_per_rev;
	char unit[10];
};
struct stMinMaxTravelInfo 
{
	char alias[30];
	char joint[30];
	long int  MinCount;
	long int  MaxCount;	
};
struct stZOInfo
{
	char alias[30];
	char joint[30];
	int  Counts;
};
struct stDisableInfo
{
	char alias[30];
	char joint[30];
	bool disable;		// FALSE ==> enable
};

bool 		all_spaces					( char* line );
void 		remove_leading_whitespace	( char* mStr );
//inline void extract_word_move_on		( char* mStr, char* mResult, char** nxtPtr );

int   lookup_mot		( char* mAlias, char* mJoint );
void  make_motors		( );					// Create CalMotor for map entry
void  read_cal_file  	( const char* mFilename );	// Reads file for all motors.
void  print_motors		( );

inline void extract_word_move_on( char* mStr, char* mResult, char** nxtPtr )
{
	// EXTRACT JOINT :
	remove_leading_whitespace(mStr);
	char* eptr = strchr( mStr, ' ');
	if (eptr != NULL) *eptr = 0;
	else {
		eptr = strchr( mStr, '\n');
		if (eptr!=NULL) {
			*eptr = 0;
		}		
	}
	strcpy( mResult, mStr );
	*nxtPtr = eptr+1;
}

