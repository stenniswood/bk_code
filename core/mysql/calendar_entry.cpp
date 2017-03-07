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
#include "calendar_entry.hpp"
#include "global_funcs.h"
#include "machine_defs.h"
#include "sql_common.hpp"
#include "sql_device.hpp"
#include "sql_users.hpp"

#define Debug 0

using namespace std;

char calendar_table_name[128];

CalendarEntry::CalendarEntry()
{
	Initialize();
}
CalendarEntry::~CalendarEntry()
{
}

void close_users_db()
{
	//mysql_close( users_db );
}

void CalendarEntry::Initialize(	)
{
	strcpy(calendar_table_name, sql_user_dbase_name );
	strcat(calendar_table_name, ".calendar");
	
	connect_to_users_db();
	create_calendar_table();
	
	m_result=NULL;
	m_display_mode=1;
	_id=0;	
	m_user_id=1;
	time(&m_scheduled_time);	
	struct tm* tmp = localtime(&m_scheduled_time);
	m_scheduled_time_bd = *tmp;		// broken down time.		
	printf("CalendarEntry:  month=%d;  day=%d\n", tmp->tm_mon, tmp->tm_mday );
}

bool CalendarEntry::calendar_table_exists( )
{
    query_string = "SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'sjtennis_bk_useraccounts' AND  TABLE_NAME = 'calendar';";
    return (query(true)>0);
}
void CalendarEntry::create_calendar_table( )
{  
	if (calendar_table_exists())
		return;
	
	query_string = "CREATE TABLE `calendar` ( \
	  `_id` int(11) NOT NULL AUTO_INCREMENT, \
	  `user_id` int(11) NOT NULL DEFAULT '0', \
	  `date` date NOT NULL,\
	  `time` time NOT NULL, \
	  `date_time` datetime NOT NULL,\
	  `location` text NOT NULL,\
	  `description` text NOT NULL,\
	  `advanced_notification` int(11) NOT NULL DEFAULT '60', \
	  `display_mode` int(11) NOT NULL DEFAULT '1' COMMENT '0=Not shown,\n1=Shown,\n2=Highlighted\netc.',\
	  PRIMARY KEY (`_id`)\
	) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1 COMMENT='Calendar for all users';";
	query(false);
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


void CalendarEntry::find_entry_nq		( int mCalendar_id   )
{  
	query_string =  "SELECT * FROM ";
	query_string += calendar_table_name;
	query_string += " WHERE ";
	query_string += " _id="; 
	query_string += append_int(mCalendar_id);
	query_string += ";";
}

void CalendarEntry::find_date( int mUser_id )
{
	query_string =  "SELECT * FROM ";
	query_string += calendar_table_name;	
	query_string += " WHERE ";
	query_string += " user_id="; 
	query_string += append_int(mUser_id);  		query_string += " and ";
	query_string += " date='"; 
	query_string += form_date_string( m_scheduled_time_bd );
	query_string += "';";	
	Dprintf("%s\n", query_string.c_str() );
	query(true);
}
void CalendarEntry::find_location		( string mLocation   )
{
	query_string =  "SELECT * FROM ";
		query_string += calendar_table_name;	
		query_string += " WHERE ";
	query_string += "location LIKE '"+mLocation+"'"; 
	query_string += ";";	
	query(true);	
}
void CalendarEntry::find_description	( string mDescription )
{
	query_string =  "SELECT * FROM ";
	query_string += calendar_table_name;	
	query_string += " WHERE ";
	query_string += "description LIKE '"+mDescription+"'"; 
	query_string += ";";	
	query(true);
}

/* fill in these member variables first:
		m_user_id
		m_scheduled_time_bd
		m_location
		m_description
		m_display_mode
*/
void CalendarEntry::add_entry_nq		( 				 )
{
	query_string = "INSERT INTO ";
	query_string += calendar_table_name;	
	query_string += " VALUES ('0', ";
	query_string += append_int(m_user_id);		query_string += ", '";
	query_string += form_date_string(m_scheduled_time_bd);			query_string += "', '";
	query_string += form_time_string(m_scheduled_time_bd);			query_string += "', '";
	query_string += form_date_time_string(m_scheduled_time_bd);	query_string += "', '";
	query_string += m_location;					query_string += "', '";
	query_string += m_description;				query_string += "', ";
	query_string += append_int(m_display_mode);
	query_string += "); ";
}


void CalendarEntry::add_entry( int mUserId, struct tm& m_scheduled_time_bd, 
			string mLocation, string mDescription, int mDisplayMode	 )
{
	query_string = "INSERT INTO ";
	query_string += calendar_table_name;	
	query_string += " VALUES ('0', ";
	query_string += append_int(m_user_id);		query_string += ", '";
	query_string += form_date_string(m_scheduled_time_bd);			query_string += "', '";
	query_string += form_time_string(m_scheduled_time_bd);			query_string += "', '";
	query_string += form_date_time_string(m_scheduled_time_bd);	query_string += "', '";
	query_string += m_location;					query_string += "', '";
	query_string += m_description;				query_string += "', ";
	query_string += append_int(m_display_mode);
	query_string += "); ";
}

void CalendarEntry::update_entry_nq		( )
{  
	query_string = "UPDATE ";
	query_string += calendar_table_name;
	query_string += " SET user_id="; 
	query_string += append_int(m_user_id);
	query_string += "date='";			query_string += form_date_string(m_scheduled_time_bd);
	query_string += "',  time='";			query_string += form_time_string(m_scheduled_time_bd);
	query_string += "',  date_time='";	query_string += form_date_time_string(m_scheduled_time_bd);
	query_string += "',  location='"	  + m_location;	
	query_string += "',  description='" + m_description;	
	query_string += "',  display_mode='";	
	query_string += append_int( m_display_mode );		
	query_string += " WHERE _id=";
	query_string += append_int( _id );
}

int  CalendarEntry::query( bool mRetrieving )
{
	printf("%s\n", query_string.c_str() );
	if (users_db==NULL)	return 0;
	
	if (mysql_query(users_db, query_string.c_str() ))
	{
	  fprintf(stderr, "Object: %s\n", mysql_error(users_db));
	  printf ( "Object: %s\n", mysql_error(users_db));
	}
	int row_count=0;
	// Question : what happens when no results?
	//   Answer : if  mysql_store_result returns a non-zero value
	if (mRetrieving) {
	  m_result = mysql_store_result(users_db);
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
	if (users_db==NULL)	return 0;
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
	int row_id = mysql_insert_id( users_db );	
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

void test_calendar_db()
{
    // Add some samples rows : 
    CalendarEntry ce;		// automatically connects and creates table is needed!
	ce.m_user_id = sql_users.sql_find("stenniswood");
	
//	ce.m_scheduled_time_bd = ;
	ce.m_location = "steve's pad";
	ce.m_description = "come join the coder!";	
	ce.m_display_mode = 1;
	ce.add_entry_nq();

//	ce.m_scheduled_time_bd = ;
	ce.m_location = "Beyond Kinetics Garage";
	ce.m_description = "we have a place!";	
	ce.m_display_mode = 1;
	ce.add_entry_nq();

//	ce.m_scheduled_time_bd = ;
	ce.m_location = "driveway";
	ce.m_description = "remember to take out the junk";	
	ce.m_display_mode = 1;
	ce.add_entry_nq();
	
//	ce.m_scheduled_time_bd = ;
	ce.m_location = "Dentist office";
	ce.m_description = "remember to get that cavity filled with junk";	
	ce.m_display_mode = 1;	
	ce.update_entry_nq();

/*	ce.find_entry_nq	(id);
	ce.find_date		( mUser_id 	);			// based on broken down time.
	ce.find_location	( string mLocation     );		
	ce.find_description	( string mDescription  );			*/
		
	printf("Done with test_calendar_db()\n");
}


/*char* CalendarEntry::form_date_string( )
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
}*/
