//
//  dwelling2.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 4/11/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include "Map2D.hpp"

#include "dwelling2.hpp"



glDwellingLevel1::glDwellingLevel1( )
{
    m_object_type_name = "dwelling level";
    m_color = 0xFFFFFFFF;
    //m_floor = NULL;
}

glDwellingLevel1::~glDwellingLevel1( )
{
    
}

/* Gives the max z for a wall origin, not necessarily the max z vertex! */
float glDwellingLevel1::get_max_x()
{
    MathVector tmp(3);
    float max_x;
    
    return max_x;
}

/* Gives the max z for a wall origin, not necessarily the max z vertex! */
float glDwellingLevel1::get_max_z()
{
    MathVector tmp(3);
    float max_z = 0.0;
    return max_z;
}

void glDwellingLevel1::mirror_image_x          ( )
{
    //float max_z = get_max_z();
    
    // for all walls, set the origin at -m_z
    //for (int w=0; w<size; w++)
    {
    }
}

void glDwellingLevel1::mirror_image_z          ( )
{
    // for all walls, set the origin at -m_z
    //float max_x = get_max_x();
    
    //for (int w=0; w<size; w++)
    {
    }
}

void glDwellingLevel1::generate_exterior_walls()
{
}


void glDwellingLevel1::create_room ( float width, float length )
{
    //glFullWall* w = new glFullWall();
    
}

/* The wall from 2D point of view consists of:
 <m_x, m_z>			and
 m_y_angle 			(in degrees)
 m_wall_length		(in inches )
 
 Creates Vertex  from the Wall 3D data    */
void glDwellingLevel1::update_line_info( )
{
    //for (int w=0; w<m_fwalls.size(); w++)
    {
        // Extracts the wall info plus copies the Doors and Windows info :
        //m_fwalls[w]->m_bare_wall.extract_2d_line_info( );
    }
}


void glDwellingLevel1::create_floor()        // to cover all.
{
    //m_floor = new glFloor();
    /*struct room  dimen;
    dimen.sx = 0;
    dimen.sz = 0;
    dimen.ex = get_max_x();
    dimen.ez = get_max_z();
    m_floor->cover_all( dimen );
    *///m_floor->m_y = 0.1;
    //m_components.push_back(m_floor);
}


/*
 mWallAngle is the horizontal angle away from the wall.
 */
void glDwellingLevel1::find_all_possibilies( float mLinearDistance, float mWallAngleRadians, float mHeight )
{
    //float perp_distance     = mLinearDistance * sin( mWallAngleRadians );
    //float parallel_distance = mLinearDistance * cos( mWallAngleRadians );
    
    // now go around all the wall and create a line segment set so far away from the wall.
    // if another wall passes within that distance, then cut the line there.
    //long wsize = m_fwalls.size();
    LineSegment lseg;
    
    // Repeat for all walls:
    /*for (int w=0; w<wsize; w++)
    {
        vector<glWallLine>* one_wall = m_fwalls[w]->m_bare_wall.create_lines_distance_from(mLinearDistance, mWallAngleRadians, mHeight);
        if (one_wall)
            for(int i=0; i<one_wall->size(); i++)
                m_line_segs.push_back( (*one_wall)[i] );
    }
    for (int l=0; l<m_line_segs.size(); l++)
        m_line_segs[l].create();
     */
}

void glDwellingLevel1::filter_blocked_possibilies( )
{
}

void glDwellingLevel1::accept_heading_measurement( float mAngleToNorth )
{
    
}


glDwellingLevel1* glDwellingLevel1::create_copy()
{
    glDwellingLevel1* dl = new glDwellingLevel1();
    return dl;
}

glDwellingLevel1* glDwellingLevel1::create_mirror_image( )
{
    glDwellingLevel1* dl = new glDwellingLevel1();
    return dl;
}

void glDwellingLevel1::setup()
{
    generate_exterior_walls();
    glMolecule::setup();
}

void  glDwellingLevel1::close_all_doors( float mFraction )
{
}

glDoorWay* glDwellingLevel1::crosses_doorway( MathVector pt1, MathVector pt2 )
{
    return NULL;
}

bool glDwellingLevel1::evaluate_collision( glSphere* mOther )
{
    return false;
}
