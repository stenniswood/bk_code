//
//  gl_window.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/18/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "gl_window.h"


/******************************************************/

/******************************************************/
glWindow::glWindow(  )
{
    m_object_type_name = "window";
}
glWindow::~glWindow( )
{
    
}
void  glWindow::set_bay_params(float mWidth,
                               float mHeight    ,
                               float mSillHeight,
                               float mThick        )
{
    
}

void  glWindow::set_params  (float mWidth,
                             float mHeight ,
                             float mSillHeight,
                             float mThick,          // total window thickness (ie. 2 frames thick)
                             bool mSideBySide )
{
    m_full_width  = mWidth;
    m_full_height = mHeight;
    m_sill_height = mSillHeight;
    m_frame_thickness = mThick;
    
    if (m_piece1==NULL)
        m_piece1 = new glFrame();
    if (m_piece2==NULL)
        m_piece2 = new glFrame();
    
    // Have to specify the inside frame width/height:
    m_piece1->set_params( mWidth/2-2*m_frame_thickness, mHeight, m_frame_thickness, mThick/2 );
    m_piece2->set_params( mWidth/2-2*m_frame_thickness, mHeight, m_frame_thickness, mThick/2 );
    m_piece1->m_y = mSillHeight;
    m_piece2->m_y = mSillHeight;
}

/* Only Piece 2 moves! */
void glWindow::open( float mfraction )
{
    m_open_fraction = mfraction;
    float TrackLength = m_full_width/2;
    float x = TrackLength * mfraction;
    m_piece2->relocate( x, m_sill_height, 0 );
 
}
void  glWindow::create_components( )
{
    if (m_piece1==NULL)
        m_piece1 = new glFrame();
    if (m_piece2==NULL)
        m_piece2 = new glFrame();

    m_piece1->setup();
    m_piece1->relocate(0, m_sill_height, 0);
    m_components.push_back(m_piece1);
    
    m_piece2->setup();
    m_piece2->relocate(m_full_width/2., m_sill_height, 0);
    m_components.push_back(m_piece2);
    
}



