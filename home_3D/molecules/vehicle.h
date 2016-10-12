//
//  vehicle.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/14/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__vehicle__
#define __home3D__vehicle__

#include <stdio.h>
#include "glMolecule.h"
#include "txt_cylinder.h"
#include "face_box.h"


class glVehicleBody : public glMolecule
{
public:
    glVehicleBody();
    ~glVehicleBody();
    
    virtual void	create_components();    
    
};


class glVehicle : public glMolecule
{
public:
    glVehicle (  );
    ~glVehicle(  );
    
    void            Initialize	( );
    virtual void	create_components();
    
    void            update_wheel_speed();   // works little bit in reverse of actual car.  The wheel match the m_velocity vector.
    // so the user just has to set the speed of the vehicle and the wheels will match it.  Same as the drivers perspective.
    
    

    float   m_wheel_radius;
    vector<glTextureCylinder>   m_wheels;
    glVehicleBody               m_body;
    
    
    float	m_depth;		// Length along 1 side.
    float	m_width;		// Distance across the table.
    float	m_height;		// how high off ground.
};


#endif /* defined(__home3D__vehicle__) */
