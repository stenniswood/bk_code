//
//  door_handle.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "door_handle.h"

//const float center_offset_from_door_edge = 2.5;
const float knob_distance_from_door      = 3.0;


glDoorHandle::glDoorHandle( ) :
m_knob1( 2.5, 24, 24 ),
m_knob2( 2.5, 24, 24 ),
m_connector( 24 )
{
    m_object_type_name = "door handle";    
    m_door_thickness = 2.5;      // door thickness
}

glDoorHandle::~glDoorHandle   ( )
{
    
}

void glDoorHandle::create_components()
{
    m_knob1.set_color       (0xAF9FFF00 );
    m_knob1.setup           (           );
    m_components.push_back  ( &m_knob1  );
    m_knob1.relocate        ( 0.0, 0.0, knob_distance_from_door );  //
    
    m_knob2.set_color       ( 0xFF9FFF00 );
    m_knob2.setup           (            );
    m_components.push_back  ( &m_knob2   );
    m_knob2.relocate        ( 0.0, 0.0, -knob_distance_from_door-m_door_thickness );
    
    m_connector.m_radius = m_knob1.m_radius * 0.60;
    m_connector.set_height  ( m_door_thickness+2*knob_distance_from_door );
    m_connector.setup       (               );
    
    m_connector.m_x_angle = 90;
    m_components.push_back  ( &m_connector  );
    m_connector.relocate    ( 0.0, 0.0, -knob_distance_from_door-m_door_thickness  );
}

//**************************************************************************//

//**************************************************************************//

glLeverHandle::glLeverHandle    ( )
: m_connector(24)
{
    m_door_thickness = 2.5;      // door thickness
}
glLeverHandle::~glLeverHandle   ( )
{
    
}

void glLeverHandle::create_components( )
{
    m_front.width  = 8.0;
    m_front.height = 0.75;
    m_front.depth  = 0.5;
    m_front.setup();
    m_components.push_back( &m_front );
    m_front.relocate    ( 0.0, 0.0, -knob_distance_from_door-m_door_thickness  );
    
    m_flange.width  = 2.0;
    m_flange.height = 0.5;
    m_flange.depth  = 0.5;
    m_flange.setup();
    m_components.push_back( &m_flange );
    m_flange.relocate    ( 0.0, 0.0, -knob_distance_from_door-m_door_thickness  );
    
    m_connector.m_radius    = 1.;
    m_connector.set_height  ( m_door_thickness+2*knob_distance_from_door );
    m_connector.setup       (               );
    
    m_connector.m_x_angle = 90;
    m_components.push_back  ( &m_connector  );
    m_connector.relocate    ( 0.0, 0.0, -knob_distance_from_door-m_door_thickness  );
}





