//
//  bookcase.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/12/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__bookcase__
#define __home3D__bookcase__

#include <stdio.h>
#include "txt_container.hpp"
#include "glMolecule.h"
#include <vector>
using namespace std;


class glBookcase : public glMolecule
{
public:
    glBookcase(  );

    void            Initialize	( );
    virtual void 	setup  		( );
    virtual void 	create 		( );
    virtual void	create_components();
    
    float	m_depth;		// Length along 1 side.
    float	m_width;		// Distance across the table.
    float	m_height;		// how high off ground.
    float   m_shelves;
};




#endif /* defined(__home3D__bookcase__) */
