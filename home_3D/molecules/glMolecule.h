//
//  glMolecule.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/8/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__glMolecule__
#define __home3D__glMolecule__

#include <stdio.h>
#include "gl_atom.hpp"
#include "gl_object.hpp"
#include "verbal_object.h"


class glMolecule : public VerbalObject
{
public:
    glMolecule ( );
    glMolecule ( string mName );
    ~glMolecule( );

    void            Initialize       ( );
    
    void        set_acceleration();         // transfers into all atoms/sub-molecules.
    void        set_veloctiy();             // transfers into all atoms/sub-molecules.
    void        set_angular_acceleration(); // transfers into all atoms/sub-molecules.
    void        set_angular_veloctiy();     // transfers into all atoms/sub-molecules.
    
    void        set_relative_x  ( glObject* mObj, float mX );
    void        set_relative_y  ( glObject* mObj, float mY );
    void        set_relative_z  ( glObject* mObj, float mZ );
    void        set_relative_xyz( glObject* mObj, float mX, float mY, float mZ);

    virtual void    compute_max();
    virtual void    compute_min();
    
    virtual void	setup            ( );
    virtual void	create_components( );
    virtual void	create           ( );
    virtual void    draw_body        ( );

    void            gl_register      ( );
    void            gl_unregister    ( );
    virtual void    release_memory(   );
    
    /* A molecule could consist of atoms or other molecules.  
        In either case the base components needs a glRegister.
     */
    vector<VerbalObject*> m_components;


    virtual bool    is_a_molecule()     { return true; }
    
    // ****** VERBAL PORTION ************
    // WE need a way - if the component is a molecule, it should then check all of its compoents for a named object.
    // Q:  How do we know if the component is a molecule derived object??
    // Ans:     in VerbalObject class put a virtual function which answers that.
    
    bool           find_component ( string&  mRequestedName, string& mTypeName,
                                   vector<VerbalObject*>* object_hier,
                                   bool mTypeMustMatch=true );    
    VerbalObject*  get_component  ( int index );
    
    bool           find_component_by_id( long mObject_id,
                                        vector<VerbalObject*>* object_hier
                                         );
    // ie "door", "window", etc.  the requested name may be an adjective to this.
    // ie. "master bedroom door"
};


#endif /* defined(__home3D__glMolecule__) */




