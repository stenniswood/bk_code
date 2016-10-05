//
//  chair.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/7/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__chair__
#define __home3D__chair__

#include <stdio.h>
#include "txt_container.hpp"
#include "glMolecule.h"
#include <vector>
using namespace std;


class glChair : public glMolecule
{
public:
    glChair(  );
    
    void            Initialize	( );
    virtual void 	setup  		( );
    virtual void 	create 		( );
    virtual void	create_components();

    float	m_seat_length;		// Length along 1 side.
    float	m_seat_width;		// Distance across the table.
    float	m_seat_height;		// how high off ground.    
};


#endif /* defined(__home3D__chair__) */
