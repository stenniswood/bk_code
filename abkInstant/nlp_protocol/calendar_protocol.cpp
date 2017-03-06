//
//  calendar_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 2/17/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <list>
#include <sys/types.h>
#include <time.h>
#include <string>
#include "protocol.h"
#include "devices.h"
#include "GENERAL_protocol.hpp"
#include "nlp_extraction.hpp"
#include "client_memory.hpp"
#include "alias.hpp"
#include "nlp_sentence.hpp"
#include "client_to_socket.hpp"

#include "calendar_entry.hpp"
#include "calendar_protocol.hpp"
#include "sql_common.hpp"


extern time_t    now;
extern struct tm bd_now;


/***********************************************************************
 
 Suggested statements:
 
 I am viki.  [response to who are you, what's your name, etc.]
 My creator is ...
 I am here to help you.  [purpose]
 My favorite color is
 I have __,__, and ___ tasks on my list todo.
 I have lived with you for x years now.
 I am composed of 24 servo motors,
 My capabilities are:
 
 I am able to vacume, sweep, paint, etc.
 ***********************************************************************/
struct tm som;
struct tm current_date;

// Question to user.

string days_of_week[] = {
    "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"
};

string Months[12] = {
    "january",  "february",  "march",
    "april", 	"may",  	"june",
    "july",  	"august", 	"september",
    "october",  "november", "december" }; 

// "((this|next|last) year)|(\d\d\d\d)"
int year_is_found( Sentence& mSentence )
{
    int retval=0;
    retval = mSentence.is_found_in_sentence("this year");
    if (retval)
    {
        som.tm_year = current_date.tm_year;
        return retval;
    } else if ((retval = mSentence.is_found_in_sentence("next year")))
    {
        som.tm_year = current_date.tm_year+1;
        return retval;
    } else if ((retval = mSentence.is_found_in_sentence("last year")))
    {
        som.tm_year = current_date.tm_year-1;
        return retval;
    }
    /* Scan numbers 1900 to 2016 */
    char year_ascii[4];
    for (int year=1900; year<2100; year++) {
        sprintf(year_ascii, "%d", year);
        if (mSentence.is_found_in_sentence( year_ascii )) {
            som.tm_year = year-1900;
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
        string tmp = mSentence.m_sentence.regex_matches[0];
        int m = atoi(tmp.c_str());
        retval = bd_now.tm_mon-m;
    }

    retval = mSentence.regex_find("(\\d+) months from now");
    if (retval)
    {
        string tmp = mSentence.m_sentence.regex_matches[0];
        int m = atoi(tmp.c_str() );
        retval = bd_now.tm_mon+m;
    }

    for (int m=0; m<12; m++)
        if (mSentence.is_found_in_sentence( Months[m].c_str() ))
            return m;
    return retval;
}

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
        string tmp = mSentence.m_sentence.regex_matches[0];
        int d= atoi(tmp.c_str());
        return bd_now.tm_mday-d;
    }
    for (int d=0; d<7; d++)
        if (mSentence.is_found_in_sentence(days_of_week[d].c_str()))
            return d;
    return retval;
}


CalendarEntry ce;

void Init_Calendar_Protocol()
{
    time_t t;
    time(&t);
    current_date = *localtime(&t);
    
    connect_to_users_db();
    //ce.connect_to_calendar_db();
}

char* form_date_string()
{
    mktime( &som );

    static char tmp[50];
    strftime( tmp, 50, "%A, %B %e ", &som);
    printf("%s\n", tmp);
    return tmp;
}

/**** ACTION INITIATORS:    (4 possible actions) *****/
/* return:  -1 no date established.
 
 */
int Parse_Time_from_now_Statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval=-1;
    //int foundP   = mSentence.is_found_in_sentence( "in" );
    bool foundNum = mSentence.next_word_is_a_number();
    int   index=-1;
    float Number=0.0;
    if (foundNum)
    {
        index  = mSentence.get_word_pointer();
        Number = mSentence.get_nth_word_as_a_number(index);
    }
    // in X days
    // in X hours
    // in X minutes
    int foundA = mSentence.is_found_in_sentence( "a" );
    int foundMo= mSentence.is_found_in_sentence( "month", true  );
    int foundW = mSentence.is_found_in_sentence( "week", true   );
    int foundD = mSentence.is_found_in_sentence( "day", true    );
    int foundH = mSentence.is_found_in_sentence( "hour", true   );
    int foundM = mSentence.is_found_in_sentence( "minute", true );
    int foundS = mSentence.is_found_in_sentence( "second", true );
    int cond2 = (foundMo || foundW || foundD || foundH || foundM || foundS );
    
    if ((foundNum || foundA) && (cond2))
    {
        time_t t = time(0);                 // get time now
        float delta_time=0.0;

        if (foundMo)
            delta_time = Number*60*60*24*30;        // this needs fixing.  In 5 months
        if (foundW)
            delta_time = Number*60*60*24*7;
        if (foundD)
            delta_time = Number*60*60*24;
        if (foundH)
            delta_time = Number*60*60;
        if (foundM)
            delta_time = Number*60;
        t += delta_time;
        som = *localtime(&t);

        char NLP_Response[256];
        sprintf( NLP_Response, "%6.1f seconds from now. is %s", delta_time, form_date_string() );
        mh->form_response(NLP_Response);
        retval=0;
    }
    return retval;
}

/* absolute date - such as 22nd of May or June 12th */
int Parse_date_Statement( Sentence& mSentence, ServerHandler* mh )
{
    int date  = -1;
    
    int foundMonth = month_is_found(mSentence);
    int foundDOW   = dow_is_found  (mSentence);

    // get the word index of the month.  day should be either:  "22nd of May" or "May 22"
    if (foundMonth>0)
    {
        som.tm_year = current_date.tm_year;
        som.tm_mon = foundMonth;
        int word_index = mSentence.get_word_index( Months[foundMonth].c_str() );
        bool date_after_month = mSentence.is_nth_word_a_number( word_index+1 ) || mSentence.is_nth_word_a_number_nd( word_index+1 );
        if (date_after_month) {
            som.tm_mday = trunc( mSentence.get_nth_word_as_a_number( word_index+1) );
            date = 1;
        } else {
            // check for 22nd of May format
            bool date_before_month = mSentence.is_nth_word_a_number( word_index-2 ) || mSentence.is_nth_word_a_number_nd( word_index-2 );
            if (date_before_month)
            {    som.tm_mday = trunc( mSentence.get_nth_word_as_a_number( word_index-2) );
                 date = 1;
            }
        }
        mktime(&som);
    } else {
        
    }
    if (foundDOW>0)
    {
        // next thursday.  last monday. a week ago
        int word_index = mSentence.get_word_index( days_of_week[foundDOW].c_str() );
        char* preceeding_word = (char*)mSentence.get_nth_word(word_index-1);
        if (preceeding_word)
        {
            if (strcmp(preceeding_word, "next")==0)
            {
                som = current_date;
                int delta_days = foundDOW - current_date.tm_wday;
                som.tm_mday  = current_date.tm_mday + delta_days + 7;
                mktime(&som);        // to ensure month transition.
                
            } else if (strcmp(preceeding_word, "this")==0)
            {
                som = current_date;
                int delta_days = foundDOW - current_date.tm_wday;
                som.tm_mday  = current_date.tm_mday + delta_days;
                mktime(&som);        // to ensure month transition.
            } else if (strcmp(preceeding_word, "last")==0)
            {
                som = current_date;
                int delta_days = foundDOW - current_date.tm_wday;
                som.tm_mday  = current_date.tm_mday - delta_days;
                mktime(&som);        // to ensure month transition.
            }
        }
    }
    
    int tomorrowFound =  mSentence.is_found_in_sentence( "tomorrow" );
    if (tomorrowFound)
    { som = current_date;  som.tm_mday+=1; mktime( &som);  date = 1; }
    int yesterdayFound =  mSentence.is_found_in_sentence( "yesterday" );
    if (yesterdayFound)
    { som = current_date;  som.tm_mday-=1; mktime( &som);  date = 1; }
    return date;
}

int get_ordinal_from_2nd_format( char* mNonOrdinal )
{
    int retval = 0;
    /* 1st, 2nd, 3rd, 4th, 5th, 6th, 7th, 8th, 9th. 10th */
    /* 11th, 12th, 13th, 14th, ... */
    /* 21st, 22nd, 23rd, 24th, 25th */
    /* 31st, 32nd, 33rd */
    // Simply remove the last 2 chars!
    size_t len = strlen(mNonOrdinal);
    mNonOrdinal[len-2] = 0;
    retval = atoi( mNonOrdinal );
    return retval;
}

enum eCalendarAction {
    NO_ACTION,
    NEW_ENTRY,
    QUERY,
    REMOVE_ENTRY
};
/*****************************************************************
 Do the work of the Telegram :
 return  -1	=> Not handled
 else number of extra bytes extracted from the mSentence buffer.
 - besides strlen(mSentence)!
 *****************************************************************/
int Parse_Calendar_Statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval=-1;
    
    int foundS = 0;
    int foundO = 0;
    char tmp[127];
    
    /***** User questioning current date, time, year.  *****/
    foundS = mSentence.is_found_in_sentence ( "what" );
    foundO = mSentence.is_found_in_sentence ( "time" );
    if (foundS && foundO)
    {
        time_t t = time(0);                 // get time now
        struct tm* now = localtime( & t );
        
        strftime( tmp, 80, "The time is now %l %M %p.", now );
        mh->form_response( tmp );
        //CLIENT_Response =  NLP_Response;
        //ClientRequestPending = true;
        retval = 0;
    }
    
    foundS = (mSentence.is_found_in_sentence( "what" ) || mSentence.is_found_in_sentence( "what's" ));
    foundO = mSentence.is_found_in_sentence ( "date" );
    if (foundS && foundO)
    {
        time_t t = time(0);   // get time now
        struct tm* now = localtime( & t );
        strftime( tmp, 80, "Today is %A, %B, %e ", now );
        mh->form_response( tmp );
        retval = 0;
    }
    
    foundS = mSentence.is_found_in_sentence( "what" );
    foundO = mSentence.is_found_in_sentence ( "year" );
    if (foundS && foundO)
    {
        time_t t = time(0);   // get time now
        struct tm* now = localtime( & t );
        strftime( tmp, 80, "It is %Y", now );
        mh->form_response(tmp);
        retval = 0;
    }
    /***** User questioning current date, time, year.  *****/
    /*****     User questioning another date, time, year.  *****/
    // What is the date next thursday?
    // What day [of the week] is April 28th on this year?
    /***** End User questioning another date, time, year.  *****/
    
    /*****     User specifying a date, time, year.  *****/
    //enum eCalendarAction calendar_entry_action = NO_ACTION;
    
    int result = Parse_Time_from_now_Statement( mSentence, mh );
    if (result>-1)
        retval=0;
    else {
        int absolute_date_found = Parse_date_Statement( mSentence, mh );
        if (absolute_date_found>-1)
            retval = 0;
    }

    ce.m_scheduled_time_bd = som;    // Store for later!
    
    /* A calendar reference should have:
            a specific date (whether arrived at absolute or relatively)
            an action:  (add, query, cancel)
     */
    int foundAdd    = mSentence.is_found_in_sentence( "add" ) || mSentence.is_found_in_sentence( "new" );
    int foundRemove = mSentence.is_found_in_sentence( "cancel" ) || mSentence.is_found_in_sentence( "delete" ) || mSentence.is_found_in_sentence( "remove" ) ||
                        mSentence.is_found_in_sentence( "forget" ) || mSentence.is_found_in_sentence( "drop" );

    int foundRemind  = mSentence.is_found_in_sentence ( "reminder" );
    if (foundRemind<=0) foundRemind = mSentence.is_found_in_sentence ( "remind" );
    if (foundRemind<=0) foundRemind = mSentence.is_found_in_sentence ( "notice" );
    if (foundRemind<=0) foundRemind = mSentence.is_found_in_sentence ( "appointment" );
    if (foundRemind<=0) foundRemind = mSentence.is_found_in_sentence ( "meeting" );
    //int foundAppoint = mSentence.is_found_in_sentence ( "appointment" ) || mSentence.is_found_in_sentence ( "meeting" );
    if (foundAdd && (foundRemind))
    {
        if (mSentence.is_question())
        {
            ce.sql_load( 1 );
        } else {
            // We still need to get a description & location.
            //form_response
            string response;
            response = mSentence.get_nth_word(foundRemind-1);
            response += " has been added to your calendar ";
            response += form_date_string();
            mh->form_response(response.c_str());
            ce.sql_add();
            //bool new_calendar_entry_desired = true;
        }
        retval = 0;
    }
    
    if (foundRemove && (foundRemind))
    {
        string response;
        response = mSentence.get_nth_word(foundRemind);
        response += " has been removed from your calendar";
        response += form_date_string();
        mh->form_response(response.c_str());

        ce.m_description = "";
        ce.m_location    = "";
        ce.m_user_id     = 0;
        ce.sql_update();
        retval = 0;
    }
    /*****     User specifying a date, time, year.  *****/
    
    
    if (retval>-1)   printf( "Parse_Calendar_Statement done\n" );
    return retval;
}

void test_calendar_protocol()
{
	printf("======= CALENDAR PROTOCOL UNIT TESTS ==========\n");
	ServerHandler mh;
	Sentence sent;
	sent.set("what time is it?");
	int result = Parse_Calendar_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );	

	sent.set("what is the date today?");
	result = Parse_Calendar_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );	

	sent.set("Set a reminder for Friday morning 8am to take out the trash");
	result = Parse_Calendar_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );	

	sent.set("Set a timer for 10 minutes");
	result = Parse_Calendar_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );	

	sent.set("I need a wake up call at 7am tomorrow");
	result = Parse_Calendar_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );	

	sent.set("make a new appointment at 2:30pm tomorrow at the dentist");
	result = Parse_Calendar_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );	

	sent.set("Do I have any appointments today?");
	result = Parse_Calendar_Statement( sent, &mh );
	if (mh.nlp_reply_formulated)  printf("RESPONSE: %s\n", mh.NLP_Response.c_str() );	

}
