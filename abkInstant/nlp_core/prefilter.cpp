#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#include <string>
#include <ctype.h>
#include "protocol.h"
#include "devices.h"
#include "prefilter.hpp"


// Hash table


/**** HELPER FUNCTIONS  ****/
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
	while (str[i])
	{
		str[i] = tolower( str[i++] );
	}
}


/**** MAIN FUNCTIONS  ****/
void  build_synonym_table()
{
}

void prefilter_init()
{
	build_synonym_table();
}


void build_numbers_table()
{	/* do we want word numbers converted to numbers?
		or numbers converted to words?
			almost always the former:  two => 2
			Definitely don't want 192:168:2:11 => One hundred Ninety eight...			
	*/
}

/* Return:	true => everything okay.
			false => This is a Voice Response and not to be processed! */
bool prefilter_text( char* mIncoming )
{ 
	char* result = strstr(mIncoming, "VR:");
	if (result!=NULL)
		return false;

	trim_leading_space(mIncoming);
	trim_trail_space( mIncoming );
	convert_to_lower_case( mIncoming );
 	printf ("Prefiltered sentence:|%s|\n", mIncoming ); 

	return true;
	// For each word
	// if found in the hash table,	
	// 	replace with the alternate hash value.
}

void prefilter_text_nip( char* mIncoming, char* mProcessed)
{
	// For each word
		
	// if found in the hash table,
	
	// 	replace with the alternate hash value.

}
