//
//  menus_orders.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/23/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef menus_orders_hpp
#define menus_orders_hpp

#include <stdio.h>
#include <my_global.h>
#include <mysql.h>


extern string          query_string;
extern MYSQL_ROW       row;
extern MYSQL_RES*      result;
extern string          known_restaurants;
int  				   query( bool mRetrieving );

/* will be my_order object! */
class RestaurantOrder
{
public:
    RestaurantOrder ();
    ~RestaurantOrder();
    
    int     start_over              ( Sentence& mSentence, ServerHandler* mh );
    void    reset                   ( );
    void    anything_else           ( ServerHandler* mh    );
    int     is_order_complete       ( Sentence& mSentence  );
    int     is_request_order_readback( Sentence& mSentence, ServerHandler* mh );
    bool    any_unanswered_questions(  );
    
    vector<MenuItem>::iterator* find_order_item_in_sentence( Sentence& mSentence);
    void        add_to_order        ( MenuItem mItem, ServerHandler* mh );
    void        add_to_order        ( RestaurantOrder& mNewItems        );

    void        remove_item_in_order( string mItem                           );
    int         cancel_item         ( Sentence& mSentence, ServerHandler* mh );
    void        select_restaurant   ( Sentence& mSentence, ServerHandler* mh );
    
    float       get_total           (               );
    void        read_back_items     ( string& mh    );
    void        read_back_order     ( string& mStr  );
    void        print_order         ( );
    
    vector<MenuItem>  m_order;
    bool        q_to_user_pending;
    MenuItem    q_item_pending;
    
    bool    orderFoundComplete;
    int     just_read_back;
};


class UserOrderHistory
{
public:
    UserOrderHistory ();
    ~UserOrderHistory();
    
    RestaurantOrder& get_order();
    void             recite_additions( string& mResponse );
    void             append_pending();
    
    RestaurantOrder         m_pending;
    vector<RestaurantOrder> m_orders;
};

extern UserOrderHistory    user;

#endif /* menus_orders_hpp */
