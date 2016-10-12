//
//  painting.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/24/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "painting.h"
#include "all_objects.h"


glPainting::glPainting()
{
    m_frame_width      = 1.5;
    m_matting_width    = 2;
    m_object_type_name = "painter";
    m_painting = NULL;
    m_frame    = NULL;
    m_matting  = NULL;
}

glPainting::~glPainting()
{
    if (m_painting)
        delete m_painting;
    if (m_frame)
        delete m_frame;
    if (m_matting)
        delete m_matting;
}

void glPainting::create_components()
{
    m_painting->setup();
    m_components.push_back(m_painting);

    m_matting = new glFrame( );
    m_matting->set_params( m_matting_width, m_painting->m_height, m_matting_width, 0.1 );
    m_matting->setup       ( );

    m_frame = new glFrame();
    m_frame->set_params( m_painting->m_width, m_painting->m_height, m_frame_width, 2.5 );
    m_frame->setup();
    m_components.push_back(m_frame);
}

void glPainting::select_work_of_art(int mIndex )
{
    if (m_painting == NULL)
        m_painting = new glPaper();

    switch(mIndex)
    {
        case 0 : m_painting->load_texture ("paintings/central park in the fall.jpg");            break;
        case 1 : m_painting->load_texture ("paintings/creation_of_adam.jpg");                    break;
        case 2 : m_painting->load_texture ("paintings/famous_oil_flowers.jpg");                  break;
        case 3 : m_painting->load_texture ("paintings/famous_oil_the_four_seasons_spring.jpg");  break;
        case 4 : m_painting->load_texture ("paintings/famous_paintings_sunday_afternoon.jpg");   break;
        case 5 : m_painting->load_texture ("paintings/famous_paintings_lady.jpg");               break;
        case 6 : m_painting->load_texture ("paintings/london.jpg");                              break;
        case 7 : m_painting->load_texture ("paintings/men_rowing.jpg");                          break;
        case 8 : m_painting->load_texture ("paintings/mona_lisa.jpg");                           break;
        case 9 : m_painting->load_texture ("paintings/munch-panic.jpg");                         break;
        case 10 : m_painting->load_texture("paintings/people.jpg");                              break;
        case 11 : m_painting->load_texture("paintings/sstapletonparknearpontefract1440.jpg");        break;
        case 12 : m_painting->load_texture("paintings/starry_night.jpg");                            break;
        case 13 : m_painting->load_texture("paintings/the_persistence_of_memory_salvador_dali.jpg"); break;
        case 14 : m_painting->load_texture("paintings/world_famous_cliff.jpg");                      break;
        case 15 : m_painting->load_texture("paintings/wpe113.jpg");                                  break;
        case 16 : m_painting->load_texture("paintings/sidewalk_painting.jpg");                       break;
            // Tenniswood's house paintings...
        case 20 : m_painting->load_texture("paintings/living_room_picture.jpg");                    break;
        case 21 : m_painting->load_texture("paintings/stairway_rug.jpg");                           break;
        default : break;
    };
    //imshow( "Display Image2", m_painting->m_texture->m_image );
}
