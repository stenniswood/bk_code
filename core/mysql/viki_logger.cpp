#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <vector>
#include <time.h>
#include <stdint.h>

#include "global.h"
#include "viki_logger.hpp"
#include "sequencer_memory.hpp"
#include <my_global.h>
#include <mysql.h>
#include "machine_defs.h"

#define Debug 0
static MYSQL 	*logger_db = NULL;
Viki_SQL_Logger viki_logger;
const char DBASE_NAME[]="sjtennis_bk_useraccounts";
const char VIKI_TABLE_NAME[]="viki_proceedings";


static void object_finish_with_error( )
{
    fprintf    ( stderr, "%s\n", mysql_error(logger_db));
    mysql_close( logger_db );
    exit(1);
}

/*********************************************************************************/
Viki_SQL_Logger::Viki_SQL_Logger()
{
	connect_to_logger_db();
	bool exists = viki_table_exists();
	if (exists==false)
		create_viki_table();
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
    if (mysql_real_connect(logger_db, "localhost", sql_username, sql_password,
                           DBASE_NAME, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "real_connect %s\n", mysql_error(logger_db));
        printf("real_connect %s\n", mysql_error(logger_db));
        mysql_close(logger_db);
		logger_db = NULL;
    }
}

/* which are online? */

void Viki_SQL_Logger::create_viki_table( )	
{
	query_string = "CREATE TABLE `viki_proceedings` (	\
  `entry_id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'count of the input statements',	\
  `user_id` int(11) NOT NULL,							\
  `device_id` int(11) DEFAULT NULL,						\
  `session_id` int(11) NOT NULL,						\
  `user_input` text NOT NULL,							\
  `viki_response` text NOT NULL,						\
  `date_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,	\
  PRIMARY KEY (`entry_id`)								\
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='Stores a history of user input to viki'";

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
  if (logger_db==NULL) return 0;
  
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

int	Viki_SQL_Logger::sql_add_phrase( string mStatement, string mResponse, int mUser )
{
	query_string =  "INSERT INTO ";
	query_string += DBASE_NAME;
	query_string += ".";
	query_string += VIKI_TABLE_NAME;
	query_string += " (user_id, user_input, viki_response, date_time) ";
	query_string += "VALUES ( '";
	query_string += "47";			query_string += "', \""; 
	query_string += mStatement;		query_string += "\", \"";
	query_string += mResponse;		query_string += "\", ";
	query_string += " NOW() );";
	//printf(query_string.c_str() );
	query(false);	
}

int	Viki_SQL_Logger::sql_query_user_input( string mUserStatement, int mUser )		// including "unknown"
{
	struct tm start_time_bd;
	struct tm end_time_bd;
	query_string =  "SELECT *,UNIX_TIMESTAMP(date_time) FROM ";
	query_string += DBASE_NAME;
	query_string += ".";
	query_string += VIKI_TABLE_NAME;
	query_string += " WHERE ";
	if (mUser!=-1) 
	{ 
		query_string += "user_input LIKE '%";
		query_string += mUserStatement;
		query_string += "%' AND ";
		query_string += "user_id= ";
		char tmp[10] = "47";		// mtenniswood
		//itoa( mUser, tmp, 10 );
		query_string += tmp;
	}
	query_string += " ORDER BY timestamp DESC;";
	//printf(query_string.c_str());
	query(true);
}

int	Viki_SQL_Logger::sql_query_viki_response( string mResponse, int mUser )		// including "unknown"
{
	struct tm start_time_bd;
	struct tm end_time_bd;
	query_string =  "SELECT *,UNIX_TIMESTAMP(date_time) FROM ";
	query_string += DBASE_NAME;
	query_string += ".";
	query_string += VIKI_TABLE_NAME;
	query_string += " WHERE ";
//	if (mUser.length()) 
	{
		query_string += "viki_response LIKE '%";
		query_string += mResponse;
		query_string += "%' AND ";
	}
	query_string += "user_id= ";
	query_string += "47";  //itoa(mUser);
	query_string += " ORDER BY timestamp DESC;";
	printf(query_string.c_str());
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
	sql_query_viki_response( "Viki System Activated", 47 );
	m_row = mysql_fetch_row( m_result );
	int rows = get_number_results();
	int cols = get_number_columns();
	mTime = atoi( m_row[cols-1] );		
	return rows;
}

int Viki_SQL_Logger::form_response__last_time_i_deactivated( time_t& mTime)
{
	sql_query_viki_response( "Viki System De-activated", 47 );
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
	string description = "Viki System Activated";
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
	string description = "Viki System De-activated";
	sql_add_phrase(description.c_str(), "", "System" );	
}

 
void test_viki_logger()
{
//	viki_logger.connect_to_logger_db();	
//	viki_logger.sql_query_user_input("When did you last see David?", 					"VR:I saw him yesterday.", 		  "Stephen" );
//	viki_logger.sql_query_user_input("How long have you been activated?", 			"VR:I have been active for 4 hours.", "Stephen" );
//	viki_logger.sql_query_user_input("How many times were you activated yesterday?", 	"VR:yesterday, 5 times", 		  "Stephen" );
	printf("Added 3 entries\n");
	exit(1);	
}

