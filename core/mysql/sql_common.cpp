#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <time.h>
#include "sql_common.hpp"
#include "machine_defs.h"


MYSQL 		  *users_db = NULL;


void object_finish_with_error( )
{
    fprintf    ( stderr, "%s\n", mysql_error(users_db));
    mysql_close( users_db );
}

void connect_to_users_db()
{
	bool fresh_start = (users_db == NULL);
	if (!fresh_start)
		return;
    
    users_db = mysql_init(NULL);
    if (users_db == NULL)
    { 
        fprintf(stderr, "init %s\n", mysql_error(users_db));
        return;
        //exit(1);
    }
    if (mysql_real_connect(users_db, "localhost", sql_username, sql_password,
                           sql_user_dbase_name, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "connect_to_users_db:: mysql_real_connect %s\n", mysql_error(users_db));
        mysql_close(users_db);
        users_db = NULL;
    }
}





char* form_date_string( struct tm& time_bd )
{
    static char tmp[16];
    sprintf(tmp, "%4d-%d-%d", time_bd.tm_year+1900,
            time_bd.tm_mon+1,
            time_bd.tm_mday );
    return tmp;
}
char* form_time_string( struct tm& time_bd )
{
    static char tmp[32];
    strftime( tmp, 32, "%H:%M:%S", &time_bd);
    return tmp;
    //return asctime( &time_bd );
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

char* append_int( int mInt )
{
	static char tmp[16];
	sprintf(tmp, "'%d'", mInt );
	return tmp;
}



