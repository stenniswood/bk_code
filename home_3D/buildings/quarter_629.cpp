//
//  quarter_629.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/12/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "quarter_629.h"
#include "all_objects.h"


#define SELECTED_BRICK 10
#define ALUMINUM_SIDING 14
#define ALUMINUM_SIDING_BLUISH 15

glUpstairs::glUpstairs()
{
    
}
glUpstairs::~glUpstairs()
{
    
}

void glUpstairs::create_exterior_walls( )
{
    glDoor* 			tdoor;
    struct  stSize 		s;		// for windows
    static  glFullWall* w;

    // Side 1 : 
    w = new glFullWall();
    w->clear( );
    w->set_length( 44*12.);                 // 24+20= 44 feet long inside
    s.width = 48.5;    s.height = 34.;
    w->m_bare_wall.add_window(  124, s, 46. );              // kitchen length:  52.25 +48.5+ 52.25 = 153
    s.width = 72;    s.height = 34.;
    w->m_bare_wall.add_window(  124+48.5+10+34.25+6+41, s, 46. );     // kitchen length:  52.25 +48.5+ 52.25 = 153
    s.width = 48.5;    s.height = 34.;
    w->m_bare_wall.add_window(  44*12-100, s, 46. );        // kitchen length:  52.25 +48.5+ 52.25 = 153
    w->set_texture( SELECTED_BRICK );
    w->m_y_angle = 0.0;
    w->m_z = 0.;
    w->m_x = 0.;
    w->setup    (  );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );
    
    // Update height stored in glDwellingLevel
    m_height = m_fwalls[0]->m_bare_wall.m_wall_height;
    
    // Side 2 :
    w = new glFullWall();
    w->clear( );
    w->set_length( 44*12.);
    tdoor = new glDoor();                   // Door #1
    tdoor->m_width = 31.5;
    tdoor->m_height = 78;
    tdoor->set_params(30.);
    tdoor->set_hinge     (true);
    tdoor->set_swing_side(true);
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door          ( 122.+40.5+64.5, tdoor );
    s.width = 48.5;    s.height = 34.;
    w->m_bare_wall.add_window( +52+6 +48+6, s, 11+36. );        // kitchen length:  52.25 +48.5+ 52.25 = 153
    s.width = 60.0;    s.height = 46.;
    w->m_bare_wall.add_window(  47.5+6+153+48+6, s, 34. );
    s.width = 36.375;  s.height = 34.;
    w->m_bare_wall.add_window(  12  +6+111+6+153+48+6, s, 80.5 );
    s.width =  48;     s.height = 34.;
    w->m_bare_wall.add_window( 6*12 +6+111+6+153+48+6, s, 46.25 );
    w->set_texture( ALUMINUM_SIDING );
    w->m_y_angle = 0.0;
    w->m_z = 24*12+6;                         // 288
    w->m_x = 0.;
    w->setup    		  (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );
    
    // Front :
    w = new glFullWall();
    w->clear( );
    w->set_length( 25*12.);
    tdoor = new glDoor();
    tdoor->m_width = 35;
    tdoor->m_height = 78.25;
    tdoor->set_params(30.);
    tdoor->set_hinge     (false);
    tdoor->set_swing_side(true );
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door  ( 25*12-(6+28+4.75+tdoor->m_width), tdoor );      // closet is 28 inch deep.
    s.width = 95.5;    s.height = 59.;
    w->m_bare_wall.add_window( +41+6, s, 22. );
    w->set_texture( SELECTED_BRICK );    
    w->m_y_angle = -90.0;
    w->m_z = 0.;
    w->m_x = 0.;
    w->setup    		  (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );
    
    // Back End :
    w = new glFullWall();
    w->clear( );
    w->set_length( 25*12.);
    s.width = 48.25;    s.height = 34.;
    w->m_bare_wall.add_window( 67+6, s, 46. );
    s.width = 48.25;    s.height = 34.;
    w->m_bare_wall.add_window( 25*12-67-48.25-6, s, 46. );
    w->set_texture( 14 );
    w->m_y_angle = -90.0;
    w->m_z = 0.;
    w->m_x = 44*12;
    w->setup    		  (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );
}
/* Wall Index Names:
0 - Upstairs Side Wall 1 (left)
1 - Upstairs Side Wall 2 (right)
2 - Upstairs front
3 - Upstairs back
4 - Closet Boys room (back)
5 - Closet Boys room (side)
6 - Hall way #1
7 - Kitchen / Living Room
8 - Hall way #2 (bathroom)
9 - Living Room/Boys Bedroom
 
10 -
11 -
12 -
13 -
14 -
15 -
16 -
17 -
18 -
19 -
20 -
 */
void glUpstairs::create_interior_walls ( )
{
    const int WALL_TEXTURE = 3;    
    glDoor* 			tdoor;
    static glFullWall*  w;

    // Closet Boys room (Stairway)
    w = new glFullWall();
    w->clear( );
    w->set_length( 41 );
    w->set_texture(4);
    w->m_y_angle = -90.0;
    w->m_z = +6;
    w->m_x = 124+48.5+10;
    w->setup    		  (   );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );
    
    // Closet Boys room (Stairway juts back)
    w = new glFullWall();
    w->clear( );
    w->set_length( 34.25 );
    w->set_texture(4);
    w->m_y_angle = 0.0;
    w->m_z = +41;
    w->m_x = 124+48.5+10;
    w->setup    (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );
    
    // Hall #1
    w = new glFullWall  ( );
    w->clear            ( );
    w->set_length       ( 24.5*12.);
    tdoor = new glDoor();                   // Door #1 Boys bedroom
    tdoor->m_width  = 29.75 ;
    tdoor->m_height = 79.375;
    tdoor->set_params(30.);
    tdoor->set_hinge     (false);
    tdoor->set_swing_side(true);
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door          ( 112., tdoor );
    tdoor = new glDoor();                   // Door #2  Mom and Dads
    tdoor->m_width = 29.75;
    tdoor->m_height = 79.375;
    tdoor->set_params(30.);
    tdoor->set_hinge     (true);
    tdoor->set_swing_side(true);
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door  ( 112.+43., tdoor );
    w->set_texture(WALL_TEXTURE);
    w->m_y_angle = 0.0;
    w->m_z = 144.;
    w->m_x = 19*12.;
    w->setup              (  );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );    
    
    // Kitchen / Living Room (similar to hall #2):
    w = new glFullWall();
    w->clear( );
    w->set_length( 181 );
    w->m_bare_wall.set_texture(4);
    w->set_texture(WALL_TEXTURE);
    w->m_y_angle = 0.0;
    w->m_z = 25*12-(88.75+28.+6);
    w->m_x = 4*12.;
    w->setup    		  (   );
    m_components.push_back( w );
    m_fwalls.push_back( w );
        
    // Hall #2
    w = new glFullWall();
    w->clear( );
    w->set_length( 33.+27.875+35 );
    tdoor = new glDoor();                   // Bathroom Door
    tdoor->m_width  = 27.875;
    tdoor->m_height = 80.0;
    tdoor->set_params(30.);
    tdoor->set_hinge     (false);
    tdoor->set_swing_side(false);
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door          ( 33., tdoor );
    //w->m_bare_wall.add_door( 122.+40.5+64.5, DEFAULT_DOOR_WIDTH, 79.0, false );
    w->set_texture(WALL_TEXTURE);
    w->m_y_angle = 0.0;
    w->m_z = 25*12-6-114; // was 144.+37.;
    w->m_x = 153+6+48+6+111;
    w->setup   			  (   );
    m_components.push_back( w );
    m_fwalls.push_back( w );
            
    // Living Room/Boys Bedroom :
    w = new glFullWall();
    w->clear        ( );
    w->set_length   ( 12*12.);         // 88.5 + 54.75 = 143.25
    w->set_texture(WALL_TEXTURE);
    w->m_y_angle = -90.0;
    w->m_z       = 0.;
    w->m_x       = 19*12;
    w->setup        (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );

    // Boys Bedroom/Mom & Dads :
    w = new glFullWall  ( );
    w->clear            ( );
    w->set_length       ( 12*12.);
    w->set_texture      ( WALL_TEXTURE);
    w->m_y_angle = -90.0;
    w->m_z = 0.;
    w->m_x = 19*12.+12*12;
    w->setup              (   );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );
    
    // Bathroom/Lynn's :
    w = new glFullWall();
    w->clear( );
    w->set_length( 10*12.);
    w->set_texture(WALL_TEXTURE);
    w->m_y_angle = -90.0;
    w->m_z = 25*12-6-114;
    w->m_x = 6+58+6+153+6+48+6+111;
    w->setup    		(  );
    m_components.push_back( w );
    m_fwalls.push_back( w );
    	
    // Bathroom/Lynn's by her door (not the closet):
    w = new glFullWall();
    w->clear( );
    w->set_length( 5*12.);
    tdoor = new glDoor();                   // Door #1 Boys bedroom
    tdoor->m_width  = 29.75 ;
    tdoor->m_height = 79.375;
    tdoor->set_params(30.);
    tdoor->set_hinge     (true);
    tdoor->set_swing_side(true);
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door          ( 3., tdoor ); 
    w->set_texture       ( WALL_TEXTURE );
    w->m_y_angle = -90.0;
    w->m_z 		 = 144.;
    w->m_x 		 = 153+6+48+6+111+33.+27.875+35 + 0.5;
    w->setup    		  (   );
    m_components.push_back( w );
    m_fwalls.push_back    ( w ); 

    // Dining Room/Bathroom :
    w = new glFullWall();
    w->clear( );
    w->set_length( 10*12.);
    w->m_y_angle = -90.0;
    w->m_z = 25*12-6-114;
    w->m_x = 153+6+48+6+111;  //528-144.-6*12;
    w->setup    		  (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );
    
    // Kitchen/Dining Room :
    w = new glFullWall();
    w->clear( );
    w->set_length( 111+5.25 );
    glDoorWay* doorw = new glDoorWay();                   // Kitchen Doorway
    doorw->m_width  = 35;
    doorw->m_height = 81;
    doorw->setup(     );
    //w->add_door(37.75, doorw);
    w->m_bare_wall.add_door( 37.75, 35, 81.0, false );   //122.+40.5+64.5
    w->set_texture(6);
    w->m_y_angle = -90.0;
    w->m_z = 25*12-6-111-5.25;
    w->m_x = 153+6+48+6;
    w->setup    		   (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );
 
    // Entryway / Kitchen :
    w = new glFullWall();
    w->clear( );
    w->set_length( 88.75+28.);          // add closest depth
//  w->m_bare_wall.set_texture( 20 );
//	w->set_texture( WALL_TEXTURE );
    w->m_y_angle = -90.0;
    w->m_z = 25*12-88.75-28-6;          //
    w->m_x = 4*12.+6;
    w->setup    (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );  
    
}

void glUpstairs::add_light_switches      ( )
{
    //glLightSwitch*      ls;
    glDualLightSwitch* dls;
    // 44 inches to bottom of cover

    // Entrance
    dls = new glDualLightSwitch();              dls->m_y = 44;
    dls->m_distance_along_wall = 8;             dls->m_wall_side = 1;
    m_fwalls[1]->add_fixture( dls, 8, 44 );

    // Living Room1
    // Living room2
    // dining room
    // dining room2 side door
    // kitchen
    // boys bedroom
    // mom & dads bedroom
    // Lynn's bedroom
    // bathroom
}

void glUpstairs::create_components       ( )
{
    create_exterior_walls();
    create_interior_walls();
    //add_light_switches   ();
    
    // Leave stairway open
    m_floor = new glFloor();
    compute_max ();
    compute_min ();
    m_floor->cover_region(0, 0, get_max_x(), get_max_z());
    m_floor->setup();
    m_components.push_back(m_floor);    
}

/********************************************/
/************ 	 BASEMENT 		*************/
/********************************************/
glBasement::glBasement ()
{
    
}
glBasement::~glBasement()
{
    
}

void glBasement::create_exterior_walls( )
{
    static glFullWall* w;

    // Side 1 :
    w = new glFullWall  ( );
    w->clear            ( );
    w->set_length       ( 44*12.);          // 24+20= 44 feet long inside
    struct stSize s;						// for windows
    s.width = 49.;    s.height = 47.;
    w->m_bare_wall.add_window( 44*12-60-49-6, s, 35. );
    w->m_bare_wall.set_texture( SELECTED_BRICK );
    w->m_y_angle = 0.0;
    w->m_z = 0.;
    w->m_x = 0.;
    w->setup    (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );
    
    // Update height stored in glDwellingLevel
    m_height = m_fwalls[0]->m_bare_wall.m_wall_height;
    
    // Side 2 :
    w = new glFullWall();
    w->clear( );
    w->set_length( 44*12.);
    s.width = 48.5;    s.height = 47;
    w->m_bare_wall.add_window( 44*12-44-48.5-6, s, 35.5 );
    w->m_bare_wall.set_texture( SELECTED_BRICK );
    w->m_y_angle = 0.0;
    w->m_z = 24*12;                         // 288
    w->m_x = 0.;
    w->setup    (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );

    // Front :
    w = new glFullWall();
    w->clear( );
    w->set_length( 25*12.);
    w->m_y_angle = -90.0;
    w->m_z = 0.;
    w->m_x = 0.;
    w->setup    		  (   );
    m_components.push_back( w );
    m_fwalls.push_back	  ( w );
    
    // Back End :
    glDoor* tdoor;
    w = new glFullWall();
    w->clear( );
    w->set_length( 25*12.);
    tdoor = new glDoor();                   // Bathroom Door
    tdoor->m_width  = 32;
    tdoor->m_height = 81;
    tdoor->set_params(30.);
    tdoor->set_hinge     (true);
    tdoor->set_swing_side(false );
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door          ( 63., tdoor );
    s.width = 74.;    s.height = 47.;
    w->m_bare_wall.add_window( 25*12-36-74-6, s, 35. );
    w->m_bare_wall.set_texture( SELECTED_BRICK );
    w->m_y_angle = -90.0;
    w->m_z = 0.;
    w->m_x = 44*12;
    w->setup              (   );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );
}

void glBasement::create_interior_walls ( )
{
    glDoor* tdoor;
    static glFullWall* w;
    struct stSize s;		// for windows
    
    // Backroom / Laundry room
    w = new glFullWall();
    w->clear( );
    w->set_length( 153 );       // = 112.+34.5+6
    w->m_bare_wall.add_door( 6, 34.5, 81.0, false );
    w->m_y_angle = -90.0;
    w->m_z = 25*12-6-153;
    w->m_x = 88+6.;
    w->setup    (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );

    // Laundry / Family room.
    w = new glFullWall();
    w->clear( );
    w->set_length( 25*12.);
    tdoor = new glDoor();                   // Bathroom Door
    tdoor->m_width  = 32;
    tdoor->m_height = 81;
    tdoor->set_params(30.);
    tdoor->set_hinge     (true);
    tdoor->set_swing_side(false );
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door          ( 1.+6, tdoor );
    w->m_bare_wall.add_door( 12*12, 35.5, 81., false );
    s.width = 36.;    s.height = 41.;
    w->m_bare_wall.add_window( 25*12-27.5-36-6, s, 36. );
    w->m_y_angle = -90.0;
    w->m_z = 0;
    w->m_x = 127+6+88+6;
    w->setup    (  );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );

    w = new glFullWall();
    w->clear( );
    w->set_length( 127. );
    tdoor = new glDoor();                   // Bathroom Door
    tdoor->m_width  = 32;
    tdoor->m_height = 81;
    tdoor->set_params(30.);
    tdoor->set_hinge     (true);
    tdoor->set_swing_side(false );
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door          ( 29.5, tdoor );
    w->m_y_angle = 0.0;
    w->m_z = 25*12-6-152;
    w->m_x = 88+6;
    w->setup              (  );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );
    
    // backroom/bathroom
    w = new glFullWall();
    w->clear( );
    w->set_length( 25*12.-153 );  // length =25*12 - 153
    w->m_y_angle = -90.0;
    w->m_z = 0;
    w->m_x = 26.375+88+6;
    w->setup    		  (  );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );

    // bathroom/toy closet
    w = new glFullWall();
    w->clear( );
    w->set_length( 25*12.-153 );  // length =25*12 - 153
    w->m_y_angle = -90.0;
    w->m_z = 0;
    w->m_x = 60+26.375+88+6;
    w->setup    		  (  );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );
}

void glBasement::add_light_switches      ( )
{
    
}

void glBasement::create_components       ( )
{
    create_exterior_walls  ( );
    create_interior_walls  ( );
    add_light_switches     ( );

    m_floor = new glFloor ( );
    m_floor->cover_region ( 0, 0, get_max_x(), get_max_z() );
    m_floor->m_y = 1.0;
    m_floor->setup		  ( );
    m_components.push_back( m_floor );
}

/********************************************/
/************ 	 BASEMENT 		*************/
/********************************************/
/********************************************/
glOurHouse::glOurHouse ( )
{    
}
glOurHouse::~glOurHouse( )
{   
}
void glOurHouse::create_components( )
{
    bm = new glBasement();
    bm->setup   ( );
    bm->relocate( 0,0,0 );
    add_level   ( bm );

/*    sw = new glStairway();
    sw->relocate( 60+26.375+88+6+sw->m_extrusion_length+1, 0, 0);
    sw->m_y_angle = -90.0;    
    sw->setup   ( );
    m_components.push_back(sw);*/

    up = new glUpstairs();
    up->setup   	(      );
    up->relocate	( 0,0,0);
    add_level   	( up   );
    
/*    sw->compute_min_max();
    float length = sw->m_z + sw->m_max.position[0];
    up->m_floor->uncover_region(sw->m_x-sw->m_extrusion_length,  length, sw->m_x, up->get_max_z() );
    up->m_floor->setup(); */
}


//    w->m_bare_wall.add_door( , DEFAULT_DOOR_WIDTH, 79.0, false );