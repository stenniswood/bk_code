//
//  floor.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/30/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "floor.h"
//#include "all_objects.h"
#include "dwelling_level.h"
#include "txt_container.hpp"

glFloor::glFloor()
{
    m_color = 0xFF5f3f3f;
}

glFloor::~glFloor()
{
    
}

void glFloor::cover_region(float sx, float sz, float ex, float ez)
{
    txtContainer* tmp = new txtContainer();
    tmp->set_color(m_color);
    tmp->width  = ex - sx;
    tmp->height = 1.0;
    tmp->depth  = ez - sz;
    tmp->setup();
    tmp->grab_back();
    tmp->grab_right();
    tmp->relocate( sx, 0, sz );
    m_cover.push_back(tmp);
    m_components.push_back(tmp);
}

void glFloor::cover_all(struct room mRectangle)
{
    m_x = mRectangle.sx;
    m_z = mRectangle.sz;
    
    txtContainer* tmp = new txtContainer();
    tmp->set_color(m_color);
    tmp->width  = mRectangle.ex - mRectangle.sx;
    tmp->height = 1.0;
    tmp->depth  = mRectangle.ez - mRectangle.sz;
    tmp->setup();
    tmp->grab_back();
    tmp->grab_right();
    m_cover.push_back(tmp);
    m_components.push_back(tmp);
}

void glFloor::divide_into_rooms()
{
    
}


