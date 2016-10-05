//
//  picnic_table.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/21/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "all_objects.h"



glPicnicTable::glPicnicTable(  )
{
    Initialize();
}

void glPicnicTable::Initialize	( )
{
    m_object_type_name = "picnic table";    
    m_length        = 4*12;     // Length along 1 side.
    m_seat_width    = 7;		// Distance across the table.
    m_seat_height   = 2.5;		// how high off ground.
    m_table_height  = 2*m_seat_height;
}

void glPicnicTable::setup  		( )
{
    create_components();
}

void glPicnicTable::create 		( )
{
    setup();
    gl_register();
}

void glPicnicTable::create_components()
{
    const long seat_color = 0xFF9F6F1F;
    // Create Seat 1 :
    glBox* box  = new glBox();
    box->height = 1.0;
    box->width  = m_seat_width;
    box->depth  = m_length;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(seat_color);
    box->m_y = m_seat_height;
    box->m_x = 0.;
    box->m_z = 0.;
    m_components.push_back(box);
    
    // Create Seat 2:
    box  = new glBox();
    box->height = 1.0;
    box->width  = m_seat_width;
    box->depth  = m_length;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(seat_color);
    box->m_y = m_seat_height;
    box->m_x = 6*m_seat_width;
    box->m_z = 0;
    m_components.push_back(box);

    const long top_color = 0x3F9F6F1F;
    
    // Table Top (4 boards)
    // Create board 1 :
    box  = new glBox();
    box->height = 1.0;
    box->width  = m_seat_width;
    box->depth  = m_length;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(top_color);
    box->m_y = m_table_height;
    box->m_x = m_seat_width*1.25;
    box->m_z = 0.;
    m_components.push_back(box);
    // Create board 2:
    box  = new glBox();
    box->height = 1.0;
    box->width  = m_seat_width;
    box->depth  = m_length;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(top_color);
    box->m_y = m_table_height;
    box->m_z = 0.;
    box->m_x = m_seat_width*1.25+m_seat_width;
    m_components.push_back(box);
    // Create board 3:
    box  = new glBox();
    box->height = 1.0;
    box->width  = m_seat_width;
    box->depth  = m_length;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(top_color);
    box->m_y = m_table_height;
    box->m_x = m_seat_width*1.25+2*m_seat_width;
    box->m_z = 0.;
    m_components.push_back(box);
    // Create board 4:
    box  = new glBox();
    box->height = 1.0;
    box->width  = m_seat_width;
    box->depth  = m_length;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(top_color);
    box->m_y = m_table_height;
    box->m_x = m_seat_width*1.25+3*m_seat_width;
    box->m_z = 0.;
    m_components.push_back(box);
    
    // Create support board 1 :
    const long support_color = 0xFF4F5F4F;
    const int support_height = 4.0;
    box  = new glBox();
    box->height = support_height;
    box->width  = m_seat_width*3+4*m_seat_width;
    box->depth  = 2;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(support_color);
    box->m_y = m_seat_height-support_height;
    box->m_x = 0.0;
    box->m_z = 6.0;
    m_components.push_back(box);
    // Create support board 2:
    box  = new glBox();
    box->height = support_height;
    box->width  = m_seat_width*3+4*m_seat_width;
    box->depth  = 2;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(support_color);
    box->m_y = m_seat_height-support_height;
    box->m_x = 0.0;
    box->m_z = m_length-6;
    m_components.push_back(box);

    // FINAL MAKE THE LEGS (all 4) :
    // Create leg board 1 :
    float center = m_seat_width*1.25+2*m_seat_width;
    box  = new glBox();
    box->height = support_height;
    box->width  = m_seat_width*5;
    box->depth  = 2;
    box->setup();
    box->grab_top();
    box->grab_right();
    box->grab_back();
    box->change_color(support_color);
    box->m_y = m_table_height;
    box->m_x = center;
    box->m_z = 6.0;
    box->m_R_position[2]=45-90;
    m_components.push_back(box);
    // Create leg board 2 :
    box  = new glBox();
    box->height = support_height;
    box->width  = m_seat_width*5;
    box->depth  = 2;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(support_color);
    box->m_y = m_table_height;
    box->m_x = center;
    box->m_z = 6.0;
    box->m_R_position[2]=-45-90;
    m_components.push_back(box);

    // Create leg board 3 :
    box  = new glBox();
    box->height = support_height;
    box->width  = m_seat_width*5;
    box->depth  = 2;
    box->setup();
    box->grab_top();
    box->grab_right();
    box->grab_back();
    box->change_color(support_color);
    box->m_y = m_table_height;
    box->m_x = center;
    box->m_z = m_length-6.0;
    box->m_R_position[2]=45-90;
    m_components.push_back(box);
    // Create leg board 4 :
    box  = new glBox();
    box->height = support_height;
    box->width  = m_seat_width*5;
    box->depth  = 2;
    box->setup();
    box->grab_bottom();
    box->grab_right();
    box->grab_back();
    box->change_color(support_color);
    box->m_y = m_table_height;
    box->m_x = center;
    box->m_z = m_length-6.0;
    box->m_R_position[2]=-45-90;
    m_components.push_back(box);
    
}



