//
//  rafter.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 8/18/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "rafter.h"
#include "gl_container.hpp"



glRafter::glRafter()
{
    m_peak_height = 5*12;
    m_wall_to_wall_width = 50*12;
    m_color = 0xFF8f8f00;
}

glRafter::~glRafter()
{
    
}

extern float degrees(float);

void  glRafter::create_A_frame( )
{
    m_angle  = degrees( atan2( m_wall_to_wall_width/2.,  m_peak_height ) );
    float a_length = sqrt(m_peak_height*m_peak_height + m_wall_to_wall_width*m_wall_to_wall_width/4);
    
    glBox* board  = new glBox();
    board->set_color(m_color);
    board->height = a_length + m_overhang_width;
    board->width  = RAFTER_BOARD_THICK;
    board->depth  = RAFTER_BOARD_WIDTH;
    board->setup();
    board->grab_bottom();
    board->m_x_angle = m_angle;
    board->relocate( 0, 0, -m_overhang_width  );
    m_components.push_back(board);
    
    board  = new glBox();
    board->set_color(m_color);
    board->height = a_length + m_overhang_width;
    board->width  = RAFTER_BOARD_THICK;
    board->depth  = RAFTER_BOARD_WIDTH;
    board->setup();
    board->grab_bottom();
    board->m_x_angle = -m_angle;
    board->relocate( 0, 0, m_wall_to_wall_width+m_overhang_width );
    m_components.push_back(board);
}

void  glRafter::create_supports( )
{
    float a_angle  = atan2( m_peak_height, m_wall_to_wall_width/2.  );
    //float a_length = sqrt(m_peak_height*m_peak_height + m_wall_to_wall_width*m_wall_to_wall_width/4);
    // Implement a "Home Girder" pattern :
    
    // Vertical support (in center of A)
    glBox* board  = new glBox();
    board->set_color(m_color);
    board->height = m_peak_height;
    board->width  = RAFTER_BOARD_THICK;
    board->depth  = RAFTER_BOARD_WIDTH;
    board->setup();
    board->grab_bottom();
    board->relocate( 0, 0, +m_wall_to_wall_width/2 );
    m_components.push_back(board);
    
    // Smaller vertical:
    float small_height = tan( a_angle )* m_wall_to_wall_width/4;
    board  = new glBox();
    board->set_color(m_color);
    board->height = small_height ;
    board->width  = RAFTER_BOARD_THICK;
    board->depth  = RAFTER_BOARD_WIDTH;
    board->setup();
    board->grab_bottom();
    board->relocate( 0, 0, +m_wall_to_wall_width/4 );
    m_components.push_back(board);
    
    board  = new glBox();
    board->set_color(m_color);
    board->height = small_height ;
    board->width  = RAFTER_BOARD_WIDTH;
    board->depth  = RAFTER_BOARD_THICK;
    board->setup();
    board->grab_bottom();
    board->relocate( 0, 0, +3*m_wall_to_wall_width/4 );
    m_components.push_back(board);

}
void  glRafter::create_diagonals( )
{
    //float a_angle  = degrees( asin( (m_wall_to_wall_width/2) * m_peak_height) );
    float a_length = sqrt(m_peak_height*m_peak_height + m_wall_to_wall_width*m_wall_to_wall_width/4);
    
    // Bottom of Center support to Top of 1/4 supports
    glBox* board  = new glBox();
    board->height = a_length + m_overhang_width;
    board->width  = RAFTER_BOARD_WIDTH;
    board->depth  = RAFTER_BOARD_THICK;
    board->setup();
    board->relocate( 0, 0, m_wall_to_wall_width/2);
    m_components.push_back(board);
    
    board  = new glBox();
    board->height = a_length + m_overhang_width;
    board->width  = RAFTER_BOARD_WIDTH;
    board->depth  = RAFTER_BOARD_THICK;
    board->setup();
    board->relocate( 0, 0, m_wall_to_wall_width/2 );
    m_components.push_back(board);
}

void  glRafter::create_components( )
{
    create_A_frame();
    create_supports();
    //create_diagonals();

    // Bottom board :
    glBox* board  = new glBox();
    board->set_color(m_color);
    board->height = m_wall_to_wall_width + m_overhang_width;
    board->width  = RAFTER_BOARD_WIDTH;
    board->depth  = RAFTER_BOARD_THICK;
    board->setup();
    board->grab_bottom();
    board->m_x_angle = 90;
    board->relocate( 0, 0, -m_overhang_width );
    m_components.push_back(board);
    
}







