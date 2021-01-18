#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <vector>


#include "drive_five.h"
#include "motor_mapping.h"
#include "cal_file.hpp"
#include "cal_motor.hpp"

std::vector<CalMotor*>  mots;

// Create CalMotor for map entry
void  make_motors()		
{
	for (int i=0; i<TOTAL_BODY_JOINTS; i++)
		{
			/*printf("%2d %20s %20s %20s %s %c\n", i,
				body_joints[i].limb,
				body_joints[i].joint,
				body_joints[i].board_name,
				body_joints[i].device_name,	
				body_joints[i].assigned_motor );*/
			CalMotor* tmp  = new CalMotor();
			tmp->set_map_info( &(body_joints[i]) );
			mots.push_back(tmp);				
		}	
	printf("there are %d motors\n", mots.size() );
}

void  print_motors()
{
	printf("=====================  Motor Calibration Data  ====================================\n");
	printf("#\tAlias\tJoint\t\tTravel\t\t\tCounts/Rev  0.0 degs at\n");

	for (int i=0; i<TOTAL_BODY_JOINTS; i++)
	{
		printf("%2d ", i );
		mots[i]->print_cal_data();
	}
	printf("================================================================================\n\n");	
}


bool all_spaces( char* line )
{
	int len = strlen(line);
	for (int i=0; i<len; i++)
	{
		if ( (line[i]!=' ' ) && (line[i]!='\t') && 
			 (line[i]!='\r') && (line[i]!='\n')  )
		{
			return false;
		}
	}
	return true;
}

// does it in place
void remove_leading_whitespace(char* mStr )
{
	bool white_space = ((mStr[0] == ' ') || (mStr[0] == '\t'));
	while (white_space)
	{
		strcpy( &(mStr[0]),  &(mStr[1]) );				
		white_space = ((mStr[0] == ' ') || (mStr[0] == '\t'));
	}
}



void parse_cpr_line( char* line, struct stCPRInfo* Info )
{
	// EXTRACT ALIAS :
	char* ptr = strchr(line, ' ');
	*ptr = 0;
	strcpy (Info->alias, line );

	// EXTRACT JOINT :
	extract_word_move_on( ptr+1, Info->joint, &ptr  );

	// EXTRACT COUNTS:
	char  tmp[30];
	extract_word_move_on( ptr, tmp, &ptr  );	
	Info->counts_per_rev = atoi( tmp );

	// EXTRACT UNIT : 
	extract_word_move_on( ptr, tmp, &ptr  );		
	strcpy( Info->unit, tmp );
	int cpd = strcmp( Info->unit, "CPD")==0;
	if (cpd)  Info->counts_per_rev = round( Info->counts_per_rev / 360);
	
	//printf("CPR Info: alias=%s; joint=%s; counts_per_rev=%d; unit=%s\n",  \
				Info->alias, Info->joint, \
				Info->counts_per_rev, Info->unit);	
}

void parse_mmt_line( char* line, struct stMinMaxTravelInfo* Info )
{
	// EXTRACT ALIAS :
	char* ptr = strchr(line, ' ');
	*ptr = 0;
	strcpy (Info->alias, line );

	// EXTRACT JOINT :
	extract_word_move_on( ptr+1, Info->joint, &ptr  );
	
	// EXTRACT MIN COUNT:
	char tmp[50];
	extract_word_move_on( ptr, tmp, &ptr );	
	Info->MinCount = atoi( tmp );

	// EXTRACT MAX COUNT:
	extract_word_move_on( ptr, tmp, &ptr );	
	Info->MaxCount = atoi( tmp );

	//printf("MM Info: alias=%s; joint=%s; min=%d; max=%d\n", Info->alias, Info->joint, \
				Info->MinCount, Info->MaxCount);
}

/*  Zero Offset is the count which corresponds to 0.0 degrees angle
	It must fall within the min/max travel range.
*/
void parse_zo_line( char* line, struct stZOInfo* Info )
{
	// EXTRACT ALIAS :
	char* ptr = strchr(line, ' ');
	*ptr = 0;
	strcpy (Info->alias, line );

	// EXTRACT JOINT :
	extract_word_move_on( ptr+1, Info->joint, &ptr  );

	// EXTRACT COUNTS:
	char  tmp[30];
	extract_word_move_on( ptr, tmp, &ptr  );	
	Info->Counts = atoi(tmp);
	//printf("ZO Info: alias=%s; joint=%s; count=%d; \n", Info->alias, Info->joint, \
				Info->Counts );

}

bool parse_disable_line( char* line, struct stDisableInfo* Info )
{
	// EXTRACT ALIAS :
	char* ptr = strchr(line, ' ');
	*ptr = 0;
	strcpy (Info->alias, line );

	// EXTRACT JOINT :
	extract_word_move_on( ptr+1, Info->joint, &ptr  );

	// EXTRACT COUNTS:
	char  tmp[30];
	extract_word_move_on( ptr, tmp, &ptr  );

	int dis = strncmp(tmp, "DISABLE",7)==0;
	int en  = strncmp(tmp, "ENABLE",7)==0;
	if (dis)		Info->disable = true;
	else if (en)	Info->disable = false;
	
	return Info->disable;
}


/* Reads the calibration file 
	and constructs a CalMotor object for each motor.
*/
int lookup_mot( char* mAlias, char* mJoint )
{
	for (int m=0; m<mots.size(); m++)
	{	
		int same_joint = strcmp( mots[m]->get_joint(),  mJoint )==0;
		int same_alias = strcmp( mots[m]->get_alias(),  mAlias )==0;		
		if ( same_joint && same_alias )
		{
			return m;
		}
	}
	return -1;
}

void read_cal_file( const char* mFilename    )
{
	FILE*   fd  = fopen( mFilename, "r" );
	size_t  len = 0;
    ssize_t nread;
	char* 	line 		= NULL;
	bool 	new_section = false;
	char 	section [80];
	
	// First section :  "MIN / MAX TRAVEL COUNTS"
	//printf("Read Min/Max Travel Counts\n");
	nread = getline( &line, &len, fd );
	new_section = (line[0] == ':');
	if (new_section)  strcpy(section, &(line[2]) );
	new_section = false;
	while ((!new_section) && (!feof(fd)))
	{
		nread = getline( &line, &len, fd );

		bool as = all_spaces( line );
		if (as)  {   continue;	}

		new_section = (line[0] == ':');
		if (new_section)  { break;	}

		struct stMinMaxTravelInfo mmt;
		parse_mmt_line( line, &mmt );
		
		int m_index = lookup_mot( mmt.alias, mmt.joint );
		if (m_index>=0) {
			mots[m_index]->set_min_max_travel( mmt.MinCount, mmt.MaxCount );
		}
	}

	// Second section :  "COUNTS PER DEGREE"
	//printf("\nRead Counts per Degree \n");
	new_section = false;
	while ((!new_section) && (!feof(fd)))
	{
		nread = getline( &line, &len, fd );
		
		bool as = all_spaces( line );
		if (as)  {   continue;	}
		new_section = (line[0] == ':');
		if (new_section)  { break;	}

		struct stCPRInfo cpr;
		parse_cpr_line( line, &cpr );
		
		int m_index = lookup_mot( cpr.alias, cpr.joint );
		if (m_index>=0)
			mots[m_index]->set_counts_per_rev( cpr.counts_per_rev );
	}
	
	// Third section :  "ZERO OFFSET"
	//printf("\nRead Zero Offset \n");
	//if (new_section)  strcpy(section, &(line[2]) );
	new_section = false;
	while ((!new_section) && (!feof(fd)))
	{
		nread = getline( &line, &len, fd );

		bool as = all_spaces( line ) || (nread==-1);
		if (as)  {  continue;	}
		
		new_section = (line[0] == ':');
		if (new_section)  { break;	}

		struct stZOInfo zo;
		parse_zo_line( line, &zo );
		
		int m_index = lookup_mot( zo.alias, zo.joint );
		if (m_index>=0)
			mots[m_index]->set_zero_offset( zo.Counts );		
	}
	
	// Fourth section :  "DISABLED"
	new_section = false;
	while ((!new_section) && (!feof(fd)))
	{
		nread = getline( &line, &len, fd );

		// ALL SPACES?  NEW SECTION?
		bool as = all_spaces( line ) || (nread==-1);
		if (as)  {  continue;	}
		new_section = (line[0] == ':');
		if (new_section)  { break;	}

		// 
		struct stDisableInfo di;
		parse_disable_line( line, &di );

		int m_index = lookup_mot( di.alias, di.joint );
		if ((m_index>=0) && (di.disable))
		{
			//printf("Mot : %s-%s diabled!\n", di.alias, di.joint );
			mots[m_index]->disable_motor( );
		}
	}

	//printf("Done reading CalFile!\n");
	fclose (fd);	
}

