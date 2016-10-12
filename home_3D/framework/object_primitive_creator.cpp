//
//  object_primitive_creator.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/12/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include "object_primitive_creator.hpp"
#include "simulator_memory.h"
#include "Texture.hpp"
#include "txt_container.hpp"
#include "all_objects.h"


struct stGenericObjectParameter NewObjectParameters;

void set_default_parameters( long mObject_type, struct stGenericObjectParameter* mParameters )
{
    switch (mObject_type)
    {
        case VERBAL_OBJECT  :
            break;
        case ATOM :
            break;
        case BOX  :
            mParameters->floats[0] = 10.0;      // l
            mParameters->floats[1] = 20.0;      // w
            mParameters->floats[2] = 30.0;      // h
            break;
        case DOOR :
            mParameters->floats[0] = 10.0;
            mParameters->floats[1] = 20.0;
            mParameters->integers[0]= 1;        // texture
            break;
        case IBEAM:
            mParameters->integers[0] = 0xFFFFFFFF;      // color
            mParameters->integers[1] = 12*10;           // Length (inches)
            break;
        case CYLINDER:
            mParameters->floats[0] = 12.0;      // radius
            mParameters->floats[1] = 12*1;      // height (inches)
            mParameters->integers[0] = 1;       // Y Axis
            break;
        case EXTRUSION:
            break;
        case PAPER:
            mParameters->floats[0] = 8.5;       // mWidth
            mParameters->floats[1] = 11.0;      // mHeight
            break;
        case STAIRWAY:
            mParameters->integers[0] = 12;      // Number of Steps
            mParameters->floats[0] = 12.0;      // rise
            mParameters->floats[1] = 12*1;      // run
            break;
        case TEXTURE_CONTAINER:
            break;
        case ROUTE:
            break;
        case FULL_WALL:
            mParameters->floats[0] = 10.0;      // length
            mParameters->floats[1] = 12*10;     // height (10 feet)
            break;
        case CHAIR:
            break;
        case TABLE_4LEGS:
            break;
        case TABLE_CENTER_POLE:
            break;
        case DRAWER:
            mParameters->floats[0] = 10.0;      // length
            mParameters->floats[1] = 12.0;      // width
            mParameters->floats[2] =  5.0;      // height
            break;
        case PALLETTE:
            break;
        case BARRICADE:
            break;
        case BOOKCASE:
            mParameters->integers[0] = 12;      // num steps
            mParameters->floats[0] =   30;      // width
            mParameters->floats[1] = 6*12;      // height
            break;
        case BRICK:
            mParameters->floats[0] = 12;     // length
            mParameters->floats[1] = 6;      // width
            mParameters->floats[2] = 6;      // height
            break;
        case SPHERE:
            mParameters->floats[0] = 25;     // radius
            mParameters->floats[1] = 6;      // width
            break;
        case PICNIC_TABLE:
            break;
        case CABINET:
            break;
        case COUNTER:
            break;
        case TRUSS2D:
            mParameters->floats[0] = 16*12;     // radius
            break;
        case TRUSS3D:
            mParameters->floats[0] = 16*12;     // radius
            break;
        case TRUSSHIGHWAY:
            break;
        case RAFTER:
            break;
        case LIGHTSWITCH:
            break;
        case HINGE:
            break;
        case ROAD:
            break;                   // 30
        case TERRAIN:
            mParameters->integers[0] = 400;      // length
            mParameters->integers[1] = 600;      // width
            break;
        case PAINTING:
            mParameters->floats[0] = 20;      // length
            mParameters->floats[1] = 30;      // width
            break;
        case BASKETBALL:
            break;               // 33
        case SOCCERBALL_5:
            break;               // 34
        case SOCCERBALL_4:
            break;               // 35
        case BASEBALL:
            break;               // 36
        case ROBOT_LEG:
            break;
        case ROBOT_ARM:
            break;
        case ROBOT:
            break;
        case STREET_SIGN:
            // (stop, yield, slippery_when_wet, parking, no_parking, handicapped, hospital, no_outlet,
            //  railroad_crossing, turn_left, turn_right, wrong_way, caution_speed_bump, caution)
            strcpy( mParameters->text_array, "stop" );
            //mParameters->integers[0]=2;      // length
            break;
        case BASKETBALL_ARENA:
            break;
        case SKETCHUP_IMPORT:
            break;
        case WINDOW:
            mParameters->floats[0] = 36;
            mParameters->floats[1] = 36;
            break;
        default:
            break;
    }
}

VerbalObject* new_object( long mObject_type, struct stGenericObjectParameter* mParameters )
{
    VerbalObject* obj = NULL;
    enum eObjectTypes  obj_type = (enum eObjectTypes) mObject_type;
    Texture* texture =NULL;
    txtContainer*  mb=NULL;
    
    const float SillHeight         = mParameters->floats[2];
    const float PositionLengthwise = mParameters->floats[3];

    switch (obj_type)
    {
        case VERBAL_OBJECT  : obj = new VerbalObject();
            break;
        case ATOM           : obj = new glAtom();
            break;
        case BOX            : obj = new glBox();
            ((glBox*)obj)->height = mParameters->floats[0];
            ((glBox*)obj)->width  = mParameters->floats[1];
            ((glBox*)obj)->depth  = mParameters->floats[2];
            obj->create();
            break;
        case DOOR           : obj = new glDoor();
            ((glDoor*)obj)->m_width  = mParameters->floats[0];
            ((glDoor*)obj)->m_height = mParameters->floats[1];
            ((glDoor*)obj)->select_texture( (int)mParameters->integers[0] );
            obj->create();
            break;
        case IBEAM          : obj = new glIbeam( mParameters->integers[1] );
            obj->m_color       = mParameters->integers[0];
            ((glIbeam*)obj)->m_is_closed   = true;
            obj->create();
            break;
        case CYLINDER    :
            obj = new glCylinder((int)(mParameters->integers[0]&0xFFFF));     // Number of samples (around circle)
            ((glCylinder*)obj)->m_radius           = mParameters->floats[0];
            ((glCylinder*)obj)->m_extrusion_length = mParameters->floats[1];
            ((glCylinder*)obj)->m_extrusion_axis   = (int)(mParameters->integers[0]&0xFF);
            obj->create();
            break;
        case EXTRUSION  : obj = new glExtrusion();
            break;
        case PAPER      : obj = new glPaper( 10 );
            ((glPaper*)obj)->set_width_height( mParameters->floats[0], mParameters->floats[1] );
            obj->create();
            break;
        case STAIRWAY   : obj = new glStairway();
            ((glStairway*)obj)->m_number_of_steps = (int)mParameters->integers[0];
            ((glStairway*)obj)->m_rise = mParameters->floats[0];
            ((glStairway*)obj)->m_run  = mParameters->floats[1];
            //((glStairway*)obj)->m_width  = mParameters->floats[2];
            obj->create();
            break;
        case TEXTURE_CONTAINER: obj = new txtContainer();
            obj->create();
            break;
        case ROUTE      : obj = new glRoute();
            break;
        case FULL_WALL  : obj = new glFullWall();
            ((glFullWall*)obj)->set_length( mParameters->floats[0] );
            obj->create();
            break;
        case CHAIR      : obj = new glChair();
            obj->create();
            break;
        case TABLE_4LEGS: obj = new glTable();
            obj->create();
            break;
        case TABLE_CENTER_POLE: obj = new glTableCenterPole();
            obj->create();
            break;
        case DRAWER      : obj = new glDrawer();
            // obj->set_width_height( );
            obj->create();
            break;
        case PALLETTE    : obj = new Palette();
            obj->create();
            break;
        case BARRICADE  : obj = new glBarricade();
            obj->create();
            break;
        case BOOKCASE   : obj = new glBookcase();
            obj->create();
            break;
            
        case SPHERE : obj = new glSphere( mParameters->floats[0], 25, 36 );
            obj->set_color(0x5f5f5f5f);
            obj->create();
            break;
            
        case PICNIC_TABLE :             // 20
            obj = new glPicnicTable();
            obj->create();
            break;
            
        case BRICK :
            texture = new Texture();
            texture->load_image("textures/cinder-block-texture.jpg");
            mb   = new txtContainer();
            mb->width  = 7.625;
            mb->height = 3.625;
            mb->depth  = 2.125;
            mb->set_color( 0xFF7FFF7F );
            mb->setup();
            mb->m_texture = texture;
            mb->grab_bottom();
            mb->grab_left();
            obj->create();
            break;
        case CABINET : obj = new glCabinet();
            obj->create();
            break;
        case TRUSS2D     : obj = new glTruss2D();
            obj->create();
            break;
        case TRUSS3D     : obj = new glTruss();
            obj->create();
            break;
        case TRUSSHIGHWAY: obj = new glHighwayOverhead();
            obj->create();
            break;
        case RAFTER      : obj = new glRafter();        // 26
            obj->create();
            break;
        case LIGHTSWITCH : obj = new glLightSwitch();
            obj->create();
            break;
        case HINGE       : obj = new glHinge();
            obj->create();
            break;
        case ROAD        : obj = new glRoad();
            obj->create();
            break;
        case TERRAIN     : obj = new glTerrain();       // 30
            obj->create();
            break;
        case PAINTING    : obj = new glPainting();
            obj->create();
            break;
            
        // No Parameters here:
        case BASKETBALL : obj = new glBasketball();
            obj->create();
            break;
        case SOCCERBALL_5 : obj = new glSoccerball5();
            obj->create();
            break;
        case SOCCERBALL_4 : obj = new glSoccerball4();
            obj->create();
            break;
        case BASEBALL : obj = new glBaseball();
            obj->create();
            break;

//        case 18: obj = new glCounter();
//            break;
//        case 20: obj = new glBrickWall();
            break;
        case ROBOT_LEG :  obj = new glLightSwitch();                                        // 36
            obj->create();
            break;
        case ROBOT_ARM :  obj = new glLightSwitch();
            obj->create();
            break;
        case ROBOT     :  obj = new glLightSwitch();
            obj->create();
            break;
        case STREET_SIGN : obj = new StreetSign();
            ((StreetSign*)obj)->m_sign_name = mParameters->text_array;
            obj->create();
            break;
        case BASKETBALL_ARENA:  obj = new glArena();
            obj->create();
            obj->relocate( -40*12, 2, -94*12 );
            ((glArena*)obj)->create_cam_routes();
            break;
        case SKETCHUP_IMPORT :
            
            break;
        case WINDOW:            obj = new glWindow();
            // To Which wall?!
            ((glWindow*)obj)->set_params( mParameters->floats[0], mParameters->floats[1],       /* window width & height             */
                                          mParameters->floats[2] );                             /* window sill height off the ground */
            //which_wall->add_window( PositionLengthwise, SillHeight, (glWindow*)obj );
            obj->create();
            break;
            
        default:
            break;
    }
    return obj;
}


void print_parameters( long mObject_type, struct stGenericObjectParameter* mParameters )
{
    switch (mObject_type)
    {
        case VERBAL_OBJECT:
            
            break;
        case ATOM :
            break;
        case BOX  :
            printf("New box: hwd = <%6.2f, %6.2f, %6.2f> \n",
                   mParameters->floats[0], mParameters->floats[1], mParameters->floats[2] );
            break;
        case DOOR :
            printf("New Door : width,height = <%6.2f, %6.2f>;\n Texture=%s\n",
                   mParameters->floats[0], mParameters->floats[1], mParameters->text_array  );
            break;
        case IBEAM:
            printf("IBeam: color,length = <%ld, %ld> \n", mParameters->integers[0], mParameters->integers[1] );           // Length (inches)
            break;
        case CYLINDER:
            printf("New cylinder: radius,length, axis = %6.2f, %6.2f, %lu \n",
                   mParameters->floats[0], mParameters->floats[1], mParameters->integers[0]);
            break;
        case EXTRUSION:
            break;
        case PAPER:
            printf("New Paper: length,width=< %6.2f, %6.2f >\n", mParameters->floats[0], mParameters->floats[1] );
            break;
        case STAIRWAY:
            printf("New Stairway: num Steps=%ld;  rise,run = < %6.2f, %6.2f >\n",
                   mParameters->integers[0], mParameters->floats[0], mParameters->floats[1] );
            break;
        case TEXTURE_CONTAINER:
            break;
        case ROUTE:
            break;
        case FULL_WALL:
            printf("New Wall: length,width=< %6.2f, %6.2f >\n", mParameters->floats[0], mParameters->floats[1] );
            break;
        case CHAIR:
            break;
        case TABLE_4LEGS:
            break;
        case TABLE_CENTER_POLE:
            break;
        case DRAWER:
            printf("New Drawer: length,width,height =< %6.2f, %6.2f, %6.2f >\n",
                   mParameters->floats[0], mParameters->floats[1], mParameters->floats[2] );
            break;
        case PALLETTE:
            break;
        case BARRICADE:
            break;
        case BOOKCASE:
            printf("New Bookcase: number of Shelves=%ld; width=%6.2f \n",
                   mParameters->integers[0], mParameters->floats[0] );
            break;
        case BRICK:
            printf("New Brick: lwh=< %6.2f, %6.2f, %6.2f > \n",
                   mParameters->floats[0], mParameters->floats[1], mParameters->floats[2]   );
            break;
        case SPHERE:
            printf("New Sphere: radius=< %6.2f > \n", mParameters->floats[0] );
            break;                 // 20
        case PICNIC_TABLE:
            break;
        case CABINET:
            break;
        case COUNTER:
            break;
        case TRUSS2D:
            printf("New Truss: length=< %6.2f > \n", mParameters->floats[0] );
            break;
        case TRUSS3D:
            printf("New Truss3D: length=< %6.2f > \n", mParameters->floats[0] );
            break;
        case TRUSSHIGHWAY:
            break;
        case RAFTER:
            break;
        case LIGHTSWITCH:
            break;
        case HINGE:
            break;
        case ROAD:
            printf("New Road: length=< %6.2f > \n", mParameters->floats[0] );
            break;                  // 30
        case TERRAIN:
            printf("New Terrain: length,width=< %6.2f, %6.2f > \n",
                   mParameters->floats[0], mParameters->floats[1] );
            break;
        case PAINTING:
            printf("New Painting: length,width=< %6.2f, %6.2f > \n",
                   mParameters->floats[0], mParameters->floats[1] );
            break;
        case BASKETBALL:
            break;                  // 33
        case SOCCERBALL_5:
            break;                  // 34
        case SOCCERBALL_4:
            break;                  // 35
        case BASEBALL:
            break;                  // 36
        case ROBOT_LEG:
            break;
        case ROBOT_ARM:
            break;
        case ROBOT:
            break;
        case STREET_SIGN:
            printf("New Street Sign: %ld  length,width=< %6.2f, %6.2f > \n",
                   mParameters->integers[0], mParameters->floats[0], mParameters->floats[1] );
            break;
        case BASKETBALL_ARENA:
            break;
        case SKETCHUP_IMPORT:
            break;
        default:
            break;
    }
}


