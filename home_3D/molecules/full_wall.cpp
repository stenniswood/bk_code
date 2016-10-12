//
//  full_wall.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "full_wall.h"
#include "all_objects.h"


glFullWall::glFullWall (  )
{
    m_object_type_name = "full wall";
}

/* Copy Constructor */
glFullWall::glFullWall( glFullWall& mSrc )
{
    m_bare_wall = mSrc.m_bare_wall;
    long size   = mSrc.m_doors.size();
    glDoorWay* dptr = NULL;
    for (int d=0; d<size; d++)
    {
        dptr  = new glDoorWay();
        *dptr = *(mSrc.m_doors[d]);
        m_doors.push_back( dptr );
    }
    
    // How do you copy a virtual object of unknown type?
    // UNRESOLVED.
    size = mSrc.m_fixtures.size();
    VerbalObject* fptr = NULL;
    for (int f=0; f<size; f++)
    {
        fptr = new VerbalObject();  // incorrect, need the real derived class.
        *fptr = *(mSrc.m_fixtures[f]);
        m_fixtures.push_back( fptr );
    }
}

glFullWall::~glFullWall(  )
{
    
}

void glFullWall::Initialize	( )
{
    
}


/* Extracts and puts into the abWall object */
void glFullWall::extract_2d_line_info()
{
    //m_bare_wall.extract_2d_line_info();     can't do this b/c m_y_angle is not in bare wall!
    // the full wall is placed, not the barewall!
    
    // Put into bare wall because the computations are done from there.
    m_bare_wall.m_line.m_origin.dimension(3);
    m_bare_wall.m_line.m_origin[0] = m_x;
    m_bare_wall.m_line.m_origin[1] = m_y;
    m_bare_wall.m_line.m_origin[2] = m_z;
    
    // Convert the wall angle to a unit vector.  This might better be done when creating the dwelling in the first place.
    float rad = -glm::radians(m_y_angle);
    m_bare_wall.m_line.m_vector.dimension(3);
    m_bare_wall.m_line.m_vector[0] = cos(rad);
    m_bare_wall.m_line.m_vector[1] = 0.0;        // y is height!
    m_bare_wall.m_line.m_vector[2] = sin(rad);
}

void glFullWall::create_components()
{
    // All DOORS HAVE TO BE ALREADY ADDED.
    m_bare_wall.setup();    // do here because all doors have already been added!
    //m_bare_wall.relocate(m_x, m_y, m_z);
    m_components.push_back( &m_bare_wall );
    
    for (int i=0; i<m_doors.size(); i++)
        m_components.push_back( m_doors[i] );
    
    for (int i=0; i<m_fixtures.size(); i++)
        m_components.push_back( m_fixtures[i] );
}

void glFullWall::set_color ( Color mColor    )
{
    glObject::set_color  ( mColor );
    m_bare_wall.set_color( mColor );
}
void glFullWall::set_color ( long mColor       )
{
    glObject::set_color  ( mColor );
    m_bare_wall.set_color( mColor );
}
void glFullWall::set_texture( int mSelection )
{
    m_bare_wall.set_texture(mSelection);
}
void glFullWall::setup( )
{
	update_body_matrix();
    create_components();
}

void glFullWall::clear( )
{
    m_bare_wall.clear();
    for(int i=0; i<m_doors.size(); i++)
        delete m_doors[i];
    m_doors.clear();
    
    for(int i=0; i<m_fixtures.size(); i++)
        delete m_fixtures[i];
    m_fixtures.clear();
}

void glFullWall::add_door( float mPositionLengthwise, glDoorWay* mDoor )
{
    m_bare_wall.extract_2d_line_info();
    m_bare_wall.add_door( mPositionLengthwise, mDoor->m_width, mDoor->m_height );
    m_doors.push_back( mDoor );

    if (mDoor->m_door_type==DOOR_TYPE_NORMAL_HINGED)
    {
        // Get Coordinate of the hinge side (we're going to place the door at that position!)
        int index  = (int)(m_bare_wall.m_doorways.size()&0xFFFF);
        MathVector mv;
        if (((glDoor*)mDoor)->m_hinge_side_near)
            mv = m_bare_wall.get_door_near_coord_plus( index-1, 0 );
        else
            mv = m_bare_wall.get_door_far_coord_plus( index-1, -m_bare_wall.m_wall_thickness/2. );

        mDoor->relocate( mv[0], 0.0, mv[2] );
    }
}

void glFullWall::add_window( float mPositionLengthwise, float mSillHeight, glWindow* mWindow )
{
    m_bare_wall.add_window( mPositionLengthwise, mWindow->m_size, mSillHeight );
}

void glFullWall::add_fixture ( VerbalObject*   mFixture, float mDistanceAlong, float mHeight )
{
    extract_2d_line_info();
    MathVector v(3);
    
    // Make Adjustment for which side of the wall it's on.
/*    if (mFixture->m_wall_side==0)           // different for horiz & vert walls!
        mFixture->m_y_angle = 180;

    float perp = (mFixture->m_wall_side) * m_bare_wall.m_wall_thickness;
    mFixture->relocate( mFixture->m_distance_along_wall, mFixture->m_y,  perp );
    mFixture->setup();
  */
    m_fixtures.push_back  ( mFixture );
    m_components.push_back( mFixture );
}



