//
//  hinge.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__hinge__
#define __home3D__hinge__

#include <stdio.h>
#include "glMolecule.h"


class glHinge : public glMolecule
{
public:
    glHinge();
    ~glHinge();
    
    virtual void    create_components();
    
    
    float   m_height;    
};
#endif /* defined(__home3D__hinge__) */
