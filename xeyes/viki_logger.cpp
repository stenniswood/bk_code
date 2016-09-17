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
#include "bk_system_defs.h"
#include "viki_logger.hpp"
#include "sequencer_memory.hpp"


#define Debug 1
static MYSQL 		 	*logger_db = NULL;
Viki_SQL_Logger viki_logger;

static void object_finish_with_error( )
{
    fprintf    ( stderr, "%s\n", mysql_error(logger_db));
    mysql_close( logger_db );
    exit(1);
}

/*********************************************************************************/
Viki_SQL_Logger::Viki_SQL_Logger()
{

}
Viki_SQL_Logger::~Viki_SQL_Logger()
{

}
void Viki_SQL_Logger::connect_to_logger_db()
{
    logger_db = mysql_init(NULL);
    if (logger_db == NULL)
    {
        fprintf(stderr, "init %s\n", mysql_error(logger_db));
        exit(1);
    }
    if (mysql_real_connect(logger_db, "localhost", "root", "password",
                           "robot_local", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "real_connect %s\n", mysql_error(logger_db));
        mysql_close(logger_db);
        exit(1);
    }
}

void Viki_SQL_Logger::create_viki_table( )	
{
	bool exists = viki_logger.viki_table_exists();
	if (exists) 
	{ 
		//printf("TABLE Already Exists\n");
		return ;
	}

  query_string = "CREATE TABLE `vision_events` (\
  `_id` int(11) NOT NULL AUTO_INCREMENT,\
  `timestamp` timestamp NULL DEFAULT NULL,\
  `viki_statement` TEXT NULL ,\
  `user_name` TEXT NULL ,\
  `video_files` TEXT NULL ,\
  `data_0` tinyint(4) DEFAULT '0',\
  `data_1` tinyint(4) DEFAULT NULL,\
  `data_2` tinyint(4) DEFAULT NULL,\
  `data_3` tinyint(4) DEFAULT NULL,\
  PRIMARY KEY (`_id`)\
  ) ENGINE=InnoDB DEFAULT CHARSET=latin1";
  query(false);
}

bool Viki_SQL_Logger::viki_table_exists( )
{
	// how to query it?
	query_string = "SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'robot_local' \
	AND  TABLE_NAME = 'viki_events';";
	query(true);
	int result_count = get_number_results();
	return (result_count>0);
}

int	Viki_SQL_Logger::get_number_columns()
{
	unsigned int cols = mysql_num_fields( m_result );
	return cols;
}

int  Viki_SQL_Logger::query( bool mRetrieving )
{
  dprintf("%s\n", query_string.c_str() );	
  if (mysql_query(logger_db, query_string.c_str() ))
  {
      fprintf(stderr, "Object: %s\n", mysql_error(logger_db));
  }
  // Question : what happens when no results?
  //   Answer : if  mysql_store_result returns a non-zero value
  if (mRetrieving) {
	  m_result = mysql_store_result(logger_db);
	  if (m_result == NULL)  {
		  dprintf("query Error result==null!\n");
		  return 0; 
	  }
	  m_row_count = mysql_num_rows( m_result );
  }    
  dprintf("row_count=%d\n", m_row_count);
  return m_row_count;
}

int	Viki_SQL_Logger::get_number_results()
{
	return mysql_num_rows(m_result);
}

extern char* form_date_string		( struct tm& time_bd );
extern char* form_time_string		( struct tm time_bd  );
extern char* form_date_time_string	( struct tm& time_bd );
extern char* append_float			( float mFloat );

int	Viki_SQL_Logger::sql_add_event( string mStatement, string mUser )
{
	query_string =  "INSERT INTO robot_local.vision_events (timestamp, viki_statement, user_name) ";
	query_string += "VALUES ( NOW(),'";
	query_string += mStatement;
	query_string += "', '";
	query_string += mUser;
	query_string += "');";	
	printf("\n%s\n", query_string.c_str() );
	query(false);	
}

int	Viki_SQL_Logger::sql_query_event( string mStatement, string mUser )		// including "unknown"
{
	struct tm start_time_bd;
	struct tm end_time_bd;
	query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM robot_local.vision_events WHERE ";
	if (mUser.length()) {
		query_string += "robot_local.vision_events.people_names LIKE '%";
		query_string += mStatement;
		query_string += "%' AND ";
	}
	query_string += "event_description LIKE '%";
	query_string += mUser;
	query_string += "%' ORDER BY timestamp DESC;";
	query(true);
}

int	Viki_SQL_Logger::sql_query_time( string mStatement, string mUser, 
									   struct tm start_time_bd, struct tm end_time_bd  )		// including "unknown"
{
	query_string =  "SELECT *,UNIX_TIMESTAMP(timestamp) FROM robot_local.vision_events WHERE ";
	query_string += "robot_local.vision_events.timestamp BETWEEN '";
	query_string += form_date_time_string(start_time_bd);
	query_string += "' AND '";
	query_string += form_date_time_string( end_time_bd );
	query_string += "' ";
	if (mUser.length())
	{
		query_string += "AND user_names='";
		query_string += mStatement;
		query_string += "' ";
	}
	if (mStatement.length())
	{
		query_string += "AND event_description LIKE '%";
		query_string += mUser;
		query_string += "%'";
	}
	query_string += " ORDER BY timestamp DESC;";
	//printf("\n%s\n", query_string.c_str() );
	query(true);
}

void Viki_SQL_Logger::print_row( )
{
	unsigned int cols = mysql_num_fields( m_result );
	for (int c=0; c<cols; c++)		
		if (m_row[c])
			printf("%s\t", m_row[c] );
	printf("\n");
}

void Viki_SQL_Logger::print_results( )
{
	m_row = mysql_fetch_row( m_result );
	while ( m_row )
	{
		print_row( );
		m_row = mysql_fetch_row( m_result );
	}
	printf("\n");
}

int Viki_SQL_Logger::form_response__how_many_times( string mStatement, string mUser, 
										struct tm start_time_bd, struct tm end_time_bd )
{	
	sql_query_time( mStatement, mUser, start_time_bd, end_time_bd );
	int retval = get_number_results();
	return retval;
}

int Viki_SQL_Logger::form_response__last_time_i_saw( string mStatement, time_t& mTime )
{
	int last_col = get_number_columns();
	sql_query_event( mStatement, "face detected" );
	m_row    = mysql_fetch_row( m_result );
	int rows = get_number_results();
	int cols = get_number_columns();
	//printf("m_row[%d]=%s;  %d\n", cols, m_row[cols-1], mTime );
	mTime    = atoi( m_row[cols-1] );		
	return rows;
}

int Viki_SQL_Logger::form_response__last_time_i_activated( time_t& mTime)
{
	sql_query_event( "", "System Activated" );
	m_row = mysql_fetch_row( m_result );
	int rows = get_number_results();
	int cols = get_number_columns();
	mTime = atoi( m_row[cols-1] );		
	return rows;
}

int Viki_SQL_Logger::form_response__last_time_i_deactivated( time_t& mTime)
{
	sql_query_event( "", "System De-activated" );
	m_row = mysql_fetch_row( m_result );
	int rows = get_number_results();
	int cols = get_number_columns();
	mTime = atoi( m_row[cols-1] );		
	return rows;
}




MYSQL_ROW Viki_SQL_Logger::goto_first_row()
{
	m_row = mysql_fetch_row( m_result );
	return m_row;
}

MYSQL_ROW Viki_SQL_Logger::goto_next_row()
{
	m_row = mysql_fetch_row( m_result );
	return m_row;
}

void Viki_SQL_Logger::extract_result	( )		// for the last fetched row.
{
	if (m_row) {
		for (int r=0; r<8; r++)
			readings[r] = atof(m_row[r+2]);
	}
}



/*int	Viki_SQL_Logger::sql_add_system_activated( bool mCamera, bool mEyes, bool mNeck )
{
	string description = "Vision System Activated";
	string names;
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
	query(false);
}*/

/*int	Viki_SQL_Logger::sql_add_system_deactivated( )
{
	string description = "Vision System De-activated";
	query_string =  "INSERT INTO robot_local.vision_events (timestamp, event_description) ";
	query_string += "VALUES ( NOW(),'";
	query_string += description;
	query_string += "' );";
	printf("\n%s\n", query_string.c_str() );
	query(false);
}*/
