//
//  ordering_protocol.cpp
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

#include "question_to_user.hpp"
#include "menu_items.hpp"
#include "GENERAL_protocol.hpp"
#include "specific_restaurants.hpp"
#include "simulator_memory.h"


string integer_e   = "(\d+ )";
string real_e      = "([-+]?[0-9]*\\.?[0-9]+ )";

/********************************* SQL FUNCTIONS *********************************************/
MYSQL               *menus_db=NULL;
string              query_string;
MYSQL_RES*          result;
MYSQL_ROW           row;
string              known_restaurants;
int                 has_verbal_focus = 0;  // Verbal focus - ie next statements will be handled here until the order is comlete.

extern class IPC_Exception IPC_Error;


static void object_finish_with_error( )
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
        printf("Cannot connect to mysql server!\n");
        return;  // exit(1);
    }
    
    if (mysql_real_connect(menus_db, "localhost", "root", "password",
                           "bk_advertisements", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "real_connect %s\n", mysql_error(menus_db));
        printf("Cannot connect to database bk_advertisements !\n" );
        mysql_close(menus_db);
        menus_db = NULL;
        return;  // exit(1);
    }
}

int query( bool mRetrieving )
{
	if (menus_db==NULL) return 0;
	
    if (mysql_query( menus_db, query_string.c_str() ))
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
    return (int)row_count;
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

/****************************************************************************************
        PickOne Class
 ****************************************************************************************/
PickOne::PickOne()
{
    size_id       =-1;
    selected_item ="none";
}
void  PickOne::list_choices( string& mstr, char mDelim, bool mConjoin  )
{
    size_t size = name.m_split_words.size();
    string list;
    for (int i=0; i<size; i++)
    {
        list += name.m_split_words[i];
        if (i<(size-1)) {
            list += mDelim;
            list += " ";
        }
        if ((i==(size-2)) && (mConjoin))
            list += "or ";
    }
    mstr += list.c_str();
}

void all_questions_answered_cb( UserForm* mQ, ServerHandler* mh )
{
    // Add the items to the order :
    string response;
    user.recite_additions( response         );
    mh->form_response    ( response.c_str() );
    user.append_pending  ( );
}

/*
   Each time a question about a required option is answered:
 */
void required_choice_cb( QuestionToUser* mQ )
{
    long int di     = (long int )mQ->m_callback_action->m_datum1;
    MenuItem* mi    = (MenuItem*)&(user.m_pending.m_order[di]);

    long int index  = (long int )mQ->m_callback_action->m_datum2;
    PickOne* po     = &(mi->m_required_options[index]);
    po->selected_item = mQ->m_matches[0];
}

void  PickOne::ask_choice( string& mQuestion, string mName, string& mResponses )
{
    string tmp;
    // [SQL Comment] + [list of choices] + "for the" + [name of item]
    // ie. "what size small, medium or large for the french fry?"
    //list_choices(tmp,',',true);

    mQuestion  = comment + " ";
    mQuestion += mName;
    mQuestion += "?";

    mResponses = "(";
    size_t size = name.m_split_words.size();
    for (int i=0; i<size; i++)
    {
        mResponses += name.m_split_words[i];
        if (i<(size-1))
            mResponses += "|";
    }
    mResponses += ")";
}

// Return:  true => match
//          false => no match
bool  PickOne::parse( Sentence& mSentence, int mStart_word, int mEnd_word )
{
    if (mStart_word==-1) mStart_word=0;     // if default value.
    if (mEnd_word  ==-1) mEnd_word  =(int) mSentence.m_sentence.m_split_words.size();  // if default value
    
    vector<int> results;
    bool   one_of_the_sizes_found = false;
    size_t num_sizes = name.m_split_words.size();
    
    // Loop thru all possible options.
    for (int i=0; i<num_sizes; i++)
    {
        int result = mSentence.m_sentence.is_found_in_sentence( name.m_split_words[i].c_str(), mStart_word, &results );
        if (result) {
            one_of_the_sizes_found = true;
            selected_item         = i;
        }
    }
    return one_of_the_sizes_found;
}

string PickOne::get_selected_choice( )
{
    string tmp;
    if (selected_item.length()) {
        return selected_item;
    }
    return "none";
}

float PickOne::get_additional_cost( )
{
    if (selected_item.length()) {
        return additional_cost[selected_item];
    }
    return 0.0;
}

/****************************************************************************************
 Topping Class
 ****************************************************************************************/
StandardOptions::StandardOptions()
{
    
}
bool StandardOptions::parse( Sentence& mSentence, int mStart_word, int mEnd_word )
{
    PickOne::parse( mSentence,mStart_word,mEnd_word );
    return true;
}

/****************************************************************************************
        Topping Class
 ****************************************************************************************/
AdditionalToppings::AdditionalToppings()
{
    size_id = 0;
}

void  AdditionalToppings::ask_toppings        ( string& mstr  )
{
    if (name.m_split_words.size()==0)   return;
    mstr += "What toppings would you like on it?";
}

bool  AdditionalToppings::parse( Sentence& mSentence, int mStart_word, int mEnd_word )
{
    int pos = 0;
    vector<int> match_indices;
    pos = mSentence.m_sentence.any_one_word_found_in_sentence(name, pos, &match_indices);
    if (pos) {
        if (match_indices.size()) {
            //requested_indices.push_back(match_indices[0]);
            return true;
        }
    }
    return false;
}

/****************************** END OF  SQL ********************************************/
MenuItem::MenuItem()
{
    m_quantity = 0;
    item_id  = 0;
    name     = "";
    price    = 0.0;
    catagory = "";
    //user_selected_size = 0;
}
MenuItem::~MenuItem()
{
    
}

void MenuItem::construct_regex_with_options( string& mRegExpression )
{
    string tmp,exp;
    exp = "\\b";      // Initial word break/start

    // QUANTITY:
    if (m_quantity)  {
        //exp += to_string( m_quantity );
        //exp += "? ?";
    }
    
    // Required Options as optional :
    if (m_required_options.size()) {
        for (int i=0; i<m_required_options.size(); i++)
        {
            exp += "(";
            exp += m_required_options[i].selected_item;
            exp += ")? ?";
        }
    }
    
    // the MenuItem name
    exp += "(";
    exp += name;
    exp += ")";
    
    if (pronounce_catagory)
    {
        exp += " ";
        exp += catagory;
        exp += " ";
    }
    // Removed items;
    size_t size = m_standard_toppings.size();
    if (size) {
        exp += " ?(without|with no|remove|take out|don't put|extra|with extra)? ?";
        for (int i=0; i<size; i++)
            exp += m_standard_toppings[i].name;
        exp += "?";
    }
    
    // Additional toppings
    size = m_additional_toppings.size();
    if (size) {
        exp += " ?(with)? ?";
        for (int i=0; i<size; i++)
            exp += m_additional_toppings[i].name;
        exp += "?";
    }
    
    exp += "\\b";
    mRegExpression = exp;
}

void MenuItem::construct_regex( )
{
    string tmp;
    string exp;
    exp = "\\b";      // Initial word break/start
    // QUANTITY:
    exp += integer_e;
    exp += "? ?";
    
    // Required Options as optional:
    if (m_required_options.size()) {
        //exp += " ";
        for (int i=0; i<m_required_options.size(); i++)
        {
            exp += m_required_options[i].name;
        }
        exp += "? ?";
    }

    // the MenuItem name 
    exp += "(";
    exp += name;
    exp += ")";

    if (pronounce_catagory)
    {
        exp += " ";
        exp += catagory;
        exp += " ";
    }
    // Removed items;
    size_t size = m_standard_toppings.size();
    if (size) {
        exp += " ?(without|with no|remove|take out|don't put|extra|with extra)? ?";
        for (int i=0; i<size; i++)
            exp += m_standard_toppings[i].name;
        exp += "?";
    }
    
    // Additional toppings
    size = m_additional_toppings.size();
    if (size) {
        exp += " ?(with)? ?";
        for (int i=0; i<size; i++)
            exp += m_additional_toppings[i].name;
        exp += "?";
    }

    exp += "\\b";
    m_regexp = exp;
    regexp = exp;
}

void MenuItem::load_size_options(int mSize_id, int mRestaurant_id)
{
    query_string = "select * from sizes_options WHERE size_id=";
    query_string += append_int(mSize_id);
    query_string += " and restaurant_id=";
    query_string += append_int(mRestaurant_id);
    query_string += ";";
    // restaurant = restaurant_id;  but we don't know the restaurant id.
    query(true);
    int                 type=-1;
    PickOne             one;
    AdditionalToppings  top;
    SuperString         extra_cost;
    StandardOptions     standard;

    while ((row = mysql_fetch_row(result)) )
    {
        type = -1;
        if (row[6])
            type = atoi(row[6]);
        extra_cost = row[4];
        extra_cost.split(',');
        extra_cost.trim_spaces();
        
        switch(type)
        {
        case 0 :
                standard.name = row[3];
                standard.name.split(',');
                standard.name.trim_spaces();
                for (int i=0; i < standard.name.m_split_words.size(); i++)
                    standard.additional_cost[standard.name.m_split_words[i] ] = stof(extra_cost.m_split_words[i]); // .convert_to_float();
                
                if (row[5]) standard.comment = row[5];
                m_standard_toppings.push_back(standard);
                break;
        case 1 :
                one.name                = row[3];
                one.name.split_wordlist();    // "(small|medium|large)"  Want to do this because the prices will be a map based on these names.
                for (int i=0; i < standard.name.m_split_words.size(); i++)
                    one.additional_cost[one.name.m_split_words[i] ] = stof(extra_cost.m_split_words[i]); // .convert_to_float();
                if (row[5])
                    one.comment = row[5];
                m_required_options.push_back(one);
                break;
        case 2 :
                top.name = row[3];
                top.name.split(',');
                top.name.trim_spaces();
                for (int i=0; i < standard.name.m_split_words.size(); i++)
                    top.additional_cost[top.name.m_split_words[i]] = stof(extra_cost.m_split_words[i]); // convert_to_float();
                if (row[5]) top.comment = row[5];
                m_additional_toppings.push_back(top);
            break;
        default:
            break;
        }
    }
    mysql_free_result( result );
}

bool MenuItem::can_be_added( )    // all required options have been selected
{
    for (int r=0; r<m_required_options.size(); r++)
        if (m_required_options[r].selected_item.compare("none")==0)
            return false;
    return true;
}

//
string MenuItem::get_name( )
{
    string response;
    
    // List required options (large, medium, small) preceed the name with the user (or default) selected size :
    for (int i=0; i<m_required_options.size(); i++)
    {
        if (m_required_options[i].selected_item.compare("none") != 0) {
            response += m_required_options[i].selected_item;
            //response = m_required_options[i].get_selected_choice();        // Just the item name "plain bagel"
            response += " ";
        }
    }
    response += name.m_split_words[0];
    if (pronounce_catagory)
    {
        response += " ";
        // adjective is the noun.  name is just an adjective.
        response += catagory.c_str();                        // ie. "plain !bagel"
    }
    return response;
}

string MenuItem::get_full_name( )        // factors in the size words if present.
{
    static string response;
    response = get_name();
    size_t size;
    
    // list toppings removed:
    if (any_standard_options_removed()) {
        response += " without ";
        for (int i=0; i<m_standard_toppings.size(); i++)    // the list has all available standard toppings.
        {
            size = m_standard_toppings[i].requested_indices.size();
            for (int j=0; j <size; j++) {
                response += m_standard_toppings[i].requested_indices[j];
                if (j==(size-2))
                    response += ", ";
            }
            // only report those which are officially removed!
        }
    }

    // list additional toppings :
    if (any_additional_toppings())
    {
        response += " with ";
        int    add=0;
        string tmp;
        for (int i=0; i<m_additional_toppings.size(); i++)
        {
            size_t ri_size = m_additional_toppings[i].requested_indices.size();
            if (ri_size)
                for (int j=0; j<ri_size; j++)
                {
                    tmp += m_additional_toppings[i].requested_indices[j];
                    if ( j < (ri_size-2) )
                        tmp += ", ";
                }
        }
        response += tmp;
    }
    return response;
}

float MenuItem::get_cost()
{
    float retval = price;
    for (int i=0; i<m_required_options.size(); i++)
        retval += m_required_options[i].get_additional_cost();

    for (int i=0; i<m_additional_toppings.size(); i++)
        retval += m_additional_toppings[i].get_additional_cost();
    return retval;
}

/* return:  1==>match (user answered the question)
            0 ==> no match (user didn't answer the question */
int MenuItem::parse_size_response( Sentence& mSentence )
{
    return (m_required_options[0].parse( mSentence )==true);
}


/*  Searches the sentence for any and all matching words
    Works "inplace" ie modifies internal selected_index variable.
 
INPUT:
    mStartingWordIndex  - word index for the menu item.  ie. "baked potatoe soup"  point to 'baked'
RETURN:
    number of distinct required_options that were found and updated.
 */
int MenuItem::find_matching_required_options( Sentence& mSentence, int mStartingWordIndex  )      // match score
{
    int retval = 0;
    
    // Search for any required options present:
    for (int ro=0; ro<m_required_options.size(); ro++ )
    {
        size_t num_words_in_option = m_required_options[ro].name.m_split_words.size();
        size_t prefix_word_index   = mStartingWordIndex - num_words_in_option;
        int result = mSentence.are_found_in_sentence( m_required_options[ro].name.c_str() );
        if (result==prefix_word_index)
        {
            m_required_options[ro].selected_item = ro;
            retval++;
        }
    }
    return retval;
}

int MenuItem::find_matching_standard_options( Sentence& mSentence, int mStartingWordIndex  )     // removed items
{
    return 0;
}

int MenuItem::find_matching_additional_options( Sentence& mSentence, int mStartingWordIndex  )   // match score
{
    return 0;
}

/* Returns the match score - Number of matching words. */
int MenuItem::is_found_in_sentence( Sentence& mSentence )
{
    mSentence.m_reduced_sentence.regex_find( m_regexp );
    size_t result = mSentence.m_reduced_sentence.regex_matches.size();
    return (result & 0xFFFF);
}

int MenuItem::file_results(Sentence& mSentence)
{
    SuperString tmp;
    //for (auto x:mSentence.m_reduced_sentence.regex_matches)
    for (int m=1; m<mSentence.m_reduced_sentence.regex_matches.size(); m++)
    {
        tmp = mSentence.m_reduced_sentence.regex_matches[m].str();
        
        if (mSentence.m_reduced_sentence.regex_matches.size())
            m_quantity = 1;
        // is first match a number?  file as Quantity
        tmp.trim_spaces();
        tmp.split(' ');
        if (tmp.is_nth_word_a_number(0))
        {
            m_quantity = stof( tmp.c_str() );
        }
        
        // is match a required option?
        if (m_required_options.size())
        {
            string exp;
            for (int ro=0; ro<m_required_options.size(); ro++)
            {
                exp += m_required_options[ro].name;
                tmp.regex_find( exp );
                size_t result = tmp.regex_matches.size();           // Select the item, if info provided in sentence.
                if (result)                                         // (we'll not have to ask the user)
                    m_required_options[ro].selected_item = tmp.regex_matches[0];
            }
        }
        
        // Is the match an additional topping?
        for (int at=0; at<m_additional_toppings.size(); at++)
        {
            string str = m_additional_toppings[at].name.c_str();    //
            tmp.regex_find( str );
            if ( tmp.regex_matches.size() )
                m_additional_toppings[at].requested_indices.push_back( tmp.c_str() );
        }
        string tmps;
        // is match a removed topping?
        for (int st=0; st<m_standard_toppings.size(); st++)
        {
            string str = m_standard_toppings[st].name.c_str();    //
            tmp.regex_find( str );
            //tmps = tmp;
            if ( tmp.regex_matches.size() )
                m_standard_toppings[st].requested_indices.push_back( tmp );
        }
    }
    return m_quantity;
}


void MenuItem::pose_question_required(int ro, ServerHandler* mh )
{
    // ACTION IN RESPONSE TO ANSWERED QUESTION :
    ActionToUserResponse* ua = new ActionToUserResponse();
    ua->m_user     = &user;
    ua->m_callback = (void*)required_choice_cb;
    //ua->m_datum1   = (void*)this;
    ua->m_datum1   = (void*)(user.m_pending.m_order.size()-1);
    ua->m_datum2   = (void*)ro;
    
    string Q,A;
    m_required_options[ro].ask_choice( Q, name.m_split_words[0], A );    // adds to the user_questions queue
    
    QuestionToUser* q = new QuestionToUser();
    q->m_callback_action = ua;
    q->m_question  = Q;
    q->m_responses = A;
    q->m_responses.split_wordlist();
    ua->m_question = q;
    
    Qs_to_user.user_questions.push_back(q);
    Qs_to_user.all_questions_answered_cb = all_questions_answered_cb;
    Qs_to_user.mh = mh;
}

bool MenuItem::any_standard_options_removed    ()
{
    for (int i=0; i<m_standard_toppings.size(); i++)    // the list has all available standard toppings.
    {
        if ( m_standard_toppings[i].requested_indices.size() )
            return true;
    }
    return false;
}

bool MenuItem::any_additional_toppings         ()
{
    for (int i=0; i<m_additional_toppings.size(); i++)    // the list has all available standard toppings.
    {
        if ( m_additional_toppings[i].requested_indices.size() )
            return true;
    }
    return false;
}

// adds them to the standard & additional toppings vectors :
int MenuItem::scan_for_toppings( Sentence& mSentence, ServerHandler* mh )
{
    for (long int s=0; s < m_required_options.size(); s++)
    {
        if (m_required_options[s].selected_item == "none")
        {
            pose_question_required(s, mh);
        }
    }
    // When should the question get asked?
    // before we exit the General Protocol.
    
    return 0;
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

void Menu::retrieve_menu( int mID, string mRestaurant_name )
{
    query_string = "SELECT * FROM bk_advertisements.restaurant_menu_items WHERE restaurant='";
    query_string += mRestaurant_name;
    query_string += "';";
    query(true);
    
    MenuItem tmpItem;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) )
    {
        // LOAD ONE ITEM:
        if (row[1]) {   tmpItem.name = row[1];  };
        if (row[2]) tmpItem.price    = atof(row[2]);
        if (row[3]) {
            tmpItem.catagory = row[3];
            tmpItem.pronounce_catagory = false;
            if (tmpItem.catagory[0] == '!') {
                tmpItem.pronounce_catagory = true;
                tmpItem.catagory.erase(0,1);
            }
            tmpItem.catagory.split(' ');
        }
        if (row[5]) {                               // Options (additional, required or standard)
            tmpItem.topping_ids = row[5];
            tmpItem.topping_ids.split(',');
            tmpItem.topping_ids.trim_spaces_in_splits();
        }
        tmpItem.name.trim_spaces();
        tmpItem.name.split('|');
        tmpItem.catagory.trim_spaces();
        m_items.push_back( tmpItem );
    }
    mysql_free_result( result );
    
    // Now load all Sizes (required options) for all Items:
    //vector<MenuItem>::iterator
    auto iter = m_items.begin();
    while ( iter != m_items.end() )
    {
        int id = 0;
        size_t size = iter->topping_ids.m_split_words.size();
        for (int i=0; i<size; i++)
        {
            id = atoi( iter->topping_ids.m_split_words[i].c_str() );
            iter->load_size_options( id, mID );
            //iter->construct_regex();
        }
        iter++;
    }
    
    // CONSTRUCT ALL REGULAR EXPRESSIONS :
    iter = m_items.begin();
    while ( iter != m_items.end() )
    {
        iter->construct_regex();
        iter++;
    }
}

int Menu::what_kind_of__do_you_have( Sentence& mSentence, ServerHandler* mh )
{
    string str;
    //if (mSentence.regex_found_in_sentence("what (kind|types) of"))
    if (mSentence.are_found_in_sentence("what kind of")||mSentence.are_found_in_sentence("what types of"))
    {
        //int windex  = mSentence.get_word_pointer();
        int index=-1;
        int result = catagory_match( mSentence, index );
        if (result==0)
            mh->form_response( "We do not have any." );
        else {
            //string next_word = mSentence.m_reduced_sentence.m_split_words[0];
            
            // search for a catagory:
            string all_of = list_all_of_catagory(m_items[index].catagory);
            str = "We have ";
            str += all_of.c_str();
            mh->form_response(str.c_str());
            return 1;
        }
    }
    return 0;
}

string Menu::list_all_of_catagory( string mCatagory )
{
    string result = "";
    vector<MenuItem>::iterator iter =  m_items.begin();
    for ( ; iter != m_items.end(); iter++)
    {
        if (iter->catagory==mCatagory) {
            result += iter->name;
            result += ", ";
        }
    }
    return result;
}

int Menu::catagory_match( Sentence& mSentence, int& match_index )
{
    int match_count = 0;
    match_index =-1;
    
    // Iterate over all menuitems:
    for (int mi=0; mi<m_items.size(); mi++)
    {
        match_count += mSentence.are_found_in_sentence( m_items[mi].catagory );
        if ((match_count) && (match_index==-1))
            match_index = mi;
    }
    return match_count;
}

int Menu::how_much_is( Sentence& mSentence, ServerHandler* mh )
{
    int result =0;
    RestaurantOrder tmpOrder;
    if (mSentence.are_found_in_sentence("how much"))
    {
        result = Parse_Menu( mSentence, mh, tmpOrder);
        if (tmpOrder.m_order.size()==0)
            mh->form_response("I could not find that on the menu.");
        else {
            SuperString tmp = "Okay, ";
            tmpOrder.read_back_items( tmp );
            tmp += " costs $";
            float total = tmpOrder.get_total();
            tmp.append_float ( total );
            mh->form_response( tmp.c_str() );
        }
        return 1;
    }
    return 0;
}

MenuItem* item;

/*
 Scans every item in the menu for matches within the sentence.
 Returns:  Number of items found.
 
 */
int Menu::Parse_Menu( Sentence& mSentence, ServerHandler* mh, RestaurantOrder& mPendingOrder )      // scan all catagories
{
    int retval = 0;
    int found  = 0;
    /* "plain cream cheese"  could be parsed as "plain [bagel]", "cheese [pastry]", cream unknown.
     plain bagel happens to be in the database before 'plain cream cheese'.
     We need to:
     a) Not remove the words from the list until all items have been compared (end of the loop)
     b) After the entire list has been compared, pick the result with the most matching words.
     c)
     */
    vector<int>  indices;
    vector<int>  scores;
    int count=0;
    
    // Find all matches score :
    vector<MenuItem>::iterator iter = m_items.begin();
    while (iter != m_items.end())
    {
        found = iter->is_found_in_sentence(mSentence);
        if (found) {
            int Quantity = iter->file_results( mSentence );
            indices.push_back( count );
            scores.push_back ( found );
            for (int s=0; s<Quantity; s++)
                mPendingOrder.m_order.push_back( *iter );
            
            int last = (int)mPendingOrder.m_order.size();
            item = &(mPendingOrder.m_order[last-1]);
            
            //int matches =
            item->scan_for_toppings( mSentence,mh );   // also send word index from the find above.
            
            
            retval++;
            //has_verbal_focus = 1;
            //mOrder.add_to_order( *iter, mh );
        }
        iter++;
        count++;
    }
    
    // Get the Best score :
    /*    int best_score_index = -1;
     int best_score       = -1;
     for (int i=0; i<indices.size(); i++) {
     if (scores[i] > best_score)
     {
     best_score = scores[i];
     best_score_index = indices[i];
     }
     } */
    
    // Add best match to order :
    
    /*if ((best_score_index >= 0) && (mAddToCart))
     {
     if ( m_items[best_score_index].m_required_options.size() == 0 )
     {
     mOrder.add_to_order( m_items[best_score_index], mh );
     } else {
     string question;
     int index = m_items[best_score_index].required_index;
     m_items[best_score_index].m_required_options[index].ask_choice( question );
     mh->append_response(question.c_str());
     }
     retval++;
     has_verbal_focus = 1;
     }*/
    
    return retval;
}


/*
 */
void save_order( QuestionToUser* mQ )   // , ServerHandler* mh, RestaurantCluster* mCluster
{
    ServerHandler* mh = (ServerHandler*)mQ->m_callback_action->m_datum1;
    
    if (mQ->m_matches.size()) {
        UserOrderHistory* uoh = mQ->m_callback_action->m_user;
        
        //
        long int new_rest_index =           (long int) mQ->m_callback_action->m_datum2;
        RestaurantCluster* rc   = (RestaurantCluster*) mQ->m_callback_action->m_datum3;
        uoh->get_order().m_order.clear();
         mh->form_response("Okay order discarded.");
        user.get_order().reset( );

        mh->form_response( rc->m_restaurants[new_rest_index].m_welcome_text.c_str() );
        rc->m_focus_index  = new_rest_index;
    }
    else if (mQ->m_matches[1] > -1) {
        string response = "Okay placing order.  Please enter credit card info.";  // at Burger King
        mh->form_response( response.c_str() );
    }
}


