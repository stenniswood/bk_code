//  Contains :  PickOne, StandardOptions, AdditionalToppings
//           :  MenuItem
//           :  RestaurantOrder, UserOrderHistory
//
//
//  Created by Stephen Tenniswood on 3/15/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef ordering_protocol_hpp
#define ordering_protocol_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <my_global.h>
#include <mysql.h>
#include <regex>
#include <map>

#include "nlp_sentence.hpp"
#include "serverthread.hpp"
#include "question_to_user.hpp"


extern string integer_e;
extern string real_e   ;


int    query( bool mRetrieving );
extern string              query_string;
extern MYSQL_ROW           row;
extern MYSQL_RES*          result;
extern string              known_restaurants;


using namespace std;

#define SMALL       0x0001
#define MEDIUM      0x0002
#define LARGE       0x0004
#define EXTRA_LARGE 0x0008
#define LUNCH_SIAE  0x0010
#define DINNER_SIZE 0x0020


class PickOne
{
public:
    PickOne();
    void            list_choices      ( string& mstr, char mDelim, bool mConjoin );
    void            ask_choice        ( string& mQuestion, string mName, string& mResponses );
    virtual bool    parse             ( Sentence& mSentence, int mStart_word=-1, int mEnd_word=-1 );
    string          get_selected_choice( );
    float           get_additional_cost( );
    
    int             size_id;
    SuperString     name;

    string              selected_item;  // from parse, user requested these. each entry is an index into name/additional cost
    map<string,float>   additional_cost;
    std::string         comment;
};

class StandardOptions : public PickOne
{
public:
    StandardOptions();
    virtual bool    parse       ( Sentence& mSentence, int mStart_word=-1, int mEnd_word=-1 );
    
    vector<string>     requested_indices; // from parse, user requested these. each entry is an index into name/additional cost.
    //vector<bool>    removed;
};

class AdditionalToppings : public PickOne
{
public:
    AdditionalToppings();
    void            ask_toppings( string& mh          );
    virtual bool    parse       ( Sentence& mSentence, int mStart_word=-1, int mEnd_word=-1 );
    
    vector<string>     requested_indices;   // from parse, user requested these. each entry is an index into name/additional cost.
};


/***********************************************************************************
 1 Item - one record in the "restaurant_menu_items" table;
            and all options that go along with it. 
 **********************************************************************************/
class MenuItem
{
public:
    MenuItem ();
    ~MenuItem();

    void        construct_regex                 ();
    void        construct_regex_with_options    ( string& mRegExpression );
    
    bool        can_be_added                    ();         // all required options have been selected
    void        state_item_with_all_options     ( std::string& mresult          );
    string      get_name                        ( );        // just the item name. (catagory if required "!" preceeding)
    string      get_full_name                   ( );        // factors in the size words and options if present.
    float       get_cost                        ( );        // cost with any additional items
    int         file_results                    ( Sentence& mSentence );
    
    int         parse_size_response             ( Sentence& mSentence              );       // returns if valid answer given
    void        load_size_options               ( int mSize_id, int mRestaurant_id );

    int         find_matching_required_options  ( Sentence& mSentence, int mStartingWordIndex );       // match score
    int         find_matching_standard_options  ( Sentence& mSentence, int mStartingWordIndex );       // removed items
    int         find_matching_additional_options( Sentence& mSentence, int mStartingWordIndex );       // match score

    void        pose_question_required          (int ro, ServerHandler* mh );
    
    int         is_found_in_sentence            ( Sentence& mSentence  );                   // match score
    int         scan_for_toppings               ( Sentence& mSentence, ServerHandler* mh ); // adds them to the standard & additional toppings vectors
    bool        any_standard_options_removed    ();
    bool        any_additional_toppings         ();

    std::string     m_regexp;
    std::regex      regexp;                 //
    int             m_quantity;             //
    int             item_id;                //
    SuperString     name;                   //
    float           price;                  //
    bool            pronounce_catagory;     // The name of the item is an adjective for the catagory
    SuperString     catagory;               //
    SuperString     topping_ids;            // ',' separated lists of id's into the size_options table

    int                         required_index;         // Ask each required option 1 at a time. This stores the current one.
    vector<PickOne>             m_required_options;     //
    vector<StandardOptions>     m_standard_toppings;    // These come with the item.
    //May be individually removed. Only removed items are listed in get_full_name().
    vector<AdditionalToppings>  m_additional_toppings;  // Added to the item by customer.
    
};

class RestaurantOrder;

class Menu                      /* Entire Menu */
{
public:
    Menu();
    ~Menu();
    
    void    retrieve_menu            ( int mID, string mRestaurant_name );
    int     how_much_is              ( Sentence& mSentence, ServerHandler* mh );
    int     catagory_match           ( Sentence& mSentence, int& match_index );
    int     what_kind_of__do_you_have( Sentence& mSentence, ServerHandler* mh );
    
    string  list_all_of_catagory     ( string mCatagory );
    int     Parse_Menu               ( Sentence& mSentence, ServerHandler* mh, RestaurantOrder& mOrder);
    
    vector<MenuItem>   m_items;
};


void    connect_menus_db        ();
string  find_known_restaurants  ();



#endif /* ordering_protocol_hpp */






