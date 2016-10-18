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

time_t now;
struct tm bd_now;

// half hour = 30 min * 60 = 1800
const int HOUR_IN_SECONDS = 3600;
const int DAY_IN_SECONDS  = 3600*24;
const int WEEK_IN_SECONDS = 3600*24*7;

void time_qualitative( time_t mTime, string& mTimeString )
{
    int mins,hours,days,weeks;
    time_t now;
    time_t deltaTime;
    
    now = time(&now);
    struct tm* bd_time     = localtime(&mTime);
    struct tm* bd_currtime = localtime(&now);
    deltaTime = difftime( now, mTime );
    //printf( "deltaTime= %d\n", deltaTime );
    
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
            mTimeString = "1 day ago";
        else {
            mTimeString = std::to_string(days);
            mTimeString += " days ago";
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


