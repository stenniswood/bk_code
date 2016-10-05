//
//  scene.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/11/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#ifndef scene_hpp
#define scene_hpp


#include <stdio.h>
#include <vector>
#include <string>

#include "verbal_object.h"
#include "room2.hpp"
#include "t_object.hpp"


#include <my_global.h>
#include <mysql.h>

class Scene3D
{
public:
    Scene3D();
    ~Scene3D();

    void        add_object                  ( Object& mObj );
    void        remove_object               ( Object& mObj );

    void        clear                       (               );
    void        find_all_objects_in_scene_nq( int mScene_id );
    void        load                        ( int mScene_id );
    void        save                        ( int mScene_id );
    int         scan_rooms                  ( MathVector mv );
    
    std::string                 query_string;
    int                         m_scene_id;
    MYSQL_RES*                  result;
    
    std::vector<Room>            m_rooms;
    std::vector<VerbalObject*>   m_objects;
};


extern Scene3D theScene;


#endif /* scene_hpp */
