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
#include "vision_logger.hpp"
#include "sequencer_memory.hpp"
#include "sql_common.hpp"


#define Debug 0
MYSQL 		 	*vision_logger_db = NULL;
Event_SQL_Logger vis_sql_logger;


/*********************************************************************************/
Event_SQL_Logger::Event_SQL_Logger()
{

}
Event_SQL_Logger::~Event_SQL_Logger()
{

}
void Event_SQL_Logger::connect_to_logger_db()
{
    vision_logger_db = mysql_init(NULL);
    if (vision_logger_db == NULL)
    {
        fprintf(stderr, "init %s\n", mysql_error(vision_logger_db));
        exit(1);
    }
    if (mysql_real_connect(vision_logger_db, "localhost", "root", "password",
                           "robot_local", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "real_connect %s\n", mysql_error(vision_logger_db));
        printf( "Event_SQL_Logger::real_connect %s\n", mysql_error(vision_logger_db));
        mysql_close(vision_logger_db);
        vision_logger_db = NULL;
    }
}

void Event_SQL_Logger::create_events_table( )	
{
	bool exists = vis_sql_logger.events_table_exists();
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

int	Event_SQL_Logger::get_number_columns()
{
	unsigned int cols = mysql_num_fields( m_result );
	return cols;
}

int  Event_SQL_Logger::query( bool mRetrieving )
{
  printf("%s\n", query_string.c_str() );
  if (vision_logger_db==NULL) return 0;
  
  if (mysql_query(vision_logger_db, query_string.c_str() ))
  {
      fprintf(stderr, "Object: %s\n", mysql_error(vision_logger_db));
  }
  // Question : what happens when no results?
  //   Answer : if  mysql_store_result returns a non-zero value
  if (mRetrieving) {
	  m_result = mysql_store_result(vision_logger_db);
	  if (m_result == NULL)  {
		  Dprintf("query Error result==null!\n");
		  return 0;
	  }
	  m_row_count = mysql_num_rows( m_result );
  }
  Dprintf("row_count=%d\n", m_row_count);
  return m_row_count;
}

int	Event_SQL_Logger::get_number_results()
{
	return mysql_num_rows(m_result);
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
	return query(false);
}

int	Event_SQL_Logger::sql_add_system_deactivated( )
{
	string description = "Vision System De-activated";
	query_string =  "INSERT INTO robot_local.vision_events (timestamp, event_description) ";
	query_string += "VALUES ( NOW(),'";
	query_string += description;
	query_string += "' );";
	printf("\n%s\n", query_string.c_str() );
	return query(false);
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
	return query(false);	
}

int	Event_SQL_Logger::sql_query_event( string mPersonName, string mDescription )		// including "unknown"
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

int	Event_SQL_Logger::sql_query_time( string mPersonName, string mDescription, 
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
	return 0;
}

int Event_SQL_Logger::form_response__how_many_times( string mNames, string mDescription, 
										struct tm start_time_bd, struct tm end_time_bd )
{	
	sql_query_time( mNames, mDescription, start_time_bd, end_time_bd );
	int retval = get_number_results();
	return retval;
}

int Event_SQL_Logger::form_response__last_time_i_saw( string mPerson, time_t& mTime )
{
	sql_query_event( mPerson, "face left" );
	m_row    = mysql_fetch_row( m_result );
	m_row_count = get_number_results();
	int cols = get_number_columns();
	printf("m_row_count=%d; cols=%d; \n", m_row_count, cols );

	mTime    = atoi( m_row[cols-1] );		
	printf("m_row[%d]=%s;  %d\n", cols, m_row[cols-1], mTime );
	return m_row_count;
}
/* how much time from face detected to face left? */
int	Event_SQL_Logger::form_response__saw_for_duration( string mPerson, time_t& mTime)
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
	printf("m_row_count=%d; cols=%d; \n", m_row_count, cols );

	sTime    = atoi( m_row[cols-1] );
	eTime    = atoi( m_row[cols-1] );
	mTime = difftime(sTime,eTime);
	
	printf("m_row[%d]=%s;  %d\n", cols, m_row[cols-1], mTime );
	return m_row_count;
}

int	Event_SQL_Logger::form_response__hours_active( string mDescription, time_t& mTime, 
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
	printf("m_row_count=%d; cols=%d; \n", m_row_count, cols );
	int row =0;
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

void Event_SQL_Logger::form_response__during_that_time_i_saw( struct tm start_time_bd, 
						struct tm end_time_bd, string& mResponse)
{
	
}

int Event_SQL_Logger::form_response__last_time_i_activated( time_t& mTime)
{
	sql_query_event( "", "System Activated" );
	m_row = mysql_fetch_row( m_result );
	int rows = get_number_results();
	int cols = get_number_columns();
	mTime = atoi( m_row[cols-1] );		
	return rows;
}

int Event_SQL_Logger::form_response__last_time_i_deactivated( time_t& mTime)
{
	sql_query_event( "", "System De-activated" );
	m_row = mysql_fetch_row( m_result );
	int rows = get_number_results();
	int cols = get_number_columns();
	mTime = atoi( m_row[cols-1] );		
	return rows;
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


void test_vision_logger()
{
	// Add some samples rows : 
	vis_sql_logger.connect_to_logger_db();
	
	vis_sql_logger.sql_add_event("Unknown", "Face Detected" );
	vis_sql_logger.sql_add_event("Unknown", "Face Detected");
/*	vis_sql_logger.sql_add_event("Stephen", "Face Left"    );
	vis_sql_logger.sql_add_event("David",   "Face Detected" );
	vis_sql_logger.sql_add_event("David",   "Face Left"  	);
	vis_sql_logger.sql_add_event("Lynn",    "Face Left"  	);*/

}

