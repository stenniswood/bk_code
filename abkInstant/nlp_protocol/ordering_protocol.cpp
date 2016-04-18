//
//  ordering_protocol.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/15/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <my_global.h>
#include <mysql.h>

#include "ordering_protocol.hpp"
#include "GENERAL_protocol.hpp"
#include "specific_restaurants.hpp"


/********************************* SQL FUNCTIONS *********************************************/
MYSQL           *menus_db;
string          query_string;
MYSQL_RES*		result;
MYSQL_ROW       row;
string          known_restaurants;
int  has_verbal_focus = 0;                 // Verbal focus - ie next statements are likely to be handled here until the order is comlete.
RestaurantOrder  my_order;


void object_finish_with_error( )
{
    fprintf    ( stderr, "%s\n", mysql_error(menus_db));
    mysql_close( menus_db );
    exit(1);
}
void connect_menus_db()
{
    menus_db = mysql_init(NULL);
    if (menus_db == NULL)
    {
        fprintf(stderr, "init %s\n", mysql_error(menus_db));
        exit(1);
    }
    
    if (mysql_real_connect(menus_db, "localhost", "root", "password",
                           "bk_advertisements", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "real_connect %s\n", mysql_error(menus_db));
        mysql_close(menus_db);
        exit(1);
    }
}
int query( bool mRetrieving )
{
    if (mysql_query(menus_db, query_string.c_str() ))
    {
        fprintf(stderr, "Object: %s\n", mysql_error(menus_db));
    }
    
    size_t row_count=0;
    // Question : what happens when no results?
    //   Answer : if  mysql_store_result returns a non-zero value
    if (mRetrieving) {
        result = mysql_store_result(menus_db);
        if (result == NULL)  {
            printf("query Error result==null!\n");
            return 0;
        }
        row_count = mysql_num_rows(result);
    }
    return row_count;
}

string find_known_restaurants()
{
    query_string = "SELECT DISTINCT restaurant from restaurant_menu_items;";
    query(true);
    string retval;
    
    while ((row = mysql_fetch_row(result)) )
    {
        retval += row[0];
        retval += ", ";
    }
    mysql_free_result( result );
    known_restaurants = retval;
    return retval;
}

static char* append_int( int mInt )
{
    static char tmp[16];
    sprintf(tmp, "%d", mInt );
    return tmp;
}

void Topping::load_size_options(int mSize_id)
{
    query_string = "select * from sizes_options WHERE size_id=";
    query_string += append_int(mSize_id);
    query_string += ";";
    // restaurant = restaurant_id;  but we don't know the restaurant id.
    query(true);
    
    while ((row = mysql_fetch_row(result)) )
    {
        name = row[3];
        name.split(',');
        name.trim_spaces_in_splits();
        if (row[4]) {
            additional_cost = row[4];
            additional_cost.split(',');
            additional_cost.trim_spaces_in_splits();
        }
    }
    mysql_free_result( result );
}

void Topping::list_toppings ( )
{
    int size =name.m_split_words.size();
    if (size==0)   return;
    string list;
    for (int i=0; i<size; i++)
    {
        list += name.m_split_words[i];
        list += ", ";
    }
    append_response( list.c_str() );
}
void  Topping::ask_toppings        ( )
{
    if (name.m_split_words.size()==0)   return;
    
    append_response("What toppings would you like on it?");
}

void  Topping::parse_toppings      ( Sentence& mSentence )
{
    int top_index = 0;
    int pos = 0;
    while ((pos = mSentence.m_sentence.any_one_word_found_in_sentence(name, top_index, pos )) < mSentence.m_sentence.m_split_words.size())
    {
        requested_indices.push_back(top_index);
    }
    
}


/******************************END OF  SQL ********************************************/
MenuItem::MenuItem()
{
}
MenuItem::~MenuItem()
{
}
void MenuItem::load_size_options(int mSize_id)
{
    query_string = "select * from sizes_options WHERE size_id=";
    query_string += append_int(mSize_id);
    query_string += ";";
    // restaurant = restaurant_id;  but we don't know the restaurant id.
    query(true);
    
    while ((row = mysql_fetch_row(result)) )
    {
        sizes.names.push_back(row[3]);
        if (row[4]) {
            float prix = atof(row[4]);
            sizes.prices.push_back(prix);
        }
    }
    mysql_free_result( result );
}

char* MenuItem::get_name( )        // factors in the size words if present.
{
    static char response[80];
    assert( user_selected_size<=sizes.names.size() );
    
    if (sizes.names.size())
    {
        sprintf(response, "%s %s %s",
            sizes.names[user_selected_size].c_str(),
            sizes.prep_modifier.c_str(),
            name.c_str());
    }
    else
        sprintf(response, "%s", name.c_str() );
    return response;
}

void MenuItem::ask_size( )
{
    char str[80];
    size_t num_sizes = sizes.names.size();
    string question = "Okay, ";    question += name;
    sprintf(str, " comes in %ld sizes: ", num_sizes );
    question += str;
    float base_cost = sizes.prices[0];         // Must be sorted lowest to highest cost!
    for (int s=0; s < num_sizes; s++)
    {
        question += sizes.names[s];
        if (s<(num_sizes-1))
            question += ", ";
        else {
            /*float extra_cost = sizes.prices[s] - base_cost;
            if (extra_cost>0)
            {
                sprintf(str, "for %6.2f extra", extra_cost);
                question += str;
            }*/
        }
    }
    my_order.q_to_user_pending = true;
    my_order.q_item_pending = *this;
    form_response( question.c_str() );
}


/* return:  1==>match (user answered the question)
            0 ==> no match (user didn't answer the question */
int MenuItem::parse_size_response( Sentence& mSentence )
{
    bool one_of_the_sizes_found = false;
    size_t num_sizes = sizes.names.size();
    for (int i=0; i<num_sizes; i++) {
        one_of_the_sizes_found = mSentence.is_found_in_sentence( sizes.names[i].c_str() );
        if (one_of_the_sizes_found) {
            user_selected_size = i;
            return 1;   // match
        }
    }
    return 0;   // no match.
}


/**************************************************************************************
 class Menu
 **************************************************************************************/

Menu::Menu()
{
    
}
Menu::~Menu()
{
    
}


/* This is the final item (after size and all options have been determined. */
void RestaurantOrder::add_to_order        ( MenuItem mItem )
{
    static char response[128];
    m_order.push_back( mItem );
    if (mItem.sizes.names.size()) {
        sprintf(response, "Okay a %s has been added to your order",  mItem.get_name() );
    } else
        sprintf(response, "Okay %s has been added to your order", mItem.name.c_str());
    form_response(response);
}

int Menu::Parse_Menu( Sentence& mSentence, bool mAddToCart )      // scan all catagories.
{
    static char response[128];
    int retval = 0;
    int found  = 0;

    vector<MenuItem>::iterator iter = m_items.begin();
    while (iter != m_items.end())
    {
        found = mSentence.is_found_in_sentence( iter->name.c_str() );
        if ((found) && (mAddToCart)) {
            if (iter->sizes.names.size()==0) {
                my_order.add_to_order(*iter);

            } else {
                iter->ask_size(  );
            }
            retval++;
            has_verbal_focus = 1;
        }
        iter++;
    }
    return retval;
}
/*
 
 int retval = 0;
 vector<MenuCatagory>::iterator iter = m_catagories.begin();
 while (iter != m_catagories.end() )
 {
 int result = iter->Parse_ItemList( mSentence, mAddToCart );
 if (result)
 retval = result;
 iter++;
 }
 return retval;

 */
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
    has_verbal_focus = 0;    
}
Restaurant::~Restaurant()
{

}
void Restaurant::retrieve_menu( )
{
    query_string = "SELECT * FROM bk_advertisements.restaurant_menu_items WHERE restaurant='";
    query_string += m_restaurant_name;
    query_string += "';";
    query(true);
    
    MenuItem tmpItem;
    tmpItem.user_selected_size = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) )
    {
        if (row[1]) tmpItem.name     = row[1];
        if (row[2]) tmpItem.price    = atof(row[2]);
        if (row[3]) tmpItem.catagory = row[3];
        if (row[5]) // number of sizes
        {
            size_t size_id;
            if (row[6])
                size_id = atoi(row[6]);
            // not great programming practice.  but we'll temporarily store the size_id in user_selected_size until properly loaded below.
            tmpItem.user_selected_size = size_id;
        }
        if (row[7]) {
            tmpItem.topping_ids = row[7];
            tmpItem.topping_ids.split(',');
            tmpItem.topping_ids.trim_spaces_in_splits();
        }
        tmpItem.name.trim_spaces();
        tmpItem.catagory.trim_spaces();
        m_menu.m_items.push_back( tmpItem );
    }
    mysql_free_result( result );
    
    // Now load all the sizes:
    vector<MenuItem>::iterator iter = m_menu.m_items.begin();
    while ( iter != m_menu.m_items.end() )
    {
        if (iter->user_selected_size)
            iter->load_size_options( iter->user_selected_size );
        int tops = iter->topping_ids.m_split_words.size();
        Topping tmpTop;
        if (tops)
            for(int t=0; t<tops; t++)
            {
                int top_id = atoi(iter->topping_ids.m_split_words[t].c_str());
                tmpTop.load_size_options( top_id );
                iter->m_toppings.push_back(tmpTop);
            }
        iter++;
    }
    
}

void Restaurant::what_kind_of__do_you_have( string mcatagory )
{
    // select all in catagory
}
void Restaurant::how_much_is              ( Sentence& mSentence, string mcatagory )
{
    int result=0;
    result = m_menu.Parse_Menu( mSentence, false );
    //form_response("item %s is $%6.2f", item.name.c_str(), item.price );
    
    
}
void Restaurant::extract_result          ( )
{
    m_id = atoi(row[0]);
    m_offical_name = row[1];
    if (row[2]) m_restaurant_name = row[2];
    if (row[3]) m_website      = row[3];
    // store locator url
    if (row[5]) m_frequency    = atof(row[5]);
    if (row[6]) m_welcome_text = row[6];
    if (row[7]) m_order_url = row[7];
    
    bool standard_welcome = m_welcome_text.compare("standard")==0;
    if (standard_welcome) {
        m_welcome_text  = "Welcome ";
        m_welcome_text += m_restaurant_name;
        m_welcome_text += " will be happy to take your order.  How may we begin? ";
    }
    
}
void Restaurant::sql_load                 ( string mRestaurantOfficialName )
{
    query_string = "SELECT * FROM bk_advertisements.restaurants WHERE official_name='";
    query_string += mRestaurantOfficialName;
    query_string += "';";
    query(true);
    
    //size_t row_count = mysql_num_rows(result);
        
    if ((row = mysql_fetch_row(result)) )
    {
        extract_result();
    }
    mysql_free_result( result );
}

int Restaurant::Parse_order_statement( Sentence& mSentence )
{
    int retval=-1;
    int handled=0;
    
    bool foundS =   mSentence.is_found_in_sentence( "place" ) &&
                    mSentence.is_found_in_sentence( "order" );
    bool found_Rest = mSentence.is_found_in_sentence( m_restaurant_name.c_str() );
    if ((foundS) && (found_Rest)) {
        m_verbal_focus = this;
        has_verbal_focus = 1;
        my_order.reset();
        form_response( m_welcome_text.c_str() );
        retval = 0;
    }
    
    if (m_verbal_focus == this) {
        if (my_order.q_to_user_pending) {
            if (my_order.q_item_pending.parse_size_response(mSentence)) {
                my_order.add_to_order( my_order.q_item_pending );
                my_order.q_to_user_pending = false;
                retval = 0;
            }
        }
//        handled  = my_order.cancel_order( mSentence );
        handled  = my_order.cancel_item( mSentence );
        
        if (handled==0)
            handled = m_menu.Parse_Menu( mSentence );
    
        if (handled==0)
        {
            handled = my_order.is_order_complete( mSentence );
            if (handled)
                if (my_order.just_read_back)
                {
                    form_response("Okay, placing order");
                } else
                    my_order.read_back_order();
        }
        if (handled==0)
        {
            handled = my_order.is_request_order_readback( mSentence );
            if (handled)
                my_order.print_order();
        }
    }
    if (retval)     printf( "Parse_restaurants_order_Statement done\n" );
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
    query(true);

    while ((row = mysql_fetch_row(result)) )
    {
        tmp_rest.extract_result();
        m_restaurants.push_back(tmp_rest);
    }
    mysql_free_result( result );
}

void    RestaurantCluster::retrieve_menu            (  )
{
    vector<Restaurant>::iterator iter = m_restaurants.begin();
    while (iter != m_restaurants.end())
    {
        // names already filled in!
        iter->retrieve_menu();
        iter++;
    }
}
int RestaurantCluster::Parse_order_statement    ( Sentence& mSentence )
{
    int retval = -1;
    bool foundS =   mSentence.is_found_in_sentence( "place" ) &&
                    mSentence.is_found_in_sentence( "order" );
    bool found_Rest=false;

    if (foundS)
    {
        // Do we know the restaurant user is talking about?
        int count =0;
        vector<Restaurant>::iterator iter = m_restaurants.begin();
        while (iter != m_restaurants.end())
        {
            found_Rest = mSentence.is_found_in_sentence( iter->m_offical_name.c_str() ) ||
                         mSentence.is_found_in_sentence( iter->m_restaurant_name.c_str() );
            if (found_Rest) {
                // YES.
                retval = 0;
                m_verbal_focus = (void*)&(*iter);      // or this  general proto one.
                m_focus_index = count;
                my_order.reset();
                form_response( iter->m_welcome_text.c_str() );
                retval = 0;
                
                //m_verbal_focus = ;
                break;
            }
            iter++;
            count++;
        }
        if ((foundS) && (!found_Rest))
        {
            char str[80];
            sprintf(str, "Sorry, I do not know about that restaurant" );
            form_response(str);
            return 0;
        }
    }
    else if (m_focus_index>=0)
        retval = m_restaurants[m_focus_index].Parse_order_statement(mSentence);
    if (my_order.q_to_user_pending)
        my_order.q_item_pending.parse_size_response(mSentence);

    
    if (my_order.orderFoundComplete)
        m_focus_index = -1;
    return retval;
}
void    RestaurantCluster::list_voice_enabled_restaurants( )
{
    string reply = "I know of these restaurants: ";
    reply += known_restaurants;
    reply += " and can place an order for you via the internet.";
    form_response( reply.c_str() );
    
}
void    RestaurantCluster::what_kind_of__do_you_have( string mcatagory )
{   /* ie. list all items in a catagory */
    
}
void    RestaurantCluster::how_much_is              ( Sentence& mSentence, string mcatagory )
{

}


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
void RestaurantOrder::reset()
{
    m_order.clear();
    orderFoundComplete = false;
    just_read_back = 0;
}

void RestaurantOrder::anything_else( )
{
    append_response("Will there be any thing else?");
}

int RestaurantOrder::is_order_complete( Sentence& mSentence )
{
    orderFoundComplete = mSentence.is_found_in_sentence( "no more" ) ||
    mSentence.is_found_in_sentence( "that's it" ) ||
    mSentence.is_found_in_sentence( "that's all" ) ||
    mSentence.is_found_in_sentence( "no" );
    
    if (orderFoundComplete) has_verbal_focus = 0;
    if (orderFoundComplete)
        return 1;
    else
        return 0;
}
int RestaurantOrder::is_request_order_readback( Sentence& mSentence )
{
    bool proceed = mSentence.is_found_in_sentence( "repeat" ) || mSentence.is_found_in_sentence( "say again" ) ||
    mSentence.is_found_in_sentence( "so far" )     || mSentence.is_found_in_sentence( "tell me" );
    
    if (proceed) { read_back_order();  anything_else(); };
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
    static vector<MenuItem>::iterator  iter = m_order.begin();
    while (iter != m_order.end())
    {
        int result = mSentence.is_found_in_sentence( iter->name.c_str() );
        if (result)
            return &iter;
        iter++;
    }
    return NULL;
}

int RestaurantOrder::cancel_item( Sentence& mSentence )
{
    bool proceed = mSentence.is_found_in_sentence( "forget" ) || mSentence.is_found_in_sentence( "cancel" ) ||
    mSentence.is_found_in_sentence( "drop" );
    
    if (proceed) {
        vector<MenuItem>::iterator* iter = find_order_item_in_sentence( mSentence );
        if (iter)
        {
            m_order.erase( *iter );
            form_response("okay removed. ");
        }
        //anything_else();
    }
    if (proceed)
        return 1;
    else
        return 0;
}

void RestaurantOrder::read_back_order(  )
{
    static string full_order;
    full_order = "Okay, your order is : ";
    float SumTotal = 0.0;
    int count =0;
    vector<MenuItem>::iterator iter = m_order.begin();
    while (iter != m_order.end())
    {
        full_order += (iter)->get_name();
        full_order += ", ";
        //printf("%3d :%s \n", count++,  );
        SumTotal += (iter)->price;
        iter++;
    }
    full_order += ".  And your total is ";
    char total[12];
    sprintf(total, "$%6.2f. ", SumTotal);
    full_order += total;
    form_response(full_order.c_str());
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
        printf("%3d :%s \n", count++, (iter)->name.c_str() );
        SumTotal += (iter)->price;
        iter++;
    }
    printf("Total: $%6.2f", SumTotal);
}






