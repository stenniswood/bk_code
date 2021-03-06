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
//#include "calendar.hpp"
//#include "calendar_entry.hpp"
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


void Event_SQL_Logger::create_events_table( )		// logs raw CAN traffic.
{
  query_string = "CREATE TABLE `can_log` (\
  `_id` int(11) NOT NULL AUTO_INCREMENT,\
  `timestamp` timestamp NULL DEFAULT NULL,\
  `id` int(11) DEFAULT NULL,\
  `instance` tinyint(4) DEFAULT NULL,\
  `dlc` tinyint(4) DEFAULT NULL,\
  `data_0` tinyint(4) DEFAULT '0',\
  `data_1` tinyint(4) DEFAULT NULL,\
  `data_2` tinyint(4) DEFAULT NULL,\
  `data_3` tinyint(4) DEFAULT NULL,\
  `data_4` tinyint(4) DEFAULT NULL,\
  `data_5` tinyint(4) DEFAULT NULL,\
  `data_6` tinyint(4) DEFAULT NULL,\
  `data_7` tinyint(4) DEFAULT NULL,\
  PRIMARY KEY (`_id`)\
  ) ENGINE=InnoDB DEFAULT CHARSET=latin1";  
  query(false);
}

void Event_SQL_Logger::create_viki_table( )		// logs viki proceedings.
{
	query_string = "CREATE  TABLE `bk_useraccounts`.`viki_log` ( \
  `_id` INT NOT NULL AUTO_INCREMENT ,\
  `user_statement` TEXT NULL ,\
  `viki_response` TEXT NULL ,\
  `timestamp` TIMESTAMP NULL ,\
  PRIMARY KEY (`_id`) );";
  query(false);	

}

char* form_date_string( struct tm& time_bd )
{
	static char tmp[16];
	sprintf(tmp, "%4d-%d-%d", time_bd.tm_year+1900,
								time_bd.tm_mon,
								time_bd.tm_mday );
	return tmp;
}
char* form_time_string( struct tm& time_bd )
{
	static char tmp[16];
	sprintf(tmp, "%2d:%2d:%2d", time_bd.tm_hour,
								time_bd.tm_min,
								time_bd.tm_sec );	
	return tmp;
}
char* form_date_time_string( struct tm& time_bd )
{
	static char tmp[32];
	strcpy(tmp, form_date_string(time_bd));
	strcat(tmp, " ");
	strcat(tmp, form_time_string(time_bd));
	return tmp;
}

void Event_SQL_Logger::find_reading( string mDataType, struct tm start_time_bd, 
							   struct tm end_time_bd,	int mUser_id )
{
	query_string =  "SELECT * FROM bk_useraccounts.data_log WHERE ";
	query_string += "bk_useraccounts.data_log.timestamp BETWEEN '";
	query_string += form_date_string(start_time_bd);
	query_string += "' AND '";		
	query_string += form_date_string( end_time_bd );
	query_string += "' AND read_type='";	
	query_string += mDataType;
	query_string += "' ;";		
	printf("   query_string=%s\n", query_string.c_str() );
	query(true);
	dprintf("Num Results=%d\n", get_number_results() );
}

char* append_float( float mFloat )
{
	static char tmp[16];
	sprintf(tmp, "'%6.2f'", mFloat );
	return tmp;
}



void Event_SQL_Logger::add_body_position( struct stBodyPositionVector& mRead )
{
	float num_elements = sizeof(struct stBodyPositionVector)/ sizeof(float);
	float* ptr = (float*)&mRead;
	
	query_string = "INSERT INTO bk_useraccounts.data_log VALUES ('0', CURRENT_TIMESTAMP,";	
	for (int r=0; r<8; r++)
	{
		query_string += append_float(ptr[r]);		
		query_string += ", ";
	}
	query_string += "'Body Position'";					query_string += ", ";
	query_string += "'servo group 1'";				
	query_string += "); ";
	query(false);

	query_string = "INSERT INTO bk_useraccounts.data_log VALUES ('0', CURRENT_TIMESTAMP,";	
	for (int r=8; r<8*2; r++)
	{
		query_string += append_float(ptr[r]);		
		query_string += ", ";
	}
	query_string += "'Body Position'";					query_string += ", ";
	query_string += "'servo group 2'";				
	query_string += "); ";
	query(false);
	
	query_string = "INSERT INTO bk_useraccounts.data_log VALUES ('0', CURRENT_TIMESTAMP,";	
	for (int r=16; r<8*3; r++)
	{
		query_string += append_float(ptr[r]);		
		query_string += ", ";
	}
	query_string += "'Body Position'";					query_string += ", ";
	query_string += "'servo group 3'";				
	query_string += "); ";
	query(false);	
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


