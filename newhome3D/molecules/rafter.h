//
//  rafter.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/18/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__rafter__
#define __home3D__rafter__

#include <stdio.h>
#include "glMolecule.h"


#define RAFTER_BOARD_WIDTH 6
#define RAFTER_BOARD_THICK 1.5


class glRafter : public glMolecule
{
public:
    glRafter();
    ~glRafter();

    void            create_A_frame( );
    void            create_supports( );
    void            create_diagonals( );
    
    void            create_components( );
    

    
    float                    m_peak_height;
    float                    m_wall_to_wall_width;
    float                    m_overhang_width;

    float                   m_angle;
};



#endif /* defined(__home3D__rafter__) */
