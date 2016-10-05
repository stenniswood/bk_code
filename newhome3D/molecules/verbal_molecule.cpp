//
//  verbal_molecule.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/22/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "verbal_molecule.h"


VerbalMolecule::VerbalMolecule()
{
    
}
VerbalMolecule::~VerbalMolecule()
{
    
}

VerbalObject*  VerbalMolecule::get_component( int index )
{
    return ((VerbalObject*)m_components[index]);
}

VerbalObject*  VerbalMolecule::find_object( string  mRequestedName, bool mTypeMustMatch )
{
    VerbalObject* retval=NULL;
    bool match  = false;
    for (int i=0; i<m_components.size(); i++)
    {
        retval = get_component(i);
        match  = (retval->m_name.compare( mRequestedName)==0);
        if (match)
        {
            if (mTypeMustMatch)
            {
                if (mRequestedName.find( m_object_type_name ) )
                    return retval;
            }
            else
                return retval;
        }
    }
    return NULL;
}


