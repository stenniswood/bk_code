#ifndef _SQL_COMMON_HPP_
#define _SQL_COMMON_HPP_

#include <my_global.h>
#include <mysql.h>


// Use this handle for sql_users, sql_devices, calendar, viki_proceedings, etc 
// Each table in the sjtennis_bk_useraccounts will share this handle:
extern MYSQL *users_db;		
void object_finish_with_error( );
void connect_to_users_db();




char* form_date_string( struct tm& time_bd );
char* form_time_string( struct tm& time_bd );
char* form_date_time_string( struct tm& time_bd );
char* append_float( float mFloat );
char* append_int( int mInt );


#endif
