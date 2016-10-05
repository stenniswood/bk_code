//
//  sign.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/25/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__sign__
#define __home3D__sign__


#include <stdio.h>
#include <string>
#include "gl_atom.hpp"
#include "Texture.hpp"
#include "gl_container.hpp"
#include "glMolecule.h"
#include "face_box.h"


extern char sign_regexp[];

/*
 
 */
class glSign : public glAtom
{
public:
    glSign(  );
    
    virtual void    generate_vertices       ( );
    virtual void 	setup                   ( );
    void            generate_vertices_colors( );
    virtual void	draw_primitive( );          // Override this with open gl commands.

    float 	 m_height;
    float 	 m_width;
};

class StreetSign : public glMolecule
{
public:
    StreetSign();
    ~StreetSign();
    
    void        create_components();
    void        select_measurements( string mText );
    void        select_image( string mText );
    
    string      m_sign_name;
    glFaceBox*     m_sign;
    glBox*      m_pole;
    float       m_height_off_ground;
    
};

#endif /* defined(__home3D__sign__) */
