//
//  scene.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/11/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include "full_wall.h"
#include "scene.hpp"
#include "t_object.hpp"
#include "physical_world.h"


Scene3D theScene;

Scene3D::Scene3D()
{
    result=NULL;
}

Scene3D::~Scene3D()
{
    
}

void Scene3D::add_object                  ( Object& mObj )
{
    
}
void Scene3D::remove_object               ( Object& mObj )
{
    
}

int Scene3D::scan_rooms( MathVector mv )
{
    int index=0;
    vector<Room>::iterator iter = m_rooms.begin();
    while (iter != m_rooms.end())
    {
        if (iter->contains(mv))
        {
            return index;
        }
        index++;
        iter++;
    }
    return -1;
}

void Scene3D::find_all_objects_in_scene_nq( int mScene_id )
{
    char tmp[16];
    query_string = "SELECT * FROM home_inventory.object_instances ";
    query_string += "INNER JOIN home_inventory.object_descriptions ON object_description_id = home_inventory.object_descriptions._id ";
    query_string += "INNER JOIN home_inventory.box_info ON home_inventory.object_instances.box_info_id = home_inventory.box_info._id ";
    query_string += " WHERE Scene_id=";
    sprintf(tmp, "%d", m_scene_id);
    query_string += tmp;
}

void Scene3D::clear( )
{
    m_objects.clear();
}


void Scene3D::load( int mScene_id )
{
    m_scene_id = mScene_id;
    find_all_objects_in_scene_nq( mScene_id);
    query(query_string, result, true);
    
    MYSQL_ROW row = mysql_fetch_row( result );
    Object    obj;
    VerbalObject* vobj=NULL;
    
    while (row)
    {
        obj.extract_result(row);
        vobj = obj.create_opengl();
        m_objects.push_back  ( vobj   );
        row = mysql_fetch_row( result );
    }
}

void Scene3D::save( int mScene_id )
{
    Object        obj;
    //VerbalObject* vobj = NULL;

    for ( auto iter = theWorld.m_moveables.begin();
          iter      != theWorld.m_moveables.end();
          iter++ )
    {
        //row = mysql_fetch_row( result );
        iter++;
    }
}


