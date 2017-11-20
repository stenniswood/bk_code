//
//  init_samples.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 2/26/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include "init_samples.hpp"
#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <list>
#include <math.h>
#include <string>
#include <string.h>
#include "imageloader.h"
#include "all_objects.h"
#include "simulator_memory.h"
#include "sequencer_memory.hpp"
#include "client_memory.hpp"
#include "proc_client_cmds.h"
#include "sign.h"
#include "quarter_629.h"
#include "hampton_complex.h"
#include "basket_ball_court.h"
#include "sports_bench.h"
#include "basketball_arena.h"
#include "atom_objects.h"
#include "truss.h"
#include "rafter.h"
#include "road.h"
#include "asset_importer.hpp"       // sjt spin on it.

vector<MathVector>   Destinations;
vector<MathVector>   Sources;
int DestinationIndex = 0;
int SourceIndex      = 0;

glStairway 			stairs;
StreetSign          street_signs[10];
glRoad              road;
glRoof              roof;
txtContainer        ground;
glHighwayOverhead   Overhead(6);
glMap2D             map2D;
glMultiRoute        multiRoute;
/*************** OBJECTS ***********************
glTerrain           terra;
glRafter            rafter;
glFaceBox           cube;
glBox               cubeR;
glRing              ring;
glLightSwitch       ls;
glBrickWall         brick_wall;
//glCameraScreen      screen;
glPaper             paper(1);
glOneBuilding       Hcomplex;
glRoute             door_path;*/

glOurHouse          house;
glApartment         apartment;
glApartment         apartment2;
glArena             bball_arena;
glCounter           KitchenSinkCounter(5.*18., 26., 37. );
glWalkingRobot      robot;
glSphere            sphere1(NBA_BASKET_BALL_RADIUS, 10, 64);
glSphere            sphere2(FIFA_SOCCER_BALL_RADIUS_SIZE5, 10, 64);
glSphere            sphere3(FIFA_SOCCER_BALL_RADIUS_SIZE4, 10, 64);
glSphere            sphere4(BASEBALL_RADIUS, 10, 64);

glWindow            window;
glTable             table;
glTableCenterPole   tableCP;
glTextureCylinder   txt_cylinder(24);
glDepthMap			dmap;
/************ END OF OBJECTS ****************/

//vector<Room>        Rooms;

/*void init_rooms()       // For Hampton apartment.
{
    // (bigger than living room b/c 3' closet is included)
    Room* last_room = &(Rooms.back());
    Room tmpR;
    tmpR.m_names = "Living Room";
    tmpR.set_size(144, 178);
    tmpR.move_to( 0,  0);
    Rooms.push_back(tmpR);        // Living room
    
    last_room = &(Rooms.back());
    tmpR.m_names = "First bedroom";
    tmpR.set_size(152, 160);
    tmpR.place_next_to_x(*last_room);
    Rooms.push_back(tmpR);        // First Bedroom    (144, 0)
    
    last_room = &(Rooms.back());
    tmpR.m_names = "Master bedroom";
    tmpR.set_size(120, 160);    tmpR.place_next_to_x(*last_room);
    Rooms.push_back(tmpR);        // Master Bedroom  (144+152,  0)
    
    last_room = &(Rooms.back());
    tmpR.m_names = "closet";
    tmpR.set_size( 70, 95);
    tmpR.place_next_to_y(*last_room);
    Rooms.push_back(tmpR);      // Closet (144+152, 160)
    
    last_room = &(Rooms.back());
    tmpR.m_names = "Bathroom";
    tmpR.set_size(302,  95);
    tmpR.move_to(50, 196);
    tmpR.place_next_to_y(*last_room);
    Rooms.push_back(tmpR);        // Bathroom  416-114=302
    
    tmpR.m_names = "Kitchen";
    tmpR.set_size(114,  95);     tmpR.move_to(50, 196);
    Rooms.push_back(tmpR);        // Kitchen
    
    tmpR.m_names = "Dining room";
    tmpR.set_size(43.5, 95);     tmpR.move_to(50, 196);
    Rooms.push_back(tmpR);        // Dining room
    
    tmpR.m_names = "storage closet";
    tmpR.set_size(70.5, 74.75);  tmpR.move_to(50, 196);
    Rooms.push_back(tmpR);        // Storage Closet
    
    tmpR.m_names = "entrance";
    tmpR.set_size(70.5, 36);     tmpR.move_to(50, 178);
    Rooms.push_back(tmpR);        // entrance
    
    last_room = &(Rooms.back());
    tmpR.m_names = "hallway";
    tmpR.set_size(194.5, 36);
    tmpR.place_next_to_y(*last_room);
    Rooms.push_back(tmpR);        // Hallway
}*/

void init_street_scene()
{
    street_signs[0].m_sign_name = "stop";
    street_signs[0].create();
    street_signs[0].relocate( 550, 0, -100);
    
    street_signs[1].m_sign_name = "handicapped";
    street_signs[1].create();
    street_signs[1].relocate( 570, 0, -110);
    
    street_signs[2].m_sign_name = "parking";
    street_signs[2].create();
    street_signs[2].relocate( 590, 0, -120);
    
    street_signs[3].m_sign_name = "no_parking";
    street_signs[3].create();
    street_signs[3].relocate( 610, 0, -130);
    
    street_signs[4].m_sign_name = "railroad_crossing";
    street_signs[4].create();
    street_signs[4].relocate( 620, 0, -140);
    
    street_signs[5].m_sign_name = "turn_left";
    street_signs[5].create();
    street_signs[5].relocate( 630, 0, -150);
    
    street_signs[6].m_sign_name = "turn_right";
    street_signs[6].create();
    street_signs[6].relocate( 640, 0, -160);
    
    street_signs[7].m_sign_name = "wrong_way";
    street_signs[7].create();
    street_signs[7].relocate( 650, 0, -170);
    
    street_signs[8].m_sign_name = "caution_speed_bump";
    street_signs[8].create();
    street_signs[8].relocate( 660, 0, -180);
    
    street_signs[9].m_sign_name = "caution";
    street_signs[9].create();
    street_signs[9].relocate( 670, 0, -190);
     
    
    MathVector point(3);
    point.set_xyz   (675, 20, -1000);
    road.m_road_width = 23*12;
    road.m_path.start_over_at2( point );
    point[2] = 0;
    road.m_path.add_way_point2( point );
    point[0] = 1000;
    point[2] = 600;
    road.m_path.add_way_point2( point );
    road.create();
    
    
    Overhead.m_road_width = 30*12;
    Overhead.m_height     = 15*12;
    
    glFaceBox* fb = new glFaceBox();
    fb->load_image("./street_signs/exit_tunnel_road.png", FACE_FRONT_ID, 1);
    fb->height= 3*12;
    fb->width = 3*12;
    fb->depth = 0.1;
    fb->setup();
    Overhead.add_sign( fb, 0, 1 );
    fb = new glFaceBox();
    fb->load_image("./street_signs/berwick_half_mile.jpg", FACE_FRONT_ID, 1);
    fb->height= 3*12;
    fb->width = 3*12;
    fb->depth = 0.1;
    fb->setup();
    Overhead.add_sign( fb, 1, 1 );
    fb = new glFaceBox();
    fb->load_image("./street_signs/downtown_exit.jpg", FACE_FRONT_ID, 1);
    fb->height= 3*12;
    fb->width = 3*12;
    fb->depth = 0.1;
    fb->setup();
    Overhead.add_sign( fb, 2, 1 );
    Overhead.create  (    );
    Overhead.relocate( 500, 0, -200 );
}

void select_route()
{
    long size = Destinations.size();
    DestinationIndex++;
    if (DestinationIndex>=size)
    {
        DestinationIndex = 0;
        SourceIndex++;
        if (SourceIndex>=size)
            SourceIndex = 0;
    }
    change_route(Sources[SourceIndex], Destinations[DestinationIndex] );
}

void init_sample_paths()
{
    // Use the 'Z' key to twiddle around from room to room:
    MathVector tmp(3);
    tmp.set_xyz( -100, 2., -50. );             // Living room
    Sources.push_back     ( tmp );
    Destinations.push_back( tmp );
    tmp.set_xyz( -100, 2., 300.);             // First bedroom
    Sources.push_back     ( tmp );
    Destinations.push_back( tmp );
    tmp.set_xyz(180, 2., 50.);              // First bedroom
    Sources.push_back     ( tmp );
    Destinations.push_back( tmp );
    tmp.set_xyz(350, 2., 50.);              // Master Bedroom
    Sources.push_back     ( tmp );
    Destinations.push_back( tmp );
    tmp.set_xyz(375, 2., 180.);             // Master Closest
    Sources.push_back     ( tmp );
    Destinations.push_back( tmp );
    tmp.set_xyz(375, 2., 275.);             // Bathroom
    Sources.push_back     ( tmp );
    Destinations.push_back( tmp );
    tmp.set_xyz(210, 2., 250.);             // Kitchen
    Sources.push_back     ( tmp );
    Destinations.push_back( tmp );
}

void init_sports_balls()
{
    sphere1.is_participating = true;
    sphere1.coef_of_restitution = COEFF_RESTITUTION_BASKETBALL;
    sphere1.set_color( 0xFF7F7F00);
    sphere1.load_texture("textures/basketball_texture.jpg" );
    sphere1.create  (  );
    sphere1.m_velocity[0] =  40;
    sphere1.m_velocity[2] = -10;
    sphere1.relocate( 65, 70, 110 );
    
    sphere2.is_participating = true;
    sphere2.coef_of_restitution = COEFF_RESTITUTION_SOCCERBALL;
    sphere2.set_color( 0xFFFF0000);
    sphere2.load_texture("textures/soccer_ball_3.jpg" );
    sphere2.create  (  );
    sphere2.m_velocity[0] = -40;
    sphere2.m_velocity[2] = -20;
    sphere2.m_R_velocity[0] = 3;
    sphere2.m_R_velocity[1] = 3;
    sphere2.relocate( 65, 70, 100 );
    
    sphere3.is_participating = true;
    sphere3.coef_of_restitution = COEFF_RESTITUTION_SOCCERBALL;
    sphere3.set_color( 0xFF5F7F2F);
    sphere3.load_texture("textures/soccer_ball_world_cup.jpg" );
    sphere3.create  (  );
    sphere3.m_velocity[0] =40;
    sphere3.m_velocity[2] = 10;
    sphere3.m_R_velocity[0] = 2;
    sphere3.m_R_velocity[1] = 3;
    sphere3.relocate( 60, 70, 100 );
    
    sphere4.is_participating = true;
    sphere4.coef_of_restitution = COEFF_RESTITUTION_BASEBALL;
    sphere4.set_color( 0xFF0F0000);
    sphere4.load_texture("textures/baseball_texture.jpg" );
    sphere4.create  (  );
    sphere4.m_velocity[0] = -45;
    sphere4.m_velocity[2] = 20;
    sphere4.relocate( 60, 70, 110 );
}
void init_robots()
{
    robot.m_show_paths = false;
    robot.m_glide_over_path_mode = false;
    robot.create();
    robot.relocate( 20, 50, 20 );
    robot.hands_on_hip();
    printf("\nROBOT_ID = %ld\n",robot.m_object_id );
}

void init_dwellings()
{
/*  roof.m_house_length = 50*12;
    roof.m_house_width  = 25*12;
    roof.create();
    roof.relocate( -800, 25*12, 0 );
*/
    house.create();
    house.relocate( -800, 0, 0 );
    //init_rooms();

    //Hcomplex.create();
   // init_sample_paths();    
    printf("========= CREATING APARTMENT ====================\n");
    apartment.create          ( );
/*    apartment.update_line_info( );
    apartment.create_map(map2D);
    map2D.create_2D_drawing();      // creates the opengl vertices.	//map2D.scale_drawing(0.125);
    map2D.print_2D_info();
    map2D.gl_register( );
    map2D.m_z_angle = 0.0;
    map2D.m_y       = 0.0;
    //apartment.find_all_possibilies( 2*12, 60*M_PI/180., 3*12 );
    select_route( );
*/    
    printf("\nApartment Create\n\n");
/*    apartment2.setup();
    apartment2.mirror_image_x();
    apartment2.gl_register   ();
    apartment2.relocate( 0, 0, 300);

    // Sample door handle path :
    //    (exact same method should be available for drawers, light switches)
    glDoor* selected_door = (glDoor*)apartment.m_fwalls[1]->m_doors[0];
    selected_door->create_handle_path( door_path );

    door_path.map_vertices( apartment.m_fwalls[1]->get_body_matrix() );
    door_path.gl_register();
*/
    string searchname     = "master bedroom";
    string searchtypename = "door";
    //VerbalObject* ptr = apartment.find_component(searchname, searchtypename);
    
    //KitchenSinkCounter.set_number_of_cabinets(4);
    //KitchenSinkCounter.create();
    //KitchenSinkCounter.relocate( 144., 0., 160.+36.+95. );
}

void init_terrain()
{
	//terra.load_image( "textures/stucco-texture.jpg" );
/*	terra.load_image( "textures/Ireland.jpg"     );
    terra.set_stretch( 10, 10 );
    terra.m_y_scale = 1.0;
    //terra.load_image( "textures/heightmap6.jpg"  );
//    terra.add_omission(&apartment);
//    terra.add_omission(&apartment2);
    //terra.add_omission(&bball_court);
    //terra.load_image( "textures/heightmap_P.png" );
    terra.create(); */
}

void init_ground()
{
}

void init_objects()
{
/*  ground.width = 2000;
    ground.depth = 2000;
    ground.height=  0.5;
    int reps_x = trunc(ground.width/300.);
    int reps_y = trunc(ground.depth/200.);
    Texture* txt = new Texture();
    txt->load_image("textures/grass2.jpg");
    //Texture* txt = ground.load_image("textures/grass2.jpg",TOP_SIDE_ID);
    ground.apply_top   ( txt, reps_x, reps_y );
    ground.apply_bottom( txt, reps_x, reps_y );
    ground.m_y = -2.0;
//    ground.update_body_matrix();
    ground.create(); 
*/
	//init_ground();
//    init_street_scene();
    init_dwellings();
    init_robots();
	//init_terrain(); 

	dmap.create(); 
	dmap.relocate(0, 150, 0);
	
/*  bball_arena.create();
    bball_arena.relocate( -40*12, 2, -94*12 );
    bball_arena.create_cam_routes(); */
    
    table.create();
    table.relocate  ( 90.+24, 0, 291.-42.);
    tableCP.create();
    tableCP.relocate( 144.+24, 0, 91.-22.);
  
    window.set_params  (50, 48, 48, 1.5 );
    window.create      (  );
    window.relocate	   ( 40, 0, 40 );

    txt_cylinder.m_radius = 15.;
    txt_cylinder.m_extrusion_length = 20.;
    txt_cylinder.m_extrusion_axis = 1;
    txt_cylinder.m_y = 12*0;
    txt_cylinder.setup();
    txt_cylinder.gl_register();
	return; 
	
    // picnic.create();
	// init_sports_balls();
	// sphere.load_texture("textures/me_in_car.bmp");

   // brick_wall.set_length_height  ( 15*12, 8*12 );
   // brick_wall.create_components  ( );
   // brick_wall.relocate           ( 0. , 0., -150.  );
/*    
    theGlobals.gv.CameraOn = 0;
    /*    float scale = 6.0;
     paper.load_texture    ( "textures/me_in_car.bmp" );
     paper.set_width_height( 10*scale, 10*scale       );
     paper.create          (                          );
     paper.relocate        ( 40., 22.*4., 5.0 );
     paper.m_x_angle       = 90.;
     paper.m_y_angle       = 90.; */
    
    /*    if (theGlobals.gv.CameraOn  ) {
     screen.set_width_height( 1280/10, 700/10 );
     ((CameraTexture*)screen.m_texture)->generate_PBO();
     // ((CameraTexture*)screen.m_texture)->
     // ((CameraTexture*)(screen.m_texture))->timeslice();
     screen.create          (                      );
     screen.relocate        ( 10., 22.*4., 9.0     );
     screen.m_x_angle       = 180.;
     }
*/   
 /*   //	  backboard->relocate( 0, 75, -50 );
    cubeR.width = 10;
    cubeR.depth = 10;
    cubeR.height = 20.0;
    cubeR.set_color(0xFFFFFF00);
    //cubeR.create();
    cubeR.relocate( 20, 90, 20 );
    
    cube.width = 10;
    cube.depth = 10;
    cube.height= 20.0;
    cube.set_color( 0xFFFFFF00 );
    //Mat* txt  = cube.load_image("textures/me_in_car.bmp",        FACE_TOP_ID );
    //Mat* txt2 = cube.load_image("textures/sidewalk_painting.jpg",FACE_LEFT_ID);        // 0 = top
    //((CuboidTexture*)cube.m_texture)->apply_front( txt);
    //((CuboidTexture*)cube.m_texture)->apply_back (txt2);
    //cube.apply_left (txt2);
    //cube.apply_right(txt2);
    /*Texture* txt3 = cube.load_image("textures/door_carmelle.jpg",FACE_FRONT_ID);        // 0 = top
     cube.apply_front(txt3,1);
     cube.apply_back (txt3,1); 
    cube.m_R_velocity[1] = 25;
    cube.is_participating = true;
    //cube.create();
    cube.relocate( 50, 90, 50 );
    
    //loadAsset();
    //init_construction_objects();
*/
	/* This causes some objects to disappear!  Mostly the glBox objects created prior to this call!  */
    //    theWorld.release_vertex_memories();
/*     stairs.m_extrusion_length = 25*12;
     stairs.set_color( 0xFF0000FF);
     stairs.create();
     stairs.m_y_angle = 0.;
     stairs.relocate( 100, 10., -25*12 );  */
}
