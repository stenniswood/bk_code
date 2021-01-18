#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "vector_file.hpp"
#include "drive_five.h"
#include "motor_mapping.h"
#include "vector_file.hpp"
#include "cal_file.hpp"
#include "cal_motor.hpp"



std::vector<struct stPose>  	poses;
std::vector<struct stSequence>  sequences;
std::vector<struct stMove>  	moves;
std::vector<struct stPlay>  	plays;

struct stParsed {
	char alias[3];
	char joint[20];
	int	 param;
};

// Head, Left Arm, Right Arm, Left Leg, Right Leg:
char limb_aliases[][5] = { "h ", "la", "ra", "ll", "rl" };


/*  Zero Offset is the count which corresponds to 0.0 degrees angle
	It must fall within the min/max travel range.
*/
void parse_PID_line( char* line, struct stPIDInfo* Info )
{
	// EXTRACT ALIAS : 
	char* ptr = strchr(line, ' ');
	*ptr = 0;
	strcpy (Info->alias, line );

	// EXTRACT JOINT :
	extract_word_move_on( ptr+1, Info->joint, &ptr  );
	
	// EXTRACT ANGLE:
	char  tmp[30];
	extract_word_move_on( ptr, tmp, &ptr  );	

	Info->Angle = atof(tmp);
	//printf("ZO Info: alias=%s; joint=%s; angle=%6.2f; \n", Info->alias, Info->joint, \
				Info->Angle );

}

// Memory is allocated thus needs free()!
void extract_deliminated_words( char* mStr, char mDelim, char* mResult )
{
	char* ptr1 = strchr( mStr,   mDelim );
	char* ptr2 = strchr( mStr+1, mDelim );
	*ptr2 = 0;
	strcpy( mResult, ptr1+1 );
}

void extract_descriptor( char* mStr, char* mType, char* mText )
{
	char* ptr1 = strchr( mStr,   '-' );
	char* ptr2 = strchr( mStr+1, '-' );
	*ptr2 = 0;

	strcpy( mType, ptr1+1 );
	
	// REMOVE \n 	
	char* ptr3 = strchr( ptr2+1, '\n' );
	if (ptr3!=NULL)  *ptr3 = 0;
	strcpy( mText, ptr2+1 );	
}

enum eSectionType identify_type( char* mKeyWord ) 
{
	// CONVERT TO UPPERCASE (in place) : 
	int len = strlen(mKeyWord);
	for (int i=0; i<len; i++)
		mKeyWord[i] = toupper( mKeyWord[i] );
		
	// COMPARE TO KNOWN TYPES:
	int type1 = strcasecmp(mKeyWord, "POSE")    ==0;
	int type2 = strcasecmp(mKeyWord, "SEQUENCE")==0;
	int type3 = strcasecmp(mKeyWord, "MOVE")	 ==0;
	int type4 = strcasecmp(mKeyWord, "PLAY")	 ==0;
	
	enum eSectionType retval;
	if (type1)  retval = POSE;
	if (type2)  retval = SEQUENCE;
	if (type3)  retval = MOVE;
	if (type4)  retval = PLAY;
	return retval;	
}

enum eSectionType Section;

struct stPose 	   tmpPose;
struct stSequence  tmpSequence;
struct stMove      tmpMove;
struct stPlay      tmpPlay;
bool entry_pending = false;

void file_name( char* mText )
{
	//printf("file_name: %s\n", mText);
	switch(Section)
	{
	case 0 : tmpPose.name = mText;		break;
	case 1 : tmpSequence.name = mText;	break;
	case 2 : tmpMove.name = mText;		break;
	case 3 : tmpPlay.name = mText;		break;
	default: break;	
	}
}

void file_description( char* mText )
{
	//printf("file_description: %s\n", mText);
	switch(Section)
	{
	case 0 : tmpPose.description = mText;		break;
	case 1 : tmpSequence.description = mText;	break;
	case 2 : tmpMove.description = mText;		break;
	case 3 : tmpPlay.description = mText;		break;
	default: break;	
	}
}

void push_entry(  )
{

	switch(Section)
	{
	case 0 : poses.push_back(tmpPose);			tmpPose.pos.clear();				break;
	case 1 : sequences.push_back(tmpSequence);	tmpSequence.pose_index.clear(); 	break;
	case 2 : moves.push_back(tmpMove);			break;
	case 3 : plays.push_back(tmpPlay);			break;
	default: break;	
	}
}

struct stPIDInfo PIDInfo;
	
void parse_n_push( char* line )
{
	switch(Section)
	{
	case 0 :		
		parse_PID_line( line, &PIDInfo );
		//printf("tmpPose.pos.size=%d\n", tmpPose.pos.size() );		
		tmpPose.pos.push_back( PIDInfo );
		break;

	case 1 : 
		break;
	default : break;
	}
}

void parse_line( char* line, struct stParsed* mResults )
{
	if (line[0]==':')	{
		if (entry_pending)
		{
			push_entry();
		}
		// EXTRACT NEW SECTION TYPE WITH ENUM state : 
		char section_type_txt[50];
		extract_deliminated_words( line, ':', section_type_txt );
		Section = identify_type( section_type_txt );
		//printf("parse_line: ':', %s   Section=%d\n", section_type_txt, Section);
		entry_pending = true;
	}
	else if (line[0]=='-')	{
		char keyword[50];
		char text[80];
		extract_descriptor( line, keyword, text );
		if (strcmp(keyword, "name")==0 )
		{
			file_name( text );
		} 
		else if (strcmp(keyword, "description")==0 )
		{
			file_description( text );
		}
		
	} else {
		bool as = all_spaces( line );
		if (as)  {   return;	}
		parse_n_push( line );
		
	}
}

int  find_pose( char* mName )
{
	for (int i=0; i<poses.size(); i++)
	{
		int match = strcmp( poses[i].name.c_str(), mName)==0;
		if (match)
		{			
			printf( "%d %s %-30s %s %d\n", i, poses[i].alias.c_str(), poses[i].name.c_str(), poses[i].description.c_str(), poses[i].pos.size() );
			return i;
		}		
	}
	return -1;	
}

void print_poses()
{
	printf("=========+POSES+===================%d\n", poses.size());
	for (int i=0; i<poses.size(); i++)
	{
		printf( "%s %-30s %s %d\n", poses[i].alias.c_str(), poses[i].name.c_str(), poses[i].description.c_str(), poses[i].pos.size() );
	}
}

void print_sequences()
{
	printf("=========+SEQUENCES+===================\n");
	for (int i=0; i<sequences.size(); i++)
	{
		printf( "%s %-30s %s \n", sequences[i].alias.c_str(), sequences[i].name.c_str(), sequences[i].description.c_str() );
	}
}

void read_vector_file()
{
	size_t  len = 0;
    ssize_t nread;
	struct stPose tmp;
	    
	FILE* fd = fopen("head_n_shoulders.vec","r" );
	char* line = NULL;
	while (!feof(fd))
	{
		nread = getline( &line, &len, fd);
		if ((line[0] != '/') && (line[1] !='/'))	// ignore comment lines
		{
			struct stParsed results;
			parse_line( line, &results );

			//char* full_limb_name = alias_lookup( results.alias );
			//tmp.alias = get_device_name( full_limb_name, results.joint );			
			//tmp.MotLetter = get_assigned_motor( full_limb_name, results.joint );
		}	
		//printf("Line:%s", line );
	}
	print_poses();
	print_sequences();
}

CalMotor* lookup_mot( struct stPIDInfo& Info )
{
	int mot_index = lookup_mot( Info.alias, Info.joint );
	return mots[mot_index];
}

void send_pose( struct stPose& mPose )
{
	for (int i=0; i<mPose.pos.size(); i++)
	{
		CalMotor* mot = lookup_mot( mPose.pos[i] );
		mot->send_pid_request( mPose.pos[i].Angle );
	}	
}

void play_sequence( struct stSequence& mSequence, long mDelay_us )
{
	for (int i=0; i<mSequence.pose_index.size(); i++)
	{
		send_pose( poses[ mSequence.pose_index[i] ] );
		usleep( mDelay_us );
	}	
}


