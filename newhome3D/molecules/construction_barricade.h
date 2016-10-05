/*
 NAME    :  construction_barricade.h
 PROJECT :  home3D
 
 Created by Stephen Tenniswood on 6/8/15.
 Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
 
 This class will generate OpenGL Vertex data
 for a restaurant style table. ie. 1 center pole, round top, feet beneath.
 */
#ifndef __home3D__construction_barricade__
#define __home3D__construction_barricade__

#include <vector>
#include <stdio.h>
#include "txt_container.hpp"
#include "cylinder.hpp"
#include "glMolecule.h"

using namespace std;

class glBarricade : public glMolecule
{
public:
    glBarricade ( );
    ~glBarricade( );

    void            Initialize       ( );
    virtual void	create_components( );
    void            draw             ( );

    glCylinder     m_light1;
    glCylinder     m_light2;
    glBox          m_battery1;        // small box below the light
    glBox          m_battery2;
    
    float	m_width;            // Length along 1 side.
    float	m_board_height;		// Distance across the table.
    float	m_board_space;		// how high off ground.
    
};

#endif /* defined(__home3D__construction_barricade__) */

