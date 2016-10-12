//
//  roof.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/19/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__roof__
#define __home3D__roof__

#include <stdio.h>
#include "glMolecule.h"
#include "rafter.h"


/* Roof consists of the rafters, wood covering, shingles texture.
 */

class glRoof : public glMolecule
{
public:
    glRoof();
    glRoof(MathVector start, MathVector end);   // close and far corner (square house)
    ~glRoof();
    
    void        select_shingles(int mShingleIndex);
    
    void        create_rafters();
    void        create_roof();
    void        create_components();
    
    vector<glRafter*>  m_rafters;      // keep handle to.
    
    float   m_house_length;
    float   m_house_width;
    float   m_rafter_spacing;
};


#endif /* defined(__home3D__roof__) */
