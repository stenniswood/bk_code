//
//  jets_pizza.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 3/15/16.
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
#include "ordering_protocol.hpp"
#include "specific_restaurants.hpp"
#include "jets_pizza.hpp"



/***********************************************************************
 
 Suggested statements:
 
 ***********************************************************************/


void init_jets( )
{
/*    Jets.m_welcome_text = "Welcome Jets Pizza will be happy to take your order.  How may we begin? " ;
    Jets.m_restaurant_name = "jets";

    struct stMenuItem mi;
    mi.available_sizes = 0;
    
    tmp_cat.m_items.clear();
    tmp_cat.m_catagory_name = "toppings";
    mi.name = "pepperoni";              mi.price = 1.34;        tmp_cat.m_items.push_back( mi );
    mi.name = "bold pepperoni";         mi.price = 1.05;        tmp_cat.m_items.push_back( mi );
    mi.name = "ham";                    mi.price = 1.34;        tmp_cat.m_items.push_back( mi );
    mi.name = "italian sausage";        mi.price = 1.34;        tmp_cat.m_items.push_back( mi );
    mi.name = "bacon";                  mi.price = 1.05;        tmp_cat.m_items.push_back( mi );
    mi.name = "hamburger";              mi.price = 1.34;        tmp_cat.m_items.push_back( mi );
    mi.name = "meatballs";              mi.price = 1.05;        tmp_cat.m_items.push_back( mi );
    mi.name = "anchovies";              mi.price = 1.34;        tmp_cat.m_items.push_back( mi );
    mi.name = "mushrooms";              mi.price = 1.34;        tmp_cat.m_items.push_back( mi );
    mi.name = "onions";                 mi.price = 1.05;        tmp_cat.m_items.push_back( mi );
    mi.name = "red onions";             mi.price = 1.05;        tmp_cat.m_items.push_back( mi );
    mi.name = "green peppers";          mi.price = 1.05;        tmp_cat.m_items.push_back( mi );
    
    // Cream Cheese Spreads:
    //bool Topic2 = tmp_cat.m_items.push_back( ( "cream cheese" );
    mi.name = "black olives";           mi.price = 1.25;        tmp_cat.m_items.push_back( mi );
    mi.name = "green olives";           mi.price = 1.25;        tmp_cat.m_items.push_back( mi );
    mi.name = "pineapple";              mi.price = 1.25;        tmp_cat.m_items.push_back( mi );
    mi.name = "mild peppers";           mi.price = 1.25;        tmp_cat.m_items.push_back( mi );
    mi.name = "jalapeño peppers";       mi.price = 1.25;        tmp_cat.m_items.push_back( mi );
    mi.name = "tomatoes";               mi.price = 1.25;        tmp_cat.m_items.push_back( mi );
    mi.name = "cheddar cheese";         mi.price = 1.25;        tmp_cat.m_items.push_back( mi );
    mi.name = "shredded parmesan cheese";  mi.price = 1.25;     tmp_cat.m_items.push_back( mi );
    Jets.m_menu.m_catagories.push_back( tmp_cat );  */
}

void Init_Jets_Order_Protocol()
{
/*    struct stMenuItem mi;
    mi.available_sizes = 0;
    
    tmp_cat.m_items.clear();
    tmp_cat.m_catagory_name = "soups";
    
    mi.name = "eight corner pizza";             mi.price = 7.99;        tmp_cat.m_items.push_back( mi );
    mi.name = "grilled chicken";                mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    mi.name = "all meaty";                      mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    mi.name = "super special";                  mi.price = 5.99;        tmp_cat.m_items.push_back( mi );
    mi.name = "veggie";                         mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    mi.name = "buffalo ranch chicken";          mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    mi.name = "bold fold";                      mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    mi.name = "gluten-free pizza";              mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    
    Jets.m_menu.m_catagories.push_back( tmp_cat );
    
    *  bool foundTopic = mSentence.is_found_in_sentence ( "pastry" ) || mSentence.is_found_in_sentence ( "sweet" );  *
    
    tmp_cat.m_items.clear();
    tmp_cat.m_catagory_name = "salads";
    mi.name = "anti-pasto";                      mi.price = 7.99;        tmp_cat.m_items.push_back( mi );
    mi.name = "greek";                             mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    mi.name = "garden";                         mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    mi.name = "chef";                         mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    mi.name = "party";                      mi.price = 5.29;        tmp_cat.m_items.push_back( mi );
    Jets.m_menu.m_catagories.push_back( tmp_cat );
    
    
    init_jets(); */
    
}



