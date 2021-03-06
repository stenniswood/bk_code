#ifndef _VIKI_SQL_LOGGER_HPP_
#define _VIKI_SQL_LOGGER_HPP_

#include <string>
#include <ctime>
#include <my_global.h>
#include <mysql.h>
#include "global.h"

/*  The purpose of this class will be to store and retrieve events.
		Primarily from and for the machine vision (xeyes) app.
		This will answer questions such as :
		"When was the last time you saw ______"
		"Who did you meet today?"
		"Did you see anyone you did't know?"
		"Have you seen John?"
		"Show me what you saw"  Plays recorded video.
*/

using namespace std;

/*

*/
class Viki_SQL_Logger 
{
public:
	Viki_SQL_Logger ();
	~Viki_SQL_Logger();
	
	void			connect_to_logger_db( );
	void			create_viki_table ( );
	bool			viki_table_exists ( );
	
	int				query				( bool mRetrieving=true );		

	int				sql_add_phrase		( string mStatement, string mResponse, int mUser );
	int				sql_add_system_activated   ( bool mCamera, bool mEyes, bool mNeck );
	int				sql_add_system_deactivated ( );

	int				sql_query_user_input   ( string mStatement, int mUser ); 
	int				sql_query_viki_response( string mResponse,  int mUser );

	int				sql_query_time		( string mPersonName, string mDescription,  struct tm start_time_bd, struct tm end_time_bd );		// including "unknown"
	int				sql_query_time_qualitative( string mDateTime);	// such as "yesterday", "today", "4 hours ago"
	void			print_row			( );
	void			print_results		( );

	int				form_response__how_many_times( string mNames, string mDescription, 
												   struct tm start_time_bd, struct tm end_time_bd );
	void			form_response__during_that_time_i_saw ( struct tm start_time_bd, struct tm end_time_bd, string& mResponse);	
	int				form_response__last_time_i_activated  ( time_t& mTime );
	int				form_response__last_time_i_deactivated( time_t& mTime );
	
	void			extract_result		( );		// for the last fetched row.
	
	MYSQL_ROW		goto_first_row	  ();
	MYSQL_ROW		goto_next_row	  ();
	int				get_number_results();
	int				get_number_columns();	
public:
	MYSQL_RES*		m_result;
	MYSQL_ROW		m_row;
	int 			m_row_count;
	float			readings[8];

	// DATA :
	int				_id;	
	int				m_user_id;	
	string			query_string;
};

extern Viki_SQL_Logger viki_logger;
void 	test_viki_logger();

 
#endif

//	void			add_body_position	( struct stBodyPositionVector& mRead );
//	void			find_reading		( string mDataType, struct tm start_time_bd, struct tm end_time_bd,	int mUser_id = 1 );
//	int				sql_add_viki_text	( string mUserStatement, string mVikiResponse );			
//	int				sql_add_system_activated  ( bool mCamera, bool mEyes, bool mNeck );
//	int				sql_add_system_deactivated( );	
