#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "read_pendant.h"


#define FILE_SIZE 10000


char pendant_values[128];
int  Slider_values[NUM_SLIDERS];
int  Dial_values  [NUM_DIALS];



char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
 
void print_values()
{
	printf("Slider Values:\n");
	for (int i=0; i<NUM_SLIDERS; i++)
		printf("%d : %d\n", i, Slider_values[i] );
		
	printf("Dial Values:\n");		
	for (int i=0; i<NUM_DIALS; i++)
		printf("%d : %d\n", i, Dial_values[i] );

}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;

	strcpy( pendant_values, (char*)contents );
	//printf( "%s\n", pendant_values ); 

	char tmp[25];
	char deliminator[] = ",";
	int  slider_index = 0;
	int  dial_index   = 0;
	char * token = strtok( (char*)contents, "," );

    // loop through the string to extract all other tokens
    while( token != NULL ) {
		strcpy (tmp, token);
		int   number = atoi(token);
		if (slider_index < NUM_SLIDERS)   Slider_values[slider_index++] = number;
		else if (dial_index < NUM_DIALS)  Dial_values[dial_index++] = number;
		//else printf("extra numbers!\n");					
        //printf( " %d\n", number ); //printing each token
       token = strtok(NULL, ",");
    }
	//print_values();
	
  return realsize;
}


int read_pendant(void)
{
	curl_global_init(CURL_GLOBAL_ALL);

	CURL * myHandle;
	CURLcode setop_result;

	if((myHandle = curl_easy_init()) == NULL)
	{
	    perror("Error");
	    exit(EXIT_FAILURE);
	}

	if((setop_result = curl_easy_setopt(myHandle, CURLOPT_URL, "http://pendant.local/raw")) != CURLE_OK)
	{
	    perror("Error");
	    exit(EXIT_FAILURE);
	}

	/* send all data to this function  */ 
  	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	if((setop_result = curl_easy_perform(myHandle)) != 0)
	{
	    perror("Error");
	    exit(EXIT_FAILURE);
	}
	curl_easy_cleanup(myHandle);
	//puts("Pendant Webpage downloaded successfully");

	return 0;
}
