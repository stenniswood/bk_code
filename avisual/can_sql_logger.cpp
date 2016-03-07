#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <vector>
#include <time.h>
#include <my_global.h>
#include <mysql.h>
#include "bk_system_defs.h"
#include "calendar.hpp"
#include "calendar_entry.hpp"
#include "can_sql_logger.hpp"

//extern MYSQL *logger_db;
MYSQL 		 *logger_db = NULL;

#define Debug 0
//#define dprintf if (Debug) printf


static void object_finish_with_error( )
{
    fprintf    ( stderr, "%s\n", mysql_error(logger_db));
    mysql_close( logger_db );
    exit(1);
}

SQL_Logger::SQL_Logger()
{
}
SQL_Logger::~SQL_Logger()
{
}

void SQL_Logger::connect_to_logger_db()
{
    logger_db = mysql_init(NULL);
    if (logger_db == NULL)
    {
        fprintf(stderr, "init %s\n", mysql_error(logger_db));
        exit(1);
    }
    
    if (mysql_real_connect(logger_db, "localhost", "root", "password",
                           "bk_useraccounts", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "real_connect %s\n", mysql_error(logger_db));
        mysql_close(logger_db);
        exit(1);
    }
}

int  SQL_Logger::query( bool mRetrieving )
{
	printf("%s\n", query_string.c_str() );
	
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
		  printf("query Error result==null!\n");
		  return 0; 
	  }
	  row_count = mysql_num_rows(m_result);
  }    
  printf("row_count=%d\n", row_count);
  return row_count;
}

int	SQL_Logger::get_number_results()
{
	return mysql_num_rows(m_result);
}


void SQL_Logger::create_readings_table( )	// logs numbers for reporting (gps, gyro, loadcells, etc)
{
	query_string = "CREATE TABLE `data_log` (\
  `_id` int(11) NOT NULL AUTO_INCREMENT,\
  `timestamp` timestamp NULL DEFAULT NULL,\
  `read_1` float NOT NULL DEFAULT '0',\
  `read_2` float NOT NULL DEFAULT '0',\
  `read_3` float NOT NULL DEFAULT '0',\
  `read_4` float NOT NULL DEFAULT '0',\
  `read_5` float NOT NULL DEFAULT '0',\
  `read_6` float NOT NULL DEFAULT '0',\
  `read_7` float NOT NULL DEFAULT '0',\
  `read_8` float NOT NULL DEFAULT '0',\
  `read_type` varchar(45) DEFAULT NULL,\
  `description` varchar(45) DEFAULT NULL,\
  PRIMARY KEY (`_id`)\
) ENGINE=InnoDB DEFAULT CHARSET=latin1";
	query( false );
}
void SQL_Logger::create_CAN_table( )		// logs raw CAN traffic.
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

void SQL_Logger::create_viki_table( )		// logs viki proceedings.
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

void SQL_Logger::find_reading( struct tm start_time_bd, 
							   struct tm end_time_bd,	int mUser_id = 1 )
{
	query_string =  "SELECT * FROM bk_useraccounts.data_log WHERE ";
	query_string += "bk_useraccounts.data_log.timestamp BETWEEN '";
	query_string += form_date_string(start_time_bd);
	query_string += "' AND '";	
	query_string += form_date_string( end_time_bd );
	query_string += "';";	
	query(true);
	printf("Num Results=%d\n", get_number_results() );
}

char* append_float( float mFloat )
{
	static char tmp[16];
	sprintf(tmp, "'%6.2f'", mFloat );
	return tmp;
}

void SQL_Logger::add_loadcell( stLoadCellReading& mRead )
{
	char value[16];
	query_string = "INSERT INTO bk_useraccounts.data_log VALUES ('0', CURRENT_TIMESTAMP,";	
	for (int r=0; r<mRead.num_reads; r++)
	{
		query_string += append_float(mRead.sensor[r]);		
		query_string += ", ";
	}
	query_string += "'LOAD CELL'";					query_string += ", ";
	query_string += "'sensors 1..8'";				
	query_string += "); ";
	query(false);
}

void SQL_Logger::add_gyro( stGyroReading& mRead				 )
{
	char value[16];
	query_string = "INSERT INTO bk_useraccounts.data_log VALUES ('0', CURRENT_TIMESTAMP, ";	
	query_string += append_float(mRead.tilt);		query_string += ", ";
	query_string += append_float(mRead.pitch);		query_string += ", ";
	query_string += append_float(mRead.heading);	query_string += ", ";
	query_string += "0, 0,0,0,0,";
	query_string += "'GYRO'";						query_string += ", ";
	query_string += "'tilt, pitch, heading'";				
	query_string += "); ";
	query(false);
}

void SQL_Logger::add_gps( stGPSReading& mRead				 )
{
	char value[16];
	query_string = "INSERT INTO bk_useraccounts.data_log VALUES ('0', CURRENT_TIMESTAMP, ";	

	query_string += append_float(mRead.longitude);		query_string += ", ";
	query_string += append_float(mRead.latitude );		
	query_string += "'GPS'";					query_string += ", ";
	query_string += "'longitude, latitude'";				
	query_string += "); ";
	query(false);	
}




