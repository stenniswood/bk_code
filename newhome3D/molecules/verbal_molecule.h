//
//  verbal_molecule.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/22/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__verbal_molecule__
#define __home3D__verbal_molecule__

#include <stdio.h>
#include "glMolecule.h"
#include "verbal_object.h"



/* ALL COMPONENTS MUST BE VerbalObjects based!! */

class VerbalMolecule : public glMolecule
{
public:
    VerbalMolecule();
    ~VerbalMolecule();
    
    VerbalObject*  get_component( int index );
    VerbalObject*  find_object	( string  mRequestedName, bool mTypeMustMatch );
    
    
};
#endif /* defined(__home3D__verbal_molecule__) */
