#ifndef _VISION_SQL_LOGGER_HPP_
#define _VISION_SQL_LOGGER_HPP_

#include <string>
#include <ctime>
#include <my_global.h>
#include <mysql.h>


using namespace std;
#define BOOL unsigned char

class Event_SQL_Logger 
{
public:
	Event_SQL_Logger ();
	~Event_SQL_Logger();
	
	void			connect_to_logger_db( );

	void			create_events_table( );		// logs raw CAN traffic.
	void			create_viki_table( );		// logs viki proceedings.
	
	int				query				( bool mRetrieving=true );
		
	void			find_reading		( string mDataType, struct tm start_time_bd, struct tm end_time_bd,	int mUser_id = 1 );

	void			add_body_position	( struct stBodyPositionVector& mRead );
	
	int				sql_add_reading		( );
	int				sql_add_viki_text	( string mUserStatement, string mVikiResponse );
			
	void			sql_update			( );
	void			extract_result		( );		// for the last fetched row.
	
	MYSQL_ROW		goto_first_row();
	MYSQL_ROW		goto_next_row();
	int				get_number_results();
	

public:
	MYSQL_RES*		m_result;
	MYSQL_ROW		m_row;
	
	float			readings[8];
	
	// DATA :	
	int				_id;	
	int				m_user_id;
	
	string			query_string;
};

extern Event_SQL_Logger sql_logger;

 
#endif
