//
//  menus_orders.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/23/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <regex>

#include "question_to_user.hpp"
#include "menu_items.hpp"
#include "GENERAL_protocol.hpp"
#include "specific_restaurants.hpp"
#include "simulator_memory.h"
#include "menus_orders.hpp"



UserOrderHistory    user;


/**************************************************************************************
 class RestaurantOrder
 **************************************************************************************/
RestaurantOrder::RestaurantOrder()
{
    q_to_user_pending = false;
    orderFoundComplete = false;
    just_read_back = 0;
}
RestaurantOrder::~RestaurantOrder()
{
    
}

/* This is the final item (after size and all options have been determined. */
void RestaurantOrder::add_to_order( MenuItem mItem, ServerHandler* mh )
{
    string response;
    m_order.push_back( mItem );
    response = "Okay, ";
    response += mItem.get_name();
    response += " has been added to your order";
    
    
    mh->form_response(response.c_str());
}

/* This is the final item (after size and all options have been determined. */
void RestaurantOrder::add_to_order        ( RestaurantOrder& mNewItems )
{
    if (mNewItems.m_order.size()==0)
        return ;
    
    vector<MenuItem>::iterator iter = mNewItems.m_order.begin();
    while (( iter != mNewItems.m_order.end() ) && (mNewItems.m_order.size()>0))
    {
        if (iter->can_be_added()) {              // if all required options are resolved.
            m_order.push_back( *iter );
            mNewItems.m_order.erase( iter );
        } else
            iter++;
        //iter++;   DO NOT Increment here, the erase above will move elements up.  So iter must stay at [0]
    }
}

void RestaurantOrder::reset()
{
    m_order.clear();
    orderFoundComplete = false;
    just_read_back = 0;
}

void RestaurantOrder::anything_else( ServerHandler* mh )
{
    mh->append_response("Will there be any thing else?");
}


int RestaurantOrder::is_order_complete( Sentence& mSentence )
{
    orderFoundComplete = mSentence.are_found_in_sentence( "no more" ) ||
    mSentence.are_found_in_sentence( "that's it" ) ||
    mSentence.are_found_in_sentence( "that's all" ) ||
    mSentence.is_found_in_sentence( "no" );    
    if (orderFoundComplete)
        return 1;
    else
        return 0;
}
int RestaurantOrder::is_request_order_readback( Sentence& mSentence, ServerHandler* mh )
{
    int proceed =   mSentence.is_found_in_sentence( "repeat" ) ||
    mSentence.are_found_in_sentence( "say again" ) ||
    mSentence.are_found_in_sentence( "say order" ) ||
    mSentence.are_found_in_sentence( "so far" )    ||
    mSentence.are_found_in_sentence( "tell me" );
    
    if (proceed) {
        string str;
        read_back_order  ( str );
        mh->form_response( str.c_str() );
        anything_else    ( mh  );
    };
    if (proceed)
        return 1;
    else
        return 0;
}

void RestaurantOrder::remove_item_in_order( string mItem )
{
    vector<MenuItem>::iterator  iter = m_order.begin();
    while (iter != m_order.end())
    {
        iter++;
        if (iter->name.compare( mItem )==0)
            m_order.erase( iter );
    }
}

vector<MenuItem>::iterator* RestaurantOrder::find_order_item_in_sentence( Sentence& mSentence)
{
    string expression;
    static vector<MenuItem>::iterator  iter = m_order.begin();
    while (iter != m_order.end())
    {
        iter->construct_regex_with_options( expression );
        int result = mSentence.m_reduced_sentence.regex_find( expression );
        //int result = mSentence.is_found_in_sentence( iter->name.c_str() );
        if (result)
            return &iter;
        iter++;
    }
    return NULL;
}

int RestaurantOrder::start_over( Sentence& mSentence, ServerHandler* mh )
{
    regex cancel_e( "[ ^](restart|cancel|forget|drop|start over) (all|order|everything|it)[ $]" );
    std::smatch m;
    regex_search ( mSentence.m_sentence, m, cancel_e );
    
    if (m.size()) {
        m_order.clear();
        mh->form_response("Okay! Let's begin again.");
        return 1;
    }
    return 0;
}

int RestaurantOrder::cancel_item( Sentence& mSentence, ServerHandler* mh )
{
    regex cancel_e("(remove|take out|take off|forget|cancel|drop|delete)");
    std::smatch m;
    regex_search(mSentence.m_sentence, m, cancel_e);
    
    if (m.size())
    {
        vector<MenuItem>::iterator* iter = find_order_item_in_sentence( mSentence );
        if (iter)
        {
            m_order.erase( *iter );
            mh->form_response("okay removed. ");
        }
        //anything_else();
    }
    if (m.size())
        return 1;
    else
        return 0;
}

float RestaurantOrder::get_total( )
{
    float SumTotal = 0.0;
    vector<MenuItem>::iterator iter = m_order.begin();
    while (iter != m_order.end())
    {
        SumTotal += (iter)->price;
        iter++;
    }
    return SumTotal;
}

bool RestaurantOrder::any_unanswered_questions(  )
{
    for (int i=0; i<m_order.size(); i++)
    {
        if (m_order[i].can_be_added()==false)
            return true;
    }
    return false;
}
/* Appends to end of string
 */
void RestaurantOrder::read_back_items( string& mStr )
{
    for (int i=0; i<m_order.size(); i++)
    {
        mStr += m_order[i].get_full_name();
        if (i<(m_order.size()-2))
            mStr += ", ";
        else if (i<(m_order.size()-1))
            mStr += ", and ";
    }
}

void RestaurantOrder::read_back_order( string& mFull_order )
{
    mFull_order     = "Okay, your order is: ";
    read_back_items( mFull_order );
    
    float SumTotal = get_total();
    mFull_order += ".  And your total is ";
    char total[12];
    sprintf(total, "$%6.2f. ", SumTotal);
    mFull_order += total;
    just_read_back = 1;
}

void RestaurantOrder::print_order( )
{
    float SumTotal = 0.0;
    
    int count =0;
    printf("Your Order:");
    vector<MenuItem>::iterator iter = m_order.begin();
    while (iter != m_order.end())
    {
        printf("%d - %s\n", count, iter->get_full_name().c_str() );
        //printf("%3d :%s \n", count++, (iter)->name.c_str() );
        SumTotal += (iter)->price;
        iter++;
        count++;
    }
    printf("Total: $%6.2f\n", SumTotal);
}

/**************************************************************************************
 class UserOrderHistory
 **************************************************************************************/
UserOrderHistory::UserOrderHistory()
{
}

UserOrderHistory::~UserOrderHistory()
{
    
}
RestaurantOrder& UserOrderHistory::get_order()
{
    static RestaurantOrder tmp;
    size_t lastIndex = m_orders.size();
    if (lastIndex==0)
        return tmp;
    return m_orders.at( lastIndex-1 );
}

void UserOrderHistory::recite_additions( string& mResponse )
{
    // add items which are ready
    mResponse = "Okay, ";
    m_pending.read_back_items(mResponse);
    if (m_pending.m_order.size() == 1)
        mResponse += " has ";
    else
        mResponse += " have ";
    mResponse += "been added to your order";
}

void UserOrderHistory::append_pending()
{
    get_order().add_to_order( m_pending );
    m_pending.reset();
}




