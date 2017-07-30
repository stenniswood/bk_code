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
#include <map>
#include <time.h>
#include <sys/types.h>
#include <regex>
#include <algorithm>
#include <string>

#include "nlp_sentence.hpp"

#include <my_global.h>
#include <mysql.h>
#include <stdint.h>
#include "global.h"
#include "sql_users.hpp"
#include "sql_device.hpp"
#include "sequencer_memory.hpp"
#include "machine_defs.h"
#include "sql_common.hpp"
#include <iostream>
#include <sstream>


#define Debug 1
SQL_Devices     sql_devices;
char table_name[128];

/*********************************************************************************/
SQL_Devices::SQL_Devices()
{
	strcpy(table_name, sql_user_dbase_name );
	strcat(table_name, ".my_devices");
	
    connect_to_users_db();
    sql_devices.create_users_table();	// if it doesn't exist already.    
}
SQL_Devices::~SQL_Devices()
{
    
}
#include "machine_defs.h"


void SQL_Devices::create_users_table( )
{
    bool exists = sql_devices.users_table_exists();
    if (exists)
    {
        //printf("TABLE Already Exists\n");
        return ;
    }

  query_string = "CREATE TABLE `my_devices` (	\
  `_id` int(11) NOT NULL AUTO_INCREMENT,		\
  `login_count` int(11) NOT NULL,				\
  `last_login` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,	\
  `user_id` int(11) NOT NULL,					\
  `regex` text COMMENT 'Contains a regex for matching user spoken name.',	\
  `preferred_name` text NOT NULL COMMENT 'Such as \"Steves phone\" rather than dev_name \"Sensation\"',	\
  `device_type` text NOT NULL COMMENT 'Phone, Humanoid, Tablet, Reader, Playstation,Wii, etc.',	\
  `hostname` text NOT NULL,			\
  `IP_address` text NOT NULL,		\
  `Mac_address` text NOT NULL,		\
  `SerialNumber` text NOT NULL COMMENT 'This was not available in Android \"Build.SERIALNUMBER\"',	\
  `phone_number` varchar(20) NOT NULL,	\
  `Operating_System` varchar(20) NOT NULL COMMENT 'Android/iPhone/WindowsCE/',	\
  `Board` text NOT NULL,			\
  `Bootloader` text NOT NULL,		\
  `Brand` text NOT NULL,			\
  `CPU_ABI` text NOT NULL,			\
  `Display` text NOT NULL,			\
  `Hardware` text NOT NULL,			\
  `Manufacturer` text NOT NULL,		\
  `Model` text NOT NULL,			\
  `Tags` text NOT NULL,				\
  PRIMARY KEY (`_id`)				\
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;";

	printf("CREATING TABLE: %s\n", query_string.c_str() );
    query(false);
} 

bool SQL_Devices::users_table_exists( )
{
    // how to query it?
    query_string = "SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE \
    TABLE_SCHEMA = 'sjtennis_bk_useraccounts' AND \
     TABLE_NAME = 'my_devices';";
    return query(true);
}

int	SQL_Devices::get_number_columns()
{
    unsigned int cols = mysql_num_fields( m_result );
    return cols;
}

/* Return:	m_row_count
*/
int  SQL_Devices::query( bool mRetrieving )
{
	if (users_db==NULL) return 0;
    Dprintf("SQL_Devices:: %s\n", query_string.c_str() );
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

int	SQL_Devices::get_number_results()
{
    return (int)mysql_num_rows(m_result);
}

int	SQL_Devices::sql_add_system_activated( bool mCamera, bool mEyes, bool mNeck )
{
    std::string description = "Viki System Activated";
    std::string names;
    if (mCamera)
        names += "Camera On";
    if (mEyes)
        names += "Eyes On";
    if (mNeck)
        names += "Neck On";
    query_string =  "INSERT INTO ";
    query_string += table_name;
    query_string +=  " (timestamp, event_description, people_names) ";
    query_string += "VALUES ( NOW(),'";
    query_string += description;
    query_string += "', '";
    query_string += names;
    query_string += "');";
    //printf("\n%s\n", query_string.c_str() );
    return query(false);
}

int	SQL_Devices::sql_add_system_deactivated( )
{
    std::string description = "Vision System De-activated";
    query_string =  "INSERT INTO ";
    query_string += table_name;
    query_string += " (timestamp, event_description) ";
    query_string += "VALUES ( NOW(),'";
    query_string += description;
    query_string += "' );";
    printf("\n%s\n", query_string.c_str() );
    return query(false);
}

int	SQL_Devices::sql_add_event( std::string mPersonName, std::string mDescription )
{
    query_string =  "INSERT INTO ";
    query_string += table_name;
    query_string +=" (timestamp, event_description, people_names) ";
    query_string += "VALUES ( NOW(),'";
    query_string += mDescription;
    query_string += "', '";
    query_string += mPersonName;
    query_string += "');";
    printf("\n%s\n", query_string.c_str() );
    return query(false);
}

int	SQL_Devices::sql_query_event( std::string mPersonName, std::string mDescription )		// including "unknown"
{
    query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM ";
    query_string += table_name;
    query_string += "table_name WHERE ";
    if (mPersonName.length()) {
       query_string += sql_user_dbase_name;
        query_string += ".my_devices.people_names LIKE '%";
        query_string += mPersonName;
        query_string += "%' AND ";
    }
    query_string += "event_description LIKE '%";
    query_string += mDescription;
    query_string += "%' ORDER BY timestamp DESC;";
    //printf("\n%s\n", query_string.c_str() );
    return query(true);
}

int	SQL_Devices::sql_query_time( std::string mPersonName, std::string mDescription,
                                     struct tm start_time_bd, struct tm end_time_bd  )		// including "unknown"
{
    query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM ";
    query_string += table_name;
    query_string += " WHERE ";
    query_string += table_name;
    query_string += ".timestamp BETWEEN '";
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

void SQL_Devices::print_row( )
{
    unsigned int cols = mysql_num_fields( m_result );
    for (int c=0; c<cols; c++)
        if (m_row[c])
            printf("%s\t", m_row[c] );
    printf("\n");
}

void SQL_Devices::print_results( )
{
    m_row = mysql_fetch_row( m_result );
    while ( m_row )
    {
        print_row( );
        m_row = mysql_fetch_row( m_result );
    }
    //printf("\n");
}

int	SQL_Devices::sql_query_time_qualitative( std::string mDateTime)	// such as "yesterday", "today", "4 hours ago"
{
    return 0;
}

int SQL_Devices::form_response__how_many_times( std::string mNames, std::string mDescription,
                                                    struct tm start_time_bd, struct tm end_time_bd )
{
    sql_query_time( mNames, mDescription, start_time_bd, end_time_bd );
    int retval = get_number_results();
    return retval;
}

int SQL_Devices::form_response__last_time_i_saw( std::string mPerson, time_t& mTime )
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
int	SQL_Devices::form_response__saw_for_duration( std::string mPerson, time_t& mTime)
{
    time_t sTime,eTime;
    query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM ";
	query_string += table_name;
    query_string += " WHERE ";
    if (mPerson.length()) {
           query_string += table_name;
        query_string += ".people_names LIKE '%";
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

int	SQL_Devices::form_response__hours_active( std::string mDescription, 
												  time_t& mTime,
                                                  struct tm start_time_bd, 
                                                  struct tm end_time_bd )
{
    time_t sTime,eTime, sumTotal, deltaTime;
    sumTotal = 0;
    deltaTime = 0;
    sTime = 0;
    query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM ";
   query_string += table_name;
   query_string +=" WHERE ";
    query_string += "(event_description LIKE '%System Activated%' ";
    query_string += "OR event_description LIKE '%System De-activated%') AND ";
           query_string += table_name;
    query_string += ".timestamp BETWEEN '";
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

void SQL_Devices::form_response__during_that_time_i_saw( struct tm start_time_bd,
                                                         struct tm end_time_bd, std::string& mResponse)
{
    
}

int SQL_Devices::form_response__last_time_i_activated( time_t& mTime)
{
    sql_query_event( "", "System Activated" );
    m_row = mysql_fetch_row( m_result );
    int rows = get_number_results();
    int cols = get_number_columns();
    mTime = atoi( m_row[cols-1] );
    return rows;
}

int SQL_Devices::form_response__last_time_i_deactivated( time_t& mTime)
{
    sql_query_event( "", "System De-activated" );
    m_row = mysql_fetch_row( m_result );
    int rows = get_number_results();
    int cols = get_number_columns();
    mTime = atoi( m_row[cols-1] );		
    return rows;
}


MYSQL_ROW SQL_Devices::goto_first_row()
{
    m_row = mysql_fetch_row( m_result );
    return m_row;
}

MYSQL_ROW SQL_Devices::goto_next_row()
{
    m_row = mysql_fetch_row( m_result );
    return m_row;
}

/* The caller may select which columns will be filled with data.
	ie. mFields = "(timestamp, firstname, lastname, username, password )"
	    mValues = "(NOW(), 'Steve', 'Tenniswood', 'stenniswood', 'Buggsbunny' )"
*/	
int	SQL_Devices::sql_add	( std::string mFields, std::string mValues  )
{
	SuperString values = mValues;
	values.convert_to_lower_case();	
	query_string =  "INSERT INTO ";
	query_string += table_name;
	query_string += " ";
	query_string += mFields;
	query_string += " VALUES "+ values + ";";
	printf("sql_add device %s\n", query_string.c_str() );
	return query(false);		
}

void SQL_Devices::close_connection()
{
//	mysql_close( users_db );
}
	
/* return:  1=> just added */
int	SQL_Devices::sql_add_if_not_already( int mUserId, 
						std::map<std::string,std::string> mFields )
{
	// First check if it already exists in the database of users:
	// (user_id, preferred_name, device_type, hostname) must all match or it gets added.
	int result = sql_find( mUserId, mFields["hostname"] );	
	if (result==0) 
	{
		std::string fields = "( user_id, ";
		std::string values = "(";
		values += std::to_string(mUserId);

		std::map<string,string>::iterator iter = mFields.begin();
		while (iter !=mFields.end())
		{
			fields += iter->first;
			values += iter->second;
			fields += ", ";
			values += ", ";
			iter++;	
		}
		fields += ")";	
		values += ")";
		printf("fields:%s\n values:%s\n", fields.c_str(), values.c_str() );

	    return sql_add( fields, values );
	} else {
	//was found:
		printf("Device already registered!\n");
	}
	return 0;
}

/* Return:  is the device_name (hostname) */
std::string	SQL_Devices::sql_find_preferred_name	( int mUser_id, std::string mPreferredName	)
{
    query_string =  "SELECT preferred_name,hostname FROM ";
    query_string += table_name;
    query_string += " WHERE ";
    query_string += " user_id=";
    query_string += append_int( mUser_id );
    query_string += " AND preferred_name='";
    query_string += mPreferredName;
    query_string += "';";
    int retval = query(true);

    // Now for all results, do the regex_search:
    goto_first_row();
    bool found=false;
    std::regex exp;    
    std::smatch match;
    while (m_row)
    {
    	if (m_row[0])  exp=m_row[0];
    	found = std::regex_search( mPreferredName, match, exp ); 
    	if (found)
    		return m_row[1];		// return device_name;
    	goto_next_row();
    }
    return NULL;
}

int	SQL_Devices::sql_find	( int mUser_id, std::string mDevice_name )
{
    query_string =  "SELECT * FROM ";
    query_string += table_name;
    query_string += " ";
    query_string += " WHERE hostname='";		// device_name is hostname
    query_string += mDevice_name;
    query_string += "' AND user_id=";
    query_string += append_int( mUser_id );
    query_string += ";";
    int retval = query(true);
    m_row = mysql_fetch_row( m_result );
    if (m_row)
	    _id = atoi(m_row[0]);
    printf("device_id = %d\n", _id );
    return retval;
}

int	SQL_Devices::get_all_device_names_regex ( int mUserID, std::string& mRegex )
{
    std::string tmp = std::to_string(mUserID);
    
    query_string =  "SELECT preferred_name,user_id FROM ";
    query_string += table_name;
    query_string += " ";
    query_string += " WHERE user_id='";
    query_string += tmp;
    query_string += "'";
    int retval = query(true);

	m_row = mysql_fetch_row( m_result );
//	mRegex = "(";
    while ( m_row ) 
    {		
		mRegex += m_row[0];
		m_row = mysql_fetch_row( m_result );		
		if (m_row)  mRegex += "|";				
    }
  //  mRegex += ")";
    printf("ALL DEVICES REGEX: %s\n\n", mRegex.c_str() );
    return retval;
}

int	SQL_Devices::get_all_device_names ( int mUserID, std::string& mList )
{
    std::string tmp = std::to_string(mUserID);
    
    query_string =  "SELECT preferred_name,user_id FROM ";
    query_string += table_name;
    query_string += " ";
    query_string += " WHERE user_id='";
    query_string += tmp;
    query_string += "'";
    int retval = query(true);

	m_row = mysql_fetch_row( m_result );
    while ( m_row ) 
    {		
		mList += m_row[0];
		m_row = mysql_fetch_row( m_result );		
		if (m_row)  mList += ", ";				
    }
    printf("ALL DEVICES: %s\n\n", mList.c_str() );
    return retval;
}


int	SQL_Devices::sql_bump_login_count( std::string mHostname, int mUser_id )
{
	std::ostringstream query_str;	
    query_str <<  "UPDATE " << table_name << " SET login_count = login_count+1";
    query_str << " WHERE hostname='" << mHostname;
    query_str << "' AND user_id='";
    query_str << mUser_id;
    query_str << "'; ";
    //std::cout << query_str.str();
    query_string = query_str.str();
    //printf("%s\n", query_string.c_str() );
    return query(false); 
}

int	SQL_Devices::sql_update_user_id( std::string mHostname, int mUser_id )
{
	std::ostringstream query_str;	
    query_string =  "UPDATE ";
    query_string += table_name;
    query_string += " ";
    query_string += " SET user_id='";
    query_string += mUser_id;
    query_string += "'";
    query_string += " WHERE hostname='";
    query_string += mHostname;
    query_string += "';";
    return query(false); 
}


void SQL_Devices::extract_result	( )		// for the last fetched row.
{
    if (m_row) {
        for (int r=0; r<8; r++)
            readings[r] = atof(m_row[r+2]);
    }
}

#include <iostream>

void test_devices_db()
{
    // Add some samples rows : 
    connect_to_users_db();
    
    std::string mRegex;
    int user_id = sql_users.sql_find("stenniswood");  
    printf("User_id=%d\n", user_id );
    sql_devices.get_all_device_names_regex ( user_id, mRegex );    


    Sentence phrase = "main TV";
    int result = phrase.regex_find( mRegex );
    printf("REGEX result: %d\n", result );
	if (result) {
		std::string tmp = phrase.m_sentence.regex_matches[1];    
		printf("REGEX MATCH: %s\n", tmp.c_str());
    }
    
    std::string tmp = "(";
	tmp += append_int( user_id );
    
    std::string tmp1 = tmp + ", 'successroad1', '(main TV|TV)', 'RPI')";
	std::string tmp2 = tmp + ", 'successroad2', '(bedroom TV)', 'RPI')";
	std::string tmp3 = tmp + ", 'portable', '(LCD pi)', 'RPI')";

//	sql_devices.sql_add_if_not_already(user_id, dev_info_map );
//	sql_devices.sql_add_if_not_already(user_id, "( user_id, device_name, preferred_name, device_type )", tmp1, "successroad1");
//	sql_devices.sql_add_if_not_already(user_id, "( user_id, device_name, preferred_name, device_type )", tmp2, "successroad2");
//	sql_devices.sql_add_if_not_already(user_id, "( user_id, device_name, preferred_name, device_type )", tmp3, "portable");
	
	sql_devices.sql_find( user_id,"successroad1" );	sql_devices.print_results();
	sql_devices.sql_find( user_id,"successroad2" );	sql_devices.print_results();
	sql_devices.sql_find( user_id,"portable" );	sql_devices.print_results();


	printf("Done with test_devices_db()\n");
}

