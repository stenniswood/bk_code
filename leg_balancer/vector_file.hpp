#ifndef _VECTOR_FILE_HPP_
#define _VECTOR_FILE_HPP_
#include <vector>
#include <string>

/***********************************
   Vector file consists of :
   Pose, Sequence, Move, play
************************************/

enum eSectionType {  POSE, SEQUENCE, MOVE, PLAY  };

struct stPIDInfo
{
	char alias[30];
	char joint[30];
	float  Angle;
};


struct stPose {	
	std::string  alias;
	std::string  name;
	std::string  description;
	std::vector<struct stPIDInfo> pos;
};

struct stSequence {	
	std::string  alias;
	std::string  name;
	std::string  description;
	std::vector<int> pose_index;
};

struct stMove {	
	std::string  alias;
	std::string  name;
	std::string  description;
	std::vector<int> pose_index;
};

struct stPlay {	
	std::string  alias;
	std::string  name;
	std::string  description;
	std::vector<int> pose_index;
};


extern std::vector<struct stPose>  		poses;
extern std::vector<struct stSequence>  	sequences;
extern std::vector<struct stMove>  		moves;
extern std::vector<struct stPlay>  		plays;



void 				extract_deliminated_words	( char* mStr, char mDelim, char* mResult );
void 				extract_descriptor			( char* mStr, char* mType, char* mText   );

enum eSectionType 	identify_type				( char* mKeyWord ) ;
void 				parse_line					( char* line, struct stParsed* mResults );
void 				read_vector_file			( );
void 				print_poses					( );
void 				print_sequences				( );
void 				print_move					( );
void 				print_play					( );


int  				find_pose					( char* mName );
void 				send_pose					( struct stPose& mPose );


#endif