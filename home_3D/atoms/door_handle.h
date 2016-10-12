//
//  door_handle.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__door_handle__
#define __home3D__door_handle__

#include <stdio.h>
#include "glMolecule.h"
#include "glSphere.h"
#include "cylinder.hpp"
#include "gl_container.hpp"


class glDoorHandle : public glMolecule
{
public:
    glDoorHandle    ( );
    ~glDoorHandle   ( );
    
    void            load_texture        ( string mImageFilename );
    void            create_components   ( );

    float           m_door_thickness;
    glSphere        m_knob1;
    glSphere        m_knob2;
    glCylinder      m_connector;
};


class glLeverHandle : public glMolecule
{
public:
    glLeverHandle    ( );
    ~glLeverHandle   ( );
    
    void            create_components( );

    float           m_door_thickness;
    glBox           m_front;
    glBox           m_flange;
    glCylinder      m_connector;
};

#endif /* defined(__home3D__door_handle__) */
