#ifndef _CALENDAR_ENTRY_HPP_
#define _CALENDAR_ENTRY_HPP_

//#include <time.h>
#include <ctime>
#include <my_global.h>
#include <mysql.h>


using namespace std;

class CalendarEntry 
{
public:
	CalendarEntry();
	~CalendarEntry();
	
	virtual void 	Initialize(	);
	void			connect_to_calendar_db();

	void			create_calendar_table( );
	
	void			find_entry_nq		( int mCalendar_id   );
	void			add_entry_nq		( 				 );
	void			update_entry_nq		( );
	int				query				( bool mRetrieving=true );
	
	void			find_date			( int mUser_id = 1  	);			// based on broken down time.
	void			find_location		( string mLocation     );		
	void			find_description	( string mDescription  );			

	int				sql_add				( );
	void			sql_update			( );
	void			sql_load			( int mCalendar_id );
	void			extract_result		( );		// for the last fetched row.
	
	MYSQL_ROW		goto_first_row();
	MYSQL_ROW		goto_next_row();
	int				get_number_results();
	
	char*			form_date_string	 (  );			// suitable to be added to SQL.
	char*			form_time_string	 (  );			// suitable to be added to SQL.
	char*			form_date_time_string(  );			// suitable to be added to SQL.
	string			form_summary_string	 (  );			// suitable for Calendar Summary View
	
	void			show_date	( int mMonth, int mDay );	// other than the current

public:
	MYSQL_RES*		m_result;
	MYSQL_ROW		m_row;
	
	// DATA :
	int				m_display_mode;
	
	int				_id;	
	int				m_user_id;
	struct tm		m_scheduled_time_bd;		// broken down time.
	time_t 			m_scheduled_time;	
	
	//string			m_title;
	string			m_location;		
	string			m_description;

	string			query_string;
};

 
#endif
