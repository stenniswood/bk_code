//
//  Panera_ordering_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/2/16.
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
#include <string>
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
#include "self_identity.hpp"
#include "menu_items.hpp"
#include "specific_restaurants.hpp"


/***********************************************************************
 
 Suggested statements:

Want to load all restaurants in the database.  Not just specific ones!
 
 ***********************************************************************/
RestaurantCluster restaurants;


void save_order( QuestionToUser* mQ );


void Init_Restaurants_Order_Protocol()
{
    /* SQL Restaurant Initialization */
    connect_menus_db();
    
    restaurants.sql_load();
    restaurants.retrieve_menus();
    
    //string dummy = find_known_restaurants();
    //printf("Known Restaurants: %s", dummy.c_str());
    
    /* END SQL Restaurant Initialization */
}


/**************************************************************************************
 class Restaurant
 **************************************************************************************/

/*****************************************************************
 Do the work of the Telegram :
 return  -1	=> Not handled
 else number of extra bytes extracted from the mSentence buffer.
 - besides strlen(mSentence)!
 *****************************************************************/
Restaurant::Restaurant ()
{
    //has_verbal_focus = 0;
}
Restaurant::~Restaurant()
{
    
}

void Restaurant::extract_result          ( MYSQL_ROW mrow )
{
    m_id = atoi( mrow[0] );
    m_offical_name             = mrow[1];
    if (mrow[2]) m_restaurant_name = mrow[2];
    if (mrow[3]) m_website      = mrow[3];
    // store locator url
    if (mrow[5]) m_frequency    = atof(mrow[5]);
    if (mrow[6]) m_welcome_text = mrow[6];
    if (mrow[7]) m_order_url    = mrow[7];
    
    bool standard_welcome = m_welcome_text.compare("standard")==0;
    if (standard_welcome) {
        m_welcome_text  = "Welcome ";
        m_welcome_text += m_restaurant_name;
        m_welcome_text += " will be happy to take your order.  How may we begin? ";
    }
}

void Restaurant::sql_load( string mRestaurantOfficialName )
{
    query_string = "SELECT * FROM bk_advertisements.restaurants WHERE official_name='";
    query_string += mRestaurantOfficialName;
    query_string += "';";
    query(true);

    //size_t row_count = mysql_num_rows(result);    
    if ((row = mysql_fetch_row(result)) )
    {
        extract_result( row );
    }
    mysql_free_result( result );
}

int Restaurant::Parse_order_statement( Sentence& mSentence, ServerHandler* mh )
{
    int retval=-1;
    int handled=0;
    RestaurantOrder tmpOrder;
    
    handled     = user.get_order().start_over ( mSentence, mh );
    if (handled==0)
        handled = user.get_order().cancel_item( mSentence, mh );
    if (handled==0)
        handled = m_menu.what_kind_of__do_you_have   ( mSentence, mh );
    if (handled==0)
        handled = m_menu.how_much_is                 ( mSentence, mh );
    if (handled==0)  {
        handled = user.get_order().is_request_order_readback( mSentence, mh );
    }
    if (handled==0) {
        user.m_pending.reset( );
        handled = m_menu.Parse_Menu( mSentence, mh, user.m_pending );     // Adds to order
        if (handled) {
            string response;
            if ( user.m_pending.any_unanswered_questions()==false )
            {
                user.recite_additions( response );
                user.get_order().add_to_order( user.m_pending );              // This will add the ones that can be added                
                mh->form_response(response.c_str());
            }
            // And the callback to the questions will add
            // the rest after all the information has been received.
        }
    }
    if (handled==0)
    {
        handled = user.get_order().is_order_complete( mSentence );
        if (handled) {
            //m_focus_index = 0;
            if (user.get_order().just_read_back)
            {
                mh->form_response("Okay, placing order");
            } else {
                string str;
                user.get_order().read_back_order(str);
                mh->form_response(str.c_str());
            }
        }
    }
    if (handled)
        return 0;
    else
        return retval;
}



/**************************************************************************************
 class RestaurantCluster
 **************************************************************************************/

RestaurantCluster::RestaurantCluster ()
{
    m_focus_index = -1;
}
RestaurantCluster::~RestaurantCluster()
{
    
}

void    RestaurantCluster::sql_load                 (  )
{
    Restaurant tmp_rest;
    query_string = "SELECT * FROM bk_advertisements.restaurants ;";
    int val = query(true);
    if (val<=0) return;
    
    while ((row = mysql_fetch_row(result)) )
    {
        tmp_rest.extract_result(row);
        m_restaurants.push_back(tmp_rest);
    }
    mysql_free_result( result );
}

void    RestaurantCluster::retrieve_menus           (  )
{
    vector<Restaurant>::iterator iter = m_restaurants.begin();
    while (iter != m_restaurants.end())
    {
        // names already filled in!
        iter->m_menu.retrieve_menu( iter->m_id, iter->m_restaurant_name );
        iter++;
    }
}

int RestaurantCluster::find_restaurant_name( Sentence& mSentence, vector<int>* mRestaurantIndex )
{
    int index      =0;
    int match_count=0;
    
    vector<Restaurant>::iterator iter = m_restaurants.begin();
    while (iter != m_restaurants.end())
    {
        int found_Rest = mSentence.are_found_in_sentence( iter->m_offical_name.c_str() ) ||
        mSentence.are_found_in_sentence( iter->m_restaurant_name.c_str() );
        if (found_Rest) {
            if (mRestaurantIndex)
                mRestaurantIndex->push_back(index);
            match_count++;
        }
        iter++;
        index++;
    }
    return match_count;
}


// Do we know the restaurant user is talking about?
int RestaurantCluster::select_restaurant( Sentence& mSentence, ServerHandler* mh )
{
    vector<int> restaurant_indices;
    int match_count = find_restaurant_name( mSentence, &restaurant_indices );
    if (match_count)
    {   // YES :
        RestaurantOrder tmp;
        user.m_orders.push_back( tmp );      // Start one order!
        
        long int rest_index = restaurant_indices[0];
        // Is an order already pending?
        //if ( (m_verbal_focus) && (m_verbal_focus != (void*)&m_restaurants[rest_index]) )
        if (m_focus_index>-1)
        {
            ActionToUserResponse* ua = new ActionToUserResponse();
            ua->m_callback = (void*)save_order;
            ua->m_user    = &user;
            ua->m_datum1  = mh;
            ua->m_datum2  = (void*)rest_index;
            ua->m_datum3  = this;

            // Yes, ask user if he wants to keep it
            QuestionToUser q;
            q.m_callback_action = ua;
            q.m_question  = "You have an order pending at ";
            q.m_question += m_restaurants[m_focus_index].m_offical_name.c_str();
            q.m_question += " Do you want me to place the order?";
            q.m_responses = "(cancel|forget|discard|scrap|no) (save|keep|later|comeback|yes)";
            q.m_responses.split(' ');
            q.m_responses.trim_spaces_in_splits();
            
            Qs_to_user.mh = mh;
            Qs_to_user.all_questions_answered_cb = NULL;
            Qs_to_user.user_questions.push_back(&q);
            
            //m_verbal_focus = (void*)&m_restaurants[rest_index];      // or this  general proto one
            m_focus_index  = rest_index;
            return 0;
        } else {
            // No, set focus on new order and play welcome prompt.
            //m_verbal_focus = (void*)&m_restaurants[rest_index];      // or this  general proto one
            m_focus_index  = rest_index;
            user.get_order().reset( );
            mh->form_response( m_restaurants[rest_index].m_welcome_text.c_str() );
            return 0;
        }
    } else {
        char str[80];
        sprintf(str, "Sorry, I do not know about that restaurant" );
        mh->form_response(str);
        return 0;
    }
    return -1;
}

int RestaurantCluster::Parse_order_statement( Sentence& mSentence, ServerHandler* mh )
{
    int  retval    = -1;
    bool found_Rest= false;
    
    // QUESTIONS? :
    if (Qs_to_user.any_questions_awaiting_response()) {
        if (Qs_to_user.parse_response( mSentence.m_sentence )) {
            
            // if all questions answered, then add to order
            //    what if there were questions not pertaining to adding an item?
            //if (Qs_to_user.any_unasked_questions())
            retval = 0;
        }
    }
    if (retval==-1) {
        int foundS = mSentence.is_found_in_sentence( "place" ) && mSentence.is_found_in_sentence( "order" );
        if (foundS)
            retval = select_restaurant( mSentence, mh );
        else if (m_focus_index>=0) {
            retval = m_restaurants[m_focus_index].Parse_order_statement(mSentence,mh);
            user.get_order().is_order_complete(mSentence);
        }
    }
    // COMPUTER ASK QUESTION :
    if (Qs_to_user.any_unasked_questions())
        Qs_to_user.ask(mh);
    
    if (user.get_order().orderFoundComplete)
        m_focus_index = -1;
    return retval;
}

void RestaurantCluster::list_voice_enabled_restaurants( ServerHandler* mh )
{
    string reply = "I know of these restaurants: ";
    reply += known_restaurants;
    reply += " and can place an order for you via the internet.";
    mh->form_response( reply.c_str() );
    
}


