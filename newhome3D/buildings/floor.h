//
//  floor.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/30/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__floor__
#define __home3D__floor__

#include <stdio.h>
#include <string>
#include <vector>
#include "glMolecule.h"
#include "txt_container.hpp"


/* 
 This class added to a dwelling leve.  Will automatically find the extents of the dwelling and cover it all.
 It will also divide it up into rooms as best as it can.  Each room can be given a label afterward.
 */
class glFloor : public glMolecule
{
public:
    glFloor();
    ~glFloor();
    
    void        cover_region     (float sx, float sz, float ex, float ez);
    void        cover_all        (struct room mDimensions);
    void        divide_into_rooms();
    
    vector<txtContainer*>    m_cover;
    vector<struct room>     m_rooms;
};

#endif /* defined(__home3D__floor__) */
