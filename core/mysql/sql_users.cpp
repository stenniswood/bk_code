//
//  sql_users.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 9/25/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <vector>
#include <time.h>
#include <my_global.h>
#include <mysql.h>
#include <stdint.h>
#include "global.h"
#include "sql_users.hpp"
#include "sql_common.hpp"
#include "sequencer_memory.hpp"
#include "machine_defs.h"


#define Debug 0

SQL_Users     sql_users;


/*********************************************************************************/
SQL_Users::SQL_Users()
{
    connect_to_users_db();
}
SQL_Users::~SQL_Users()
{
    
}
#include "machine_defs.h"

/*void SQL_Users::connect_to_users_db()
{
    users_db = mysql_init(NULL);
    if (users_db == NULL)
    { 
        fprintf(stderr, "init %s\n", mysql_error(users_db));
        exit(1);
    }
    if (mysql_real_connect(users_db, "localhost", sql_username, sql_password,
                           sql_user_dbase_name, 0, NULL, 0) == NULL)
                           // was robot_local
    {
        fprintf(stderr, "SQL_Users:: real_connect %s\n", mysql_error(users_db));
        mysql_close(users_db);
        //exit(1);
    }
}*/

void SQL_Users::create_users_table( )
{
    bool exists = sql_users.users_table_exists();
    if (exists)
    {
        //printf("TABLE Already Exists\n");
        return ;
    }

    query_string = "CREATE TABLE `users` ( \
    `id_user` int(11) NOT NULL AUTO_INCREMENT, \
    `gcal_token` varchar(64) NOT NULL,    \
    `email` varchar(64) NOT NULL, \
    `phone_number` varchar(16) NOT NULL,  \
    `username` varchar(16) NOT NULL,  \
    `_password` varchar(32) NOT NULL, \
    `new_password` varchar(32) NOT NULL,  \
    `confirmcode` varchar(32) DEFAULT NULL, \
    `firstname` varchar(64) NOT NULL, \
    `lastname` varchar(64) NOT NULL, \
    `Address_1` varchar(40) NOT NULL, \
    `Address_2` varchar(40) NOT NULL, \
    `City` varchar(20) NOT NULL, \
    `State` varchar(20) NOT NULL, \
    `ZipCode` varchar(10) NOT NULL, \
    `Country` varchar(20) NOT NULL, \
    `nick_name` varchar(20) NOT NULL, \
    `MedRef_Provider_id` int(11) NOT NULL, \
    `MedRef_Patient_id` int(11) NOT NULL, \
    `Advertiser_id` int(11) NOT NULL, \
    `Profile_pic_id` int(11) NOT NULL, \
    `Profile_greeting_id` int(11) NOT NULL, \
    `Birthdate` timestamp(11) NOT NULL, \
    `Age` int(11) NOT NULL, \
    `bedtime` timestamp NULL DEFAULT NULL, \
    `wakeup` timestamp NULL DEFAULT NULL, \
    PRIMARY KEY (`id_user`) \
    ) ENGINE=MyISAM AUTO_INCREMENT=102 DEFAULT CHARSET=latin1";
    query(false);
}

bool SQL_Users::users_table_exists( )
{
    // how to query it?
    query_string = "SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'sjtennis_bk_useraccounts' AND  TABLE_NAME = 'Users';";
    return query(true);
}

int	SQL_Users::get_number_columns()
{
    unsigned int cols = mysql_num_fields( m_result );
    return cols;
}

/* Return:	m_row_count
*/
int  SQL_Users::query( bool mRetrieving )
{
	if (users_db==NULL) return 0;
	
    Dprintf("SQL_Users:: %s\n", query_string.c_str() );
    if (mysql_query(users_db, query_string.c_str() ))
    {
        fprintf(stderr, "Object: %s\n", mysql_error(users_db));
    }
    // Question : what happens when no results?
    //   Answer : if  mysql_store_result returns a non-zero value
    if (mRetrieving) {
        m_result = mysql_store_result(users_db);
        if (m_result == NULL)  {
            Dprintf("query Error result==null!\n");
            return 0;
        }
        m_row_count = mysql_num_rows( m_result );
    }
    Dprintf("row_count=%ld\n", m_row_count);
    return (int)m_row_count;
}

int	SQL_Users::get_number_results()
{
    return (int)mysql_num_rows(m_result);
}

int	SQL_Users::sql_add_system_activated( bool mCamera, bool mEyes, bool mNeck )
{
    std::string description = "Vision System Activated";
    std::string names;
    if (mCamera)
        names += "Camera On";
    if (mEyes)
        names += "Eyes On";
    if (mNeck)
        names += "Neck On";
    query_string =  "INSERT INTO robot_local.vision_events (timestamp, event_description, people_names) ";
    query_string += "VALUES ( NOW(),'";
    query_string += description;
    query_string += "', '";
    query_string += names;
    query_string += "');";
    //printf("\n%s\n", query_string.c_str() );
    return query(false);
}

int	SQL_Users::sql_add_system_deactivated( )
{
    std::string description = "Vision System De-activated";
    query_string =  "INSERT INTO robot_local.vision_events (timestamp, event_description) ";
    query_string += "VALUES ( NOW(),'";
    query_string += description;
    query_string += "' );";
    printf("\n%s\n", query_string.c_str() );
    return query(false);
}

int	SQL_Users::sql_add_event( std::string mPersonName, std::string mDescription )
{
    query_string =  "INSERT INTO robot_local.vision_events (timestamp, event_description, people_names) ";
    query_string += "VALUES ( NOW(),'";
    query_string += mDescription;
    query_string += "', '";
    query_string += mPersonName;
    query_string += "');";
    printf("\n%s\n", query_string.c_str() );
    return query(false);
}

int	SQL_Users::sql_query_event( std::string mPersonName, std::string mDescription )		// including "unknown"
{
    query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM robot_local.vision_events WHERE ";
    if (mPersonName.length()) {
        query_string += "robot_local.vision_events.people_names LIKE '%";
        query_string += mPersonName;
        query_string += "%' AND ";
    }
    query_string += "event_description LIKE '%";
    query_string += mDescription;
    query_string += "%' ORDER BY timestamp DESC;";
    //printf("\n%s\n", query_string.c_str() );
    return query(true);
}

int	SQL_Users::sql_query_time( std::string mPersonName, std::string mDescription,
                                     struct tm start_time_bd, struct tm end_time_bd  )		// including "unknown"
{
    query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM robot_local.vision_events WHERE ";
    query_string += "robot_local.vision_events.timestamp BETWEEN '";
    query_string += form_date_time_string(start_time_bd);
    query_string += "' AND '";
    query_string += form_date_time_string( end_time_bd );
    query_string += "' ";
    if (mPersonName.length())
    {
        query_string += "AND people_names='";
        query_string += mPersonName;
        query_string += "' ";
    }
    if (mDescription.length())
    {
        query_string += "AND event_description LIKE '%";
        query_string += mDescription;
        query_string += "%'";
    }
    query_string += " ORDER BY timestamp DESC;";
    //printf("\n%s\n", query_string.c_str() );
    return query(true);
}

void SQL_Users::print_row( )
{
    unsigned int cols = mysql_num_fields( m_result );
    for (int c=0; c<cols; c++)
        if (m_row[c])
            printf("%s\t", m_row[c] );
    printf("\n");
}

void SQL_Users::print_results( )
{
    m_row = mysql_fetch_row( m_result );
    while ( m_row )
    {
        print_row( );
        m_row = mysql_fetch_row( m_result );
    }
    //printf("\n");
}

int	SQL_Users::sql_query_time_qualitative( std::string mDateTime)	// such as "yesterday", "today", "4 hours ago"
{
    return 0;
}

int SQL_Users::form_response__how_many_times( std::string mNames, std::string mDescription,
                                                    struct tm start_time_bd, struct tm end_time_bd )
{
    sql_query_time( mNames, mDescription, start_time_bd, end_time_bd );
    int retval = get_number_results();
    return retval;
}

int SQL_Users::form_response__last_time_i_saw( std::string mPerson, time_t& mTime )
{
    sql_query_event( mPerson, "face left" );
    m_row    = mysql_fetch_row( m_result );
    m_row_count = get_number_results();
    int cols = get_number_columns();
    printf("m_row_count=%ld; cols=%d; \n", m_row_count, cols );
    
    mTime    = atoi( m_row[cols-1] );
    printf("m_row[%d]=%s;  %ld\n", cols, m_row[cols-1], mTime );
    return m_row_count;
}
/* how much time from face detected to face left? */
int	SQL_Users::form_response__saw_for_duration( std::string mPerson, time_t& mTime)
{
    time_t sTime,eTime;
    query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM robot_local.vision_events WHERE ";
    if (mPerson.length()) {
        query_string += "robot_local.vision_events.people_names LIKE '%";
        query_string += mPerson;
        query_string += "%' AND ";
    }
    query_string += "(event_description LIKE '%face detected%' ";
    query_string += "OR event_description LIKE '%face left%')";
    query_string += " ORDER BY timestamp DESC;";
    //printf("\n%s\n", query_string.c_str() );
    query(true);
    
    m_row    = mysql_fetch_row( m_result );
    m_row_count = get_number_results();
    int cols = get_number_columns();
    printf("m_row_count=%ld; cols=%d; \n", m_row_count, cols );
    
    sTime    = atoi( m_row[cols-1] );
    eTime    = atoi( m_row[cols-1] );
    mTime = difftime(sTime,eTime);
    
    printf("m_row[%d]=%s;  %ld\n", cols, m_row[cols-1], mTime );
    return m_row_count;
}

int	SQL_Users::form_response__hours_active( std::string mDescription, time_t& mTime,
                                                  struct tm start_time_bd, struct tm end_time_bd )
{
    time_t sTime,eTime, sumTotal, deltaTime;
    sumTotal = 0;
    deltaTime = 0;
    sTime = 0;
    query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM robot_local.vision_events WHERE ";
    query_string += "(event_description LIKE '%System Activated%' ";
    query_string += "OR event_description LIKE '%System De-activated%') AND ";
    query_string += "robot_local.vision_events.timestamp BETWEEN '";
    query_string += form_date_time_string(start_time_bd);
    query_string += "' AND '";
    query_string += form_date_time_string( end_time_bd );
    query_string += "' ";
    query_string += " ORDER BY timestamp ASC;";
    query(true);
    
    m_row       = mysql_fetch_row( m_result );
    m_row_count = get_number_results();
    int cols    = get_number_columns();
    printf("m_row_count=%ld; cols=%d; \n", m_row_count, cols );
    while (m_row) {
        
        if (strcmp(m_row[2], "Vision System Activated")==0)
            sTime = atoi( m_row[9] );
        if (strcmp(m_row[2], "Vision System De-activated")==0) {
            eTime = atoi( m_row[9] );
            if (sTime) {
                deltaTime = difftime(eTime, sTime);
                sumTotal += deltaTime;
            }
        }
        //printf("%d m_row[%d]=%s; %d <%d..%d>\n", row++, cols-1, m_row[cols-1], deltaTime, sTime, eTime );
        m_row = mysql_fetch_row( m_result );
    }
    //printf("Sum Total Active Time = %6.1f hours.\n", sumTotal/3600. );
    mTime = sumTotal;
    return m_row_count;
}

void SQL_Users::form_response__during_that_time_i_saw( struct tm start_time_bd,
                                                         struct tm end_time_bd, std::string& mResponse)
{
    
}

int SQL_Users::form_response__last_time_i_activated( time_t& mTime)
{
    sql_query_event( "", "System Activated" );
    m_row = mysql_fetch_row( m_result );
    int rows = get_number_results();
    int cols = get_number_columns();
    mTime = atoi( m_row[cols-1] );
    return rows;
}

int SQL_Users::form_response__last_time_i_deactivated( time_t& mTime)
{
    sql_query_event( "", "System De-activated" );
    m_row = mysql_fetch_row( m_result );
    int rows = get_number_results();
    int cols = get_number_columns();
    mTime = atoi( m_row[cols-1] );		
    return rows;
}


MYSQL_ROW SQL_Users::goto_first_row()
{
    m_row = mysql_fetch_row( m_result );
    return m_row;
}

MYSQL_ROW SQL_Users::goto_next_row()
{
    m_row = mysql_fetch_row( m_result );
    return m_row;
}

/* The caller may select which columns will be filled with data.
	ie. mFields = "(timestamp, firstname, lastname, username, password )"
	    mValues = "(NOW(), 'Steve', 'Tenniswood', 'stenniswood', 'Buggsbunny' )"
*/	
int	SQL_Users::sql_add	( std::string mFields, std::string mValues  )
{
	query_string =  "INSERT INTO ";
	query_string += sql_user_dbase_name;
	query_string += ".Users ";
	query_string += mFields;
	query_string += " VALUES "+ mValues + ";";
	return query(false);		
}

void SQL_Users::close_connection()
{
	mysql_close( users_db );
}

int	SQL_Users::sql_add_if_not_already( std::string mFields, std::string mValues, std::string mUsername  )
{
	// First check if it already exists in the database of users:
	int result = sql_find( mUsername );
	if (result==0) {
	    return sql_add( mFields, mValues );
	} else 
		printf("Person already exists in database!\n");
	return 0;
}

int	SQL_Users::sql_find	( std::string mName						 )
{
    query_string =  "SELECT username,password,id_user FROM ";
    query_string += sql_user_dbase_name;
    query_string += ".Users ";
    query_string += " WHERE username='";
    query_string += mName;
    query_string += "'";
    query(true);
    m_row = mysql_fetch_row( m_result );    
    return atoi(m_row[2]);
}

bool SQL_Users::verify_password( std::string mName, std::string mPassword )
{
	sql_find(mName);		// fetches first row    
	if (m_row)
        if (m_row[1])
        {            
            int result = mPassword.compare( m_row[1] );
			if (result==0) {
				printf("PASSWORD VERIFIED!\n");
				return true;
			}
		}
				printf("PASSWORD BAD!\n");		
	return false;
}

int	SQL_Users::sql_update_password ( std::string mName, std::string mPasswd )
{
    query_string =  "UPDATE ";
    query_string += sql_user_dbase_name;
    query_string += ".Users ";
    query_string += " SET password='";
    query_string += mPasswd;
    query_string += "'";
    query_string += " WHERE username='";
    query_string += mName;
    query_string += "';";
    return query(true); 
}


void SQL_Users::extract_result	( )		// for the last fetched row.
{
    if (m_row) {
        for (int r=0; r<8; r++)
            readings[r] = atof(m_row[r+2]);
    }
}


void test_users_db()
{
    // Add some samples rows : 
    connect_to_users_db();
    bool found = sql_users.users_table_exists();
	printf("Users table exists()=%d\n", found);
    
	sql_users.sql_add_if_not_already("( username, password )", "( 'stenniswood', 'Mozart2nd')", "stenniswood");
	sql_users.sql_add_if_not_already("( firstname, lastname, username, password )", "( 'Mark','Tenniswood', 'mtenniswood', 'Tobar')", "mtenniswood"  );
	sql_users.sql_add_if_not_already("( username, password )", "( 'dtenniswood', 'Medical')", "dtenniswood");
	sql_users.sql_add_if_not_already("( username, password )", "( 'ltenniswood', 'Sugar')", "ltenniswood"  );		

	sql_users.sql_find( "mtenniswood" );	sql_users.print_results();
	sql_users.sql_find( "ltenniswood" );	sql_users.print_results();
	sql_users.sql_find( "dtenniswood" );	sql_users.print_results();

	sql_users.verify_password( 	"mtenniswood", "Tobar" );
	sql_users.verify_password( 	"ltenniswood", "Sugar" );

	printf("Change passwords.  Should fail:\n");	
	sql_users.sql_update_password ( "mtenniswood", "ToyRobot" );
	sql_users.sql_update_password ( "ltenniswood", "NutzBoltz" );		
	sql_users.verify_password( 	"mtenniswood", "Tobar" );
	sql_users.verify_password( 	"ltenniswood", "Sugar" );		

	printf("Now put passwords back to original.  Should Verify\n");
	sql_users.sql_update_password ( "mtenniswood", "Tobar" );
	sql_users.sql_update_password ( "ltenniswood", "Sugar" );	
	sql_users.verify_password( 	"mtenniswood", "Tobar" );
	sql_users.verify_password( 	"ltenniswood", "Sugar" );		

	printf("Done with test_users_db()\n");
}

