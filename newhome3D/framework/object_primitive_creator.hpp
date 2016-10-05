//
//  object_primitive_creator.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/12/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef object_primitive_creator_hpp
#define object_primitive_creator_hpp

#include <stdio.h>
#include "verbal_object.h"

void          set_default_parameters( long mObject_type, struct stGenericObjectParameter* mParameters );
VerbalObject* new_object            ( long mObject_type, struct stGenericObjectParameter* mParameters );
void          print_parameters      ( long mObject_type, struct stGenericObjectParameter* mParameters );


const int MAX_FLOATS=20;
const int MAX_INTS=20;
const int MAX_TEXT_LENGTH=4096;

struct stGenericObjectParameter
{
    float     floats    [MAX_FLOATS];
    long int  integers  [MAX_INTS];
    char      text_array[MAX_TEXT_LENGTH];
};

extern struct stGenericObjectParameter NewObjectParameters;


#endif /* object_primitive_creator_hpp */
