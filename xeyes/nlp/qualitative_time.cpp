//
//  qualitative_time.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 9/6/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <string>
#include <time.h>
#include <ctime>
#include <math.h>
#include "qualitative_time.hpp"

using namespace std;

string integer_e   = "(\\d+ )";
string real_e      = "([-+]?[0-9]*\\.?[0-9]+ )";

bool parsed_qualitative_time;		// True if any qualitative time is stated : 
bool parsed_qualitative_duration;	// True if any qualitative time is stated : 

time_t    now;
struct tm bd_now;

// half hour = 30 min * 60 = 1800
const int HOUR_IN_SECONDS = 3600;
const int DAY_IN_SECONDS  = 3600*24;
const int WEEK_IN_SECONDS = 3600*24*7;

void update_now_time()
{
    now 	  = time(&now);
    bd_now    = *localtime(&now);
}

// Compared to now.
void compose_time_qualitative_duration( time_t deltaTime, string& mTimeString )
{
    int mins,hours,days,weeks;
    char str[20];

    if (deltaTime < 60 )
        mTimeString = std::to_string(deltaTime) + " seconds";
    else if (deltaTime < 1800 )     // less than 1/2 hour.
    {
        mins = round(deltaTime / 60);
        if (mins==1)
            mTimeString = "1 minute";
        else {
            mTimeString = std::to_string(mins);
            mTimeString += " minutes";
        }
    }
    else if ((deltaTime >= HOUR_IN_SECONDS) && (deltaTime < (HOUR_IN_SECONDS*24) ))
    {
        hours = round(deltaTime / HOUR_IN_SECONDS);
        if (hours==1)
            mTimeString = "1 hour";
        else {
            mTimeString = std::to_string(hours);
            mTimeString += " hours";
        }
    }
    else if ((deltaTime >= DAY_IN_SECONDS) && (deltaTime < DAY_IN_SECONDS*7 ))
    {
        days = round(deltaTime / DAY_IN_SECONDS);
        if (days==1)
            mTimeString = "1 day"; // "1 day ago";
        else {
			// Compare 
			mTimeString = std::to_string(days);
			mTimeString += " days";
        }
    }
    else if ((deltaTime >= WEEK_IN_SECONDS) && (deltaTime < WEEK_IN_SECONDS*3) )
    {
        weeks = round(deltaTime / WEEK_IN_SECONDS);
        if (weeks==1)
            mTimeString = "1 week ";
        else {
            mTimeString = std::to_string(hours);
            mTimeString += " weeks ";
        }
    }
}

void compose_time_qualitative( time_t mAbsTime, string& mTimeString )
{
    int mins,hours,days,weeks;
    char str[20];
	update_now_time();
    struct tm* bd_Time = localtime(&mAbsTime);
    time_t deltaTime = difftime( now, mAbsTime );

    if (deltaTime < 60 )
        mTimeString = "Just now";
    else if (deltaTime < 1800 )     // less than 1/2 hour.
    {
        mins = round(deltaTime / 60);
        if (mins==1)
            mTimeString = "1 minute ago";
        else {
            mTimeString = std::to_string(mins);
            mTimeString += " minutes ago";
        }
    }
    else if ((deltaTime >= HOUR_IN_SECONDS) && (deltaTime < (HOUR_IN_SECONDS*24) ))
    {
        hours = round(deltaTime / HOUR_IN_SECONDS);
        if (hours==1)
            mTimeString = "1 hour ago";
        else {
            mTimeString = std::to_string(hours);
            mTimeString += " hours ago";
        }
    }
    else if ((deltaTime >= DAY_IN_SECONDS) && (deltaTime < DAY_IN_SECONDS*7 ))
    {
        days = round(deltaTime / DAY_IN_SECONDS);
        if (days==1)
            mTimeString = "yesterday"; // "1 day ago";
        else {
			// Compare 
			if (days < bd_now.tm_wday) {
				strftime( str, 20, "%A", bd_Time );
				mTimeString = str; 
			} else {
	            mTimeString = std::to_string(days);
   	         	mTimeString += " days ago";
        	}
        }
    }
    else if ((deltaTime >= WEEK_IN_SECONDS) && (deltaTime < WEEK_IN_SECONDS*3) )
    {
        weeks = round(deltaTime / WEEK_IN_SECONDS);
        if (weeks==1)
            mTimeString = "1 week ago";
        else {
            mTimeString = std::to_string(hours);
            mTimeString += " weeks ago";
        }
    }
}

void compose_time_qualitative_future( time_t mAbsTime, string& mTimeString )
{
    int mins,hours,days,weeks;
    char str[20];
	update_now_time();
    struct tm* bd_Time = localtime(&mAbsTime);
    time_t deltaTime   = difftime(now, mAbsTime);

    if (deltaTime < 60 )
        mTimeString = "in " + std::to_string(deltaTime) + " seconds";
    else if (deltaTime < 1800 )     // less than 1/2 hour.
    {
        mins = round(deltaTime / 60);
        if (mins==1)
            mTimeString = "1 minute from now";
        else {
            mTimeString = std::to_string(mins);
            mTimeString += " minutes from now";
        }
    }
    else if ((deltaTime >= HOUR_IN_SECONDS) && (deltaTime < (HOUR_IN_SECONDS*24) ))
    {
        hours = round(deltaTime / HOUR_IN_SECONDS);
        if (hours==1)
            mTimeString = "1 hour from now";
        else {
            mTimeString = std::to_string(hours);
            mTimeString += " hours from now";
        }
    }
    else if ((deltaTime >= DAY_IN_SECONDS) && (deltaTime < DAY_IN_SECONDS*7 ))
    {
        days = round(deltaTime / DAY_IN_SECONDS);
        if (days==1)
            mTimeString = "tomorrow"; // "1 day ago";
        else {
			// Compare 
			if (days < bd_now.tm_wday) {
				strftime( str, 20, "on %A", bd_Time );
				mTimeString = str; 
			} else {
	            mTimeString = std::to_string(days);
   	         	mTimeString += " days from now";
        	}
        }
    }
    else if ((deltaTime >= WEEK_IN_SECONDS) && (deltaTime < WEEK_IN_SECONDS*3) )
    {
        weeks = round(deltaTime / WEEK_IN_SECONDS);
        if (weeks==1)
            mTimeString = "1 week from now";
        else {
            mTimeString = std::to_string(hours);
            mTimeString += " weeks from now";
        }
    }
}


void mk_startofday(struct tm& sTime)
{
	sTime.tm_hour = 0;
	sTime.tm_min  = 0;
	sTime.tm_sec  = 0;
}
void mk_endofday(struct tm& eTime)
{
	eTime.tm_hour = 23;
	eTime.tm_min  = 59;
	eTime.tm_sec  = 59;
}

void mk_yesterday(struct tm& sTime)
{
	sTime.tm_mday--;
	
	time_t tmp = mktime(&sTime);
	sTime = *localtime( &tmp );
}
bool is_yesterday(struct tm sTime)
{
	struct tm  yesterday = bd_now;
	mk_yesterday( yesterday );
	mk_startofday( yesterday );
	return ((sTime.tm_mon == yesterday.tm_mon) && (sTime.tm_mday == yesterday.tm_mday) && (sTime.tm_year == yesterday.tm_year));
}

bool is_start_of_day(struct tm& sTime)
{
	return ((sTime.tm_hour == 0) && (sTime.tm_min == 0) && (sTime.tm_sec == 0));
}
bool is_today(struct tm& sTime)
{
	return ((sTime.tm_mon == bd_now.tm_mon) && (sTime.tm_mday == bd_now.tm_mday) && (sTime.tm_year == bd_now.tm_year));
}
bool is_morning(struct tm& sTime)
{
	return (sTime.tm_hour < 12);		
}
bool is_afternoon(struct tm& sTime)
{
	return (sTime.tm_hour >= 12);
}
bool is_evening(struct tm& sTime)
{
	return (sTime.tm_hour >= 17);
}
bool is_night(struct tm& sTime)
{
	return (sTime.tm_hour >= 20);
}


void compose_time_period_qualitative( struct tm* sTime, struct tm* eTime, std::string& mTimeString )
{
	update_now_time();
	// today
	bool s_today = is_today(*sTime);
	bool e_today = is_today(*eTime);
	int s_morning = is_morning(*sTime);
	int e_morning = is_morning(*eTime);	
	//printf("today s,e=%d,%d; morn: %d,%d \n", s_today, e_today, s_morning, e_morning );

	if (s_today)
	{
		if (e_morning)
		{
			mTimeString = "this morning";
			return;
		}
		if ( is_afternoon(*sTime) && is_afternoon(*eTime))
		{
			mTimeString = "this afternoon";
			return;
		}
		if (is_evening(*sTime) && is_evening(*eTime))
		{
			mTimeString = "this evening";
			return;
		}
		if (is_night(*sTime) && is_night(*eTime))
		{
			mTimeString = "tonight";
			return;
		}
		if (e_today)
		{
			mTimeString = "today";
			return;
		}	
	}
	if (is_yesterday(*sTime))
	{
		if (e_morning)
		{
			mTimeString = "yesterday morning";
			return;
		}
		if ( is_afternoon(*sTime) && is_afternoon(*eTime))
		{
			mTimeString = "yesterday afternoon";
			return;
		}
		if (is_evening(*sTime) && is_evening(*eTime))
		{
			mTimeString = "yesterday evening";
			return;
		}
		if (is_night(*sTime) && is_night(*eTime))
		{
			mTimeString = "last night";
			return;
		}
		if (is_yesterday(*sTime))
		{
			mTimeString = "yesterday";
			return;
		}	
	}		
	mTimeString = " Between ";
	int len=0;
	char* s = asctime( sTime );
	len = strlen(s);
	s[len-2]=0;		// Remove the '\n'
	mTimeString += s;
	mTimeString += " and ";
	
	char* e = asctime( eTime );
	len = strlen(e);
	e[len-2]=0;	
	mTimeString += e;
	
	// for nearly the last 2 hours.

	// from noon until 2 minutes ago.

	// yesterday afternoon
	// yesterday morning (between 10 and 11)
}
/****************************************************************/
/************************* PARSING ******************************/
/****************************************************************/
string days_of_week[] = {
    "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"
};

string Months[12] = {
    "january",  "february",  "march",
    "april", 	"may",  	"june",
    "july",  	"august", 	"september",
    "october",  "november", "december" };

// "((this|next|last) year)|(\d\d\d\d)"
int year_is_found( Sentence& mSentence, struct tm* mFillin=NULL )
{
    int retval=0;
    retval = mSentence.is_found_in_sentence("this year");
    if (retval)
    {
        if (mFillin) mFillin->tm_year = retval = bd_now.tm_year;
        return retval;
    } else if ((retval = mSentence.is_found_in_sentence("next year")))
    {
        if (mFillin) mFillin->tm_year = retval = bd_now.tm_year+1;
        return retval;
    } else if ((retval = mSentence.is_found_in_sentence("last year")))
    {
        if (mFillin) mFillin->tm_year = retval = bd_now.tm_year-1;
        return retval;
    }
    /* Scan numbers 1900 to 2016 */
    char year_ascii[4];
    for (int year=1900; year<2100; year++) {
        sprintf(year_ascii, "%d", year);
        if (mSentence.is_found_in_sentence( year_ascii )) {
            if (mFillin) mFillin->tm_year = year-1900;
            return mSentence.get_word_index(year_ascii);
        }
    }
    return retval;
}

/* returns -1 none found.  or index of month. */
// "((this|last|next) month)|(\d+ months ago)|(\d+ months from now)|(in the last \d+ months)"
int month_is_found( Sentence& mSentence )
{
    int retval=0;
    retval = mSentence.is_found_in_sentence("this month");
    if (retval)
    {
    	retval = bd_now.tm_mon;
    	return retval;	
    }
    retval = mSentence.is_found_in_sentence("last month");    
    if (retval)
    {  retval = bd_now.tm_mon-1;	return retval;	}

	retval = mSentence.is_found_in_sentence("next month");
    if (retval)
    {  retval = bd_now.tm_mon+1;	return retval;	}
    
    retval = mSentence.regex_find("(\\d+) months ago");
    if (retval) 
    {
    	mSentence.m_sentence.print_matches();
    	string val = mSentence.m_sentence.regex_matches[0];
    	int m = atoi(val.c_str());
    	retval = bd_now.tm_mon-m;
    }
    
    retval = mSentence.regex_find("(\\d+) months from now");
    if (retval) 
    {
    	mSentence.m_sentence.print_matches();    
    	string val = mSentence.m_sentence.regex_matches[0];
    	int m = atoi( val.c_str() );
    	retval = bd_now.tm_mon+m;
    }
    
    for (int m=0; m<12; m++)
        if (mSentence.is_found_in_sentence( Months[m].c_str() ))
            return m;
    return retval;
}

/* Return : day of week */
// "(yesterday|today|tomorrow)|(\d+ days ago)|(sunday|monday|tuesday|wednesday|thursday|friday|saturday)"
int dow_is_found( Sentence& mSentence )
{
    int retval=0;
    retval = mSentence.is_found_in_sentence("today");
    if (retval)    	return bd_now.tm_mday;
    retval = mSentence.is_found_in_sentence("yesterday");
    if (retval)    	return bd_now.tm_mday-1;
    retval = mSentence.is_found_in_sentence("tomorrow");
    if (retval)    	return bd_now.tm_mday+1;
    
    retval = mSentence.regex_find("(\\d+ )days ago");
    if (retval)  {
		string val = mSentence.m_sentence.regex_matches[0];
    	int d= atoi(val.c_str());
      	return bd_now.tm_mday-d;
    }
    for (int d=0; d<7; d++)
        if (mSentence.is_found_in_sentence(days_of_week[d].c_str()))
            return d;
    return retval;
}

int parse_qualitative_1_time( Sentence& mSentence, time_t& sTime )
{
	// Parse Qualitative time:
	parsed_qualitative_duration = mSentence.are_found_in_sentence( "in the last %d (day|days|hour|hours|minute|minutes|seconds)" ) || 
								  mSentence.are_found_in_sentence( "between" );	// hour, day, 5 hours, 5 minutes.
	
	parsed_qualitative_duration  = mSentence.are_found_in_sentence( "at [specific time ie five thirty]" );
	parsed_qualitative_time = parsed_qualitative_duration;	
}

/* Return : -1 not processed */
int parse_qualitative_2_time( Sentence& mSentence, struct tm& sTime, struct tm& eTime )
{
	int retval = -1;
	vector<int> answers;
	vector<int> remove_wi;	
	parsed_qualitative_time     = false;
	parsed_qualitative_duration = false;

	time_t start;
	time_t now;
	time( &now );

	int result;
	int between = mSentence.is_found_in_sentence( "between" );
//	if (result)  retval = 1;
	result = mSentence.is_found_in_sentence( "today" );
	if (result)  {
		parsed_qualitative_time     = true;
		parsed_qualitative_duration = true;		
		eTime = *(localtime( &now ));
		sTime = eTime;
		sTime.tm_hour = 0;
		sTime.tm_min = 0;
		sTime.tm_sec = 0;
		retval = 1;
	}
	result = mSentence.is_found_in_sentence( "yesterday" );
	if (result)  {
		parsed_qualitative_time     = true;
		parsed_qualitative_duration = true;		
		update_now_time();
		eTime = bd_now;	
		eTime.tm_mday--;
		eTime.tm_wday--;
		eTime.tm_hour = 23;
		eTime.tm_min = 59;
		eTime.tm_sec = 59;

		sTime = eTime;
		sTime.tm_hour = 0;
		sTime.tm_min = 0;
		sTime.tm_sec = 0;
		retval = 1;
	}

	result = mSentence.are_found_in_sentence( "this week" );
	if (result)  {
		parsed_qualitative_time = true;
		parsed_qualitative_duration = true;
		eTime = bd_now;
		sTime = eTime;
		sTime.tm_mday-=sTime.tm_wday;
		sTime.tm_wday = 0;
		sTime.tm_hour =0; sTime.tm_min=0; sTime.tm_sec=0;
		retval = 1;
	}	

	// 
    result = mSentence.regex_find("(\\d+) days ago");
    if (result)  {
		SuperString val( mSentence.m_reduced_sentence.regex_matches[0] );		
		string num  = "\\d+";
		int result2 = val.regex_find(num);
		num = val.regex_matches[0];
    	int    d = atoi( num.c_str() );
	    //printf("days ago: %s  result=%d\n\n", num.c_str(), d );
		parsed_qualitative_time = true;
		parsed_qualitative_duration = true;
		eTime = bd_now;
		eTime.tm_mday = bd_now.tm_mday-d;					
		sTime = eTime;
		mk_startofday(sTime);
		mk_endofday  (eTime);
      	retval = 1; 
    }
    for (int d=0; d<7; d++)
        if (mSentence.is_found_in_sentence(days_of_week[d].c_str()))
            return d;

	int dans = mSentence.are_found_in_sentence( "in the last" );
	if (dans)
		retval = 1;

	string exp = integer_e;
	exp       += "(day|days|hour|hours|minute|minutes|seconds)";

	//parsed_qualitative_duration  = mSentence.are_found_in_sentence( "at [specific time ie five thirty]" );
	//parsed_qualitative_time = parsed_qualitative_duration;
	return retval;
}

//	parsed_qualitative_duration = mSentence.m_sentence.regex_find( exp, &answers, &remove_wi );
//	if (parsed_qualitative_duration)
//	{
//		start = now ;
//	}
//  mSentence.are_found_in_sentence( "between" );	// hour, day, 5 hours, 5 minutes.
