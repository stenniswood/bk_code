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
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <map>

#include "mcp2515.h"
#include "pican_defines.h"
#include "bk_system_defs.h"
#include "can_id_list.h"
#include "preferences.hpp"




Preferences::Preferences( char* mFileName )
{
	printf("Preferences()...\n");
	FileName = mFileName;
	fd = 0;
}

BOOL Preferences::file_exists()
{
	fd = fopen(FileName, "r");
	if (fd==0) return FALSE;
	return TRUE;
}

// ==0 means no errors.  >0 is line number of first error.	
char Preferences::load_all_keys( )
{
	printf("Load all keys...\n");
	open  (TRUE);
	int LineCount = 1;
	char retval	  = 0;
	bool blank    = false;
	struct sKeyValuePair* ptr = NULL;

	while ((!feof(fd)) && (retval==0))
	{
		blank = readln_nb();
		//printf("%s\n", Line);

		if ((strchr(Line, '=')==0) || (blank==true))
		{  
			close();  
			return LineCount;  
		}
		if (blank==false)  {		
			char* k_ptr = ( char*)getKey();
			char* v_ptr = ( char*)getValue();
			//printf("key=%s \t\t\t value=%s\n", key, value );
			ptr = new sKeyValuePair( key, value );
			key_table.push_back( ptr );
			LineCount++;
		}
	}
	close();	
	return retval;
}

void Preferences::print_all_keys( )
{
	std::list<sKeyValuePair*>::iterator iter = key_table.begin();
	while (iter != key_table.end() ) 
	{
		printf("key=%s;\t\tvalue=%s\n", (*iter)->key.c_str(), (*iter)->value.c_str() );
		iter++;
	}
}

struct sKeyValuePair* 	Preferences::find_key ( const char* mKey )
{
	std::list<struct sKeyValuePair*>::iterator iter = key_table.begin();
	for( ; iter!=key_table.end(); iter++)
	{
		if (strcmp((*iter)->key.c_str(), mKey)==0)
		{
			return *iter;
		}
	}
	return NULL;
}

bool Preferences::find_bool( const char* mKey )
{
	struct sKeyValuePair* ptr = find_key(mKey);
	if (ptr==NULL)
		return false; 

	if ((ptr->value.compare("TRUE")==0) ||
		(ptr->value.compare("ENABLE")==0) ||
		(ptr->value.compare("ENABLED")==0) ||
		(ptr->value.compare("YES")==0) ||
		(ptr->value.compare("1")==0) ||
		(ptr->value.compare("ON")==0) )
		return true;
	else 
		return false;
}

const char*  Preferences::find_string ( const char* mKey )
{
	struct sKeyValuePair* ptr = find_key(mKey);
	if (ptr==NULL)
		return NULL;

	return (ptr->value.c_str());
}

float 	Preferences::find_float( const char* mKey )
{
	struct sKeyValuePair* ptr = find_key(mKey);
	if (ptr==NULL)
		throw "Not found";

//	printf("value=%s\n", ptr->value.c_str() );
	float tmp = atof( ptr->value.c_str() );
//	printf("float=%6.4f\n", tmp);
	return tmp;
}

int 	Preferences::find_int( const char* mKey )
{
	struct sKeyValuePair* ptr = find_key(mKey);
	if (ptr==NULL)	
		throw "Not found";

	return atoi(ptr->value.c_str());
}

unsigned int  Preferences::find_hex( const char* mKey )
{
	struct sKeyValuePair* ptr = find_key(mKey);
	if (ptr==NULL)	
		throw "Not found";

	unsigned int tmp;
	sscanf(ptr->value.c_str(), "%x", &tmp ); 
	//printf("find_hex  %s = %d  %x\n", ptr->value.c_str(), tmp, tmp );
	return tmp;
}

/* This will read the first non-blank line.	
    ie. it skips blank lines  			
    Holds result in Line (member variable)		
return true -> read correctly (valid data)
	   false => no blank line found, end of file (not valid data)
*/
bool Preferences::readln_nb(  )
{
	bool blank  = false;
	int  length = 0;
	do {
		readln( );
		length = strlen(Line);
		
		blank = true;	// Start
		for (int i=0; ((i<length) && (Line[i])); i++)
		{
			if ((Line[i] != ' ') && (Line[i] != '\t') && (Line[i] != '\n') && (Line[i] != '\r'))
				blank = false;
		}
		if (length==0) blank=true;
	} while (blank && (!feof(fd)) );
	return blank;
}

/* Caller provides the memory! */
void Preferences::readln( )
{
	byte i=0;
	Line[0] = 0;
	while (!feof(fd))
	{
		Line[i] = fgetc(fd);
		if ((Line[i] == '\n') || (Line[i] == '\r')) 
		{
			Line[i] = 0;
			return ;
		}
		i++;
	}
	if (i==0) *Line=0;
}

/* Incoming char* must have the \n */
void Preferences::write( char* mstr )
{
	fwrite( mstr, strlen(mstr), 1, fd );
}

char* Preferences::getKey( )
{
	char* delim = strchr(Line, '=' );
	if (delim) {
		*delim = 0;
		strcpy(key, Line);
		*delim = '=';
	}
	return key;
}
char* Preferences::getValue( )
{
	char* delim = strchr(Line, '=');
	if (delim)
		strcpy(value, delim+1);
	return value;
}

float Preferences::getFloatValue(  )
{
	getValue();	
	return (atof(value));
}

int Preferences::getIntValue( )
{
	char* delim = strchr(Line, '=');
	return (atoi(delim+1));
}

uint16_t Preferences::getHexValue( )
{
	getValue();	
	unsigned short tmp;
	sscanf(value, "%4x", &tmp );
	return tmp;
}

char* Preferences::getString( )
{
	char* delim = strchr(Line, '=');
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

void Preferences::open( BOOL mReadOnly )
{
	if (mReadOnly)
		fd = fopen(FileName, "r");
	else 
		fd = fopen(FileName, "w+");
	//printf("FN=%s; fd=%x\n", FileName, fd);
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

