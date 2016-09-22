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
//#include "bk_system_defs.h"
#include "global.h"
#include "viki_logger.hpp"
#include "sequencer_memory.hpp"


#define Debug 1
static MYSQL 	*logger_db = NULL;
Viki_SQL_Logger viki_logger;
#define DBASE_NAME 		"bk_useraccounts"
#define VIKI_TABLE_NAME "viki_log"


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
                           DBASE_NAME, 0, NULL, 0) == NULL)
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

  query_string = "CREATE TABLE `viki_log` ( \
  `_id` int(11) NOT NULL AUTO_INCREMENT, \
  `user_name` text, \
  `user_statement` text, \
  `viki_response` text, \
  `timestamp` timestamp NULL DEFAULT NULL, \
  PRIMARY KEY (`_id`) \
) ENGINE=InnoDB DEFAULT CHARSET=latin1";
  query(false);
}

bool Viki_SQL_Logger::viki_table_exists( )
{
	// how to query it?
	query_string = "SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = '";
	query_string += DBASE_NAME;
	query_string += "' AND  TABLE_NAME = '";
	query_string += VIKI_TABLE_NAME;
	query_string += "';";
	query(true);
	int result_count = get_number_results();
	return (result_count>0);
}

int	Viki_SQL_Logger::get_number_columns()
{
	unsigned int cols = mysql_num_fields( m_result );
	return cols;
}

int Viki_SQL_Logger::query( bool mRetrieving )
{
  Dprintf("%s\n", query_string.c_str() );	
  if (mysql_query(logger_db, query_string.c_str() ))
  {
      fprintf(stderr, "Object: %s\n", mysql_error(logger_db));
  }
  // Question : what happens when no results?
  //   Answer : if  mysql_store_result returns a non-zero value
  if (mRetrieving) {
	  m_result = mysql_store_result(logger_db);
	  if (m_result == NULL)  {
		  Dprintf("query Error result==null!\n");
		  return 0; 
	  }
	  m_row_count = mysql_num_rows( m_result );
  }    
  Dprintf("row_count=%d\n", m_row_count);
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

int	Viki_SQL_Logger::sql_add_phrase( string mStatement, string mResponse, string mUser )
{
	query_string =  "INSERT INTO bk_useraccounts.";
	query_string += VIKI_TABLE_NAME;
	query_string += " (user_name, user_statement, viki_response, timestamp) ";
	query_string += "VALUES ( '";
	query_string += mUser;			query_string += "', '";
	query_string += mStatement;		query_string += "', '";
	query_string += mResponse;		query_string += "', ";
	query_string += " NOW() );";
	query(false);	
}

int	Viki_SQL_Logger::sql_query_phrase( string mStatement, string mUser )		// including "unknown"
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

/*int Viki_SQL_Logger::form_response__last_time_i_saw( string mStatement, string mUser, time_t& mTime )
{
	int last_col = get_number_columns();
	sql_query_phrase( mStatement, mUser );
	m_row    = mysql_fetch_row( m_result );
	int rows = get_number_results();
	int cols = get_number_columns();
	mTime    = atoi( m_row[cols-1] );		
	return rows;
}*/

int Viki_SQL_Logger::form_response__last_time_i_activated( time_t& mTime)
{
	sql_query_phrase( "", "System Activated" );
	m_row = mysql_fetch_row( m_result );
	int rows = get_number_results();
	int cols = get_number_columns();
	mTime = atoi( m_row[cols-1] );		
	return rows;
}

int Viki_SQL_Logger::form_response__last_time_i_deactivated( time_t& mTime)
{
	sql_query_phrase( "", "System De-activated" );
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

int	Viki_SQL_Logger::sql_add_system_activated( bool mCamera, bool mEyes, bool mNeck )
{
	string description = "Vision System Activated";
	string names;
	if (mCamera)
		names += "Camera On";
	if (mEyes)
		names += "Eyes On";
	if (mNeck)
		names += "Neck On";
	sql_add_phrase(description.c_str(), names.c_str(), "System" );
}

int	Viki_SQL_Logger::sql_add_system_deactivated( )
{
	string description = "Vision System De-activated";
	sql_add_phrase(description.c_str(), "", "System" );	
}

 
void test_viki_logger()
{
	viki_logger.connect_to_logger_db();
	
	viki_logger.sql_add_phrase("When did you last see David?", 					"VR:I saw him yesterday.", 			  "Stephen" );
	viki_logger.sql_add_phrase("How long have you been activated?", 			"VR:I have been active for 4 hours.", "Stephen" );
	viki_logger.sql_add_phrase("How many times were you activated yesterday?", 	"VR:yesterday, 5 times", 			  "Stephen" );	
	printf("Added 3 entries\n");
	exit(1);	
}

