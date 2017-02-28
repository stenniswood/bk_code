//
//  sql_people.cpp
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
#include "sql_people.hpp"
#include "sequencer_memory.hpp"


#define Debug 0
MYSQL 		 	*friends_db = NULL;
SQL_Friends     sql_friends;


static void object_finish_with_error( )
{
    fprintf    ( stderr, "%s\n", mysql_error(friends_db));
    mysql_close( friends_db );
    exit(1);
}

/*********************************************************************************/
SQL_Friends::SQL_Friends()
{
    
}
SQL_Friends::~SQL_Friends()
{
    
}
#include "machine_defs.h"

void SQL_Friends::connect_to_friends_db()
{
    friends_db = mysql_init(NULL);
    if (friends_db == NULL)
    {
        fprintf(stderr, "init %s\n", mysql_error(friends_db));
        exit(1);
    }
    if (mysql_real_connect(friends_db, "localhost", sql_username, sql_password,
                           sql_user_dbase_name, 0, NULL, 0) == NULL)
                           // was robot_local
    {
        fprintf(stderr, "real_connect %s\n", mysql_error(friends_db));
        mysql_close(friends_db);
        exit(1);
    }
}

void SQL_Friends::create_friends_table( )
{
    bool exists = sql_friends.friends_table_exists();
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

bool SQL_Friends::friends_table_exists( )
{
    // how to query it?
    query_string = "SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'robot_local' AND  TABLE_NAME = 'vision_events';";
    query(true);
    int result_count = get_number_results();
    return (result_count>0);
}

int	SQL_Friends::get_number_columns()
{
    unsigned int cols = mysql_num_fields( m_result );
    return cols;
}

int  SQL_Friends::query( bool mRetrieving )
{
    printf("%s\n", query_string.c_str() );
    if (mysql_query(friends_db, query_string.c_str() ))
    {
        fprintf(stderr, "Object: %s\n", mysql_error(friends_db));
    }
    // Question : what happens when no results?
    //   Answer : if  mysql_store_result returns a non-zero value
    if (mRetrieving) {
        m_result = mysql_store_result(friends_db);
        if (m_result == NULL)  {
            Dprintf("query Error result==null!\n");
            return 0;
        }
        m_row_count = mysql_num_rows( m_result );
    }
    Dprintf("row_count=%ld\n", m_row_count);
    return (int)m_row_count;
}

int	SQL_Friends::get_number_results()
{
    return (int)mysql_num_rows(m_result);
}

char* form_date_string( struct tm& time_bd )
{
    static char tmp[16];
    sprintf(tmp, "%4d-%d-%d", time_bd.tm_year+1900,
            time_bd.tm_mon+1,
            time_bd.tm_mday );
    return tmp;
}

char* form_time_string( struct tm time_bd )
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
    printf("%s \n", tmp);
    return tmp;
}

int	SQL_Friends::sql_add_system_activated( bool mCamera, bool mEyes, bool mNeck )
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

int	SQL_Friends::sql_add_system_deactivated( )
{
    std::string description = "Vision System De-activated";
    query_string =  "INSERT INTO robot_local.vision_events (timestamp, event_description) ";
    query_string += "VALUES ( NOW(),'";
    query_string += description;
    query_string += "' );";
    printf("\n%s\n", query_string.c_str() );
    return query(false);
}

int	SQL_Friends::sql_add_event( std::string mPersonName, std::string mDescription )
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

int	SQL_Friends::sql_query_event( std::string mPersonName, std::string mDescription )		// including "unknown"
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

int	SQL_Friends::sql_query_time( std::string mPersonName, std::string mDescription,
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

void SQL_Friends::print_row( )
{
    unsigned int cols = mysql_num_fields( m_result );
    for (int c=0; c<cols; c++)
        if (m_row[c])
            printf("%s\t", m_row[c] );
    printf("\n");
}

void SQL_Friends::print_results( )
{
    m_row = mysql_fetch_row( m_result );
    while ( m_row )
    {
        print_row( );
        m_row = mysql_fetch_row( m_result );
    }
    printf("\n");
}

int	SQL_Friends::sql_query_time_qualitative( std::string mDateTime)	// such as "yesterday", "today", "4 hours ago"
{
    return 0;
}

int SQL_Friends::form_response__how_many_times( std::string mNames, std::string mDescription,
                                                    struct tm start_time_bd, struct tm end_time_bd )
{
    sql_query_time( mNames, mDescription, start_time_bd, end_time_bd );
    int retval = get_number_results();
    return retval;
}

int SQL_Friends::form_response__last_time_i_saw( std::string mPerson, time_t& mTime )
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
int	SQL_Friends::form_response__saw_for_duration( std::string mPerson, time_t& mTime)
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

int	SQL_Friends::form_response__hours_active( std::string mDescription, time_t& mTime,
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

void SQL_Friends::form_response__during_that_time_i_saw( struct tm start_time_bd,
                                                         struct tm end_time_bd, std::string& mResponse)
{
    
}

int SQL_Friends::form_response__last_time_i_activated( time_t& mTime)
{
    sql_query_event( "", "System Activated" );
    m_row = mysql_fetch_row( m_result );
    int rows = get_number_results();
    int cols = get_number_columns();
    mTime = atoi( m_row[cols-1] );
    return rows;
}

int SQL_Friends::form_response__last_time_i_deactivated( time_t& mTime)
{
    sql_query_event( "", "System De-activated" );
    m_row = mysql_fetch_row( m_result );
    int rows = get_number_results();
    int cols = get_number_columns();
    mTime = atoi( m_row[cols-1] );		
    return rows;
}

char* append_float( float mFloat )
{
    static char tmp[16];
    sprintf(tmp, "'%6.2f'", mFloat );
    return tmp;
}

MYSQL_ROW SQL_Friends::goto_first_row()
{
    m_row = mysql_fetch_row( m_result );
    return m_row;
}

MYSQL_ROW SQL_Friends::goto_next_row()
{
    m_row = mysql_fetch_row( m_result );
    return m_row;
}

void SQL_Friends::extract_result	( )		// for the last fetched row.
{
    if (m_row) {
        for (int r=0; r<8; r++)
            readings[r] = atof(m_row[r+2]);
    }
}


void test_friends_db()
{
    // Add some samples rows : 
    sql_friends.connect_to_friends_db();
    
    sql_friends.sql_add_event("Unknown", "Face Detected" );
    sql_friends.sql_add_event("Unknown", "Face Detected");
  /* sql_friends.sql_add_event("Stephen", "Face Left"     );
     sql_friends.sql_add_event("David",   "Face Detected" );
     sql_friends.sql_add_event("David",   "Face Left"  	);
     sql_friends.sql_add_event("Lynn",    "Face Left"  	); */
}

