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
//#include "protocol.h"
//#include "devices.h"
#include "prefilter.hpp"
//#include "string_util.h"



// Hash table


/**** HELPER FUNCTIONS  ****/

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


void prefilter_text_nip( char* mIncoming, char* mProcessed)
{
	// For each word
		
	// if found in the hash table,
	
	// 	replace with the alternate hash value.

}
