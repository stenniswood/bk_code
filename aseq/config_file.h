#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_


struct sCalInfo
{
	float	Angle;		// stores angle of stop in degrees
	word	PotValue;	// stores Pot value corresponding to Angle.
	
};

struct sAppendageInfo
{
	char* Name;	// An array of strings
	byte  Dimension;		// Holds the number of motors on each appendage.
							// (ie the number of actuators)
	byte* instances;		// 2DIMENSIONAL ARRAY: For each Robot Appendage & 
							// for each element within the appendage
	struct sCalInfo* CalInfos;
};

// Notice Config file is different than the Sequence file!
struct sConfigInfo
{
	byte   Number_of_appendages;		// For the entire robot
	struct sAppendageInfo** LimbInfo;
};
extern struct sConfigInfo ConfigInfo;

// FUNCTIONS:
void 					read_config( );

// "PROTECTED" FUNCTIONS:
char**  split				( char *src_str, const char deliminator, int *num_sub_str );
int* 	atoi_array			( char** string, int num 	);
float*  atof_array			( char** string, int num 	);

char*	getWord				( char* mLine 				);
char*	skipNext			( char* mStr				);
void 	getLine				( FILE* f, char* mLine 		);
byte	getIntArray			( char* mLine, int* mInts	);
void	read_instance_line	( FILE* f 					);
void	read_stop_line		( FILE* f					);  // motor stop positions
struct	sAppendageInfo* construct_LimbInfo(byte Dimension, char* mName);
char*	readvec				( char* mName 				);


#endif
