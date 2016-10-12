//
//  apartment.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/7/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__apartment__
#define __home3D__apartment__

#include <stdio.h>
#include "dwelling_level.h"
#include "painting.h"

class glApartment : public glDwellingLevel
{
public:
    glApartment ();
    ~glApartment();
    
    void            create_vertical_walls   ( );
    void            create_horizontal_walls ( );
    void            create_apartment        ( );

    virtual void    add_light_switches      ( );
    virtual void    create_components       ( );
    
    void            hang_paintings          ( );
    glPainting      sidewalk_painting;    
};

class glMezannineUpstairs : public glDwellingLevel
{
public:
    glMezannineUpstairs ();
    ~glMezannineUpstairs();
    
    virtual void    add_light_switches       ( );
    virtual void    create_components       ( );
    
};

class glMezannineDownstairs : public glDwellingLevel
{
public:
    glMezannineDownstairs ();
    ~glMezannineDownstairs();
    
    virtual void    add_light_switches       ( );
    virtual void    create_components       ( );
    
};


#endif /* defined(__home3D__apartment__) */
