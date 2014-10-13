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
#include "protocol.h"
#include "devices.h"

// Hash table

void prefilter_init()
{
	build_synonym_table();
}

void  build_synonym_table()
{
	

}

void build_numbers_table()
{

}

void prefilter_text( char* mIncoming, char* mProcessed)
{
	// For each word
		
	// if found in the hash table,
	
	// 	replace with the alternate hash value.

}
