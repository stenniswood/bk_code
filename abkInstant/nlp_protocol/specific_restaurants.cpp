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
#include "Alias.hpp"
#include "nlp_sentence.hpp"
#include "client_to_socket.hpp"

#include "self_identity.hpp"
#include "ordering_protocol.hpp"
#include "specific_restaurants.hpp"



/***********************************************************************
 
 Suggested statements:
 
 ***********************************************************************/
RestaurantCluster restaurants;
Restaurant   Panera;
Restaurant   Jets;
Restaurant   Subway;


void Init_Restaurants_Order_Protocol()
{
    /* SQL Restaurant Initialization */
    connect_menus_db();
    
    restaurants.sql_load();
    restaurants.retrieve_menu();
    
    string dummy = find_known_restaurants();
    printf("Known Restaurants: %s", dummy.c_str());

/*    Panera.sql_load("Panera Bread");
    Panera.retrieve_menu( );

    Panera.sql_load("Panera Bread");
    Panera.retrieve_menu( );
    
    Jets.sql_load("Jets Pizza");
    Jets.retrieve_menu();

    Subway.sql_load("Subway");
    Subway.retrieve_menu(); */
    
    /* END SQL Restaurant Initialization */
}


