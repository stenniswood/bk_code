//
//  sports_bench.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/3/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__sports_bench__
#define __home3D__sports_bench__

#include <stdio.h>
#include "glMolecule.h"


class glBench : public glMolecule
{
public:
    glBench ();
    ~glBench();
    
    virtual void    create_components       ( );
    
    float       m_length;
    float       m_height;
//    Texture*    m_texture;        // is applied always to the first m_component[]
    
};




#endif /* defined(__home3D__sports_bench__) */
