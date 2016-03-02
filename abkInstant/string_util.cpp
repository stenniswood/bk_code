#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <stdint.h>

#include "bk_system_defs.h"
#include "string_util.h"

#define TRUE 1
#define FALSE 0

char line[255];

char** split(char *src_str, const char deliminator, int *num_sub_str)
{
  //InPlace, replace deliminator's with zeros.
  //  and count how many sub strings with length >= 1 exist
  *num_sub_str = 0;
  char *src_str_tmp = src_str;
    
  while(*src_str_tmp)
  {
    if(*src_str_tmp == deliminator) {
       *src_str_tmp = 0;
       (*num_sub_str)++;
    }
    src_str_tmp++;
  }
  (*num_sub_str)++;         // unlesss the delim is very first or very last char.

  // Now form an array of pointers :
    
  //if you want to use a c++ vector and push onto it, the rest of this function
  //can be omitted (obviously modifying input parameters to take a vector, etc)
  char **sub_strings = (char **)malloc( (sizeof(char*) * *num_sub_str) + 1);

  const char *src_str_terminator = src_str_tmp;
  src_str_tmp = src_str;
  bool found_null = true;
  size_t idx = 0;
  while(src_str_tmp < src_str_terminator)
  {
    if(*src_str_tmp==0)
      found_null = true;
    else if (found_null)
    {
      sub_strings[idx++] = src_str_tmp;
      //printf("sub_string_%ld: [%s]\t%x\n", idx-1, sub_strings[idx-1], sub_strings[idx-1] );
      printf("sub_string_%ld: [%s]\n", idx-1, sub_strings[idx-1] );
        
      found_null = false;
    }
    src_str_tmp++;
  }
  sub_strings[*num_sub_str] = NULL;
  return(sub_strings);
}


char* getWord( char* mLine )
{
	static char tmp[50];
	char* firstspace = strchr(mLine, ' ');
	strncpy(tmp, mLine, (firstspace-mLine));
	tmp[(firstspace-mLine)] = 0;
	return tmp;
}


// Get a Pointer to the start of the next word!
char* skipNext( char* mStr)
{
	char* tPtr = strchr( mStr, ' ' );
	if (tPtr==NULL) return NULL;
	while (*tPtr == ' ') { tPtr++; };		
	return tPtr;
}

/* This will read the first non-blank line.
   ie. it skips blank lines */
void getLine_nb( FILE* f, char* mLine )
{
	bool blank  = false;
	int  length = 0;
	do {
		getLine( f, mLine );
		length = strlen(mLine);
		blank  = (length==0);
		for (int i=0; (i<length) && (mLine[i]); i++)
		{
			if ((mLine[i] != ' ') && (mLine[i] != '\t'))
				blank = false;
		}
	} while (blank);
}

/* Reads from the text file until a complete line has been read */
void getLine( FILE* f, char* mLine )
{
	byte i=0;
	while (!feof(f))
	{
		mLine[i] = fgetc(f);
		if (mLine[i] == '\n') {
			mLine[i] = 0;
			return ;
		}
		i++;
	}
	if (i==0) *mLine=0;
}


/*
	Input must be all integers
			mInts should be allocated large enough to hold the expected size.
	Return : Size of the array.
*/
byte getIntArray( char* mLine, int* mInts )
{
	char* tPtr = mLine;
	static char tmp[50];
	int i=0;

	do {
		char* firstspace = strchr(tPtr, ' ');
		if (firstspace==NULL) return i;
		
		strncpy(tmp, tPtr, (firstspace-tPtr));
		tmp[(firstspace-tPtr)]=0;		// null terminator
		mInts[i++] = atoi(tmp);

		// leap frog to next word:
		tPtr = firstspace;
		while (*tPtr == ' ') { tPtr++; };		
	} while( 1 );
}


bool is_word_break( char mTest )
{
    if ((mTest==' ') || (mTest=='.') || (mTest==',') || (mTest==';') || (mTest=='?') || (mTest=='!'))
    {
        if (mTest=='.')
            printf("is_word_break\n");
        
        return true;
    }
    else 
        return false;	
}


void trim_trail_space( char* str )
{
    int   len = strlen(str)-1;
    char* ptr = &(str[len]);
    while(( len>0 ) && (*ptr==' '))
    {
        *ptr = 0;
        ptr--;  len--;
    }
}
void trim_leading_space( char* str )
{
    while((str[0]==' ') || (str[0]=='\t'))
    {
        strcpy( str, str+1 );
    }
}

void convert_to_lower_case( char* str )
{
    int   i=0;
    char tmp;
    while (str[i])
    {
        tmp = tolower( str[i] );
        str[i] = tmp;
        i++;
    }
}

bool is_punctuation_mark(char mTest)
{
    if ((mTest=='.') || (mTest==',') || (mTest=='?') || (mTest==';') || (mTest==':') || (mTest=='!'))
        return true;
    return false;
}



/*char* readvec( char* mName )
{
	FILE* f = fopen( mName,"r" );
	getLine( f, line );
	do 
	{	
		char* name = getWord( line );
		if (strcmp( name, mName ) == 0)			
			return line;
		getLine( f, line );		
	} while (!feof(f));	
	fclose(f);
}*/






