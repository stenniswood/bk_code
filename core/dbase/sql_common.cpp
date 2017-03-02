#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>

#include <time.h>
#include "sql_common.hpp"


char* form_date_string( struct tm& time_bd )
{
	static char tmp[16];
	sprintf(tmp, "%4d-%d-%d", time_bd.tm_year+1900,
								time_bd.tm_mon,
								time_bd.tm_mday );
	return tmp;
}
char* form_time_string( struct tm& time_bd )
{
	static char tmp[16];
	sprintf(tmp, "%2d:%2d:%2d", time_bd.tm_hour,
								time_bd.tm_min,
								time_bd.tm_sec );	
	return tmp;
}
char* form_date_time_string( struct tm& time_bd )
{
	static char tmp[32];
	strcpy(tmp, form_date_string(time_bd));
	strcat(tmp, " ");
	strcat(tmp, form_time_string(time_bd));
	return tmp;
}

char* append_float( float mFloat )
{
	static char tmp[16];
	sprintf(tmp, "'%6.2f'", mFloat );
	return tmp;
}

