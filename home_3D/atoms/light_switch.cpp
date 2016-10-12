

#include "all_objects.h"
#include "light_switch.hpp"


glFixture::glFixture()
{
    m_object_type_name = "fixture";
}
glFixture::~glFixture()
{
    
}


//==================================================================
//==================================================================
glLightSwitch::glLightSwitch ( )
{
    m_object_type_name = "light switch";
}

glLightSwitch::~glLightSwitch( )
{
    
}

void glLightSwitch::Initialize ( )
{
    m_object_class  = 21;
}

void glLightSwitch::create_components( )
{
    m_plate.set_color( 0xFFBFBF7F);
    m_plate.height = 3;
    m_plate.width  = 2.5;
    m_plate.depth  = 0.125;
    m_plate.setup();
    m_components.push_back(&m_plate);

    m_lever.set_color( 0xFFFFFF00);
    m_lever.height = 0.25;
    m_lever.width  = 0.25;
    m_lever.depth  = 0.75;
    m_lever.grab_back();
    m_lever.setup();
    m_lever.grab_back();
    m_components.push_back(&m_lever);
    m_lever.relocate( 0.0, 0.0, 0 );
    turn_up();
}

void glLightSwitch::toggle( )
{
    if (m_position_is_up)
        turn_down();
    else
        turn_up();
}
void glLightSwitch::turn_up( )
{
    m_position_is_up = true;
    m_lever.m_x_angle = -30;
}
void glLightSwitch::turn_down( )
{
    m_position_is_up = false;
    m_lever.m_x_angle = -30;
}

void glLightSwitch::turn_on( )
{
    if (m_on_is_up)
        turn_up();
    else
        turn_down();
}

void glLightSwitch::turn_off( )
{
    if (m_on_is_up)
        turn_down();
    else
        turn_up();
}


/**********************************************************************
 
 **********************************************************************/
glDualLightSwitch::glDualLightSwitch()
{
    
}

glDualLightSwitch::~glDualLightSwitch()
{
    
}

