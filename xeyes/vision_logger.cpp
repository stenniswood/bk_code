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
#include "vision_logger.hpp"
#include "sequencer_memory.hpp"


//extern MYSQL *logger_db;
MYSQL 		 *logger_db = NULL;
#define Debug 0

Event_SQL_Logger sql_logger;

static void object_finish_with_error( )
{
    fprintf    ( stderr, "%s\n", mysql_error(logger_db));
    mysql_close( logger_db );
    exit(1);
}

/*********************************************************************************/
Event_SQL_Logger::Event_SQL_Logger()
{

}
Event_SQL_Logger::~Event_SQL_Logger()
{

}
void Event_SQL_Logger::connect_to_logger_db()
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

void Event_SQL_Logger::create_events_table( )	
{
	bool exists = sql_logger.events_table_exists();
	if (exists) 
	{ 
		//printf("TABLE Already Exists\n");
		return ;
	}

  query_string = "CREATE TABLE `vision_events` (\
  `_id` int(11) NOT NULL AUTO_INCREMENT,\
  `timestamp` timestamp NULL DEFAULT NULL,\
  `event_description` TEXT NULL ,\
  `people_names` TEXT NULL ,\
  `video_files` TEXT NULL ,\
  `data_0` tinyint(4) DEFAULT '0',\
  `data_1` tinyint(4) DEFAULT NULL,\
  `data_2` tinyint(4) DEFAULT NULL,\
  `data_3` tinyint(4) DEFAULT NULL,\
  PRIMARY KEY (`_id`)\
  ) ENGINE=InnoDB DEFAULT CHARSET=latin1";
  query(false);
}

bool Event_SQL_Logger::events_table_exists( )
{
	// how to query it?
	query_string = "SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'robot_local' AND  TABLE_NAME = 'vision_events';";
	query(true);
	int result_count = get_number_results();
	return (result_count>0);
}

int  Event_SQL_Logger::query( bool mRetrieving )
{
  dprintf("%s\n", query_string.c_str() );	
  if (mysql_query(logger_db, query_string.c_str() ))
  {
      fprintf(stderr, "Object: %s\n", mysql_error(logger_db));
  }
  int row_count=0;
  // Question : what happens when no results?
  //   Answer : if  mysql_store_result returns a non-zero value
  if (mRetrieving) {
	  m_result = mysql_store_result(logger_db);
	  if (m_result == NULL)  {
		  dprintf("query Error result==null!\n");
		  return 0; 
	  }
	  row_count = mysql_num_rows(m_result);
  }    
  dprintf("row_count=%d\n", row_count);
  return row_count;
}

int	Event_SQL_Logger::get_number_results()
{
	return mysql_num_rows(m_result);
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

int	Event_SQL_Logger::sql_add_system_activated( bool mCamera, bool mEyes, bool mNeck )
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
}

int	Event_SQL_Logger::sql_add_system_deactivated( )
{
	string description = "Vision System De-activated";
	query_string =  "INSERT INTO robot_local.vision_events (timestamp, event_description) ";
	query_string += "VALUES ( NOW(),'";
	query_string += description;
	query_string += "' );";
	printf("\n%s\n", query_string.c_str() );
	query(false);
}

int	Event_SQL_Logger::sql_add_event( string mPersonName, string mDescription )
{
	query_string =  "INSERT INTO robot_local.vision_events (timestamp, event_description, people_names) ";
	query_string += "VALUES ( NOW(),'";
	query_string += mDescription;
	query_string += "', '";
	query_string += mPersonName;
	query_string += "');";	
	printf("\n%s\n", query_string.c_str() );
	query(false);	
}

int	Event_SQL_Logger::sql_query_event( string mPersonName, string mDescription )		// including "unknown"
{
	struct tm start_time_bd;
	struct tm end_time_bd;
	query_string =  "SELECT * FROM robot_local.vision_events WHERE ";
	query_string += "robot_local.vision_events.people_names LIKE '%";
	query_string += mPersonName;
	query_string += "%' AND event_description LIKE '%";
	query_string += mDescription;
	query_string += "%' ORDER BY timestamp DESC;";
	//printf("\n%s\n", query_string.c_str() );
	query(true);
}

int	Event_SQL_Logger::sql_query_time( string mPersonName, string mDescription, 
									   struct tm start_time_bd, struct tm end_time_bd  )		// including "unknown"
{
	query_string =  "SELECT * FROM robot_local.vision_events WHERE ";
	query_string += "robot_local.vision_events.timestamp BETWEEN '";
	query_string += form_date_time_string(start_time_bd);
	query_string += "' AND '";		
	query_string += form_date_time_string( end_time_bd );
	query_string += "' AND people_names='";	
	query_string += mPersonName;
	query_string += "' ";
	if (mDescription.length())
	{
		query_string += "AND event_description LIKE '%";
		query_string += mDescription;
		query_string += "'";
	}
	query_string += ";";
	printf("\n%s\n", query_string.c_str() );	
	query(true);	
}

void Event_SQL_Logger::print_row( )
{
	unsigned int cols = mysql_num_fields( m_result );
	for (int c=0; c<cols; c++)		
		if (m_row[c])
			printf("%s\t", m_row[c] );
	printf("\n");
}

void Event_SQL_Logger::print_results( )
{
	m_row = mysql_fetch_row( m_result );
	while ( m_row )
	{
		print_row( );
		m_row = mysql_fetch_row( m_result );
	}
	printf("\n");
}

int	Event_SQL_Logger::sql_query_time_qualitative( string mDateTime)	// such as "yesterday", "today", "4 hours ago"
{

}

void Event_SQL_Logger::form_response__how_many_times( struct tm start_time_bd, struct tm end_time_bd )
{
	sql_query_event( "David", "face detected" );
	print_results();
}

void Event_SQL_Logger::form_response__last_time_i_saw( string mPerson, string& mResponse)
{
	sql_query_event( mPerson, "face detected" );
	m_row = mysql_fetch_row( m_result );
	int rows = get_number_results();
	if (rows==0)
		mResponse = "According to my database, I have never met " + mPerson;
	else {
		mResponse  = "The last time I saw " + mPerson;
		mResponse += " was ";
		mResponse += m_row[1];
		mResponse += "\n";
	}
	//printf("%s\n", mResponse.c_str() );		
}

void Event_SQL_Logger::form_response__during_that_time_i_saw( struct tm start_time_bd, 
						struct tm end_time_bd, string& mResponse)
{

}

char* append_float( float mFloat )
{
	static char tmp[16];
	sprintf(tmp, "'%6.2f'", mFloat );
	return tmp;
}


MYSQL_ROW Event_SQL_Logger::goto_first_row()
{
	m_row = mysql_fetch_row( m_result );
	return m_row;
}

MYSQL_ROW Event_SQL_Logger::goto_next_row()
{
	m_row = mysql_fetch_row( m_result );
	return m_row;
}

void Event_SQL_Logger::extract_result	( )		// for the last fetched row.
{
	if (m_row) {
		for (int r=0; r<8; r++)
			readings[r] = atof(m_row[r+2]);
	}
}



