//
//  sign.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 6/25/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//
#include "sign.h"
#include "all_objects.h"

char sign_regexp[] = "(stop|yield|slippery_when_wet|parking|no_parking|handicapped|hospital|no_outlet|railroad_crossing|turn_left|turn_right|wrong_way|caution_speed_bump|caution)";


glSign::glSign	(  )
{
    m_object_type_name = "sign";
    m_width  = 160;
    m_height = 144;
}

void glSign::generate_vertices( )
{
    m_vertices.clear();
    struct Vertex v;
    v.color[0] = ((m_color & 0x00FF0000)>>16);
    v.color[1] = ((m_color & 0x0000FF00)>>8 );
    v.color[2] = ((m_color & 0x000000FF)    );
    v.color[3] = ((m_color & 0xFF000000)>>24);

    v.position[0] = 0 ;
    v.position[1] = 2.;
    v.position[2] = 0 ;
    m_vertices.push_back(v);
    
    v.position[0] = 0 ;
    v.position[1] = 2.;
    v.position[2] = m_width;
    m_vertices.push_back(v);

    v.position[0] = m_height;
    v.position[1] = 2.;
    v.position[2] = m_width;
    m_vertices.push_back(v);

    v.position[0] = m_height;
    v.position[1] = 2.;
    v.position[2] = 0 ;
    m_vertices.push_back(v);
}

void glSign::setup( )
{
    generate_vertices();
    m_indices.clear();
    m_indices.push_back( 0 );
    m_indices.push_back( 1 );
    m_indices.push_back( 2 );
    m_indices.push_back( 3 );
    
    m_texture = new Texture();    
//    m_texture->load_image( "textures/me_in_car.bmp"   );
//    m_texture->load_image( "textures/light_house.jpg" );
//    m_texture->load_image( "textures/oak_wood.jpg"    );
//    m_texture->load_image( "textures/bambo-fence-texture.jpg" );
//    m_texture->load_image( "textures/cinder-block-texture.jpg" );
    m_texture->load_image( "textures/drywall-texture.jpg" );
    m_texture->m_repetitions_x = 2.0;
    m_texture->generate_texture_coords();
    m_texture->generate_TBO           ();
    m_texture->generate_VBOTexCoords  ();
}

void glSign::draw_primitive( )
{
    glDrawElements(GL_POLYGON, (int)m_vertices.size(), GL_UNSIGNED_INT, 0 );
}

/**************************************************************************************/

StreetSign::StreetSign()
{

}
StreetSign::~StreetSign()
{
    
}

//
void        StreetSign::create_components()
{
    
    m_sign = new glFaceBox();
    m_sign->set_color( 0xFFAFAFAF );
    m_sign->width  = 12;
    m_sign->height = 18;
    m_sign->depth  = 0.1;
    m_sign->setup();
    m_sign->grab_bottom();
    m_components.push_back(m_sign);
    select_measurements (m_sign_name);
    select_image        (m_sign_name);
    m_sign->m_y_angle = -90;

    
    m_pole = new glBox();
    m_pole->height = m_height_off_ground + m_sign->height;
    m_pole->width  = 1;
    m_pole->depth  = 1;
    m_pole->setup();
    m_pole->grab_bottom();
    m_pole->relocate( 0, 0, 0 );
    m_components.push_back(m_pole);

    m_sign->relocate( -m_pole->depth, m_height_off_ground, 0 );
}

void StreetSign::select_measurements( string mText )
{
    if (mText.compare("stop")== 0)
    {
        m_height_off_ground = 5*12;
    }
    else if (mText.compare("yield")== 0)
    {
        m_height_off_ground = 5*12;
    }
    else if (mText.compare("slippery_when_wet")== 0)
    {
        m_height_off_ground = 5*12;
    }
    else if (mText.compare("parking")== 0)
    {
        m_height_off_ground = 5*12;
    }
    else if (mText.compare("no_parking")== 0)
    {
        m_height_off_ground = 5*12;
    }
    else if (mText.compare("handicapped")== 0)
    {
        m_height_off_ground = 5*12;
    } else if (mText.compare("hospital")== 0)
    {
        m_height_off_ground = 5*12;
    } else if (mText.compare("no_outlet")== 0)
    {
        m_height_off_ground = 5*12;
    } else if (mText.compare("railroad_crossing")== 0)
    {
        m_height_off_ground = 5*12;
    } else if (mText.compare("turn_left")== 0)
    {
        m_height_off_ground = 5*12;
    } else if (mText.compare("turn_right")== 0)
    {
        m_height_off_ground = 5*12;
    } else if (mText.compare("wrong_way")== 0)
    {
        m_height_off_ground = 5*12;
    } else if (mText.compare("caution_speed_bump")== 0)
    {
        m_height_off_ground = 5*12;
    } else if (mText.compare("caution")== 0)
    {
        m_height_off_ground = 5*12;
    }
    
}

/* List of available words:
    (stop, yield, slippery_when_wet, parking, no_parking, handicapped, hospital, no_outlet,
    railroad_crossing, turn_left, turn_right, wrong_way, caution_speed_bump, caution)
 
 */
void StreetSign::select_image( string mText )
{
    if (mText.compare("stop")== 0)
    {
        m_sign->load_image( "street_signs/stop.jpg", FACE_FRONT_ID, 1  );
    }
    else if (mText.compare("yield")== 0)
    {
        m_sign->load_image( "street_signs/yield", FACE_FRONT_ID, 1 );
    }
    else if (mText.compare("slippery_when_wet")== 0)
    {
        m_sign->load_image( "street_signs/slippery_when_wet", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("parking")== 0)
    {
        m_sign->load_image( "street_signs/parking", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("no_parking")== 0)
    {
        m_sign->load_image( "street_signs/no_parking", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("handicapped")== 0)
    {
        m_sign->load_image( "street_signs/handicapped.jpg", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("hospital")== 0)
    {
        m_sign->load_image( "street_signs/hospital.jpg", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("no_outlet")== 0)
    {
        m_sign->load_image( "street_signs/no_outlet.jpg", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("railroad_crossing")== 0)
    {
        m_sign->load_image( "street_signs/railroad_crossing.jpg", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("turn_left")== 0)
    {
        m_sign->load_image( "street_signs/turn_left.jpg", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("turn_right")== 0)
    {
        m_sign->load_image( "street_signs/turn_right.jpg", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("wrong_way")== 0)
    {
        m_sign->load_image( "street_signs/wrong_way.jpg", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("caution_speed_bump")== 0)
    {
        m_sign->load_image( "street_signs/caution_speed_bump.jpg", FACE_FRONT_ID,1 );
    }
    else if (mText.compare("caution")== 0)
    {
        m_sign->load_image( "street_signs/caution", FACE_FRONT_ID,1 );
    }
    
}



