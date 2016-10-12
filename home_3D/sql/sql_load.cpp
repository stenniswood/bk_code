//
//  sql_load.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 2/25/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <stdlib.h>
#include <vector>
#include <string>
#include <cstring>

#include "vector.hpp"
#include "sql_load.hpp"
#include "scene.hpp"


MYSQL *inventory_db;

void connect_inventory_db()
{
    inventory_db = mysql_init(NULL);
    if (inventory_db == NULL)
    {
        fprintf(stderr, "init %s\n", mysql_error(inventory_db));
        exit(1);
    }
    if (mysql_real_connect(inventory_db, "localhost", "root", "password",
                           "home_inventory", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "real_connect %s\n", mysql_error(inventory_db));
        mysql_close(inventory_db);
        exit(1);
    }
}



void sql_setup_descriptor_table();

/*
 use home_inventory;
 Select * from object_instances inner join object_descriptions where object_instances.object_description_id=object_descriptions._id AND
 object_instances.scene_id=1;
 
 We want to load everything from a given scene number.
        The object_types in descriptor will be well defined!
        Then pass this to new_object()  and do a relocate to m_origin.
*/
void sql_load_scene( int mScene_id )
{
    connect_inventory_db();
    //theScene.load( 1 );
}


/* This will initialize the descriptor table with the primitive object info.
    Remove this code once run one time!
 */
void sql_setup_descriptor_table()
{
 /* sqlObj.m_descriptor.moveability     = MOVEABLE;
    sqlObj.m_descriptor.adjectives      = "";
    sqlObj.m_descriptor.box_info_id     = 0;        // size info
    sqlObj.m_descriptor.is_a_container  = 0;
    sqlObj.m_descriptor.most_common_location_id  = 0;
    sqlObj.m_descriptor.most_common_container_id = 0;
    sqlObj.m_descriptor.purpose         = "";
    sqlObj.m_descriptor.description     = "primitive";
    sqlObj.m_descriptor.asset_filename  = "";

    //sqlObj.m_descriptor.object_type     = VERBAL_OBJECT;  // Verbal object?!@
    sqlObj.m_descriptor.object_type     =    ATOM;           // ATOM?! SPHERE?
    sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    BOX;                       sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    DOOR;                      sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    IBEAM;                     sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    CYLINDER;                  sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    EXTRUSION;                 sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    PAPER;                     sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    STAIRWAY;                  sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    TEXTURE_CONTAINER;         sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    ROUTE;                     sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    FULL_WALL;                 sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    CHAIR;                     sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    TABLE_4LEGS;               sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    TABLE_CENTER_POLE;         sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    DRAWER;                    sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    PALLETTE;                  sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    BARRICADE;                 sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    BOOKCASE;                  sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    BRICK;                     sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    SPHERE;                    sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    PICNIC_TABLE;              sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    CABINET;                   sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    COUNTER;                   sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    TRUSS2D;                   sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    TRUSS3D;                   sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    TRUSSHIGHWAY;              sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    RAFTER;                    sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    LIGHTSWITCH;               sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    HINGE;                     sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    ROAD;                      sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    TERRAIN;                   sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    PAINTING;                  sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    BASKETBALL;                sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    SOCCERBALL_5;              sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    SOCCERBALL_4;              sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    BASEBALL;                  sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    ROBOT_LEG;                 sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    ROBOT_ARM;                 sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    ROBOT;                     sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    STREET_SIGN;               sqlObj.m_descriptor.sql_add();
    sqlObj.m_descriptor.object_type     =    BASKETBALL_ARENA;          sqlObj.m_descriptor.sql_add();
    
    sqlObj.m_descriptor.object_type     =    SKETCHUP_IMPORT;           sqlObj.m_descriptor.sql_add();    
  */  
}


