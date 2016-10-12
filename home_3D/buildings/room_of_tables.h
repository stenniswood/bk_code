//
//  room_of_tables.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/3/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#ifndef __home3D__room_of_tables__
#define __home3D__room_of_tables__

#include <stdio.h>
#include "dwelling_level.h"


class glRestaurant : public glDwellingLevel
{
public:
    glRestaurant ();
    ~glRestaurant();
    
    void            create_vertical_walls   ( );
    void            create_horizontal_walls ( );
    void            create_table_arrangement( );
    virtual void    add_light_switches      ( );
    virtual void    create_components       ( );
    
    
};


#endif /* defined(__home3D__room_of_tables__) */
