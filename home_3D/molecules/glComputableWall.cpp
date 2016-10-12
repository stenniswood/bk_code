//
//  glComputableWall.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/2/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "glComputableWall.h"
#include "gl_misc.h"

const float WALL_END_OFFSET = 12.;      // for routing the robot around a corner


glComputeableWall::glComputeableWall( )
{  
}

glComputeableWall::~glComputeableWall( )
{
}

void glComputeableWall::relocate( float mX, float mY, float mZ )
{
    glAtom::relocate(mX,mY,mZ);
    extract_2d_line_info();
}

/*
 Assumption is that mIntersection is a 3D point on the base of the wall
 */
int glComputeableWall::find_closest_door( MathVector mIntersection )
{
    int   min_door_index = -1;
    float min_distance   = m_wall_length;
    int   num_doors      = (int)m_doorways.size();
    
    // Find closest Door to the Intersection.
    for (int d=0; d<num_doors; d++)
    {
        MathVector dc = get_door_center_coord_plus( d, 0 );
        float distance = mIntersection.get_distance( dc );
        if (distance < min_distance)
        {
            min_door_index = d;
            min_distance = distance;
        }
    }
    return min_door_index;
}

/* Assumption is that mIntersection is a 3D point on the base of the wall */
float glComputeableWall::get_distance_along( MathVector mIntersection )
{
    // m_origin + m_vector * mIntersection
    MathVector delta = (mIntersection - m_line.m_origin);
    float  distance = delta.magnitude();
    return distance;
}

// returns -1.0 or +1.0
float glComputeableWall::is_on_which_side( MathVector mPoint )
{
    // Find which side of the wall we are on (with respect to the unit vector):
    MathVector dv = mPoint - m_line.m_origin;
    float side_projection = dv.dot( m_line.m_vector.get_perp_xz() );
    if (side_projection>=0.0)
    {
        side_projection = 1.0;      printf("is on + side\n");
    } else {
        side_projection = -1.0;     printf("is on - side\n");
    }
    return side_projection;
}

void glComputeableWall::find_paths_around ( list<float>& mDistanceAlong )
{
    // LIST ALL DOORS :
    long size = m_doorways.size();
    for (int i=0; i<size; i++) {
        mDistanceAlong.push_back( get_door_center(i) );
    }
    
    // LIST END POINTS :
    float tmp = -WALL_END_OFFSET ;
    mDistanceAlong.push_back(tmp);
    
    tmp = (m_wall_length+WALL_END_OFFSET);
    mDistanceAlong.push_back(tmp);
}

void glComputeableWall::find_paths_around( list<MathVector>& mPoints, float mOffsetAwayFromWall )
{
    // LIST ALL DOORS :
    long size = m_doorways.size();
    for (int i=0; i<size; i++)
        mPoints.push_back( get_door_center_coord_plus( i, mOffsetAwayFromWall ) );
    
    // LIST END POINTS :
    MathVector tmp = m_line.m_origin + m_line.m_vector * -WALL_END_OFFSET + m_line.m_vector.get_perp_xz()*mOffsetAwayFromWall;
    mPoints.push_back(tmp);
    
    tmp = m_line.m_origin + m_line.m_vector * (m_wall_length + WALL_END_OFFSET);
    tmp += m_line.m_vector.get_perp_xz()*mOffsetAwayFromWall;
    mPoints.push_back(tmp);
}

int glComputeableWall::find_alternate_via( MathVector mIntersection, MathVector& mNewPoint )
{
    //
    float distance1 = mIntersection.get_distance( m_line.m_origin );
    MathVector far_end = m_line.m_origin + (m_line.m_vector * m_wall_length);
    float distance2 = mIntersection.get_distance( far_end );
    
    //
    int index = find_closest_door(mIntersection);
    MathVector dc = get_door_center_coord_plus( index,0 );
    float distance3 = mIntersection.get_distance( dc );
    float min_dist  = distance1;
    int min = 1;
    
    //
    if (distance2 < min_dist)
    {    min = 2;   min_dist = distance2; }
    if (distance3 < min_dist)
    {    min = 3;   min_dist = distance2; }
    switch (min)
    {
        case 1: mNewPoint = m_line.m_origin;
            break;
        case 2: mNewPoint = far_end;
            break;
        case 3: mNewPoint = dc;
            break;
    }
    return min;
}

bool  glComputeableWall::intersects(  LineSegment mPath, float& t, float& s )
{
    int result = m_line.line_intersection(mPath, t, s);
    if (result)
    {
        // Yes they intersect.
        //      adjust endpoint
        return true;
    }
    else
        return false;
}

/****** Door Linear Measurements ********/
// Distance along wall
float glComputeableWall::get_door_near(  int mDoorIndex  )
{
    return m_doorways[mDoorIndex].position_lengthwise;
}

float glComputeableWall::get_door_center(  int mDoorIndex  )
{
    float distance_to_door_center = m_doorways[mDoorIndex].position_lengthwise + (m_doorways[mDoorIndex].width/2.0);
    return distance_to_door_center;
}

float glComputeableWall::get_door_far	 (  int mDoorIndex  )
{
    float distance_to_far = m_doorways[mDoorIndex].position_lengthwise + m_doorways[mDoorIndex].width;
    return distance_to_far;
}
/****** Door Linear Measurements ********/
/****** Door Vertices ********/
struct Vertex glComputeableWall::get_door_near_coord( int mDoorIndex )
{
    int v_index = m_doorways[mDoorIndex].FirstVertexIndex;
    return m_vertices[v_index];
}
struct Vertex glComputeableWall::get_door_center_coord( int mDoorIndex )
{
    int v_index   = m_doorways[mDoorIndex].FirstVertexIndex;
    struct Vertex v1 =  m_vertices[v_index  ];
    struct Vertex v2 =  m_vertices[v_index+1];
    struct Vertex vc;
    vc.color[0] = v1.color[0];
    vc.color[1] = v1.color[1];
    vc.color[2] = v1.color[2];
    vc.color[3] = v1.color[3];
    vc.position[1] = v1.position[1];                                // Floor Height.
    vc.position[0] = (v1.position[0] + v2.position[0]) / 2.;        // Average close & far sides.
    vc.position[2] = (v1.position[2] + v2.position[2]) / 2.;        //
    return vc;
}
struct Vertex glComputeableWall::get_door_far_coord(  int mDoorIndex  )
{
    int v_index = m_doorways[mDoorIndex].FirstVertexIndex;
    return m_vertices[v_index+1];
}
/****** Door Vertices ********/

/****** Door  ********/
MathVector glComputeableWall::get_door_near_coord_plus( int mDoorIndex, float mPerpDistance )
{
    float  distance = get_door_near(mDoorIndex);
    return get_point_away_from( distance, mPerpDistance );
}

MathVector glComputeableWall::get_door_center_coord_plus( int mDoorIndex, float mPerpDistance )
{
    float distance = get_door_center(mDoorIndex);
    return get_point_away_from( distance, mPerpDistance );
}

MathVector glComputeableWall::get_door_far_coord_plus(  int mDoorIndex, float mPerpDistance  )
{
    float distance = get_door_far(mDoorIndex);
    return get_point_away_from( distance, mPerpDistance );
}
/****** Door Vertices ********/


/* Extracts and puts into the abWall object */
void glComputeableWall::extract_2d_line_info()
{
    m_line.m_origin.dimension(3);
    m_line.m_origin[0] = m_x;
    m_line.m_origin[1] = m_y;
    m_line.m_origin[2] = m_z;
    
    // Convert the wall angle to a unit vector.  This might better be done when creating the dwelling in the first place.
    float rad = -glm::radians(m_y_angle);
    m_line.m_vector.dimension(3);
    m_line.m_vector[0] = cos(rad);
    m_line.m_vector[1] = 0.0;        // y is height!
    m_line.m_vector[2] = sin(rad);
}


MathVector glComputeableWall::get_point_away_from( float mDistanceAlong, float mPerpendicularDistance, bool mPositiveSide )
{
    MathVector pt("point away from wall", 3);
    pt = m_line.m_origin + m_line.m_vector * mDistanceAlong;
    // Define Positive Side:
    //          side of increasing perpendicular vector.
    
    //    if (mPositiveSide)
        
    pt += m_line.m_vector.get_perp_xz() * mPerpendicularDistance;
    return pt;
}


MathVector	glComputeableWall::get_origin (  )
{
    return m_line.m_origin;
}

MathVector	glComputeableWall::get_far_end (  )
{
    MathVector end = m_line.m_origin + (m_line.m_vector * m_wall_length);
    return end;
}


bool glComputeableWall::wall_is_between( glWallLine* mWallLine, glComputeableWall* mOrigWall )
{
    //glComputeableWall* originalWall = &(m_fwall[ mWallLine->m_wall_index ].m_bare_wall);
    MathVector perp_main = mOrigWall->m_line.m_vector.get_perp_xz();
    
    //  The Line                                This Wall
    // m_Start + perp_main * X_unknown = m_line.m_origin + m_line.m_vector * Y_unknown;
    // m_End   + perp_main * X_unknown = m_line.m_origin + m_line.m_vector * Y_unknown;

    
    // Then see if X_unknown <= mWallLine->m_perp_distance
    // y_Unkown is don't care for the test.  Use only to change the line end point.
    
    return false;
}

void glComputeableWall::add_line_seg(glWallLine &line, float mHeight, MathVector &end_retraction, vector<glWallLine>* lines )
{
    float magn = end_retraction.magnitude();
    if (magn>0)
        line.m_End   -= end_retraction;
    else
        line.m_Start += end_retraction;

    line.m_Start[1]= mHeight;
    line.m_End  [1]= mHeight;
    lines->push_back(line);
}

/*
    Where's Waldo?
 
    Creates a break for each doorway.  So doorway+1 line segments on each side of the wall.
        used for locating on a map.
 */
vector<glWallLine>* glComputeableWall::create_lines_distance_from(  float mLinearDistance, float mWallAngleRadians, float mHeight  )
{
    static vector<glWallLine> lines;
    int    doors = (int)m_doorways.size();
    
    glWallLine line1;       // working copy
    glWallLine line2;       // working copy
    line1.m_color = 0xFF00FF00;
    
    MathVector perp       = m_line.m_vector.get_perp_xz();
    float  perp1_distance = mLinearDistance * sin(mWallAngleRadians);
    float  perp2_distance = -(mLinearDistance + m_wall_thickness) * sin(mWallAngleRadians);
    line1.m_perp_distance = perp1_distance;
    line2.m_perp_distance = perp2_distance;

    // Now take parallel_distance off of 1 end.
    float parallel_distance   = mLinearDistance * cos(mWallAngleRadians);
    if (mWallAngleRadians > M_PI/2)
        parallel_distance = -parallel_distance;
    else
        parallel_distance = parallel_distance;
    MathVector end_retraction = m_line.m_vector * parallel_distance;
    
    // First line:  m_origin to first door.
    line1.m_Start = m_line.m_origin + perp * perp1_distance;
    line2.m_Start = m_line.m_origin + perp * perp2_distance;
    
    if (doors==0)
    {
        line1.m_End = get_far_end() + perp * perp1_distance;    // if no doors :
        line2.m_End = get_far_end() + perp * perp2_distance;
    } else {
        line1.m_End = get_door_near_coord_plus(0,0) + perp * perp1_distance;  // if 1 or more doors:
        line2.m_End = get_door_near_coord_plus(0,0) + perp * perp2_distance;
    }
    add_line_seg( line1, mHeight, end_retraction, &lines );
    add_line_seg( line2, mHeight, end_retraction, &lines );
    
    
    // 2nd line  :  end of first door to start of next.... repeat for all doors.
    for (int l=0; l<doors-1; l++)
    {
        line1.m_Start = get_door_far_coord_plus(l,perp1_distance); // + perp * ;  // if 1 or more doors:
        line2.m_Start = get_door_far_coord_plus(l,perp2_distance); // + perp * ;
        line1.m_End   = get_door_near_coord_plus(l+1,0) + perp * perp1_distance;  // if 1 or more doors:
        line2.m_End   = get_door_near_coord_plus(l+1,0) + perp * perp2_distance;

        add_line_seg( line1, mHeight, end_retraction, &lines );
        add_line_seg( line2, mHeight, end_retraction, &lines );
    }
    
    // last line :  end of last door to end of wall.
    if (doors>0)
    {
        line1.m_Start = get_door_far_coord_plus(doors-1,perp1_distance); // + perp * ;  // if 1 or more doors:
        line2.m_Start = get_door_far_coord_plus(doors-1,perp2_distance); // + perp * ;
        
        line1.m_End = get_far_end() + perp * perp1_distance;
        line2.m_End = get_far_end() + perp * perp2_distance;

        add_line_seg( line1, mHeight, end_retraction, &lines );
        add_line_seg( line2, mHeight, end_retraction, &lines );
    }
    return &lines;
}

//bool glComputeableWall::evaluate_collision( glMovingObject* mOther )
bool glComputeableWall::evaluate_collision( glSphere* mOther )
{
    // for now we assume it's a sphere:

    // IS IT ABOVE/BELOW the WALL?
    //bool candidate = false;
    float extent = mOther->m_position[1] + mOther->m_radius;
    if (extent < m_y)                return false;
    extent = mOther->m_position[1] - mOther->m_radius;
    if (extent > m_y+m_wall_height)  return false;

    // IS IT PAST THE START or END of the WALL?
    MathVector delta = mOther->m_position - m_line.m_origin;
    float distance_along_wall = m_line.m_vector.dot(delta);
    if (distance_along_wall < -mOther->m_radius) return false;
    if (distance_along_wall > (m_wall_length+mOther->m_radius)) return false;

    
    // We are now within the extents of the wall.  Check the actual distance away:
    MathVector perp = m_line.m_vector.get_perp_xz();
    float distance_away_from_wall = fabs(perp.dot( delta ));
    
    if (distance_away_from_wall > mOther->m_radius)
        return false;
    
    // Only chance of no collision now is in a doorway:
    long size = m_doorways.size();
    for(int d=0; d<size; d++)
    {
        // If it's in the doorway, let it pass.
        if ((distance_along_wall > m_doorways[d].position_lengthwise) &&
            (distance_along_wall < (m_doorways[d].position_lengthwise+m_doorways[d].width)) )
            if (mOther->m_y < (m_doorways[d].height-mOther->m_radius))
                return false;
    }
    
    return true;
}

