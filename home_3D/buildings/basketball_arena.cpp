//
//  basketball_arena.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 8/3/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "basketball_arena.h"
#include "basket_ball_court.h"
#include "stairway.hpp"
#include "sports_bench.h"
#include "walking_robot.h"



const int NUMBER_STEPS           = 20;
//const float AUDIENCE_TIER_LENGTH = 50*24;

glArena::glArena ()
{
    
}
glArena::~glArena()
{
    
}
void      glArena::create_vertical_walls   ( )
{
    
}


void glArena::add_light_switches      ( )
{
    
}



void glArena::create_cam_routes       ( )
{
    bball_court->create_cam_routes();
    
    glRoute* tmpR = bball_court->m_routes[0];
    long size = tmpR->m_vertices.size();
    MathVector tmpV(3);
    MathVector wpt (3);
    for (int v=0; v<size; v++)
    {
        struct Vertex vertex = tmpR->m_vertices[v];
        tmpV[0] = vertex.position[0];
        tmpV[1] = vertex.position[1];
        tmpV[2] = vertex.position[2];

            // arena has to be located first! so can't do at setup.
        wpt = map_coords( tmpV );
        tmpR->m_vertices[v].position[0] = wpt[0];
        tmpR->m_vertices[v].position[1] = wpt[1];
        tmpR->m_vertices[v].position[2] = wpt[2];
    }
}

/* The length of the basketball court.  */
void glArena::create_stands           ( )
{
    // A "STAIRWAY" (supper long!)
    glStairway* tmp = new glStairway();
    tmp->m_run = 24.;
    tmp->m_extrusion_length = 94*12;
    tmp->set_color( 0xFFFF00FF );
    tmp->m_number_of_steps = NUMBER_STEPS;
    tmp->setup();
    tmp->relocate( 25*12+24, 0, -tmp->m_extrusion_length+NBA_COURT_LENGTH/2 );
    m_components.push_back( tmp );
    
    // Audience benches:
    glBench* tmpB = NULL;
    for (int i=0; i<NUMBER_STEPS; i++)
    {
        tmpB = new glBench();
        tmpB->m_length = 50*12;
        tmpB->m_height = 2*12;
        tmpB->set_color( 0xFF5f50AF);
        tmpB->setup();
        //((glFaceBox*)(tmpB->m_components[0]))->m_texture = m_bench_texture;     // set pointer
        tmpB->m_y_angle = 90;
        tmpB->relocate( tmp->get_front_edge(i)+tmp->m_run*1.5+25*12, tmp->get_height(i), -94*12/4+NBA_COURT_LENGTH/2 );
        m_components.push_back( tmpB );
    }

    tmp = new glStairway();
    tmp->m_run = 24.;
    tmp->m_extrusion_length = 94*12;
    tmp->set_color( 0xFFFF00FF );
    tmp->m_number_of_steps = NUMBER_STEPS;
    tmp->setup();
    tmp->m_y_angle = 180;
    tmp->relocate( -25*12+24, 0, NBA_COURT_LENGTH/2 );
    m_components.push_back( tmp );
    
    // Audience benches:
    tmpB = NULL;
    for (int i=0; i<NUMBER_STEPS; i++)
    {
        tmpB = new glBench();
        tmpB->m_length = 50*12;
        tmpB->m_height = 2*12;
        tmpB->set_color( 0xFF5f50AF);
        tmpB->setup();
        //((glFaceBox*)(tmpB->m_components[0]))->m_texture = m_bench_texture;     // set pointer
        tmpB->m_y_angle = 90;
        tmpB->relocate( -(tmp->get_front_edge(i)+tmp->m_run*1.5+25*12), tmp->get_height(i), -94*12/4+NBA_COURT_LENGTH/2 );
        m_components.push_back( tmpB );
    }
}

void glArena::create_stands_ends( )
{
    glStairway* tmp = new glStairway();
    tmp->m_run = 24.;
    tmp->m_extrusion_length = 50*12;
    tmp->set_color( 0xFFFF00FF );
    tmp->m_number_of_steps = NUMBER_STEPS;
    tmp->setup();
    tmp->m_y_angle = -90;
    tmp->relocate( NBA_COURT_WIDTH/2, 0, NBA_COURT_LENGTH/2 );
    m_components.push_back( tmp );

    // Audience benches:
    glBench* tmpB = NULL;
    for (int i=0; i<NUMBER_STEPS; i++)
    {
        tmpB = new glBench();
        tmpB->m_length = 50*12;
        tmpB->m_height = 2*12;
        tmpB->setup();
        ((glFaceBox*)(tmpB->m_components[0]))->m_texture = m_bench_texture;     // set pointer
        tmpB->m_y_angle = 0;
        tmpB->relocate( -25*12, tmp->get_height(i), tmp->get_front_edge(i)+tmp->m_run*1.5+NBA_COURT_LENGTH/2 );
        m_components.push_back( tmpB );
    }
    
    tmp = new glStairway();
    tmp->m_run = 24.;
    tmp->m_extrusion_length = 50*12;
    tmp->set_color( 0xFFFF00FF );
    tmp->m_number_of_steps = NUMBER_STEPS;
    tmp->setup();
    tmp->m_y_angle = 90;
    tmp->relocate( -tmp->m_extrusion_length/2, 0, -NBA_COURT_LENGTH/2 );
    m_components.push_back( tmp );
    
    // Audience benches:
    tmpB = NULL;
    for (int i=0; i<NUMBER_STEPS; i++)
    {
        tmpB = new glBench();
        tmpB->m_length = 50*12;
        tmpB->m_height = 2*12;
        tmpB->setup();
        ((glFaceBox*)(tmpB->m_components[0]))->m_texture = m_bench_texture;     // set pointer
        tmpB->m_y_angle = 0;
        tmpB->relocate( -25*12, tmp->get_height(i), -(tmp->get_front_edge(i)+tmp->m_run*1.5)-NBA_COURT_LENGTH/2 );
        m_components.push_back( tmpB );
    }
}

void glArena::create_components       ( )
{
    create_stands();
    create_stands_ends();
    
    // CREATE COURT:
    bball_court = new glBasketballCourt();
    bball_court->setup();
    bball_court->relocate( 0*12, 1, -0*12 );   // centered
    m_components.push_back( bball_court);
}

void glArena::setup( )
{
/*    m_bench_texture = new CuboidTexture();
    Mat* image = m_bench_texture->load_image( "textures/corrogated_aluminum.jpg", FACE_TOP_ID );
    //Mat* image = m_bench_texture->load_image( "textures/corrogated_steel_metal.jpg", FACE_TOP_ID );
    m_bench_texture->apply_bottom( image ); */
    
    m_bench_texture = NULL;
    
    glMolecule::setup();
    
}


