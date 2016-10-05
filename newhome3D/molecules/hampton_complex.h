//
//  hampton_complex.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/15/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__hampton_complex__
#define __home3D__hampton_complex__

#include <stdio.h>
#include "dwelling.h"
#include "apartment.h"


class glOneBuilding : public glDwelling
{
public:
    glOneBuilding ( );
    ~glOneBuilding( );
    
    void            create_one_level    ();
    virtual void    create_components   ();

    
};


#endif /* defined(__home3D__hampton_complex__) */
