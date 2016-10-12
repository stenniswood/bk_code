//
//  road.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/14/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__road__
#define __home3D__road__

#include <stdio.h>
#include "gl_atom.hpp"
#include "route.hpp"


class glRoad : public glAtom
{
public:
    glRoad();
    ~glRoad();

    void    generate_vertices();
    void    generate_indices ( );
    void    select_texture();
    
    virtual void draw_primitive( );
    
    
    glRoute m_path;
    float   m_height;
    float   m_road_width;
};


#endif /* defined(__home3D__road__) */
