//
//  basketball_arena.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/3/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__basketball_arena__
#define __home3D__basketball_arena__

#include <stdio.h>
#include <vector>
#include "glMolecule.h"
#include "glSphere.h"
#include "walking_robot.h"
#include "basket_ball_court.h"


/* Origin is :
 */

class glArena : public glMolecule
{
public:
    glArena ();
    ~glArena();
    
    virtual void	setup            ( );
    
    void            create_vertical_walls   ( );
    void            create_stands           ( );
    void            create_stands_ends      ( );
    
    
    void            create_cam_routes       ( );
    virtual void    add_light_switches      ( );
    virtual void    create_components       ( );
    
    glBasketballCourt*  bball_court;    
    CuboidTexture*      m_bench_texture;        // load once here and applied to all benches to save memory.
    
};



#endif /* defined(__home3D__basketball_arena__) */
