//
//  Panera_ordering_protocol.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/2/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef Panera_ordering_protocol_hpp
#define Panera_ordering_protocol_hpp

#include <stdio.h>
#include "nlp_sentence.hpp"
#include "menu_items.hpp"
#include "menus_orders.hpp"


void Init_Restaurants_Order_Protocol( );


/* Everything Restaurant
 
 Restaurant has a set of locations
 Hours,
 name, website, etc.
 */
class Restaurant
{
public:
    Restaurant ();
    ~Restaurant();

    void    extract_result           ( MYSQL_ROW mrow 						  );
    void    sql_load                 ( string mRestaurantOfficialName 		  );
    int     Parse_order_statement    ( Sentence& mSentence, ServerHandler* mh );
        
    //int     has_verbal_focus;
    int                     m_id;
    std::string             m_offical_name;
    std::string             m_restaurant_name;
    std::string             m_website;
    std::string             m_order_url;
    float                   m_frequency;
    std::string             m_welcome_text;
    Menu                    m_menu;
    vector<std::string>     m_locations;
};


class RestaurantCluster                /* Everything Restaurant */
{
public:
    RestaurantCluster ();
    ~RestaurantCluster();
    
    void    sql_load                 (  );
    void    retrieve_menus           (  );
    
    int     Parse_order_statement         ( Sentence& mSentence, ServerHandler* mh             );
    int     find_restaurant_name          ( Sentence& mSentence, vector<int>* mRestaurantIndex );
    void    list_voice_enabled_restaurants( ServerHandler* mh                                  );
    
    void    what_kind_of__do_you_have     ( string mcatagory                      );
    void    how_much_is                   ( Sentence& mSentence, string mcatagory );
    int     select_restaurant             ( Sentence& mSentence, ServerHandler* mh );
    
    vector<Restaurant> m_restaurants;
    int     m_focus_index;
};


extern RestaurantCluster restaurants;

#endif /* Panera_ordering_protocol_hpp */
