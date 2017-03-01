//
//  adrenaline_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/15/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <regex>
#include <my_global.h>
#include <mysql.h>
#include "nlp_sentence.hpp"
#include "GENERAL_protocol.hpp"
//#include "question_to_user.hpp"
//#include "menu_items.hpp"
//#include "specific_restaurants.hpp"
//#include "simulator_memory.h"

using namespace std;


static MYSQL               *creator_db;
static string              query_string;
static MYSQL_RES*          result;
//static MYSQL_ROW           row;

#include "machine_defs.h"

void connect_creator_db()
{
    creator_db = mysql_init(NULL);
    if (creator_db == NULL)
    {
        fprintf(stderr, "init %s\n", mysql_error(creator_db));
        exit(1);
    }
    
    if (mysql_real_connect(creator_db, "localhost", sql_username, sql_password,
                           sql_global_knowledge_name, 0, NULL, 0) == NULL)
                           // was robot_local
    {
        fprintf(stderr, "Adrenaline:: real_connect %s\n", mysql_error(creator_db));
        mysql_close(creator_db);
        exit(1);
    }
}

int cquery( bool mRetrieving )
{
    if (mysql_query(creator_db, query_string.c_str() ))
    {
        fprintf(stderr, "Object: %s\n", mysql_error(creator_db));
    }
    
    size_t row_count=0;
    // Question : what happens when no results?
    //   Answer : if  mysql_store_result returns a non-zero value
    if (mRetrieving) {
        result = mysql_store_result(creator_db);
        if (result == NULL)  {
            printf("query Error result==null!\n");
            return 0;
        }
        row_count = mysql_num_rows(result);
    }
    return (int)row_count;
}


void Init_Adrenaline_Protocol( )
{
    /* SQL Restaurant Initialization */
    connect_creator_db();
}

void form_query_string( Sentence& mSentence )
{
    query_string = "SELECT * FROM `robot_local`.`creator` WHERE \"";
    query_string += mSentence.m_reduced_sentence;
    
    query_string += "\" RLIKE element;";
}

int Parse_adrenaline_statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval = -1;
    form_query_string( mSentence );
    retval = cquery  ( true );
    
    if (result)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) )
        {
            // LOAD ONE ITEM :
            if (row[1]) {
                printf(": %s\t", row[1] );
            }
            if (row[2]) {
                printf("%s\t", row[2] );
            }
            if (row[3]) {
                printf("%s\t", row[3] );
            }
            if (row[4]) {
                printf("%s\t", row[4] );
            }
        }
    }
    return (retval>0)?retval:-1;  // at least 1 row found.
}

