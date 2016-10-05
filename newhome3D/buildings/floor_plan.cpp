//
//  floor_plan.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 10/8/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "floor_plan.h"



glFloorPlan::glFloorPlan()
{
    
}

glFloorPlan::~glFloorPlan()
{
    
}

void glFloorPlan::add_room( Rectangle* mLevel, std::string mName )
{
    m_rooms.push_back(*mLevel);
    m_names.push_back( mName );
}
