/****************************************************************
INI FILE 
CONTENTS:
	M1		M2		instance1
	M3		M4		instance2

instance1, instance2, alpha, swap_xy[0,1]
 
To Fly!
	Stand alone solution
		Thrust following a sequence
			
	String solution	

	Socket connection to cell phone (involves Wifi though)
		SocketServer - simple app for GPIO
		
		Simple A) Thumb slider for thrust ( auto balance from tilt )
		Stage  B) Tilt matches tilt
		
****************************************************************/
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "mcp2515.h"
#include "pican_defines.h"
#include "bk_system_defs.h"
#include "can_id_list.h"
#include "ini_file.hpp"


Preferences::Preferences( char* mFileName )
{
/*	int length = strlen(mFileName)+1;
	FileName = new char[length];
	strcpy(FileName, mFileName);
	FileName[length-1] = 0; */
	FileName = mFileName;
	//printf("Pref:FileName=%s\n", FileName );
	fd = 0;
}

BOOL Preferences::file_exists()
{
	fd = fopen(FileName, "r");
	if (fd==0) return FALSE;
	return TRUE;
}

void Preferences::open( BOOL mReadOnly )
{
	if (mReadOnly)
		fd = fopen(FileName, "r");
	else 
		fd = fopen(FileName, "w+");
	//printf("FN=%s; fd=%x\n", FileName, fd);
}
/* This will read the first non-blank line.
   ie. it skips blank lines */
void Preferences::readln_nb( char* mLine )
{
	BOOL blank  = FALSE;
	int  length = 0;
	do {
		readln( mLine );
		length = strlen(mLine);
		blank  = (length==0);
		for (int i=0; (i<length) && (mLine[i]); i++)
		{
			if ((mLine[i] != ' ') && (mLine[i] != '\t'))
				blank = FALSE;
		}
	} while (blank);
}

/* Caller provides the memory! */
void Preferences::readln( char* mLine )
{
	byte i=0;
	while (!feof(fd))
	{
		mLine[i] = fgetc(fd);
		if (mLine[i] == '\n') {
			mLine[i] = 0;
			return ;
		}
		i++;
	}
	if (i==0) *mLine=0;
}

/* Incoming char* must have the \n */
void Preferences::write( char* mstr )
{
	fwrite( mstr, strlen(mstr), 1, fd );
}

char* Preferences::getKey( char* mLine )
{
	char* delim = strchr(mLine, '=');
	*delim = 0;
	strcpy(key, mLine);	
	*delim = '=';
	return mLine;
}

float Preferences::getFloatValue( char* mLine )
{
	char* delim = strchr(mLine, '=');
	return (atof(delim+1));
}

int Preferences::getIntValue( char* mLine )
{
	char* delim = strchr(mLine, '=');
	return (atoi(delim+1));
}

char* Preferences::getString( char* mLine )
{
	char* delim = strchr(mLine, '=');
	//char* str = new char[strlen(delim+1)+1];
	//strcpy (str, delim+1);
	return delim+1; // str;
}

/*char* Preferences::readvec( char* mName )
{
	getLine( f, line );
	do {
		char* name = getWord( line );
		if (strcmp( name, mName ) == 0)			
			return line;
		getLine( f, line );
	} while (!feof(f));
}*/

void Preferences::show_file( char* mFileName )
{
	// PRINT .ini file
	char c = fgetc(fd);
	while (!feof(fd))
	{	
		printf("%c", c);
		c = fgetc(fd);
	}
}

void Preferences::close(  )
{
	fclose(fd);
}

/*void Preferences::save( int argc, char* argv[], char* mVectName, byte vector_size )
{
	byte  Instances[MAX_VECTOR_SIZE];

	fprintf(fd, "%s %d ", argv[first_param+2], vector_size );
	byte instances[50];
	
	for (int i=0; i<vector_size; i++)
	{
		if ((i+3) > argc)
		{
			printf("Not enough instances!");
			return 0;
		} else {
			instances[i] = atoi( argv[first_param+4+i] );
			fprintf( fd, "%d ", instances[i] );
		}
	}
	fprintf( fd, "\n");
	fclose(fd);
}*/

/* This modifies the input string! */
char** Preferences::split(char *src_str, const char deliminator, int *num_sub_str)
{
  //replace deliminator's with zeros and count how many
  //sub strings with length >= 1 exist
  *num_sub_str = 0;
  char *src_str_tmp = src_str;
  BOOL found_delim  = true;
  while(*src_str_tmp) {
    if(*src_str_tmp == deliminator) {
       *src_str_tmp = 0;
       found_delim = true;
    }
    if(found_delim){ //found first character of a new string
      (*num_sub_str)++;
      found_delim = false;
    }
    src_str_tmp++;
  }
  //printf("Start - found %d sub strings\n", *num_sub_str);
  if(*num_sub_str <= 0) {
    printf("str_split() - no substrings were found\n");
    return(0);
  }

  //if you want to use a c++ vector and push onto it, the rest of this function
  //can be omitted (obviously modifying input parameters to take a vector, etc)
  char **sub_strings = (char **)malloc( (sizeof(char*) * *num_sub_str) + 1);
  const char *src_str_terminator = src_str_tmp;
  src_str_tmp = src_str;
  BOOL found_null = true;
  size_t idx = 0;
  while(src_str_tmp < src_str_terminator){
    if(!*src_str_tmp) //found a NULL
      found_null = true;
    else if(found_null){
      sub_strings[idx++] = src_str_tmp;
      //printf("sub_string_%d: [%s]\n", idx-1, sub_strings[idx-1]);
      found_null = false;
    }
    src_str_tmp++;
  }
  sub_strings[*num_sub_str] = NULL;
  return(sub_strings);
}

