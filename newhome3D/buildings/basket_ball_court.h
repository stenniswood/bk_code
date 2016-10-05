//
//  basket_ball_court.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/30/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__basket_ball_court__
#define __home3D__basket_ball_court__

#include <stdio.h>
#include "dwelling_level.h"
#include "route.hpp"
#include "walking_robot.h"


class glBackboard : public glMolecule
{
public:
    glBackboard();
    ~glBackboard();
    
    virtual void       create_components( );
};


const float NBA_COURT_WIDTH  = 50*12;       //  600
const float NBA_COURT_LENGTH = 94*12;       // 1128

const float CENTER_COURT_X = 0.0;
const float CENTER_COURT_Y = 0.0;

class glBasketballCourt : public glDwellingLevel
{
public:
    glBasketballCourt ();
    ~glBasketballCourt();
    
    void            select_floor( int mIndex );
    
    virtual void    create_floor   ( );
    void            create_baskets ( );
    void            create_cam_routes       ( );

    // Players:
    void            create_team1            ( );
    void            create_team2            ( );
    void            position_lineup_tunnel  ( );
    void            position_team_initial   ( );
    
    
    virtual void    add_light_switches      ( );
    virtual void    create_components       ( );
    

    vector<glRoute*>    m_routes;       // camera routes

    vector<glSphere*>   m_balls;
    vector<glRobot*>    m_team1;
    vector<glRobot*>    m_team2;    
};



#endif /* defined(__home3D__basket_ball_court__) */
