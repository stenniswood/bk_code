//
//  floor_plan.h
//  home3D
//
//  Created by Stephen Tenniswood on 10/8/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__floor_plan__
#define __home3D__floor_plan__

#include <stdio.h>
#include <vector>
#include <string>
#include "rectangle.h"


class glFloorPlan
{
public:
    glFloorPlan();
    ~glFloorPlan();
    
    void    add_room( Rectangle* mLevel, std::string mName );
    
    // Find door, window :
    float                    m_level_height;
    std::vector<Rectangle>   m_rooms;
    std::vector<std::string> m_names;

};

#endif /* defined(__home3D__floor_plan__) */
