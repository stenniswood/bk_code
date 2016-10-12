//
//  txt_cylinder.h
//  home3D
//
//  Created by Stephen Tenniswood on 6/26/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__txt_cylinder__
#define __home3D__txt_cylinder__

#include <stdio.h>
#include "extrusion.hpp"
#include "cylinder.hpp"

class glTextureCylinder : public glCylinder
{
public:
    glTextureCylinder( int mNumberSamples );
    
    virtual void    setup         ( );   //float mLength = -1, int mAxis=2
    virtual void    draw_primitive( );

    void            generate_text_coords();
    virtual void	draw_body			( );
    
    //float   m_height;             Use m_extrusion_length instead.  Baseclass.    
};



#endif /* defined(__home3D__txt_cylinder__) */
