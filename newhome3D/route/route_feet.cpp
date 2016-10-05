//
//  route_feet.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 2/17/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "all_objects.h"
#include "route_feet.hpp"


glRouteWithFeet::glRouteWithFeet()
:glRoute()
{
    m_object_class          = 12;
    m_number_steps_indices  = 0;
    m_number_steps_vertices = 0;
    m_show_robot_feet       = false;
}

glRouteWithFeet::glRouteWithFeet( glRouteWithFeet& mCopy )
{
    
}
glRouteWithFeet::~glRouteWithFeet( )
{
    
}
void glRouteWithFeet::start_over_at( struct Vertex mStart )
{
    reset();
    reset_foot_prints();
    glRoute::start_over_at( mStart );
}

void glRouteWithFeet::reset( )
{
    m_number_steps_indices  = 0;
    m_number_steps_vertices = 0;
    m_left_steps.clear();
    m_right_steps.clear();
    gl_unregister();
    glRoute::reset();
}

/* Starts the entire path over */
void glRouteWithFeet::reset_foot_prints( )
{
    m_left_steps.clear();
    m_right_steps.clear();
    // want to delete the VBO/IBO as well:
    gl_unregister();
    m_vertices.clear();
    m_indices.clear();
}

void glRouteWithFeet::create_from_multi( glMultiRoute& mMulti )
{
    glRoute::create_from_multi( mMulti );
    compute_angles         ( );
    generate_steps_vertices( );
}

/* Prints the heel to toe coordinates & vector. */
void glRouteWithFeet::print_step( int mIndex, bool mLeft )
{
    struct stFootPosition fp;
    if (mIndex>m_left_steps.size())
    {  printf("Step is beyond end of route!\n");    return ;  }
    
    if (mLeft)
        fp = m_left_steps[mIndex];
    else
        fp = m_right_steps[mIndex];
    
    printf("%d heel:%6.1f, %6.1f, %6.1f \t", mIndex, fp.heel[0], fp.heel[1], fp.heel[2] );
    printf("toe:%6.1f, %6.1f, %6.1f \t",     fp.toe[0], fp.toe[1], fp.toe[2] 		 );
    printf("angle:%3.3f, %3.3f, %3.3f \n",   fp.angle[0], fp.angle[1], fp.angle[2] );
}


/*void glRouteWithFeet::place_robot_over_vertex( size_t mVertexIndex, glRobot &mRobot )
{
    float leg_height   = mRobot.m_left_leg.get_length();
    float torso_height = mRobot.m_torso.height/2.;

    mRobot.relocate( m_vertices[mVertexIndex].position[0],
                     leg_height + torso_height,
                     m_vertices[mVertexIndex].position[2] );

    float rise = m_vertices[mVertexIndex+1].position[0] - m_vertices[mVertexIndex].position[0];
    float run  = m_vertices[mVertexIndex+1].position[2] - m_vertices[mVertexIndex].position[2];
    mRobot.m_y_angle = atan2( rise, run ) * 180./M_PI;
}*/

void glRouteWithFeet::compute_angle( struct stFootPosition& mfoot )
{
    float dX = mfoot.toe[0] - mfoot.heel[0];
    float dY = mfoot.toe[1] - mfoot.heel[1];
    float dZ = mfoot.toe[2] - mfoot.heel[2];
    float distance2D = sqrt(dX*dX + dZ*dZ);
    
    mfoot.angle[0] = atan2(dY, distance2D);
    mfoot.angle[1] = atan2(dX, dZ);
    mfoot.angle[2] = 0.0;   // can't measure inside to outside!
}

/*
    Compute the angle for each step.  This will be needed for the leg rotations!
*/
void glRouteWithFeet::compute_angles( )
{
    for (int i=0; i<m_left_steps.size(); i++)
        compute_angle( m_left_steps[i] );
    for (int i=0; i<m_right_steps.size(); i++)
        compute_angle( m_right_steps[i] );
}

/* Special blue mark to show point along the path the foot print corresponds to. */
void glRouteWithFeet::colorize_center( int mVertexIndex )
{
    m_vertices[mVertexIndex].color[1] =   0.;      // colorize the center path vertex.
    m_vertices[mVertexIndex].color[2] = 0xFF;
}

/*
 
 */
/*
 
void glRouteWithFeet::compute_foot_print( float mVertexIndex, glWalkingRobot& mRobot, bool mLeft )
{
 m_vertices[mVertexIndex].color[1] =   0.;      // colorize the center path vertex.
 m_vertices[mVertexIndex].color[2] = 0xFF;

    // We'll put the foot print parallel to the path.  So we need 2 path points to find the angle.
    MathVector rr = compute_slope( mVertexIndex );

    // If all path points really are 2 inches apart, can eliminate sqrt and just / 2.0
    float hypotenus           = rr.magnitude();
    float path_center_to_heel = mRobot.get_feet_separation();
    float multiplier          = ( path_center_to_heel / hypotenus );

    if (mLeft==false)
        multiplier = -multiplier;

    // HEEL :
    struct stFootPosition fp;
    fp.heel[0] = m_vertices[mVertexIndex].position[0] + rr[2]*multiplier;
    fp.heel[1] = m_vertices[mVertexIndex].position[1];
    fp.heel[2] = m_vertices[mVertexIndex].position[2] - rr[0]*multiplier;
    fp.angle[1]= atan2(rr[0], rr[2]);

    // TOE :
    multiplier = (mRobot.m_left_leg.m_foot.m_foot_length / hypotenus );
    fp.toe[0] = fp.heel[0] + rr[0] * multiplier;
    fp.toe[1] = fp.heel[1];
    fp.toe[2] = fp.heel[2] + rr[2] * multiplier;
    if (mLeft)
        m_left_steps.push_back ( fp );
    else
        m_right_steps.push_back( fp );
}*/

/* We've placed vertices every 2 inches.
 Sometimes stride is considered every 2 steps (ie. left, right).
 Anthropromorphic data says average human stride is ~31 inches.
 We'll take 30 inches ==> every 15 vertices
 But we're going to use it as: left to right.
 This will give orientation as well as position.

 The standing feet pattern should be applied prior to this.  We'll
 compute the first step as 1/2 stride.
 
void glRouteWithFeet::compute_robot_steps( float mStride, glWalkingRobot& mRobot, bool mLeftFootFirst )
{
    struct Vertex v;
    v.position[1] = 0.0;
    bool  left_foot_turn  = mLeftFootFirst;
    int   stride_vertices = round(mStride/2.0);

    // First step = vertex 7 (half stride)
    for (int i=7; i<m_vertices.size()-1; i+=stride_vertices)
    {
        compute_foot_print( i, mRobot, left_foot_turn );
        left_foot_turn = !left_foot_turn;
    }
}

// The Robot object is used like a rubber stamp.  So we have to put it back where it was after finishing.
void glRouteWithFeet::compute_robot_path( float mStride, bool mLeftFootFirst, glWalkingRobot& mRobot )
{
    // Start by standing in the m_start position
    compute_robot_feet_standing( 0, mRobot );

    // WALK TO DESTINATION :
    compute_robot_steps( mStride, mRobot, true );
    // ONE EXTRA on the RIght Foot here!!
 
    // END STANDING POSITION :
                * What about non multiples of the stride?  how to handle.
                    Let's say - 32 inches left (1 more than a full stride)
                    it would be nice to do a 1/2 step prior.  
                    So 15 inches then a 17 inch (roughly 2 half strides)
                *

    compute_robot_feet_standing( m_number_path_vertices-2, mRobot );
}*/

/* Translates the m_left_steps<> and m_right_steps<> into vertices  */
void glRouteWithFeet::generate_steps_vertices( )
{
    struct Vertex v;
    v.color[3]= 0xFF;
    
    size_t lsize = m_left_steps.size();
    size_t rsize = m_right_steps.size();
     
    // Left Foot First:
    for (int i=0; i<lsize; i++)
    {
        if (i==0)
        {   v.color[2] = 0x0;  v.color[1]= 0xFF;   v.color[0]= 0x2F;     // Want red/greenish for "start".
        } else if (i==(lsize-1))
        {   v.color[2]= 0;  	v.color[1]= 0x00;   v.color[0] = 0xFF;
        } else {
            v.color[2]= 0;  	v.color[1]= 0xAF;  	v.color[0]= 0x7F;    // blue/red
        }
 
        // HEEL:
        memcpy (v.position, m_left_steps[i].heel, sizeof(float)*3 );
        m_vertices.push_back( v );

        // TOE :	(same location as heel plus 1 foot delta along rise/run)
        memcpy (v.position, m_left_steps[i].toe, sizeof(float)*3 );
        m_vertices.push_back( v );
    }

    v.color[3]= 0xFF;  	    // Right Foot Second:
    for (int i=0; i<rsize; i++)
    {
        if (i==0) {
           v.color[2] = 0x2F;  v.color[1]= 0xFF;    v.color[0]= 0x00;     // green
        } else if (i==(rsize-1)) {
           v.color[2]= 0;      v.color[1] = 0x00; 	v.color[0]= 0xFF;     // red
        } else {                                                          // red/blue
           v.color[2]= 0x7F;      v.color[1]= 0xAf;  	v.color[0]= 0x00;
        }
        
        // HEEL:
        memcpy (v.position, m_right_steps[i].heel, sizeof(float)*3 );
        m_vertices.push_back( v );
        
        // TOE :	(same location as heel plus 1 foot delta along rise/run)
        memcpy (v.position, m_right_steps[i].toe, sizeof(float)*3 );
        m_vertices.push_back( v );
    }

    m_number_steps_vertices = (int)(lsize*2 + rsize*2);
    printf("Route::generate_steps_vertices() num_robot_vertices= %d \n", m_number_steps_vertices );
}

void glRouteWithFeet::draw_body( )
{
    glRoute::draw_body();
    
    // Draw Foot steps :
    if ( m_number_steps_vertices ) {
        glLineWidth( 4.0 );
        glDrawArrays(GL_LINES,  m_number_path_vertices, m_number_steps_vertices );
        glLineWidth( 1.0 );
    }

}

