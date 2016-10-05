//
//  truss.h
//  home3D
//
//  Created by Stephen Tenniswood on 8/18/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__truss__
#define __home3D__truss__

#include <stdio.h>
#include <vector>
#include "glMolecule.h"
#include "face_box.h"


/* 4 poles with interweaved supports. */
class glTruss : public glMolecule
{
public:
    glTruss ();
    ~glTruss();

    void        create_columns  ( );
    void        create_diagonals( );
    void        create_perpendiculars( );
    void        create_components    ( );

    float       m_length;
    float       m_separation_distance;     // ie perpendicular
    float       m_perpendicular_spacing;          //
  
    float       m_column_radius;
};


/* This is just 2 poles with diagonal connects.  */
class glTruss2D : public glMolecule
{
public:
    glTruss2D();
    ~glTruss2D();
    
    void        create_columns       ( );
    void        create_diagonals     ( );
    void        create_perpendiculars( );
    void        create_components    ( );
    
    float       m_length;
    float       m_separation_distance;     // ie perpendicular
    float       m_perpendicular_spacing;          //
    
    float       m_column_radius;
};

/* 3 Trusses - 2 sides holding an overhead */
class glHighwayOverhead : public glMolecule
{
public:
    glHighwayOverhead();
    glHighwayOverhead( int Number_of_lanes );
    ~glHighwayOverhead();

    void          create_components();
    void          add_sign( glFaceBox* mSign, int mLane=0, int mSide=0 );

    vector<glFaceBox*>  m_signs;
    float         m_road_width;
    float         m_height;
    glTruss*      m_overhead;
    glTruss2D*    m_right_side_support;
    glTruss2D*    m_left_side_support;
    
};


#endif /* defined(__home3D__truss__) */

