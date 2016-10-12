//
//  apartment.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/7/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "apartment.h"
#include "all_objects.h"
#include "tk_colors.h"



glApartment::glApartment()
{
    
}

glApartment::~glApartment()
{
    
}

int vwalls=0;

void glApartment::create_vertical_walls()
{
    glDoor* tdoor;
    static glFullWall* w;
    struct stSize s;		// for windows
    //w->set_texture(1);
    
    // Outside wall (sliding door)                  # 0
    w = new glFullWall();
    w->set_length(144.+36.+116.+120.);
    s.width = 46.25;    s.height = 46.25;       // Window Dimensions!
    w->m_bare_wall.add_window( 66.5+144.+36.   , s, 33.5 );
    w->m_bare_wall.add_window( 144.+36.+116.+3., s, 33.5 );
    w->m_name = "outside";
    w->set_texture( 8 );
    w->m_y_angle = 0.0;
    w->m_z = 0.;
    w->m_x = 0.;
    w->setup( );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );

    // Update height stored in glDwellingLevel
    m_height = m_fwalls[0]->m_bare_wall.m_wall_height;
    // Hall way 1 :                             #1
    w = new glFullWall();
    w->clear( );
    w->set_length( 36.+116.+120.-3.);
    tdoor = new glDoor();                   // Door #1
    tdoor->set_params(30.);
    tdoor->set_hinge     (false);
    tdoor->set_swing_side(true);
    tdoor->m_name = "first bedroom";
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door          ( 110.     , tdoor );
    tdoor = new glDoor();                   // Door #2
    tdoor->set_params(30.);
    tdoor->set_hinge     (true );
    tdoor->set_swing_side(true);
    tdoor->m_name = "master bedroom";
    tdoor->setup         (     );
    tdoor->open          ( 0.4 );
    w->add_door  ( 122.+40.5, tdoor );
    w->m_bare_wall.add_door( 122.+40.5+64.5, DEFAULT_DOOR_WIDTH, 79.0, false );
    w->set_color ( tan0 );
    w->m_y_angle = 0.0;
    w->m_z = 160.;
    w->m_x = 144.;
    w->setup     (  );
    m_components.push_back( w );
    m_fwalls.push_back    ( w );
    
    // Hall way 2 :                             #2
    w = new glFullWall();
    w->clear();
    w->set_length(36.+116.+120.-77.5);
    tdoor = new glDoor();                       // Door #3
    tdoor->set_params( 28.);
    tdoor->set_hinge     (true);
    tdoor->set_swing_side(false);
    tdoor->m_name = "bathroom";
    tdoor->setup         (     );
    tdoor->open          ( 0.1 );
    w->add_door( 164.5, tdoor );
    //w->add_door( 164.5+52.,28. );		 // really need short inside walls
    w->set_color( salmon );
    w->m_y_angle = 0.0;
    w->m_z = 160.+36.;                   // 196
    w->m_x = 144.;                       //
    w->setup(  );                        // length = 194.5
    m_components.push_back( w );
    m_fwalls.push_back( w );
    
    // Final (kitchen to neighbors)             #3
    w = new glFullWall();
    w->clear();
    w->set_length(144.+36.+116.+120.);
    w->m_bare_wall.set_texture( 0 );
    w->m_y_angle = 0.0;
    w->m_z = 160.+36.+95.;               // 291
    w->m_x = 0.;
    w->setup(  );                       // 416
    m_components.push_back( w );
    m_fwalls.push_back( w );
    // 4 WALLS TOTAL.   [0-3]

    // Bathroom                                     #4 (zero indexed)
    w = new glFullWall();                   // WALL #5
    w->clear();
    w->set_length( 416. - 338.5 );
    tdoor = new glDoor();                       // Door #3
    tdoor->set_params    ( 28.);
    tdoor->set_hinge     (false);
    tdoor->set_swing_side(false);
    tdoor->m_name = "closest";
    tdoor->setup         (     );
    tdoor->open          ( 0.1 );
    w->add_door ( 19.5, tdoor  );
    w->set_color( bisque );
    w->m_y_angle = 0.0;
    w->m_z = 160.+70.;                       // 230
    w->m_x = 144.+122.+40.5+30.+2.;          // 338
    w->setup    (  );
    m_components.push_back( w );
    m_fwalls.push_back( w );

    for (int i=0; i<m_fwalls.size(); i++)
    {
    	printf("V Wall[%d] m_x,y,z = %6.1f,%6.1f,%6.1f\t", i, m_fwalls[i]->m_x, m_fwalls[i]->m_y, m_fwalls[i]->m_z ); 
    	printf(" : angles = %6.1f,%6.1f,%6.1f\n", m_fwalls[i]->m_x_angle, m_fwalls[i]->m_y_angle, m_fwalls[i]->m_z_angle ); 
    }
	vwalls = m_fwalls.size();    
}

void glApartment::create_horizontal_walls()
{
    glDoor* tdoor;
    static glFullWall* w;
    struct stSize s;		// for windows

    ////////////////////////// horizontals //////////////////////////
    // End of Hallway                       [5]
    w = new glFullWall();
    w->clear();
    w->set_length( 70.   );
    w->set_color( bisque );
    w->m_y_angle = -90.0;
    w->m_z       = 160.;
    w->m_x       = 144.+122.+40.5+30.+2.;			// 338.5 = (144 + 194.5)
    w->setup     (     );
    m_fwalls.push_back    ( w );
    m_components.push_back( w );

    // Divide (living & bed1)               [6]
    w = new glFullWall();
    w->clear();
    w->set_length( 160. );
    w->set_color( bisque );
    w->m_bare_wall.set_texture( 0 );
    w->m_y_angle = -90.0;
    w->m_z = 0.;
    w->m_x = 144.;
    w->setup();
    m_fwalls.push_back( w );
    m_components.push_back( w );
    
    // Divide (bed1 & bed2)
    w = new glFullWall();
    w->clear();
    w->set_length( 160. );
    w->set_color( bisque );
    w->m_bare_wall.set_texture( 0 );
    w->m_y_angle = -90.0;
    w->m_z = 0.;
    w->m_x = 144.+36.+116.;             // 296
    w->setup( );
    m_fwalls.push_back( w );
    m_components.push_back( w );
    
    // Outside End                  WALL #8
    w = new glFullWall();
    w->clear();
    w->set_length( 160.+36.+95. );              // 297
    s.width = 34.0;  s.height = 34.0;
    w->m_bare_wall.add_window( 97.5+6., s, 46.0 );
    w->set_color( bisque );
    w->set_texture(8);
    w->m_y_angle = -90.0;
    w->m_z = 0.;
    w->m_x = 144.+36.+116.+120.;                // 416
    w->setup    (  );
    m_fwalls.push_back( w );
    m_components.push_back( w );
    
    // Inside End:
    w = new glFullWall();
    w->clear();
    w->set_length( 160.+36.+95. );
    tdoor = new glDoor();                   // Door #2
    tdoor->set_params(30.);
    tdoor->set_hinge     (false );
    tdoor->set_swing_side(true);
    tdoor->m_name = "entrance";
    w->set_color( bisque );
    tdoor->setup         (     );
    tdoor->open          ( 0.5 );
    w->add_door( 160.+18.+5., tdoor  );		// Main Entrance Door
    w->set_texture(8);
    w->m_y_angle = -90.0;
    w->m_z = 0.;
    w->m_x = 0.;
    w->setup(  );
    m_fwalls.push_back( w );
    m_components.push_back( w );
    
    // Divide (kitchen & bath)              WALL #10
    w = new glFullWall();
    w->clear();
    w->set_length(  95.0  );
    tdoor = new glDoor();                   // Door #2
    tdoor->set_params(24.);
    tdoor->set_hinge     (true );
    tdoor->set_swing_side(false);
    tdoor->m_name = "pantry";
    tdoor->setup         (     );
    tdoor->open          ( 0.2 );
    w->add_door( 33.0, tdoor ); 	// pantry door.
    w->set_color( bisque );
    w->m_y_angle = -90.0;
    w->m_z = 160.+36.;
    w->m_x = 144.+114;
    w->setup( );
    m_fwalls.push_back( w );
    m_components.push_back( w );
    
    // Divide (pantry & linen)
    w = new glFullWall();
    w->clear();
    w->set_length( 31.5 );
    //tdoor->m_name = "linen";
    w->m_y_angle = 0.0;
    w->m_z = 160.+36.+26;
    w->m_x = 144.+114;
    w->setup(  );
    m_fwalls.push_back( w );
    m_components.push_back( w );
    
    // Divide (kitchen & bath)          WALL #12
    w = new glFullWall();
    w->clear();
    w->set_length( 95.0 );
    tdoor = new glDoor();
    tdoor->set_params(23.5);            // Door #2
    tdoor->set_hinge     (true );
    tdoor->set_swing_side(true);
    tdoor->m_name = "linen closet";
    tdoor->setup         (     );
    tdoor->open          ( 0.2 );
    w->add_door( 2.0, tdoor ); 	// linen closet "door"
    w->set_color( bisque );
    w->m_y_angle = -90.0;
    w->m_z = 160.+36.;
    w->m_x = 144.+114+31.5;
    w->setup(  );
    m_fwalls.push_back( w );
    m_components.push_back( w );

    
    // Stub (entry):                WALL #13
    w = new glFullWall();
    w->clear();
    w->m_bare_wall.m_wall_height = 43.;
    w->set_length( 70.5-4.25);      // 66.25
    w->set_color( PaleGreen );
    w->m_y_angle = 0.0;
    w->m_z = 160.+18.;
    w->m_x = 0.;
    w->setup(  );
    m_fwalls.push_back( w );
    m_components.push_back( w );
    
    // Post :
    w = new glFullWall();
    w->clear();
    w->set_length( 4.25 );
    w->set_color( PaleGreen );
    w->m_y_angle = 0.0;
    w->m_z = 160.+18.;
    w->m_x = 70.5-4.25;
    w->setup(  );
    m_fwalls.push_back    ( w );
    m_components.push_back( w );
    
    // entry closet :           WALL #15
    w = new glFullWall();
    w->clear();
    w->set_length( 70.5 );      // Critical Note: This wall length has to be set prior to add door!
    // Got lucky above because the length is not normally set until create()
    tdoor = new glDoor( );
    tdoor->set_params ( 28.);
    tdoor->set_hinge     (false );
    tdoor->set_swing_side(true);
    tdoor->m_name = "front closet";
    tdoor->setup         (     );
    tdoor->open          ( 0.2 );
    w->add_door( 35., tdoor ); 	// linen closet "door"
    w->set_color( PaleGreen );
    w->m_y_angle = -90.0;
    w->m_z       = 160.+36.+95.-70.5;
    w->m_x       = 74.75;
    w->setup(  );
    m_fwalls.push_back( w );
    m_components.push_back( w );
    
    w = new glFullWall();
    w->clear();
    w->set_length( 74.75 );
    w->set_color( gold );
    w->m_bare_wall.set_texture( 10 );
    w->m_y_angle = 0.0;
    w->m_z = 160.+36.+95.-70.5;
    w->m_x = 0.;
    w->setup(  );
    m_fwalls.push_back( w );
    m_components.push_back( w );
    
    for (int i=vwalls; i<m_fwalls.size(); i++)
    {
    	printf("  Wall[%d] m_x,y,z = %6.1f,%6.1f,%6.1f\t", i, m_fwalls[i]->m_x, m_fwalls[i]->m_y, m_fwalls[i]->m_z ); 
    	printf(" : angles = %6.1f,%6.1f,%6.1f\n", m_fwalls[i]->m_x_angle, m_fwalls[i]->m_y_angle, m_fwalls[i]->m_z_angle ); 
    }
}

void glApartment::add_light_switches( )
{
    glLightSwitch*      ls;
    glDualLightSwitch* dls;
    // 44 inches to bottom of cover
    
    // Entrance switches :
    //   dls->m_distance_along_wall= 8;
    dls = new glDualLightSwitch();
    dls->m_wall_side = 1;
    m_fwalls[13]->add_fixture( dls, 8, 44 );

    // Dining Room switches :
    dls = new glDualLightSwitch();
    // dls->m_distance_along_wall = 5;
    dls->m_wall_side = 0;
    m_fwalls[15]->add_fixture( dls, 5, 44 );           // Chandeliere Lights

//    dls = new glDualLightSwitch();
//    dls->m_distance_along_wall = 0;     dls->m_wall_side = 1;
//    m_fwalls[0]->add_fixture( dls );         // Fan
    
    // Kitchen Switches:
    ls = new glLightSwitch();
    //         ls->m_distance_along_wall = 9;
    ls->m_wall_side = 1;
    m_fwalls[2]->add_fixture( ls, 9, 44 );         // Lights
    dls = new glDualLightSwitch();
    // dls->m_distance_along_wall = 0;
    dls->m_wall_side = 1;
    m_fwalls[3]->add_fixture( dls, 1, 44 );         // Garbage Disposal (outside wall)

    // Hallway Lights:
    ls = new glLightSwitch();
    //ls->m_distance_along_wall = 5;
    ls->m_wall_side = 0;
    m_fwalls[2]->add_fixture( ls, 5, 44 );         // Close to door end.
    ls = new glLightSwitch();
    float distance_along_wall = m_fwalls[2]->m_bare_wall.get_door_near(0)-4;
    ls->m_wall_side = 0;
    m_fwalls[2]->add_fixture( ls, distance_along_wall, 44 );         // End of hall

    // First Bedroom:
    dls = new glDualLightSwitch();
    float tmp = m_fwalls[1]->m_bare_wall.get_door_near(0) - 4; // - dls->m_panel_width;
    dls->m_wall_side = 0;
    m_fwalls[1]->add_fixture( dls, tmp, 44 );         // by door
    
    // Master Bedroom:
    dls = new glDualLightSwitch();
    tmp = m_fwalls[1]->m_bare_wall.get_door_far(1) + 4;
    dls->m_distance_along_wall = tmp;       dls->m_wall_side = 0;
    m_fwalls[1]->add_fixture( dls, tmp, 44 );         // by door
    
    // Bathroom:
    dls = new glDualLightSwitch();
    dls->m_distance_along_wall = 60;         dls->m_wall_side = 1;
    m_fwalls[5]->add_fixture( dls, 60, 44 );         // by door
    
    // Pantry:
    dls = new glDualLightSwitch();
    tmp = m_fwalls[10]->m_bare_wall.get_door_far(0) + 4;
    dls->m_distance_along_wall = tmp;         dls->m_wall_side = 1;
    m_fwalls[10]->add_fixture( dls, tmp, 44 );         // by door
}

void glApartment::create_apartment()
{
    create_vertical_walls  ( );
    create_horizontal_walls( );
    //add_light_switches     ( );
    //hang_paintings         ( );
}


void glApartment::create_components( )
{
    create_apartment();
    create_floor();
}

void glApartment::hang_paintings( )
{
    //float scale = 6.0;
//  sidewalk_painting.m_painting->load_texture( "textures/sidewalk_painting.jpg" );
    sidewalk_painting.select_work_of_art( 5 );
//    sidewalk_painting.m_painting->set_width_height( 5*scale, 5*scale );
//    sidewalk_painting.setup();
    sidewalk_painting.m_painting->grab_bottom();
  //  sidewalk_painting.gl_register();
    sidewalk_painting.relocate( 144-7, 60, 80 );
    sidewalk_painting.m_x_angle = 0.;
    sidewalk_painting.m_y_angle = 90;
}

/****************************************************************
 
 ****************************************************************/

glMezannineUpstairs::glMezannineUpstairs ()
{
    
}
glMezannineUpstairs::~glMezannineUpstairs()
{
    
}

void    glMezannineUpstairs::add_light_switches       ( )
{
    
}
void    glMezannineUpstairs::create_components       ( )
{
    glDoor* tdoor;
    static glFullWall* w;
   // struct stSize s;		// for windows

    // Neighbors 1:                WALL #1
/*    w = new glFullWall();
    w->clear();
    w->m_bare_wall.m_wall_height = 43.;
    w->set_length( 70.5-4.25);
    w->setup(  );
    w->m_y_angle = -90.0;
    w->m_z = 160.+18.;
    w->m_x = 0.;
    m_components.push_back( w );
    m_fwalls.push_back( w );
    
    // Neighbors 2:                WALL #2
    w = new glFullWall();
    w->clear();
    w->m_bare_wall.m_wall_height = 43.;
    w->set_length( 70.5-4.25);
    w->setup(  );
    w->m_y_angle = -90.0;
    w->m_z = 160.+18.;
    w->m_x = 0.;
    m_components.push_back( w );
    m_fwalls.push_back( w );    */
    
    // Back wall 1:                WALL #1
    w = new glFullWall();
    w->clear();
    w->m_bare_wall.m_wall_height = 43.;
    w->set_length( 70.5-4.25);
    tdoor = new glDoor( );              // laundry room door
    tdoor->set_params ( 28.);
    tdoor->set_hinge     (false );
    tdoor->set_swing_side(true);
    tdoor->setup         (     );
    tdoor->open          ( 0.2 );
    w->add_door( 35., tdoor );
    w->setup(  );
    w->m_y_angle = -90.0;
    w->m_z = 160.+18.;
    w->m_x = 0.;
    m_components.push_back( w );
    m_fwalls.push_back( w );

    // Front Outside wall :                WALL #1
    w = new glFullWall();
    w->clear();
    w->m_bare_wall.m_wall_height = 43.;
    w->set_length( 70.5-4.25);
    w->setup(  );
    w->m_y_angle = -90.0;
    w->m_z       = 160.+18.;
    w->m_x       = 0.;
    m_components.push_back( w );
    m_fwalls.push_back( w );

    // Back Outside wall :                WALL #1
    w = new glFullWall();
    w->clear();
    w->m_bare_wall.m_wall_height = 43.;
    w->set_length( 70.5-4.25);
    w->setup(  );
    w->m_y_angle = -90.0;
    w->m_z       = 160.+18.;
    w->m_x       = 0.;
    m_components.push_back( w );
    m_fwalls.push_back( w );

    glStairway* stairs = new glStairway();
    stairs->create();
    stairs->relocate( -5*12-stairs->m_extrusion_length, 0, 0);
    stairs->m_y_angle = 90;
    m_components.push_back(stairs);
}


/****************************************************************
 
 ****************************************************************/
glMezannineDownstairs::glMezannineDownstairs ( )
{
    
}
glMezannineDownstairs::~glMezannineDownstairs( )
{
    
}
void    glMezannineDownstairs::add_light_switches( )
{
    
}
void    glMezannineDownstairs::create_components ( )
{
    
}


