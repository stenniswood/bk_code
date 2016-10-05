//
//  elevator.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/19/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__elevator__
#define __home3D__elevator__

#include <stdio.h>
#include "glMolecule.h"


class Elevator : public glMolecule
{
public:
    Elevator();
    ~Elevator();
    
    void    create_car();
    void    create_truss();
    void    create_componenets();
    
    float   m_vertical_position;        // current location.
    float   m_door_open_fraction;
    
    
};

#endif /* defined(__home3D__elevator__) */
