//
//  sql_people.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 9/25/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef sql_users_hpp
#define sql_users_hpp

#include <stdio.h>
#include <string>
#include <ctime>

#include <my_global.h>
#include <mysql.h>

/*  The purpose of this class will be to store and retrieve events.
 Primarily from and for the machine vision (xeyes) app.
 This will answer questions such as :
 "When was the last time you saw ______"
 "Who did you meet today?"
 "Did you see anyone you did't know?"
 "Have you seen John?"
 "Show me what you saw"  Plays recorded video.
 */

//using namespace std;

/*
 */
class SQL_Users
{
public:
    SQL_Users ();
    ~SQL_Users();
    
    void			create_users_table ( );
    bool			users_table_exists ( );
    void			close_connection();
    
    int				query                   ( bool mRetrieving=true );

	// Please use if_not_already to prevent duplicates in the database!
	int				sql_add_if_not_already( std::string mFields, std::string mValues, std::string mUsername  );
    int				sql_add				( std::string mName, std::string mPasswd  );
    int				sql_find			( std::string mName						 );
    bool			verify_password		( std::string mName, std::string mPassword );
    int				sql_update_password ( std::string mName, std::string mPasswd );
    
/* NOTE: These remaining functions can possibly be removed (copied from friends_db for vision event processsing)*/
    int				sql_add_system_activated( bool mCamera, bool mEyes, bool mNeck );
    int				sql_add_system_deactivated( );
    int				sql_add_event           ( std::string mPersonName, std::string mDescription );
    int				sql_query_event         ( std::string mPersonName, std::string mDescription );
    int				sql_query_time          ( std::string mPersonName, std::string mDescription,
                                              struct tm start_time_bd, struct tm end_time_bd );		// including "unknown"
    int				sql_query_time_qualitative( std::string mDateTime);	// such as "yesterday", "today", "4 hours ago"
    void			print_row               ( );
    void			print_results           ( );

    int				form_response__how_many_times( std::string mNames, std::string mDescription,
                                                  struct tm start_time_bd, struct tm end_time_bd );
    int				form_response__last_time_i_saw	( std::string mPerson, time_t& mTime);
    int				form_response__saw_for_duration	( std::string mPerson, time_t& mTime);
    int				form_response__hours_active		( std::string mDescription, time_t& mTime,
                                                     struct tm start_time_bd, struct tm end_time_bd );
    void			form_response__during_that_time_i_saw ( struct tm start_time_bd, struct tm end_time_bd, std::string& mResponse);
    int				form_response__last_time_i_activated  ( time_t& mTime );
    int				form_response__last_time_i_deactivated( time_t& mTime );
    
    void			extract_result	  ();		// for the last fetched row.
    
    MYSQL_ROW		goto_first_row	  ();
    MYSQL_ROW		goto_next_row	  ();
    int				get_number_results();
    int				get_number_columns();

public:
    MYSQL_RES*		m_result;
    MYSQL_ROW		m_row;
    size_t 			m_row_count;
    float			readings[8];

    // DATA :
    int				_id;
    int				m_user_id;
    std::string			query_string;
};

extern SQL_Users sql_users;

void	test_users_db();


#endif /* sql_people_hpp */
