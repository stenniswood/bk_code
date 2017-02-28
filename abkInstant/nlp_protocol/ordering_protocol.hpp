//
//  ordering_protocol.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/15/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef ordering_protocol_hpp
#define ordering_protocol_hpp

#include <stdio.h>
#include <string>
#include <vector>

#include "super_string.hpp"
#include "nlp_sentence.hpp"


using namespace std;


#define SMALL       0x0001
#define MEDIUM      0x0002
#define LARGE       0x0004
#define EXTRA_LARGE 0x0008
#define LUNCH_SIAE  0x0010
#define DINNER_SIZE 0x0020

struct stSizeVariation
{
    // in english size usually comes before the object.
    // and if the size is a noun, such as "cup" or "bowl" we need a preposition between size and the object.  This is the prep_modifier.
    string          prep_modifier;
    vector<string>  names;
    vector<float>   prices;
};

class Topping
{
public:
    void    list_toppings       ( );
    void    ask_toppings        ( );
    void    parse_toppings      ( Sentence& mSentence );
    void    load_size_options   ( int mSize_id );
    
    int size_id;
    vector<int> requested_indices;      // from parse, user requested these. each entry is an index into name/additional cost.
    SuperString name;
    SuperString additional_cost;
};

class MenuItem               /* 1 Item */
{
public:
    MenuItem();
    ~MenuItem();

    void    ask_size            ( );
    char*   get_name            ( );        // factors in the size words if present.
    int     parse_size_response ( Sentence& mSentence  );       // returns if valid answer given.
    void    load_size_options   ( int mSize_id );
    
    int     item_id;
    SuperString  name;
    float        price;
    SuperString  catagory;
    struct stSizeVariation  sizes;
    int     user_selected_size;
    SuperString topping_ids;        // ',' separated lists of id's into the size_options table.
    vector<Topping> m_toppings;     // may have multiple rows in the size_options table.
    // toppings will be MenuItem objects.
};

class Menu                      /* Entire Menu */
{
public:
    Menu();
    ~Menu();
        
    void    ask_size            ( MenuItem& mItem );
    void    parse_size_response ( MenuItem& mItem, Sentence& mSentence  );    
    int		Parse_Menu( Sentence& mSentence, bool mAddToCart = true );      // scan all catagories.
    
    vector<MenuItem>   m_items;
};

/*class RestaurantCluster                * Everything Restaurant *
{
public:
    RestaurantCluster ();
    ~RestaurantCluster();
    
    void    sql_load                 (  );
    void    retrieve_menu            (  );
    int     Parse_order_statement    ( Sentence& mSentence );

    void    list_voice_enabled_restaurants( );
    void    what_kind_of__do_you_have( string mcatagory );
    void    how_much_is              ( Sentence& mSentence, string mcatagory );

    vector<Restaurant>      m_restaurants;
    int     m_focus_index;
}; */


void    connect_menus_db        ();
void    close_menus_db          ();
//string  find_known_restaurants  ();
//void    retrieve_menu           ( Restaurant& mRest, char* mRestaurant);



#endif /* ordering_protocol_hpp */






