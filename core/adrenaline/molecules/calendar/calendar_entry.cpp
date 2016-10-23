// line graph OpenVG program
// Steve Tenniswood

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
//#include "calendar.hpp"
#include "calendar_entry.hpp"
#include "global_funcs.h"


//extern MYSQL *calendar_db;
MYSQL 		 *calendar_db = NULL;

#define Debug 0


void object_finish_with_error( )
{
    fprintf    ( stderr, "%s\n", mysql_error(calendar_db));
    mysql_close( calendar_db );
    exit(1);
}

CalendarEntry::CalendarEntry()
{
	Initialize();
}
CalendarEntry::~CalendarEntry()
{
	
}

void CalendarEntry::connect_to_calendar_db()
{
    calendar_db = mysql_init(NULL);
    if (calendar_db == NULL)
    {
        fprintf(stderr, "init %s\n", mysql_error(calendar_db));
        exit(1);
    }
    
    if (mysql_real_connect(calendar_db, "localhost", "root", "password",
                           "bk_useraccounts", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "real_connect %s\n", mysql_error(calendar_db));
        mysql_close(calendar_db);
        exit(1);
    }
}

void CalendarEntry::Initialize(	)
{
	if (calendar_db==NULL)
		connect_to_calendar_db();
		
	m_result=NULL;
	m_display_mode=1;
	_id=0;	
	m_user_id=1;
	time(&m_scheduled_time);	
	struct tm* tmp = localtime(&m_scheduled_time);
	m_scheduled_time_bd = *tmp;		// broken down time.		
	printf("CalendarEntry:  month=%d;  day=%d\n", tmp->tm_mon, tmp->tm_mday );
}

char* append_int( int mInt )
{
	static char tmp[16];
	sprintf(tmp, "'%d'", mInt );
	return tmp;
}
char* CalendarEntry::form_date_string( )
{
	static char tmp[16];
	sprintf(tmp, "'%4d-%d-%d'", m_scheduled_time_bd.tm_year+1900,
								m_scheduled_time_bd.tm_mon,
							m_scheduled_time_bd.tm_mday );
	return tmp;
}
char* CalendarEntry::form_time_string( )
{
	static char tmp[16];
	sprintf(tmp, "'%2d:%2d:%2d'", m_scheduled_time_bd.tm_hour,
								m_scheduled_time_bd.tm_min,
							m_scheduled_time_bd.tm_sec );	
	return tmp;
}
char* CalendarEntry::form_date_time_string( )
{
	static char tmp[32];
	strcpy(tmp, form_date_string());
	strcat(tmp, " ");
	strcat(tmp, form_time_string());
	return tmp;
}

string CalendarEntry::form_summary_string	 (  )			// suitable for Calendar Summary View
{
	static string summary;
	char tmp[26];
	strftime( tmp, 25, "%I:%M %p   ", &m_scheduled_time_bd );	
	summary = tmp;
	//summary = form_time_string();			summary += "\t:";
	summary += m_description;				summary += "   :";
	summary += m_location;
	
	// who else is going to be there?
	return summary;
}

MYSQL_ROW	CalendarEntry::goto_first_row()
{
	// force a rewind:	
	mysql_data_seek( m_result, 0 );
	m_row = mysql_fetch_row( m_result );
	return m_row;
}

MYSQL_ROW	CalendarEntry::goto_next_row()
{
	m_row = mysql_fetch_row( m_result );
	return m_row;	
}


void CalendarEntry::create_calendar_table( )
{  
	query_string = "CREATE TABLE `calendar` ( \
	  `_id` int(11) NOT NULL AUTO_INCREMENT, \
	  `user_id` int(11) NOT NULL DEFAULT '0', \
	  `date` date NOT NULL,\
	  `time` time NOT NULL, \
	  `date_time` datetime NOT NULL,\
	  `location` text NOT NULL,\
	  `description` text NOT NULL,\
	  `display_mode` int(11) NOT NULL DEFAULT '1' COMMENT '0=Not shown,\n1=Shown,\n2=Highlighted\netc.',\
	  PRIMARY KEY (`_id`)\
	) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COMMENT='Calendar for all users';";
	query(false);
}

void CalendarEntry::find_entry_nq		( int mCalendar_id   )
{  
	query_string =  "SELECT * FROM bk_useraccounts.calendar WHERE ";
	query_string += "bk_useraccounts.calendar._id="; 
	query_string += append_int(mCalendar_id);
	query_string += ";";
}

void CalendarEntry::find_date( int mUser_id )
{
	query_string =  "SELECT * FROM bk_useraccounts.calendar WHERE ";
	query_string += "bk_useraccounts.calendar.user_id="; 
	query_string += append_int(mUser_id);  		query_string += " and ";
	query_string += "bk_useraccounts.calendar.date="; 
	query_string += form_date_string( );
	query_string += ";";	
	Dprintf("%s\n", query_string.c_str() );
	query(true);
}
void CalendarEntry::find_location		( string mLocation   )
{
	query_string =  "SELECT * FROM bk_useraccounts.calendar WHERE ";
	query_string += "location LIKE '"+mLocation+"'"; 
	query_string += ";";	
	query(true);	
}
void CalendarEntry::find_description	( string mDescription )
{
	query_string =  "SELECT * FROM bk_useraccounts.calendar WHERE ";
	query_string += "description LIKE '"+mDescription+"'"; 
	query_string += ";";	
	query(true);
}

void CalendarEntry::add_entry_nq		( 				 )
{
	char value[16];
	query_string = "INSERT INTO bk_useraccounts.calendar VALUES ('0', ";
	query_string += append_int(m_user_id);		query_string += ", ";
	query_string += form_date_string();			query_string += ", ";
	query_string += form_time_string();			query_string += ", ";
	query_string += form_date_time_string();	query_string += ", '";
	query_string += m_location;					query_string += "', '";
	query_string += m_description;				query_string += "', ";
	query_string += append_int(m_display_mode);
	query_string += "); ";
}


void CalendarEntry::update_entry_nq		( )
{  
	char value[16];
	query_string = "UPDATE bk_useraccounts.calendar SET user_id="; 
	query_string += append_int(m_user_id);
	query_string += "date=";			query_string += form_date_string();
	query_string += ",  time=";			query_string += form_time_string();
	query_string += ",  date_time=";	query_string += form_date_time_string();
	query_string += ",  location="	  + m_location;	
	query_string += ",  description=" + m_description;	
	query_string += ",  display_mode=";	
	query_string += append_int( m_display_mode );		
	query_string += " WHERE _id=";
	query_string += append_int( _id );
}

int  CalendarEntry::query( bool mRetrieving )
{
	printf("%s\n", query_string.c_str() );
	
  if (mysql_query(calendar_db, query_string.c_str() ))
  {
      fprintf(stderr, "Object: %s\n", mysql_error(calendar_db));
  }
  int row_count=0;
  // Question : what happens when no results?
  //   Answer : if  mysql_store_result returns a non-zero value
  if (mRetrieving) {
	  m_result = mysql_store_result(calendar_db);
	  if (m_result == NULL)  {
		  printf("query Error result==null!\n");
		  return 0; 
	  }
	  row_count = mysql_num_rows(m_result);
  }    
  printf("row_count=%d\n", row_count);
  return row_count;
}

int	CalendarEntry::get_number_results()
{
	return mysql_num_rows(m_result);
}
void CalendarEntry::extract_result( )
{
	if (m_row) {
		_id 	= atoi(m_row[0]);
		m_user_id = atoi(m_row[1]);		
//		if (m_row[2])  date = m_row[2];
//		if (m_row[3])  time = m_row[3];
//		if (m_row[4])  date_time = m_row[4];
		char combined[80];
		strcpy(combined, m_row[2] );
		strcat(combined, " ");
		strcat(combined, m_row[3] );
		strptime( combined, "%Y-%m-%d %H:%M:%S", &m_scheduled_time_bd );
		
		m_location		= m_row[5];
		m_description	= m_row[6];
		m_display_mode	= m_row[7];	
	}
}
int CalendarEntry::sql_add				( )
{  
	add_entry_nq();
	query(false);	
	int row_id = mysql_insert_id( calendar_db );	
	return row_id;
}
void CalendarEntry::sql_update			( )
{  
	update_entry_nq();
	query(false);	
}
void CalendarEntry::sql_load			( int mCalendar_id )
{ 
	find_entry_nq( mCalendar_id );
	query(true);	
	m_row = mysql_fetch_row( m_result );
	extract_result( ); 
	mysql_free_result( m_result );
}


void CalendarEntry::show_date( int mMonth, int mDay )	// other than the current
{

}


    
